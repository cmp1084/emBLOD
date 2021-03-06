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

#ifndef __GPIO_H_
#define __GPIO_H_

#include <avr32/io.h>

//PeripherialEnable function
enum { FUNC_GPIO = -1, FUNC_A = 0, FUNC_B, FUNC_C, FUNC_D };
enum { OUTPUT, INPUT };


/***********************************************************
 * GPIO API
 *
 * Comment:
 * 	Various functions for GPIO control
 *
 **********************************************************/
void gpioPeripheralEnable(int pin, int function);
void gpioPeripheralEnableMultiPin(void * gpioport, int pin, int function);

//Initialize a specific GPIO pin
void gpioInit(int pinnr, int type);

//Set the value of a GPIO pin
void gpioSet(int pinnr);

//Clear the value of a GPIO pin
void gpioClear(int pinnr);

//Toggle the value of a GPIO pin
void gpioToggle(int pinnr);

//Get the value from a GPIO pin
int gpioGet(int pinnr);

#endif /* __GPIO_H_ */
