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

#ifndef _EMBLOD_INT_H_
#define _EMBLOD_INT_H_

#include <avr32/io.h>

/***********************************************************
 * System interrupt handling functions
 ***********************************************************/
//Disable interrupts
extern void interruptDisable(void);

//Enable interrupts
extern void interruptEnable(void);

//Check if interrupts are enabled.
extern int isInterruptEnabled(void);

//Disable interrupt if interruptOnOff is TRUE
void interruptDisableIf(int interruptOnOff);

//Enable interrupt if interruptOnOff is TRUE
void interruptEnableIf(int interruptOnOff);

//Example of use:
//	...
//	int onOff = isInterruptEnabled()
//	interruptDisableIf(onOff)
//	...
//	interruptEnableIf(onOff);
//	....


#endif /* _EMBLOD_INT_H_ */
