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

//Standard library includes
#include <avr32/io.h>

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"
#include "drivers/button.h"
#include "drivers/spi.h"
#include "drivers/sdramc.h"

//Include configuration
#include "emblod_config.h"

//Include application
#include "app/emblod.h"

#define VERSION "0.1.4"
const char version[] = "\n\nm32BLOD " VERSION " (" __DATE__ ")\n";

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	int err;
	bootparam_t bootparam;

	//Initial initialisation
	pmClkInit(FCPU, FPBA);
	usartInit(USART0, &usart0_options, FPBA);    //Serial communication
	ledInit();

	//Load boot parameters from SD-card
	bootparam = bootparamLoad("bootparm.txt");

	//Second init according to boot parameters
	//pmClkReInit(bootparam.fcpu, bootparam.fpba);
	usartInit(USART0, &usart0_options, bootparam.fpba);
	sdramInit();

	usartWriteLine(USART0, version); //TODO ♥
	spiReset(&AVR32_SPI1);

	//Optional loading a banner from SD-card and display on terminal.
	banner(&bootparam);

	//Boot file from SD-card, according to the bootparam.txt that should exist
	//on the SD-card. Otherwise default settings will be used.
	//If the boot is successful we (probably) never come back here.
	//If the boot is unsuccessful we come back here.
	err = boot(&bootparam);

	if(err) {
		//TODO: Booting another file might NOT be the proper way to handle failure to load the user specified file. Reconsider!
		usartWriteLine(USART0, "\nCould not boot ");
		usartWriteLine(USART0, bootparam.bootfile);
		usartWriteLine(USART0, "\nWarning: Trying to boot ");
		usartWriteLine(USART0, DEFAULT_AUTORUN_FILE);

		strcpy(bootparam.bootfile, DEFAULT_AUTORUN_FILE);
		err = boot(&bootparam);
	}

	if(err) {
		/* Execution will only return to here on error */
		usartWriteLine(USART0, "\nError booting, stopping here.");
		while(1); //TODO: Sleep
	}
	while(1); //TODO: Sleep
}
