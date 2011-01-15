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

#ifndef __EMBLOD_SDHC_H__
#define __EMBLOD_SDHC_H__

#include <avr32/io.h>
#include "drivers/filesystem/fat/diskio.h"
#include "drivers/spi.h"
#include "drivers/led.h"

extern unsigned char sdhcBuf[512+16];
extern unsigned char * sdhcSectorBuf;

unsigned char sdhcInit(void);
unsigned char sdhcCardInfo(unsigned char which);
unsigned int  sdhcCardSize(void);
unsigned char sdhcReadSector(unsigned int blkaddr);
unsigned char sdhcWriteSector(unsigned int blkaddr);

#endif /* __EMBLOD_SDHC_H_ */