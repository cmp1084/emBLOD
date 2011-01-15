/*****************************************************************************
                             ___________    ____________
    __________________  ____ \  _   \   \   \        \  \_
    \    __    \      \/    \/  7  _/   /   /  __    /    \_
    /     7    /            /   _   \  /___/    7   /  _    \_
   /_     ____/_    /\_/   /    7   /     /_       /   7      \
     \________/ \__/  /___/________/\______/\_____/___________/
                                            [asmegir.hopto.org]
             e       m       B       L       O       D

    Copyright 2011 Marcus Jansson <mjansson256@yahoo.se>

    This file is part of emBLOD - the embedded bootloader

    emBLOD is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    emBLOD is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with emBLOD.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
/* Tab size = 4 */

#include <avr32/io.h>

#include "drivers/sdhc.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

__attribute__((section(".data")))
unsigned char sdhcBuf[512+16];
unsigned char * sdhcSectorBuf = &sdhcBuf[4];

void sdhcSendSpiBlock(unsigned char * buf, unsigned short len)
{
	ledOn(LED0_GPIO);				//Not neccesary but can be nice.
	while(len--) {
		spiWriteByte(SD_SPI, *buf);
		buf++;
	}
	ledOff(LED0_GPIO);
}


static void sdhcReceiveSpiBlock(unsigned short len)
{
	unsigned char * buf = sdhcSectorBuf;
	unsigned int byte;
	if(!len) {
		return;
	}
	while(--len) {
		spiWriteByte(SD_SPI, 0xff);
		spiReadByte(SD_SPI, &byte);
		*buf = (char)byte;
		buf++;
	}
}


static void sdhcSendffSpi(unsigned short len)
{
	while(len--) {
		spiWriteByte(SD_SPI, 0xff);
	}
}


static void sdhcWaitForSpi(unsigned char val)
{
	unsigned int byte;
	while(1) {
		spiWriteByte(SD_SPI, 0xff);
		spiReadByte(SD_SPI, &byte);
		if(byte == val) {
			return;
		}
	}
}


static unsigned char waitnotspi(unsigned char val)
{
	unsigned char timeout = 8;    //8 from SD specification
	unsigned int got;

	while(timeout--) {
		spiWriteByte(SD_SPI, 0xff);
		spiReadByte(SD_SPI, &got);
		if(val != (char)got) {
			return got;
		}
	}
	return got;
}

static void csact(void)
{
	spiChipSelect(SD_SPI, SD_CS);
	sdhcSendffSpi(1);
}

static void csinact(void)
{
	spiChipDeselect(SD_SPI);
	sdhcSendffSpi(1);
}

#define SDECHO 0xaa
#define RECVBUFOFFSET 1
static unsigned char goidle[6] = { 0+0x40, 0, 0, 0, 0, 0x95 };            // R1
static unsigned char sendifcond[6] = { 8+0x40, 0, 0, 1, SDECHO, 0x87 };   //TODO: Autocalc CRC and send a random echo value each time :)
static unsigned char readocr[6] = { 58+0x40, 0, 0, 0, 0, 0xff };          // R3
static unsigned char doapp[6] = { 55+0x40, 0, 0, 0, 0, 0xff };            // R1
static unsigned char sendopcond[6] = { 41+0x40, 0x40, 0, 0, 0, 0xff };    // HCS, R1 - activate init

static unsigned char ishccard;

// send command to SPI card, wait for response
static unsigned char sdhcSendCommand(unsigned char * cmd)
{
	sdhcSendffSpi(1);           //needed to make sure card is ready
	sdhcSendSpiBlock(cmd, 6);
	return waitnotspi(0xff);    //return first status byte
}

static unsigned char sdhcInitSeq(void)
{
	unsigned char needretry, nov2card;
	unsigned short i;

	#define MAXTRIES1 500
	for (i = 0; i < MAXTRIES1; ++i) {
		if (1 == sdhcSendCommand(goidle)) {    //R1 idle bit
			break;
		}
	}
	if (i >= MAXTRIES1) {
		return 1;
	}

	/* check for version of SD card */
	i = sdhcSendCommand(sendifcond);
	nov2card = 0;

	if (!(4 & i)) {                                       //legal command - 2.0 spec
		sdhcReceiveSpiBlock(5);
		if (!(sdhcSectorBuf[1+RECVBUFOFFSET] & 0x01)      //Voltage accepted?
		  || (sdhcSectorBuf[2+RECVBUFOFFSET] != SDECHO))  //Test pattern echoed back correctly?
			return 2;
	} else {
		nov2card = 1;
	}

	//technically optional
	ishccard = 0;
	needretry = 0;
	i = sdhcSendCommand(readocr);
	// should verify not illegal command, if so, it is not this kind of card
	sdhcReceiveSpiBlock(5);
	// Should verify voltage range here, bit 23 is 3.5-3.6, .1v lower going down
	// maybe later wait until not busy, but get the sdhc flag here if set
	if (sdhcSectorBuf[0] & 0x80) {
		ishccard = (sdhcSectorBuf[0] & 0x40);            // has CCS
	}
	else {
		needretry = 1;
	}

	//Most cards take only a few loops if already inited
	#define MAXTRIES2 40000
	for (i = 0; i < MAXTRIES2; ++i) {
		doapp[1] = ishccard;
		sdhcSendCommand(doapp);                //Not for MMC or pre-1, but ignored
		if (!(1 & sdhcSendCommand(sendopcond)))
			break;
	}

	//~ TODO: remove
	//~ usartWriteLine(USART0, "# retries: ");
	//~ usartWriteValue(USART0, i);
	//~ usartWriteLine(USART0, "\n");

	if (i >= MAXTRIES2)
		return 3;
	while(needretry && i++ < MAXTRIES2) {      // card was busy so CCS not valid
		i = sdhcSendCommand(readocr);          // 0x3f, then 7 more	//Uuh, wtf 'i'?!!1
		sdhcReceiveSpiBlock(5);
		ishccard = (sdhcSectorBuf[0] & 0x40);  // has CCS
		needretry = !(sdhcSectorBuf[0] & 0x80);
	}

	if (i >= MAXTRIES2)
		return 4;
	if (!ishccard) {                           // set block to 512 for plain SD card
		unsigned char setblocklen[6] = { 16+0x40, 0, 0, 2, 0, 0xff };
		sdhcSendCommand(setblocklen);
	}
	return 0;
}

