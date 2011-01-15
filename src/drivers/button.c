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

#include "drivers/button.h"
#include "drivers/gpio.h"


#if (CONFIG_BUTTON)


/**********************************************************
 * buttonInit
 *
 * Comment:
 * 	Initialize all three EVK1100 buttons.
 *
 * Returns:
 * 	TRUE or FALSE
 *
 *********************************************************/
#ifdef EVK1100
void buttonInit(void)
{
	gpioInit(PUSH_BUTTON_0, INPUT);
	gpioInit(PUSH_BUTTON_1, INPUT);
	gpioInit(PUSH_BUTTON_2, INPUT);
}
#endif /* EVK1100 */

#ifdef MIZAR32
void buttonInit(void)
{
	gpioInit(PUSH_BUTTON_0, INPUT);
}
#endif /* MIZAR32 */

/**********************************************************
 * isButton
 *
 * Comment:
 * 	Check if a button is pressed or not.
 *
 * Returns:
 * 	TRUE or FALSE
 *
 *********************************************************/
int isButton(int nr)
{
	return !gpioGet(nr);
}

#if(CONFIG_JOYSTICK && CONFIG_BUTTON)
/**********************************************************
 * initJoystick
 *
 * Comment:
 * 	Initialize the GPIOs connected to the joystick on
 * 	the EVK1100.
 *
 * Returns:
 * 	Nothing
 *
 *********************************************************/
void joystickInit(void)
{
	gpioInit(JOYSTICK_DOWN, INPUT);
	gpioInit(JOYSTICK_LEFT, INPUT);
	gpioInit(JOYSTICK_RIGHT, INPUT);
	gpioInit(JOYSTICK_PRESS, INPUT);
	gpioInit(JOYSTICK_UP, INPUT);
}


/**********************************************************
 * isJoyStickXYZ()
 *
 * Returns:
 * 	TRUE or FALSE depending on the joysticks direction.
 *
 *********************************************************/
int isJoystickUp(void)
{
	return isButton(JOYSTICK_UP);
}

int isJoystickDown(void)
{
	return isButton(JOYSTICK_DOWN);
}

int isJoystickLeft(void)
{
	return isButton(JOYSTICK_LEFT);
}

int isJoystickRight(void)
{
	return isButton(JOYSTICK_RIGHT);
}

int isJoystickPressed(void)
{
	return isButton(JOYSTICK_PRESS);
}

#endif /* CONFIG_JOYSTICK */

#endif /* CONFIG_BUTTON */
