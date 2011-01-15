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

#include "drivers/delay.h"

// TODO: This should not be used. Because it is ugly.
// The following code produce a inner loop with six assembler instructions,
// corresponding to a 11 cycle loop, Which give approx. FCPU / 1000 / 11 cycles per ms.
// Accuracy is low. We dont care about the fractions.
#define CYCLES_PER_MS (FCPU / 1000 / 11)
void delay_ms(unsigned int milliseconds)
{
	unsigned long cycles = (milliseconds * CYCLES_PER_MS);

	/* Busy-wait a number of clock cycles */
	while(cycles != 0) {
		cycles--;
	}
}


void delay_us(unsigned int us)
{
	unsigned long cycles = (us * CYCLES_PER_MS / 1000);

	/* Busy-wait a number of clock cycles */
	while(cycles != 0) {
		cycles--;
	}
}

__attribute__((always_inline))
unsigned int sysreg_read(unsigned int sysreg)
{
	unsigned int r12;
	asm("mfsr %0,264" : "=r"(r12) );
	return r12;
}

#define AVR32_SYSREG_COUNT 66
void delay_ns(unsigned long ck)
{
  // Use the CPU cycle counter (CPU and HSB clocks are the same).
  unsigned long delay_start_cycle = (unsigned long)sysreg_read(AVR32_SYSREG_COUNT);
  unsigned long delay_end_cycle = delay_start_cycle + ck;

  // To be safer, the end of wait is based on an inequality test, so CPU cycle
  // counter wrap around is checked.
  if (delay_start_cycle > delay_end_cycle)
  {
    while ((unsigned long)sysreg_read(AVR32_SYSREG_COUNT) > delay_end_cycle);
  }
  while ((unsigned long)sysreg_read(AVR32_SYSREG_COUNT) < delay_end_cycle);
}
