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

#include "drivers/spi.h"
#include "drivers/led.h"

#ifdef CONFIG_SPI_TIMEOUTVALUE
#define SPI_TIMEOUTVALUE CONFIG_SPI_TIMEOUTVALUE
#else
#define SPI_TIMEOUTVALUE 10000
#endif

int spiCalcBaudrateDivisor(const spi_cfg_t * spiConfigStruct, unsigned int mck)
{
	int scbr;
	return scbr = (mck + spiConfigStruct->spck / 2) / spiConfigStruct->spck;
}

void spiReset(volatile avr32_spi_t * spi)
{
	//SW reset SPI
	//~ spi->cr = 1 << AVR32_SPI_CR_SWRST_OFFSET;	//This doesnt get optimized enough
	//The code below saves 2 bytes.
	int cr;
	asm volatile("sbr  %0,%1" : "=r" (cr)      : "i" (AVR32_SPI_CR_SWRST_OFFSET));
	asm volatile("st.w %0,%1" : "=m" (spi->cr) : "r" (cr) : "memory"  );
}

void spiSetup(volatile avr32_spi_t * spi, const spi_cfg_t * spiConfigStruct, const spi_pin_cfg_t * spiPinConfigStruct)
{
	//Errata:
	//41.2.3
	//do not set SPI FDIV = 1
	//Bad serial clock on 2nd chip select SCBR = 1, CPOL = 1, NCPHA = 0

	//Enable SPI1 clocking
	pmPbaMaskSet((1 << PM_PBAMASK_SPI1_OFFSET ) | pmPbaMaskGet());

	//Enable SPI peripheral pins
#if(CONFIG_SPI0)
	gpioPeripheralEnable(CONFIG_SPI0_MISO_PIN, CONFIG_SPI0_MISO_FUNCTION);
	gpioPeripheralEnable(CONFIG_SPI0_MOSI_PIN, CONFIG_SPI0_MOSI_FUNCTION);
	gpioPeripheralEnable(CONFIG_SPI0_SCK_PIN, CONFIG_SPI0_SCK_FUNCTION);
#endif

#if(CONFIG_SPI1)
	gpioPeripheralEnable(CONFIG_SPI1_MISO_PIN, CONFIG_SPI1_MISO_FUNCTION);
	gpioPeripheralEnable(CONFIG_SPI1_MOSI_PIN, CONFIG_SPI1_MOSI_FUNCTION);
	gpioPeripheralEnable(CONFIG_SPI1_SCK_PIN, CONFIG_SPI1_SCK_FUNCTION);
#endif

	//Enable the chip select pin
	gpioPeripheralEnable(spiPinConfigStruct->cs_pin, spiPinConfigStruct->cs_function);
}

#define AVR32_SPI_CSRX_CSAAT_OFFSET AVR32_SPI_CSR0_CSAAT_OFFSET
void spiSetMode(volatile avr32_spi_t * spi, const spi_cfg_t * spiConfigStruct)
{
	int csrxValue;
	void * csrx;
	int scbr;

	//Set master mode
	//Disable mode fault detection
	//Deselect chip
	spi->mr = (spiConfigStruct->mstr << AVR32_SPI_MSTR_OFFSET) |    \
	          (spiConfigStruct->modfdis << AVR32_SPI_MODFDIS_OFFSET) | \
	          (0x0f << AVR32_SPI_PCS_OFFSET);

	//Calculate baudrate divisor for SPCK speed
	scbr = spiCalcBaudrateDivisor(spiConfigStruct, FCPU);	//TODO: Fix autoread Fcpu when using PLL

	//Set polarity
	//Set phase
	//Set CSAAT, chip select active after transfer
	csrxValue = (spiConfigStruct->cpol << AVR32_SPI_CPOL_OFFSET) | \
	            (spiConfigStruct->ncpha << AVR32_SPI_NCPHA_OFFSET) | \
	            (spiConfigStruct->csaat << AVR32_SPI_CSRX_CSAAT_OFFSET) | \
	            ((spiConfigStruct->bits - 8) << AVR32_SPI_BITS_OFFSET) | \
	            (scbr << AVR32_SPI_CSR0_SCBR_OFFSET);

	//~ csrxValue = (1 << AVR32_SPI_NCPHA_OFFSET) | (1 << AVR32_SPI_CSR0_CSAAT_OFFSET);

	//Calculate the CSRx to use and write the modeValue
	csrx = (void *)&(spi->csr0);
	csrx += 4 * spiConfigStruct->cs;
	//~ csrx = (void *)((int)csrx + 4 * spiConfigStruct->cs);

	*(int *)csrx = csrxValue;

	//~ *(int *)((void *)&(spi->csr0) + 4 * spiConfigStruct->cs) = csrxValue;    //sweet
}

