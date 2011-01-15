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

#ifndef __EMBLOD_SPI_H_
#define __EMBLOD_SPI_H_

#include <avr32/io.h>
#include "drivers/gpio.h"
#include "drivers/pm.h"
#include "emblod_config.h"

/**
 * In order to start the SPI the following functions should be called
 * (in this order):
 *
 * cs -          the chip select, 0, 1, 2 or 3
 * cs_pin -      the pin which the cs uses
 * cs_function - the multiplexed peripherial that correspond to the cs_pin
 * spck -        the SPI clock rate, expressed in Hz.
 *
 * spiSetup(spi, cs, cs_pin, cs_function);
 * spiSetMode(spi, cs);
 * spiSetSpck(spi, cs, spck);
 * spiEnable();
 *
 */

//!SPI Status codes
enum {
		SPI_TIMEOUT,
		SPI_ERROR,
		SPI_OK
};

//!SPI Chip select
enum {
		CS0 = 0,
		CS1,
		CS2,
		CS3
};


typedef struct {
	//MR bits
	int mstr;    //spi mode                             { MASTER = 1, SLAVE = 0 }
	int fdiv;    //mck/2 divisor                        { YES = 1, NO = 0 }
	int modfdis; //mode fault detect                    { YES = 1, NO = 0 }
	int cs;      //Chip select (peripheria 	l chip select) { CS0, CS1, CS2, CS3 }

	//CSRx bits
	int cpol;   //polarity
	int ncpha;  //phase
	int csaat;  //chip select active after transfer
	int bits;   //databits { 8, 9, 10, 11, 12, 13, 14, 15, 16 }
	unsigned int spck;   //Spi clock rate [Hz]
} spi_cfg_t;

typedef struct {
	//GPIO pin settings
	int cs_pin;
	int cs_function;
} spi_pin_cfg_t;

void spiReset(volatile avr32_spi_t * spi);
void spiSetup(volatile avr32_spi_t * spi, const spi_cfg_t * spiConfigStruct, const spi_pin_cfg_t * spiPinConfigStruct);
void spiSetMode(volatile avr32_spi_t * spi, const spi_cfg_t * spiConfigStruct);
//~ void spiSetSpck(volatile avr32_spi_t * spi, const spi_cfg_t * spiConfigStruct);
void spiEnable(volatile avr32_spi_t * spi);

void spiLLBSet(volatile avr32_spi_t * spi, const int onoff);
void spiChipSelect(volatile avr32_spi_t * spi, const int cs);
int spiChipDeselect(volatile avr32_spi_t * spi);
int spiReadByte(volatile avr32_spi_t * spi, unsigned int * byte);
int spiWriteByte(volatile avr32_spi_t * spi, const unsigned int byte);
int spiWriteLastByte(volatile avr32_spi_t * spi, const int byte);
int spiReadSR(volatile avr32_spi_t * spi);


#endif /* __EMBLOD_SPI_H_ */