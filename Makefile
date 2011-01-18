#*****************************************************************************
#
#   [SimpleMachines.it]     ___________    ____________
#  __________________  ____ \  _   \   \   \        \  \_
#  \    __    \      \/    \/  7  _/   /   /  __    /    \_
#  /     7    /            /   _   \  /___/    7   /  _    \_
# /_     ____/_    /\_/   /    7   /     /_       /   7      \
#   \________/ \__/  /___/________/\______/\_____/___________/
#                                          [asmegir.hopto.org]
#           e       m       B       L       O       D
#
#    Copyright 2011 Marcus Jansson <mjansson256@yahoo.se>
#
#    This file is part of emBLOD - the embedded bootloader
#
#    emBLOD is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    emBLOD is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with emBLOD.  If not, see <http://www.gnu.org/licenses/>.
#*****************************************************************************
# Tab size: 4

export

##############################################################
#The target board
##############################################################
#The default board is Mizar32. Compilation for another board can be done
#with, e.g.:
#    make BOARD=EVK1100
ifndef $(BOARD)
	BOARD = MIZAR32
endif


##############################################################
#The target MPU
##############################################################
ifeq ($(BOARD),MIZAR32)
	PART = uc3a0128
endif

ifeq ($(BOARD),EVK1100)
	PART = uc3a0512
endif


##############################################################
#Binaries are located in this dir
##############################################################
BINDIR = bin
#TODO: Put in bin
ELFDIR = .


##############################################################
# The name of the program, elf and binary
##############################################################
PROGRAM = emblod
ELF = $(ELFDIR)/$(PROGRAM).elf
BINARY = $(BINDIR)/$(PROGRAM).bin


##############################################################
#Sources are located in this dir
##############################################################
SOURCEDIR = src


##############################################################
#Various source subdirectories
##############################################################
STARTUPDIR = $(SOURCEDIR)/cpu
KERNELDIR = $(SOURCEDIR)/kernel
SYSTEMDIR = $(SOURCEDIR)/system
DRIVERSDIR = $(SOURCEDIR)/drivers
FILESYSTEMDIR = $(DRIVERSDIR)/filesystem
FATDIR = $(FILESYSTEMDIR)/fat
APPDIR = $(SOURCEDIR)/app


##############################################################
#The assembler sources
##############################################################
ASMSOURCE += $(STARTUPDIR)/startup/crt0.S
ASMSOURCE += $(STARTUPDIR)/startup/irq.S


##############################################################
#The driver sources
##############################################################
SOURCE += $(DRIVERSDIR)/gpio.c
SOURCE += $(DRIVERSDIR)/led.c
SOURCE += $(DRIVERSDIR)/button.c
SOURCE += $(DRIVERSDIR)/usart.c
SOURCE += $(DRIVERSDIR)/delay.c
SOURCE += $(DRIVERSDIR)/pm.c
SOURCE += $(DRIVERSDIR)/sdramc.c
SOURCE += $(DRIVERSDIR)/spi.c
SOURCE += $(DRIVERSDIR)/sdhc.c
SOURCE += $(DRIVERSDIR)/wdt.c


##############################################################
#The FAT filesystem library
##############################################################
SOURCE += $(FATDIR)/diskio.c
SOURCE += $(FATDIR)/ff.c

##############################################################
SOURCE += $(SOURCEDIR)/main.c


##############################################################
#Application sources
##############################################################
APPSOURCE=$(APPDIR)/emblod.c


##############################################################
#Header files are located in these files
##############################################################
INCDIRS = -Isrc -Isrc/include

ifndef $(FATDIR)
INCDIRS += -Isrc/include/drivers/filesystem/fat
endif

##############################################################
#Various build programs
##############################################################
CC = avr32-gcc
LD = avr32-ld
AS = avr32-as
OBJCOPY = avr32-objcopy
OBJDUMP = avr32-objdump
SIZE = avr32-size
MKDIR = mkdir
TEST = test
LESS = less
ECHO = echo


##############################################################
#JTAG tool which will be used
##############################################################
PROGRAMMER = avr32program
JTAGTOOL = avrdragon
#JTAGTOOL = jtagicemkii


##############################################################
#Various compile flags etc
##############################################################
LDSCRIPT = $(STARTUPDIR)/linkscript/$(PART).lds

DEBUG = -ggdb
OPT = s

AFLAGS = -x assembler-with-cpp

CFLAGS = -c
CFLAGS += -mpart=$(PART) -D$(BOARD) -DBOARD=$(BOARD)
CFLAGS += $(DEBUG) -DO$(OPT)
CFLAGS += -Wall -Wa,-R
CFLAGS += $(INCDIRS)
CFLAGS += -msoft-float
CFLAGS += -fomit-frame-pointer
CFLAGS += -mrelax
CFLAGS += -fno-common
CFLAGS += -fsection-anchors
#~ CFLAGS += -fdata-sections
CFLAGS += -ffunction-sections
CFLAGS += -mno-use-rodata-section
CFLAGS += -std=gnu99
#~ CFLAGS += -pedantic


##############################################################
#Various link flags etc
##############################################################
LDFLAGS = -Wl,--gc-sections
LDFLAGS += -Wl,--relax
LDFLAGS += -Wl,--direct-data
LDFLAGS += -nostartfiles
LDFLAGS += -mpart=$(PART)
LDFLAGS += -T$(LDSCRIPT)


