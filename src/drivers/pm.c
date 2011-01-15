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
#include "drivers/pm.h"

unsigned int get_hsb_clk_rate(void)
{
	return FCPU;
}

void pmPbaMaskSet(unsigned int mask)
{
	volatile avr32_pm_t * pm = &AVR32_PM;
	pm->pbamask = mask;
}

unsigned int pmPbaMaskGet(void)
{
	volatile avr32_pm_t * pm = &AVR32_PM;
	return pm->pbamask;
}

void pmPbbMaskSet(unsigned int mask)
{
	volatile avr32_pm_t * pm = &AVR32_PM;
	pm->pbbmask = mask;
}

unsigned int pmPbbMaskGet(void)
{
	volatile avr32_pm_t * pm = &AVR32_PM;
	return pm->pbbmask;
}

// Supported frequencies:
// Fosc0 mul div PLL div2_en cpu_f pba_f   Comment
//  12   15   1  192     1     12    12
//  12    9   3   40     1     20    20    PLL out of spec
//  12   15   1  192     1     24    12
//  12    9   1  120     1     30    15
//  12    9   3   40     0     40    20    PLL out of spec
//  12   15   1  192     1     48    12
//  12   15   1  192     1     48    24
//  12    8   1  108     1     54    27
//  12    9   1  120     1     60    15   <--- We set up PLL0 according to this, mjan 20101228
//  12    9   1  120     1     60    30
//  12   10   1  132     1     66    16.5
void pmClkInit(unsigned int fcpu, unsigned int fpba)
{
	volatile avr32_pm_t * pm = &AVR32_PM;
	volatile avr32_flashc_t * flashc = &AVR32_FLASHC;

	//Kill the watchdog
	wdtDisable();

	//Set up PLL0
	pm->pll[0] = (0x01 << AVR32_PM_PLLEN_OFFSET)
				|(0x01 << AVR32_PM_PLLDIV_OFFSET)
				|((0x0a - 0x01) << AVR32_PM_PLLMUL_OFFSET)
				|(0x02 << AVR32_PM_PLL0_PLLOPT_OFFSET)
				|(0x10 << AVR32_PM_PLLCOUNT_OFFSET)
				|(0x01 << 0x07);	//Errata rev. E, we should not need it for the rev. K chip.

	//Wait for lock and stabilisation
	while(!(pm->poscsr) & (1 << AVR32_PM_LOCK0_OFFSET));
	while(!(pm->poscsr) & (1 << AVR32_PM_CKRDY_OFFSET));

	//Set up clock for all peripheral busses and CPU
	pm->cksel =  (1 << AVR32_PM_CKSEL_PBADIV_OFFSET)
				|(1 << AVR32_PM_CKSEL_PBASEL_OFFSET);

	//Set flash WaitState to 1 cycle
	flashc->fcr |= 1 << AVR32_FLASHC_FCR_FWS_OFFSET;

	//Switch to PLL0
	pm->mcctrl = (AVR32_PM_MCCTRL_MCSEL_PLL0 << AVR32_PM_MCCTRL_MCSEL_OFFSET)
				|(1 << AVR32_PM_MCCTRL_OSC0EN_OFFSET);
}
