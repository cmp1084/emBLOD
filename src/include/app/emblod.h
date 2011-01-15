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
#include <avr32/io.h>
#include <string.h>
#include "emblod_config.h"
#include "drivers/delay.h"
#include "drivers/filesystem/fat/diskio.h"
#include "drivers/sdhc.h"
#include "drivers/sdramc.h"
#include "drivers/usart.h"
#include "ff.h"

#define BUFMAXLEN 128
#define EOF -1
#define MAXSTRLEN 0x80

/********************************************************
 * bootparam_t
 *
 * Comment:
 * These are the possible boot parameters
 ********************************************************/
typedef struct {
	unsigned int baudrate;
	unsigned int bootdelay;
	unsigned int fcpu;
	unsigned int fpba;
	unsigned int loadaddr;
	unsigned int bootaddr;
	char bootfile[MAXSTRLEN];
	char banner[MAXSTRLEN];
} bootparam_t;

/********************************************************
 * emblod
 *
 * Comment:
 * The bootloader. Boot a file from SD(HC) card
 ********************************************************/
void emblod(bootparam_t * bootparam);
void banner(bootparam_t * bootparam);
int boot(bootparam_t * bootparam);

/********************************************************
 * bootparamLoad
 *
 * Comment:
 * Function to load bootloader parameters from SD card.
 ********************************************************/
bootparam_t bootparamLoad(char * filename);

