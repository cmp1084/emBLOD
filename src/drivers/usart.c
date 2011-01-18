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

#include "drivers/usart.h"

void usartReset(volatile avr32_usart_t *usart)
{
	int interruptEnabled;

	//Disable all USART interrupts.
	interruptEnabled = isInterruptEnabled();
	if(interruptEnabled) {
			interruptDisable();
	}
	usart->idr = 0xFFFFFFFF;
	usart->csr;
	if(interruptEnabled) {
		interruptEnable();
	}

	//Reset mode and other registers that could cause unpredictable behavior after reset.
	usart->mr = 0;
	usart->rtor = 0;
	usart->ttgr = 0;
	usart->cr = AVR32_USART_CR_RSTRX_MASK   |
				AVR32_USART_CR_RSTTX_MASK   |
				AVR32_USART_CR_RSTSTA_MASK  |
				AVR32_USART_CR_RSTIT_MASK   |
				AVR32_USART_CR_RSTNACK_MASK |
	#ifndef AVR32_USART_440_H_INCLUDED
	// Note: Modem Signal Management DTR-DSR-DCD-RI are not included in USART rev.440.
				AVR32_USART_CR_DTRDIS_MASK  |
	#endif
				AVR32_USART_CR_RTSDIS_MASK;
}

/*
 * Calculates the baudrate for the USART
 *
 * Returns:
 *	USART_SUCCESS, baudrate successfully initialized.
 *	USART_INVALID_INPUT, baud rate successfully initialized.
 */
int usartSetBaudrate(volatile avr32_usart_t *usart, unsigned int baudrate, unsigned long pba_hz)
{
	unsigned int over = (pba_hz >= 16 * baudrate) ? 16 : 8;
	unsigned int cd_fp = ((1 << AVR32_USART_BRGR_FP_SIZE) * pba_hz + (over * baudrate) / 2) / (over * baudrate);
	unsigned int cd = cd_fp >> AVR32_USART_BRGR_FP_SIZE;
	unsigned int fp = cd_fp & ((1 << AVR32_USART_BRGR_FP_SIZE) - 1);

	if (cd < 1 || cd > (1 << AVR32_USART_BRGR_CD_SIZE) - 1)
		return USART_INVALID_INPUT;

	usart->mr = (usart->mr & ~(AVR32_USART_MR_USCLKS_MASK |
				 AVR32_USART_MR_SYNC_MASK |
				 AVR32_USART_MR_OVER_MASK)) |
				AVR32_USART_MR_USCLKS_MCK << AVR32_USART_MR_USCLKS_OFFSET |
				((over == 16) ? AVR32_USART_MR_OVER_X16 : AVR32_USART_MR_OVER_X8) << AVR32_USART_MR_OVER_OFFSET;

	usart->brgr = cd << AVR32_USART_BRGR_CD_OFFSET |
				  fp << AVR32_USART_BRGR_FP_OFFSET;

	return USART_SUCCESS;
}