static unsigned char getinfo[6] = { 9+0x40, 0, 0, 0, 0, 0xff };
unsigned int sdnumsectors = 0;

unsigned int cardsize(void)
{
	getinfo[0] = 9+0x40;                       //CSD
	if (!sdhcSendCommand(getinfo)) {
		sdhcWaitForSpi(0xfe);
		sdhcReceiveSpiBlock(18);
	}
	else
		return 0;
	// Note: fsb[3] will contain max trans speed to set the SPI,
	// All SD memory cards will be 0x32 or 0x5a for 25Mhz or 50Mhz.
	// SDIO or older MMC may be slower.
	// Low 3 bits are bits per sec 100k * 10 ** bits (up to 100M)
	// bits 6:3 are multiplier:
	// RSV,1,1.2,1.3, 1.5,2.2.5,3, 3.5,4,4.5,5, 5.5,6,7,8
	unsigned long size;
	if( sdhcSectorBuf[0] & 0x40 ) {
		size = sdhcSectorBuf[7] & 0x3f;
		size <<= 16;
		size |= sdhcSectorBuf[8] << 8;
		size |= sdhcSectorBuf[9];
		++size;
		return size << 10;
	}
	else  {
		size = (sdhcSectorBuf[6] & 3 ) << 10;
		size |= sdhcSectorBuf[7] << 2;
		size |= sdhcSectorBuf[8] >> 6;
		size++;
		unsigned mult;
		mult = ( sdhcSectorBuf[9] & 3 ) << 1;
		mult |= sdhcSectorBuf[10] >> 7;
		mult += 2 + (sdhcSectorBuf[5] & 0xf);
		size <<= mult - 9;
		return size;
	}
}

#define SDHC_INIT_SUCCESSFUL 1
#define SDHC_INIT_MAX_RETRIES 50000
unsigned char sdhcInit(void)
{
	unsigned char i = 0;
	unsigned int retries = 0;

	//Wait atleast 1 ms for voltage to stabilize above 2.2 V
	do {
		//spiReset(&AVR32_SPI1);
		delay_ms(1);
		//~ usartWriteLine(USART0, "Init SD-card: ");
		csinact();
		sdhcSendffSpi(10);          //SD cards need at least 74 clocks to start properly
		csact();
		i = sdhcInitSeq();
		//~ usartWriteValue(USART0, i);
		//~ usartWriteLine(USART0, "\n");
		sdnumsectors = cardsize();
		csinact();
	} while((i == SDHC_INIT_SUCCESSFUL) && (retries++ < SDHC_INIT_MAX_RETRIES));
	return i;
}

unsigned char sdhcCardInfo(unsigned char which)
{
	unsigned char ret = 1;
	csact();
	getinfo[0] = 0x40 + (which ? 10 : 9);   //CID:CSD
	if (!sdhcSendCommand(getinfo)) {
		sdhcWaitForSpi(0xfe);
		sdhcReceiveSpiBlock(18);
		ret = 0;
	}
	csinact();
	return ret;
}

static unsigned char rw1bcmd[6];

static void sdhcSetBlockAddress(unsigned int blkaddr)
{
	if (!ishccard) {
		blkaddr <<= 9;
	}
	rw1bcmd[1] = blkaddr >> 24;
	rw1bcmd[2] = blkaddr >> 16;
	rw1bcmd[3] = blkaddr >> 8;
	rw1bcmd[4] = blkaddr;
	rw1bcmd[5] = 0xff;
}

unsigned char sdhcReadSector(unsigned int blkaddr)
{
	unsigned char ret = 1;
	rw1bcmd[0] = 0x51;          // read
	sdhcSetBlockAddress(blkaddr);
	csact();
	if (!sdhcSendCommand(rw1bcmd)) {
		sdhcWaitForSpi(0xfe);
		sdhcReceiveSpiBlock(514);
		// CRC16 appended H,L
		ret = 0;
	}
	csinact();
	return ret;
}

unsigned char sdhcWriteSector(unsigned int blkaddr)
{
	unsigned char ret = 1;
	rw1bcmd[0] = 0x58;          // write
	sdhcSetBlockAddress(blkaddr);
	csact();
	if (!sdhcSendCommand(rw1bcmd)) {
		sdhcSectorBuf[-1] = 0xfe;
		// CRC
		sdhcSectorBuf[512] = 0xff; // CRCH
		sdhcSectorBuf[513] = 0xff; // CRCL
		sdhcSendSpiBlock(&sdhcSectorBuf[-1], 515);
		sdhcSendffSpi(20);           // wait for busy!
		sdhcWaitForSpi(0xff);        //wait for not busy - equiv while == 0
		ret = 0;
	}
	csinact();
	return ret;
}
