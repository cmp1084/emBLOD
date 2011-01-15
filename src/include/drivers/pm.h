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

#ifndef _EMBLOD_PM_H_
#define _EMBLOD_PM_H_

#include <avr32/io.h>
#include "drivers/wdt.h"
#include "emblod_config.h"

enum {
	PM_CPUMASK_OCD_OFFSET = 1
};

enum {
	PM_PBAMASK_INTC_OFFSET = 0,
	PM_PBAMASK_GPIO_OFFSET,
	PM_PBAMASK_PDCA_OFFSET,
	PM_PBAMASK_PM_RTC_EIC_OFFSET,
	PM_PBAMASK_ADC_OFFSET,
	PM_PBAMASK_SPI0_OFFSET,
	PM_PBAMASK_SPI1_OFFSET,
	PM_PBAMASK_TWI_OFFSET,
	PM_PBAMASK_USART0_OFFSET,
	PM_PBAMASK_USART1_OFFSET,
	PM_PBAMASK_PWM_OFFSET,
	PM_PBAMASK_SSC_OFFSET
};

enum {
	PM_PBBMASK_HMATRIX_OFFSET = 0,
	PM_PBBMASK_USBB_OFFSET,
	PM_PBBMASK_FLASHC_OFFSET,
	PM_PBBMASK_MACB_OFFSET,
	PM_PBBMASK_SMC_OFFSET,
	PM_PBBMASK_SDRAMC_OFFSET,
};


unsigned int get_hsb_clk_rate(void);
//~ void pmCpuMaskSet(unsigned int mask);
//~ unsigned int pmCpuMaskGet(void);
void         pmHsbMaskSet(unsigned int mask);
unsigned int pmHsbMaskGet(void);
void         pmPbaMaskSet(unsigned int mask);
unsigned int pmPbaMaskGet(void);
void         pmPbbMaskSet(unsigned int mask);
unsigned int pmPbbMaskGet(void);
void         pmClkInit(unsigned int fcpu, unsigned int fpba);

#endif /* _EMBLOD_PM_H_ */