int usartInit(volatile avr32_usart_t * usart, const usart_options_t * opt, const long pba_hz)
{
	volatile avr32_gpio_port_t * gpio;
	gpio = (avr32_gpio_port_t *) &AVR32_GPIO;	//Gives annoying warning, but is correct for our purpose.
	gpio->gperc = 1 << (opt->rx_pin & 0x1f);
	gpio->gperc = 1 << (opt->tx_pin & 0x1f);
	//~ gpio->gperc = 1 << (USART_RX_PIN & 0x1f);
	//~ gpio->gperc = 1 << (USART_TX_PIN & 0x1f);

	usartReset(usart);

	usartSetBaudrate(usart, opt->baudrate, pba_hz);
	if(opt->charlength == 9) {
		//Character length set to 9 bits. MODE9 dominates CHRL.
		usart->mr |= AVR32_USART_MR_MODE9_MASK;
	}
	else {
		//CHRL gives the character length (- 5) when MODE9 = 0.
		usart->mr |= (opt->charlength - 5) << AVR32_USART_MR_CHRL_OFFSET;
	}

	usart->mr |= opt->paritytype << AVR32_USART_MR_PAR_OFFSET |
	             opt->channelmode << AVR32_USART_MR_CHMODE_OFFSET;

	if (opt->stopbits > USART_2_STOPBITS) {
		//Set two stop bits and a timeguard period gives the rest.
		usart->mr |= AVR32_USART_MR_NBSTOP_2 << AVR32_USART_MR_NBSTOP_OFFSET;
		usart->ttgr = opt->stopbits - USART_2_STOPBITS;
	}
	else
		//Insert 1, 1.5 or 2 stop bits.
		usart->mr |= opt->stopbits << AVR32_USART_MR_NBSTOP_OFFSET;

	//Set normal mode.
	usart->mr = (usart->mr & ~AVR32_USART_MR_MODE_MASK) |
			    (AVR32_USART_MR_MODE_NORMAL << AVR32_USART_MR_MODE_OFFSET);

	//~ usart->mr = (usart->mr & ~AVR32_USART_MR_CHMODE_MASK) | (opt->channelmode << AVR32_USART_MR_CHMODE_OFFSET);

	//Setup complete; enable communication.
	//Enable input and output.
	usart->cr = AVR32_USART_CR_RXEN_MASK | AVR32_USART_CR_TXEN_MASK;

	return USART_SUCCESS;
}

void usartWriteChar(volatile avr32_usart_t * usart, const char ch)
{
#if SERIAL_OUTPUT == YES
	int timeout = 0x400;
	while((timeout-- != 0) && ((usart->csr & AVR32_USART_CSR_TXRDY_MASK) == 0));
	usart->thr = ch & AVR32_USART_THR_TXCHR_MASK;
#endif
}

void usartWriteLine(volatile avr32_usart_t * usart, const char * string)
{
	while(*string != '\0') {
		switch(*string) {
			case '\n':
				usartWriteChar(usart, '\r');
				break;
			default:
				break;
		}
		usartWriteChar(usart, *string);
		++string;
	}
}

void usartWriteValue(volatile avr32_usart_t * usart, const unsigned int i)
{
	unsigned int nyb, shift, mask, val;
	mask = 0x0f;	//One nybble mask
	shift = 0x1c;	//32 bits - 4 bits (which is the nybble)
	nyb = mask << shift; //Here is the nybble

	usartWriteLine(usart, (char *)"0x");
	while(nyb) {
		val = (i & nyb) >> shift;
		usartWriteChar(usart, val <= 9 ? val + '0' : val + 'a' - 0x0a);
		nyb = (nyb >> 0x04);
		shift -= 0x04;
	}
}

//Get a char from the USART
//
//Returns:
//	USART_NO_RX_CHAR, if no char could be read
//	char c from USART, if char could be read
char usartGetChar(volatile avr32_usart_t * usart)
{
	char ch;

	/* Check to see if a char have been received */
	if((usart->csr & AVR32_USART_CSR_RXRDY_MASK) == 0) {
		return USART_NO_RX_CHAR;
	}
	/* Read the received char */
	ch = usart->rhr & AVR32_USART_RHR_RXCHR_MASK;
	return ch;
}

//Get chars from the serial terminal into buffer and Echo chars
void usartGetLine(volatile avr32_usart_t * usart, char * buf)
{
	char c;

	while(1) {
		c = usartGetChar(usart);

		/* Did we get any new char? */
		if(c != USART_NO_RX_CHAR) {
				/* Was it backspace? */
				if(c == '\b') {
					buf--;
				}
				else {
					/* Put the char into the buffer */
					*buf = c;
					buf++;
			}

			/* Echo char and check if it was carrige return */
			usartWriteChar(usart, c);
			if(c == '\r') {
				usartWriteChar(usart, '\n');
				/* Finish the string with a \0 */
				--buf;
				*buf = '\0';
				return;
			}
		}
	}
}
