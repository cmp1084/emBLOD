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

#ifndef _EMBLOD_CONFIG_H_
#define _EMBLOD_CONFIG_H_

#include <avr32/io.h>
#include "drivers/debug.h"

/***********************************************************
 * Boot parameters
 *
 * When bootloading from SD-card, this file will contain
 * the boot parameters.
 * Legal parameters are:
 * bootfile [ = name]
 * bootdelay [ = seconds]
 * fcpu [ = Hz]
 * fpba [ = Hz]
 * loadaddr [ = memory address to load the binary to]
 * bootaddr [ = memory address to start execute the binary from] *
 *
 *
 * If the bootfile can not be found on the
 * SD-card a default file is loaded.
 *
 * Note:
 * All filenames must be 8.3 format.
 *
 ***********************************************************/
#define DEFAULT_BOOTPARAMETER_FILE "bootparm.txt"
#define DEFAULT_AUTORUN_FILE "autorun.bin"
#define DEFAULT_AUTOBANNER_FILE "banner.txt"
#define DEFAULT_BAUDRATE 115200
#define DEFAULT_BOOTDELAY 0
#define DEFAULT_FCPU 60000000
#define DEFAULT_FPBA 15000000
#define DEFAULT_LOADADDR 0xd0000000
#define DEFAULT_BOOTADDR 0xd0000000


/***********************************************************
 * "Nice-to-have" defines
 **********************************************************/
#define YES 1
#define NO 0

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

//TODO: Reconsider OK and ERROR
//~ enum { OK, ERROR }
#ifndef OK
#define OK TRUE
#endif
#ifndef ERROR
#define ERROR FALSE
#endif

#ifndef ON
#define ON 1
#endif
#ifndef OFF
#define OFF 0
#endif


/***********************************************************
 * Debug output configures
 **********************************************************/
//Which debug level will output from the various dgbXYZ() functions be?
//Valid values are DEBUGLEVEL0-DEBUGLEVEL3
#define DEBUGLEVEL DEBUGLEVEL1

//Allow serial output through usartWriteLine() etc.
//Set to NO to get silent boot.
#define SERIAL_OUTPUT YES

/***********************************************************
 * Push Button GPIO configures
 **********************************************************/
#define CONFIG_BUTTON YES

#if(CONFIG_BUTTON)
#if(EVK1100)
#define PUSH_BUTTON_0   AVR32_PIN_PX16
#define PUSH_BUTTON_1   AVR32_PIN_PX19
#define PUSH_BUTTON_2   AVR32_PIN_PX22
#endif /* EVK1100 */

#if(MIZAR32)
#define PUSH_BUTTON_0   AVR32_PIN_PX16
#endif /* MIZAR32 */
#endif /* CONFIG_BUTTON */

/***********************************************************
 * Joystick GPIO configures
 **********************************************************/
#define CONFIG_JOYSTICK NO	//To configure joystick CONFIG_BUTTON must be YES too

#if(CONFIG_JOYSTICK)
#define JOYSTICK_PRESS    AVR32_PIN_PA20
#define JOYSTICK_LEFT     AVR32_PIN_PA25
#define JOYSTICK_RIGHT    AVR32_PIN_PA28
#define JOYSTICK_UP       AVR32_PIN_PA26
#define JOYSTICK_DOWN     AVR32_PIN_PA27
#endif /* CONFIG_JOYSTICK */

/***********************************************************
 * LED GPIO configures
 **********************************************************/
#define CONFIG_LED YES

#ifdef MIZAR32
#define LEDMAX 1
#define LED0_GPIO AVR32_PIN_PB29
#endif /* MIZAR32 */

#ifdef EVK1100
#if(CONFIG_LED)
//The number of LEDs to control
//EVK1100
#define LEDMAX 8                    //The number of LEDs on the EVK1100
#define LED0_GPIO   AVR32_PIN_PB27  //LED1 on the EVK1100
#define LED1_GPIO   AVR32_PIN_PB28  //LED2
#define LED2_GPIO   AVR32_PIN_PB29  //...and so on
#define LED3_GPIO   AVR32_PIN_PB30
#define LED4_GPIO   AVR32_PIN_PB19
#define LED5_GPIO   AVR32_PIN_PB20
#define LED6_GPIO   AVR32_PIN_PB21
#define LED7_GPIO   AVR32_PIN_PB22
#endif /* CONFIG_LED */
#endif /* EVK1100 */

/***********************************************************
 * POT configures
 **********************************************************/
#define CONFIG_POT NO

/***********************************************************
 * Timer configures
 **********************************************************/
#define FOSC0 12000000    //Osc0 clock frequencey
#define FPBA  15000000    //PBA frequency
#define FCPU  60000000    //CPU/HSB Frequency

#define CONFIG_TIMER NO

#if(CONFIG_TIMER)
#define PERIODTIME_MS 1
#endif

/***********************************************************
 * USART configures
 **********************************************************/
#define CONFIG_USART0 YES
#define CONFIG_USART1 YES
#define CONFIG_USART2 NO
#define CONFIG_USART3 NO

