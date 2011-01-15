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

#include <avr32/io.h>

	//General constants
	.equ TRUE,0x01
	.equ FALSE,0x00

	//Global functions
	.global interruptDisable
	.global interruptEnable
	.global isInterruptEnabled
	.global interruptInit

	//Interrupt constants
	.equ INTLEVEL0,0
	.equ INTLEVEL_OFFSET,30
	.equ GRP14,0x0e*0x04					//Grp 14 for TC
	.equ LINE0,0x01							//Line 0 for TC0

	//Interrupt vectors
	//Put function code at an aligned address in the .exception section, make it executable.
	.section  .exception, "ax", @progbits
	.balign 0x200
	.global _evba
	.type _evba, @function
_evba:
  .org  0x000
        // Unrecoverable Exception.
_handle_Unrecoverable_Exception:
        rjmp $

        .org  0x004
        // TLB Multiple Hit.
_handle_TLB_Multiple_Hit:
        rjmp $

        .org  0x008
        // Bus Error Data Fetch.
_handle_Bus_Error_Data_Fetch:
        rjmp $

        .org  0x00C
         // Bus Error Instruction Fetch.
_handle_Bus_Error_Instruction_Fetch:
        rjmp $

        .org  0x010
        // NMI.
_handle_NMI:
        rjmp $

        .org  0x014
        // Instruction Address.
_handle_Instruction_Address:
        rjmp $

        .org  0x018
        // ITLB Protection.
_handle_ITLB_Protection:
        rjmp $

        .org  0x01C
        // Breakpoint.
_handle_Breakpoint:
        rjmp $

        .org  0x020
        // Illegal Opcode.
_handle_Illegal_Opcode:
        rjmp $

        .org  0x024
        // Unimplemented Instruction.
_handle_Unimplemented_Instruction:
        rjmp $

        .org  0x028
        // Privilege Violation.
_handle_Privilege_Violation:
        rjmp privilegedCalls

        .org  0x02C
        // Floating-Point: UNUSED IN AVR32UC and AVR32AP.
_handle_Floating_Point:
        rjmp $

        .org  0x030
        // Coprocessor Absent: UNUSED IN AVR32UC.
_handle_Coprocessor_Absent:
        rjmp $

        .org  0x034
        // Data Address (Read).
_handle_Data_Address_Read:
        rjmp $

        .org  0x038
        // Data Address (Write).
_handle_Data_Address_Write:
        rjmp $

        .org  0x03C
        // DTLB Protection (Read).
_handle_DTLB_Protection_Read:
        rjmp $

        .org  0x040
        // DTLB Protection (Write).
_handle_DTLB_Protection_Write:
        rjmp $

        .org  0x044
        // DTLB Modified: UNUSED IN AVR32UC.
_handle_DTLB_Modified:
        rjmp $

	.org  0x050
	// ITLB Miss.
_handle_ITLB_Miss:
	rjmp $

	.org  0x060
	// DTLB Miss (Read).
_handle_DTLB_Miss_Read:
	rjmp $

	.org  0x070
	// DTLB Miss (Write).
_handle_DTLB_Miss_Write:
	rjmp $

	.org  0x100
_handle_Supervisor_Call:
	mov pc,lr								//Kernel function calls pass through here.

	.section .text, "ax", @progbits
/***********************************************************
 * Privileged Calls
 *
 * Comment:
 * 	Handle privileged function calls.
 *  Look for the legal priviledged calls,
 *  Stop execution if this was an illegal priviledged call.
 *
 **********************************************************/
.equ PRIV_STACKOFFSET,0x08					//Due to r11, r12 pushed to stack. 0x04 * 2 = 0x08
.equ USERSR,0x00+PRIV_STACKOFFSET
.equ USERPC,0x04+PRIV_STACKOFFSET
.equ USERR12,0x00

//TODO: make a jumptable out of this, if it is ever worth it
//~ privilegedCallsJumptable:
	//~ dc.w interruptDisable, priv_interruptdisable
	//~ dc.w interruptEnable, priv_interruptenable
	//~ dc.w isInterruptEnabled, priv_isinterruptenable
	//~ dc.w -1,-1

privilegedCalls:
//Stack frame at this point in time:
//------------------------------------
//USERSR - user SR at the offending instant
//USERPC - user PC which contain the offending instruction

	//Investigate if this is a legal disable/enable interrupt requests
	pushm r11,r12
	ld.w r11,sp[USERPC]
	mov r12,r11
	sub r12,-0x02							//Modify to get return address
	st.w sp[USERPC],r12						//Put return address on the stack

	//Look for interruptDisable
priv_interruptdisable:
	lda.w r12,interruptDisable
	cp.w r11,r12
	brne priv_interruptenable
	ld.w r12,sp[USERSR]
	sbr r12,AVR32_SR_GM						//Ok, disable interrupts
	st.w sp[USERSR],r12
	popm r11,r12
	rete

	//Look for interruptEnable
priv_interruptenable:
	lda.w r12,interruptEnable
	cp.w r11,r12
	brne priv_isinterruptenabled
	ld.w r12,sp[USERSR]
	cbr r12,AVR32_SR_GM						//Ok, enable interrupts
	st.w sp[USERSR],r12
	popm r11,r12
	rete

	//Look for isInterruptEnable
priv_isinterruptenabled:
	lda.w r12,isInterruptEnabled
	cp.w r11,r12
	brne privexception
	ld.w r12,sp[USERSR]						//Ok, read interrupt flag
	st.w sp[USERR12],r12
	popm r11,r12
	rete

privexception:								//Not ok.
	rjmp $									//End here as we received an illegal privileged call.


/*********************************************************
 * interruptDisable
 * interruptEnable
 * isInterruptEnabled
 * contextSaveFromISR
 * contextRestoreFromISR
 *
 * Comment:
 * 	Helper functions, called from user mode and
 * 	transfer control to supervisor mode
 *  They are used for kernel controll and context switching.
 *
 *********************************************************/
/**********************************************************
 * interruptDisable
 *
 * Comment:
 * 	Disable global interrupts.
 *
 * C prototype:
 * 	void interruptDisable(void);
 *
 *********************************************************/
interruptDisable:
	ssrf AVR32_SR_GM
	mov pc,lr


/**********************************************************
 * interruptEnable
 *
 * Comment:
 * 	Enable global interrupts.
 *
 * C prototype:
 * 	void interruptEnable(void);
 *
 *********************************************************/
interruptEnable:
	csrf AVR32_SR_GM
	mov pc,lr


/**********************************************************
 * isInterruptEnabled
 *
 * Comment:
 * 	Check if global interrupts are enabled.
 *
 * Returns:
 * TRUE or FALSE
 *
 * C prototype:
 * 	int isInterruptEnabled(void);
 *
 *********************************************************/
isInterruptEnabled:
	mfsr r12,0
	lsr r12,AVR32_SR_GM
	andl r12,TRUE
	eorl r12,TRUE
	mov pc,lr