##############################################################
#The object files
##############################################################
#TODO: Not ok yet. Please, only use BUILDDIR = .
BUILDDIR = .
OBJ = $(ASMSOURCE:%.S=%.o)
OBJ += $(SOURCE:%.c=%.o)
OBJ += $(SYSTEMSOURCE:%.c=%.o)
OBJ += $(APPSOURCE:%.c=%.o)


##############################################################
#Misc niceness
##############################################################
TEXTPRECOMPILE = @$(ECHO) -n "---  $< "
TEXTPOSTCOMPILE = @$(ECHO) -e "\r[OK]"


##############################################################
#Makefile rules
##############################################################
.PHONY: all
all: makeSureWeAlwaysLink PrintBoardInfo $(OBJ) $(ELF) $(BINARY) ok size crlf

Print:
	@$(ECHO) $(OBJ)

ok:
	@$(ECHO) -e "\r[ALL OK]"

crlf:
	@$(ECHO)

PrintBoardInfo:
	@echo "================================"
	@echo Compiling $(PROGRAM) with -O$(OPT)
	@echo BOARD = $(BOARD)
	@echo PART = $(PART)
	@echo "================================"

#TODO: This should be removed in the final release
makeSureWeAlwaysLink:
	rm -f $(PROGRAM).elf

#TODO: delay_ms should be fixed up nice, until then, dont optimize the delay_ms function in src/drivers/delay.c
$(BUILDDIR)/$(DRIVERSDIR)/delay.o: $(DRIVERSDIR)/delay.c
	$(TEXTPRECOMPILE) with -O0
	@$(TEST) -d $(BUILDDIR)/$(@D) || $(MKDIR) $(BUILDDIR)/$(@D)
	@$(CC) $(CFLAGS) -O0 $< -o$(BUILDDIR)/$@
	$(TEXTPOSTCOMPILE)

#~ $(BUILDDIR)/$(DRIVERSDIR)/__removed__sddamc.o: $(DRIVERSDIR)/sdramc.c
	#~ $(TEXTPRECOMPILE) with -O0
	#~ @$(TEST) -d $(BUILDDIR)/$(@D) || $(MKDIR) $(BUILDDIR)/$(@D)
	#~ @$(CC) $(CFLAGS) -O0 $< -o$(BUILDDIR)/$@
	#~ $(TEXTPOSTCOMPILE)

%.o: %.S
	$(TEXTPRECOMPILE)
	@$(TEST) -d $(BUILDDIR)/$(@D) || $(MKDIR) -p $(BUILDDIR)/$(@D)
	@$(CC) $(CFLAGS) $(AFLAGS) -O$(OPT) $< -o$(BUILDDIR)/$@
	$(TEXTPOSTCOMPILE)

%.o: %.c
	$(TEXTPRECOMPILE)
	@$(TEST) -d $(BUILDDIR)/$(@D) || $(MKDIR) -p $(BUILDDIR)/$(@D)
	@$(CC) $(CFLAGS) -O$(OPT) $< -o$(BUILDDIR)/$@
	$(TEXTPOSTCOMPILE)

$(BINARY): ./$(PROGRAM).elf
	@$(TEST) -d $(BINDIR) || $(MKDIR) -p $(BINDIR)
	$(OBJCOPY) -O binary $(ELF) $(BINARY)

$(ELF): $(OBJ)
	@$(TEST) -d $(ELFDIR) || $(MKDIR) -p $(ELFDIR)
	@echo
	@echo "Linking, using $(LDSCRIPT)"
	@$(ECHO) -n "...  Linking $(ELF)"
	@$(CC) $(LDFLAGS) $(OBJ:%=$(BUILDDIR)/%) -o $(ELF)
	$(TEXTPOSTCOMPILE)

.PHONY: program
program: $(BINARY)
	#erase sectors [-e], program internal flash [-f] at offset [-O] 0x80000000 using the xtal as clock [-cxtal], verify [-v], reset [-R] and run [-r]
	$(PROGRAMMER) program -O0x80002000 -finternal@0x80000000 -e -v -cxtal -Rr $(BINARY)

.PHONY: reset
reset:
	$(PROGRAMMER) reset

.PHONY: resetevk1100
resetevk1100:
	$(PROGRAMMER) -c USB:00A200007395 reset

.PHONY: resetmizar
resetmizar:
	$(PROGRAMMER) -c USB:00A20000793A reset

.PHONY: run
run:
	$(PROGRAMMER) run

.PHONY: cpuinfo
cpuinfo:
	$(PROGRAMMER) cpuinfo

.PHONY: size
size: $(BINARY)
	@$(SIZE) --target=binary $(BINARY)

.PHONY: gdb
gdb:
	avr32gdbproxy -k -a localhost:4711 -cUSB -e$(JTAGTOOL)

.PHONY: kill
kill:
	killall avr32gdbproxy

.PHONY: killall
	killall avr32gdbproxy

.PHONY: dump
dump:
	$(OBJDUMP) -S -x $(ELF)|$(LESS)

.PHONY: remake
remake:	clean all

.PHONY: semiclean
semiclean:
	rm -f $(BINARY) $(ELF)

.PHONY: clean
clean:
	rm -f $(OBJ) $(ELF) $(BINARY)