void spiEnable(volatile avr32_spi_t * spi)
{
	//Enable SPI
	spi->cr = 1 << AVR32_SPI_SPIEN_OFFSET;
}


void spiLLBSet(volatile avr32_spi_t * spi, const int onoff)
{
	switch(onoff) {
		case ON:
			spi->mr |= 1 << AVR32_SPI_MR_LLB_OFFSET;
			break;
		default:
			spi->mr &= ~(1 << AVR32_SPI_MR_LLB_OFFSET);
			break;
	}
}

//input cs - Chip Select, legal values: CS0, CS1, CS2, CS3 (0, 1, 2, 3)
void spiChipSelect(volatile avr32_spi_t * spi, const int cs)
{
	int mr;
	mr = spi->mr;
	mr |= AVR32_SPI_MR_PCS_MASK;
	mr &= ~(1 << (AVR32_SPI_MR_PCS_OFFSET + cs));
	spi->mr = mr;

	//Unfortunately this line doesnt compile cleanly every time.
	//~ spi->mr |= AVR32_SPI_MR_PCS_MASK & ~(1 << (AVR32_SPI_MR_PCS_OFFSET + cs));
	//The code above save 12 bytes compared to the code below. :/
	//~ spi->mr |= AVR32_SPI_MR_PCS_MASK;
	//~ spi->mr &= ~(1 << (AVR32_SPI_MR_PCS_OFFSET + cs));
}

int spiChipDeselect(volatile avr32_spi_t * spi)
{
	 int timeout = SPI_TIMEOUTVALUE;

	while(!(spi->sr & AVR32_SPI_SR_TXEMPTY_MASK)) {
		if (!timeout--) {
			return SPI_TIMEOUT;
		}
	}
	spi->mr |= AVR32_SPI_MR_PCS_MASK;
	spi->cr = AVR32_SPI_CR_LASTXFER_MASK;
	return SPI_OK;
}

int spiWriteByte(volatile avr32_spi_t * spi, const unsigned int byte)
{
	int timeout = SPI_TIMEOUTVALUE;

	//Is the transmit data register empty?
	while(!(spi->sr & AVR32_SPI_SR_TDRE_MASK)) {
		if(!timeout--) {
			return SPI_TIMEOUT;
		}
	}
	//Ok, previous byte have been sent. Send new byte.
	spi->tdr = byte;
	return SPI_OK;
}

int spiWriteLastByte(volatile avr32_spi_t * spi, const int byte)
{
	//Set the LASTXFER bit to deassert current NPCS after last transfer
	spi->cr = (1 << AVR32_SPI_LASTXFER_OFFSET);
	return spiWriteByte(spi, byte);
}

int spiReadByte(volatile avr32_spi_t * spi, unsigned int * byte)
{
	int timeout = SPI_TIMEOUTVALUE;

	while((spi->sr & (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) != (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) {
		if(!timeout--) {
			return SPI_TIMEOUT;
		}
	}
	*byte = spi->rdr & 0x000000ff;	//TODO: Constant
	return SPI_OK;
}

__attribute__((always_inline))
int spiReadSR(volatile avr32_spi_t * spi)
{
	return spi->sr;
}