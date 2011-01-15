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
/* Tab size: 4 */

#ifndef _EMBLOD_LED_H_
#define _EMBLOD_LED_H_

#include <avr32/io.h>
#include "drivers/gpio.h"
#include "emblod_config.h"

/***********************************************************
 * LED API
 *
 * Comment:
 * 	Various functions for LED control
 *
 * valid 'lednr' parameters are:
 * LED0_GPIO	//Monocolor green, EVK1100 LED1
 * LED1_GPIO	//Monocolor green, EVK1100 LED2
 * LED2_GPIO	//Monocolor green, EVK1100 LED3
 * LED3_GPIO	//Monocolor green, EVK1100 LED4
 * LED4_GPIO	//Bicolor red, EVK1100 LED5
 * LED5_GPIO	//Bicolor green, EVK1100 LED5
 * LED6_GPIO	//Bicolor red, EVK1100 LED6
 * LED7_GPIO	//Bicolor green, EVK1100 LED6
 *
 **********************************************************/
//Initialize all LEDs, LEDx_GPIO
void ledInit(void);

//Turn a LED on
void ledOn(int lednr);

//Turn a LED off
void ledOff(int lednr);

//Toggle a LED
void ledToggle(int lednr);

#endif /* _EMBLOD_LED_H_ */