//~ #if(CONFIG_USART0)
#define USART0              (&AVR32_USART0)
#define USART0_BAUDRATE     115200	//57600
#define USART0_CHARLEN      8
#define USART0_RX_PIN       AVR32_USART0_RXD_0_0_PIN
#define USART0_RX_FUNCTION  AVR32_USART0_RXD_0_0_FUNCTION
#define USART0_TX_PIN       AVR32_USART0_TXD_0_0_PIN
#define USART0_TX_FUNCTION  AVR32_USART0_TXD_0_0_FUNCTION
//~ #endif

//~ #if(CONFIG_USART1)
#define USART1              (&AVR32_USART1)
#define USART1_BAUDRATE     115200	//57600
#define USART1_CHARLEN      8
#define USART1_RX_PIN       AVR32_USART1_RXD_0_0_PIN
#define USART1_RX_FUNCTION  AVR32_USART1_RXD_0_0_FUNCTION
#define USART1_TX_PIN       AVR32_USART1_TXD_0_0_PIN
#define USART1_TX_FUNCTION  AVR32_USART1_TXD_0_0_FUNCTION
//~ #endif

/***********************************************************
 * Task default priority configures
 *
 * Comment:
 * Low value give low priority
 * High value give high priority
 **********************************************************/

#define IDLEPRIORITY 0
#define DEFAULTLOWPRIO 1
#define TASKINITIALPRIORITY 1
#define DEFAULTRELEASETIME 0

/*********************************************************
 * SPI configurations (beware, these are partly moved to at45db.c, dip204 etc
 *********************************************************/
//TODO: Move some of the defines to drivers/spi.h
#define CONFIG_SPI0 NO
#define CONFIG_SPI0_MISO_PIN AVR32_SPI0_MISO_0_0_PIN
#define CONFIG_SPI0_MOSI_PIN AVR32_SPI0_MOSI_0_0_PIN
#define CONFIG_SPI0_SCK_PIN AVR32_SPI0_SCK_0_0_PIN
#define CONFIG_SPI0_CS0_PIN AVR32_SPI0_NPCS_0_0_PIN
#define CONFIG_SPI0_CS1_PIN AVR32_SPI0_NPCS_1_0_PIN
#define CONFIG_SPI0_CS2_PIN AVR32_SPI0_NPCS_2_0_PIN
#define CONFIG_SPI0_CS3_PIN AVR32_SPI0_NPCS_3_PIN

#define CONFIG_SPI0_MISO_FUNCTION AVR32_SPI0_MISO_0_0_FUNCTION
#define CONFIG_SPI0_MOSI_FUNCTION AVR32_SPI0_MOSI_0_0_FUNCTION
#define CONFIG_SPI0_SCK_FUNCTION AVR32_SPI0_SCK_0_0_FUNCTION
#define CONFIG_SPI0_CS0_FUNCTION AVR32_SPI0_NPCS_0_0_FUNCTION
#define CONFIG_SPI0_CS1_FUNCTION AVR32_SPI0_NPCS_1_0_FUNCTION
#define CONFIG_SPI0_CS2_FUNCTION AVR32_SPI0_NPCS_2_0_FUNCTION
#define CONFIG_SPI0_CS3_FUNCTION AVR32_SPI0_NPCS_3_FUNCTION


#define CONFIG_SPI1 YES
#define CONFIG_SPI1_MISO_PIN AVR32_SPI1_MISO_0_0_PIN
#define CONFIG_SPI1_MOSI_PIN AVR32_SPI1_MOSI_0_0_PIN
#define CONFIG_SPI1_SCK_PIN AVR32_SPI1_SCK_0_0_PIN
#define CONFIG_SPI1_CS0_PIN AVR32_SPI1_NPCS_0_0_PIN
#define CONFIG_SPI1_CS1_PIN AVR32_SPI1_NPCS_1_0_PIN
#define CONFIG_SPI1_CS2_PIN AVR32_SPI1_NPCS_2_0_PIN
#define CONFIG_SPI1_CS3_PIN AVR32_SPI1_NPCS_3_PIN

#define CONFIG_SPI1_MISO_FUNCTION AVR32_SPI1_MISO_0_0_FUNCTION
#define CONFIG_SPI1_MOSI_FUNCTION AVR32_SPI1_MOSI_0_0_FUNCTION
#define CONFIG_SPI1_SCK_FUNCTION AVR32_SPI1_SCK_0_0_FUNCTION
#define CONFIG_SPI1_CS0_FUNCTION AVR32_SPI1_NPCS_0_0_FUNCTION
#define CONFIG_SPI1_CS1_FUNCTION AVR32_SPI1_NPCS_1_0_FUNCTION
#define CONFIG_SPI1_CS2_FUNCTION AVR32_SPI1_NPCS_2_0_FUNCTION
#define CONFIG_SPI1_CS3_FUNCTION AVR32_SPI1_NPCS_3_FUNCTION


//SPI timeout defaults to 10000 (see drivers/spi.c) if not configured here
#define CONFIG_SPI_TIMEOUTVALUE 10000

#endif /* _EMBLOD_CONFIG_H_ */
