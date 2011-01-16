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

#include <string.h>

#include "drivers/filesystem/fat/diskio.h"
#include "drivers/filesystem/fat/ffconf.h"

#include "drivers/spi.h"
#include "drivers/delay.h"
#include "drivers/sdhc.h"

__attribute__((section(".data")))
DSTATUS drive_status[_VOLUMES] = { STA_NOINIT };

DSTATUS disk_initialize(BYTE Drive)
{
	int i;
	spi_cfg_t spiConfigStruct = {
		//MR
		1,        //mstr	//TODO: This will always (? at least in the case of SD-card, LCD display, dataflash, etc.) be master. Optimize away!
		0,        //fdiv
		1,        //modfdis
		SD_CS,    //cs

		//CSRx
		0,        //cpol	//Set 0 to get SPI mode 0
		1,        //ncpha	//Set 1 to get SPI mode 0
		1,        //csaat
		8,        //bits

		400000 //spck, initial SD-card SPI speed 100-400 kHz
	};

	const spi_pin_cfg_t spiPinConfigStruct = {
		SD_CS_PIN,
		SD_CS_FUNCTION
	};

	spiSetup(SD_SPI, &spiConfigStruct, &spiPinConfigStruct);
	spiSetMode(SD_SPI, &spiConfigStruct);
	spiEnable(SD_SPI);

	i = sdhcInit();
	drive_status[Drive] = i;
	//spiConfigStruct.spck = FPBA;	//Final SD-card SPI speed	//TODO: Get actual speed from card
	//spiSetup(SD_SPI, &spiConfigStruct, &spiPinConfigStruct);
	//spiSetMode(SD_SPI, &spiConfigStruct);
	//spiEnable(SD_SPI);				//TODO: Remove when tested.
	return 0;
}

/**
 * Check the disk status
 *
 * Parameters:
 * Drive = Physical drive number
 *
 * Output:
 * STA_NOINIT
 * STA_NODISK
 * STA_PROTECTED
 * depending on the status of the disk Drive
 *
 */
//TODO:
DSTATUS disk_status(BYTE Drive)
{
	return drive_status[Drive];
}

/**
 * disk_read
 *
 * Parameters:
 *
 * Drive
 *  Specifies the physical drive number.
 *
 * Buffer
 *     Pointer to the byte array to store the read data. The buffer size of number of bytes to be read, sector size * sector count, is required. Note that the specified memory address is not that always aligned to word boundary. If the hardware does not support misaligned data transfer, it must be solved in this function.
 *
 * SectorNumber
 *     Specifies the start sector number in logical block address (LBA).
 *
 * SectorCount
 *     Specifies number of sectors to read. The value can be 1 to 128. Generally, a multiple sector transfer request must not be split into single sector transactions to the device, or you may not get good read performance.
 *
 *  Return Value:
 *
 * RES_OK (0)
 *     The function succeeded.
 * RES_ERROR
 *     Any hard error occured during the read operation and could not recover it.
 * RES_PARERR
 *     Invalid parameter.
 * RES_NOTRDY
 *     The disk drive has not been initialized.
 */
DRESULT disk_read (BYTE Drive, BYTE* Buffer,  DWORD SectorNumber, BYTE SectorCount)
{
	int ret = 0;
	if(!SectorCount) {
		return RES_ERROR;
	}

	while(SectorCount--) {
		while((ret = sdhcReadSector(SectorNumber)));	//Todo: Retries.
		SectorNumber++;
		memcpy(Buffer, sdhcSectorBuf, sizeof(char) * 512);
		Buffer += sizeof(char) * 512;
	}

	switch(ret) {
		case 0:
			return RES_OK;
		default:
			return RES_ERROR;
	}
	return RES_OK;	//TODO:
}

/**
 * disk_write
 *
 * Parameters:
 *
 * Drive
 *    Specifies the physical drive number.
 *
 * Buffer
 *    Pointer to the byte array to be written. Note that the specified memory address is not that always aligned to word boundary. If the hardware does not support misaligned data transfer, it must be solved in this function.
 *
 * SectorNumber
 *    Specifies the start sector number in logical block address (LBA).
 *
 * SectorCount
 *    Specifies the number of sectors to write. The value can be 1 to 128. Generally, a multiple sector transfer request must not be split into single sector transactions to the device, or you will never get good write performance.
 *
 * Return Values
 *
 * RES_OK (0)
 *    The function succeeded.
 * RES_ERROR
 *    Any hard error occured during the write operation and could not recover it.
 * RES_WRPRT
 *    The medium is write protected.
 * RES_PARERR
 *    Invalid parameter.
 * RES_NOTRDY
 *    The disk drive has not been initialized.
 */
DRESULT disk_write (
  BYTE Drive,          /* Physical drive number */
  const BYTE * Buffer,  /* Pointer to the write data (may be non aligned) */
  DWORD SectorNumber,  /* Sector number to write */
  BYTE SectorCount     /* Number of sectors to write */
)
{
	int ret = 0;
	BYTE * sector = (BYTE *)Buffer;

	if(!SectorCount) {
		return RES_ERROR;
	}

	while(SectorCount--) {
		memcpy(sdhcSectorBuf, sector, sizeof(char) * 512);
		ret = sdhcWriteSector(SectorNumber++);	//Todo: Retries.
		sector += sizeof(BYTE) * 512;
	}

	switch(ret) {
		case 0:
			return RES_OK;
		default:
			return RES_ERROR;
	}
}

/**
 * disk_ioctrl
 *
 * Parameters
 *
 * Drive
 *    Specifies the drive number (0-9).
 * Command
 *    Specifies the command code.
 * Buffer
 *    Pointer to the parameter buffer depends on the command code. When it is not used, specify a NULL pointer.
 *
 * Return Value
 *
 * RES_OK (0)
 *    The function succeeded.
 * RES_ERROR
 *    Any error occured.
 * RES_PARERR
 *    Invalid command code.
 * RES_NOTRDY
 *    The disk drive has not been initialized.
 */
DRESULT disk_ioctl (
  BYTE Drive,      /* Drive number */
  BYTE Command,    /* Control command code */
  void* Buffer     /* Parameter and data buffer */
)
{
	switch(Command) {
		case CTRL_SYNC:
			return RES_OK;	//TODO:
			break;
		case GET_SECTOR_SIZE:

			break;
		case GET_SECTOR_COUNT:

			break;
		case GET_BLOCK_SIZE:

			break;
		case CTRL_ERASE_SECTOR:

			break;
		default:
			return RES_ERROR;
	}
	return RES_OK;
}

DWORD get_fattime (void)
{
	return 0;
}
