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

#include "drivers/led.h"
#if(CONFIG_LED)

/**********************************************************
 * ledInit
 *
 * Comment:
 * 	Initialize all EVK1100 LEDs.
 *
 * Returns:
 * 	Nothing
 *
 *********************************************************/

void ledInit(void)
{
	int leds[] = {
		LED0_GPIO,
#ifdef EVK1100
		LED1_GPIO,
		LED2_GPIO,
		LED3_GPIO,
		LED4_GPIO,
		LED5_GPIO,
		LED6_GPIO,
		LED7_GPIO
#endif
	};

	int nr = 0;
	do {
		ledOff(leds[nr]);
		gpioInit(leds[nr], OUTPUT);
	} while(nr++ != LEDMAX - 1);
}


/**********************************************************
 * ledOn
 *
 * Comment:
 * 	Make a LED light up
 *
 * Returns:
 * 	Nothing
 *
 *********************************************************/
#if BOARD == MIZAR32
void ledOn(int lednr) { gpioSet(lednr); }
#endif
#if BOARD == EVK1100
void ledOn(int lednr) { gpioClear(lednr); }
#endif


/**********************************************************
 * ledOff
 *
 * Comment:
 * 	Make a LED light down
 *
 * Returns:
 * 	Nothing
 *
 *********************************************************/
#if BOARD == MIZAR32
void ledOff(int lednr) { gpioClear(lednr); }
#endif
#if BOARD == EVK1100
void ledOff(int lednr) { gpioSet(lednr); }
#endif


/**********************************************************
 * ledToggle
 *
 * Comment:
 * 	Make a LED toggle its light on or off, depending on
 * 	previous state.
 *
 * Returns:
 * 	Nothing
 *
 *********************************************************/
void ledToggle(int lednr) { gpioToggle(lednr); }

#else
//Satisfy ISO C
void nothing(void)
{
}
#endif /* CONFIG_LED */
