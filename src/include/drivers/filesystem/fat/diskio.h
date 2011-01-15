/*----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.08a                    (C)ChaN, 2010
/-----------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following trems.
/
/  Copyright (C) 2010, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-----------------------------------------------------------------------------/
/-----------------------------------------------------------------------------
/  Low level disk interface
/----------------------------------------------------------------------------*/

#ifndef _DISKIO_H_
#define _DISKIO_H_

#include <avr32/io.h>
#include "emblod_config.h"
#include "drivers/filesystem/fat/integer.h"

//TODO: This should perhaps be moved to xyz_config.h
#define SD_SPI                (&AVR32_SPI1)
#ifdef MIZAR32
	#define SD_CS             CS0
	#define SD_CS_PIN         CONFIG_SPI1_CS0_PIN
	#define SD_CS_FUNCTION    CONFIG_SPI1_CS0_FUNCTION
	//#warning Mizar32 is setup to use CS0. This is good.
#endif /* MIZAR32 */

#ifdef EVK1100
	#define SD_CS             CS1
	#define SD_CS_PIN         CONFIG_SPI1_CS1_PIN
	#define SD_CS_FUNCTION    CONFIG_SPI1_CS1_FUNCTION
	//#warning EVK1100 is setup to use CS1. This is good.
#endif /* EVK1100 */

#define DI_HIGH               0xff

#define _READONLY	0	/* 1: Remove write functions */
#define _USE_IOCTL	0	/* 1: Use disk_ioctl fucntion */

/* Status of Disk Functions */
typedef BYTE DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

int assign_drives (int, int);
DSTATUS disk_initialize (BYTE);
DSTATUS disk_status (BYTE);
DRESULT disk_read (BYTE Drive, BYTE* Buffer,  DWORD SectorNumber, BYTE SectorCount);
//~ DRESULT disk_read (BYTE, BYTE*, DWORD, BYTE);
#if	_READONLY == 0
DRESULT disk_write (BYTE, const BYTE*, DWORD, BYTE);
#endif
DRESULT disk_ioctl (BYTE, BYTE, void*);



/* Disk Status Bits (DSTATUS) */

#define STA_INITED		0x00	/* Drive have been initialized */
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (defined for FatFs) */
#define CTRL_SYNC			0	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	1	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR	4	/* Force erased a block of sectors (for only _USE_ERASE) */

/* Generic command */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

/* NAND specific ioctl command */
#define NAND_FORMAT			30	/* Create physical format */

#endif /* _DISKIO_H_ */
