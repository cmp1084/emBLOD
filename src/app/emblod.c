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
//TODO: This is a hack. Revisit.

#include "app/emblod.h"

BYTE chreadpos = 0;
static UINT br = 0;

void warning(const char * msg)
{
	usartWriteLine(USART0, msg);
	//~ usartWriteLine(USART0, "\n");
}

void error(const char * err)
{
	usartWriteLine(USART0, err);
	while(1);
}

//Check end of buffer
//~ __attribute__((noinline))
int b_eof(void)
{
	return chreadpos == br;
}

BYTE chread(FIL ** fp, BYTE ** buf)
{
	BYTE ch;
	FRESULT rc;

	if(chreadpos == 0) {
		rc = f_read(*fp, *buf, BUFMAXLEN, &br); //TODO: We dont take care of errors here.
	}
	ch = *(*buf+chreadpos);
	chreadpos++;
	if(chreadpos == BUFMAXLEN) {
		chreadpos = 0;
	}
	return ch;
}

void readawayline(FIL ** fp, BYTE ** buf)
{
	BYTE ch;
	//Read away this line
	do {
		ch = chread(fp, buf);
		if(ch == '\n') break;
	} while(!b_eof());
}

BYTE readawayspace(FIL ** fp, BYTE ** buf)
{
	BYTE ch;
	//Read away space
	do {
		ch = chread(fp, buf);
	} while(((ch == ' ') || (ch == '\t') || (ch == '\n')) && !b_eof() );
	return ch;
}

/********************************************************
 * readparam
 *
 * Comment:
 * Read a parameter or a parameter value from a file.
 *
 ********************************************************/
int readparam(char * str, FIL * fp, BYTE * buf)
{
	BYTE ch;
	int i = 0;


	//Read away any commented line
	ch = readawayspace(&fp, &buf);
	while(ch == '#' && !b_eof()) {
		readawayline(&fp, &buf);
		ch = readawayspace(&fp, &buf);
	}
	//Get the parameter/value
	while(!b_eof()) {
		//End of parameter/value?
		if((ch == '\n') || (ch == '=')) {
			str[i] = '\0';
			return OK;
		}
		else {
			//Build the parameter/value string
			if(ch != ' ') {
				str[i++] = ch;
				if(i == MAXSTRLEN) {
					return MAXSTRLEN;
				}
			}
		}
		ch = chread(&fp, &buf);
	}
	return EOF;
}


/********************************************************
 * hexdecstrtoi
 *
 * Comment:
 * Convert a hexadecimal or decimal string into an integer
 * A hex string must be prefixed 0x.
 * Anything else will be intepreted as a decimal number.
 * Any illegal chars in the string will terminate the conversion
 * and return what was converted so far.
 *
 ********************************************************/
int hexdecstrtoi(char * str)
{
	int i, j = 0;

	char hexdec = *(str+1);

	if((hexdec == 'x') || (hexdec == 'X')) {
		//hex value
		str += 2;
		while((i = *str++) != '\0') {
			j <<= 4;
			if(i > 0x60) {
				i -= 'a' - 'A';
			}
			if(i > 0x39) {
				i += 0x0a - 'A';
			}
			else {
				i -= '0';
			}
			j += i;
		}
	} else {
		//dec value
		while(*str  != '\0') {
			j *= 10;
			if((*str < '0') || (*str > '9')) {
				break;	//return 0xbad;  //Illegal char was found.
			}
			j += *str - '0';
			str++;
		}
	}
	return j;
}


/********************************************************
 * bootparamLoad
 *
 * Comment:
 * Load boot parameters from a file with 'filename'
 *
 ********************************************************/
bootparam_t bootparamLoad(char * filename)
{
	FIL file_bootparam;
	FATFS fatfs_bootparam;           //Filesystem object
	BYTE buff_bootparam[BUFMAXLEN];
	FRESULT rc;
	char param[MAXSTRLEN];
	char value[MAXSTRLEN];

	//Default boot parameters
	bootparam_t bootparam = {
		DEFAULT_BAUDRATE,            //baudrate, This is the fallback baudrate if bootparameter file can not be read.
		DEFAULT_BOOTDELAY,           //bootdelay
		DEFAULT_FCPU,                //fcpu
		DEFAULT_FPBA,                //fpba
		DEFAULT_LOADADDR,            //loadaddr
		DEFAULT_BOOTADDR,            //bootaddr
		DEFAULT_AUTORUN_FILE,        //bootfile
		DEFAULT_AUTOBANNER_FILE      //Banner/ASCII-art file
	};

	f_mount(0, &fatfs_bootparam);
	rc = f_open(&file_bootparam, filename, FA_READ);

	if(rc) {
		usartWriteLine(USART0, "\nError opening ");
		usartWriteLine(USART0, filename);
	}

	while(readparam(param, &file_bootparam, buff_bootparam) == OK) {
		if(readparam(value, &file_bootparam, buff_bootparam) != OK) break;

		//~ Debug TODO: remove
		usartWriteLine(USART0, " -> ");
		usartWriteLine(USART0, param);
		usartWriteLine(USART0, " = ");
		usartWriteLine(USART0, value);
		usartWriteLine(USART0, " <- ");
		usartWriteLine(USART0, "\n");

		if(strncmp(param, "baudrate", MAXSTRLEN) == 0) {
			bootparam.baudrate = hexdecstrtoi(value);
		}
		else
		if(strncmp(param, "bootdelay", MAXSTRLEN) == 0) {
			bootparam.bootdelay = hexdecstrtoi(value);
		}
		else
		if(strncmp(param, "bootfile", MAXSTRLEN) == 0) {
			strncpy(bootparam.bootfile, value, MAXSTRLEN);
		}
		else
		if(strncmp(param, "banner", MAXSTRLEN) == 0) {
			strncpy(bootparam.banner, value, MAXSTRLEN);
		}
		else
		if(strncmp(param, "fcpu", MAXSTRLEN) == 0) {
			bootparam.fcpu = hexdecstrtoi(value);
		}
		else
		if(strncmp(param, "fpba", MAXSTRLEN) == 0) {
			bootparam.fpba = hexdecstrtoi(value);
		}
		else
		if(strncmp(param, "loadaddr", MAXSTRLEN) == 0) {
			bootparam.loadaddr = hexdecstrtoi(value);
		}
		else
		if(strncmp(param, "bootaddr", MAXSTRLEN) == 0) {
			bootparam.bootaddr = hexdecstrtoi(value);
		}
		else {
			usartWriteLine(USART0, "\nUnknown parameter: ");
			usartWriteLine(USART0, param);
			usartWriteLine(USART0, " = ");
			usartWriteLine(USART0, value);
			usartWriteLine(USART0, "\n");
		}
	}

	//TODO: Remove debug output
	usartWriteLine(USART0, "\nbaudrate: ");
	usartWriteValue(USART0, bootparam.baudrate);
	//~ //~
	usartWriteLine(USART0, "\nbootdelay: ");
	usartWriteValue(USART0, bootparam.bootdelay);
	//~ //~
	usartWriteLine(USART0, "\nbootfile: ");
	usartWriteLine(USART0, bootparam.bootfile);
	//~ //~
	usartWriteLine(USART0, "\nbanner: ");
	usartWriteLine(USART0, bootparam.banner);
	//~ //~
	usartWriteLine(USART0, "\nfcpu: ");
	usartWriteValue(USART0, bootparam.fcpu);
	//~ //~
	usartWriteLine(USART0, "\nloadaddr: ");
	usartWriteValue(USART0, bootparam.loadaddr);
	//~ //~
	//~ //~
	usartWriteLine(USART0, "\nbootaddr: ");
	usartWriteValue(USART0, bootparam.bootaddr);
	usartWriteLine(USART0, "\n");
	f_close(&file_bootparam);
	return bootparam;
}


/********************************************************
 * banner
 *
 * Comment:
 * Show a banner on the serial terminal before bootloading
 *
 ********************************************************/
void banner(bootparam_t * bootparam)
{
	FRESULT rc;
	FATFS fatfs;	//Filesystem object
	FIL file;		//File object
	UINT br;
	int i;
	BYTE buf[BUFMAXLEN];

	f_mount(0, &fatfs);                            //Register workarea
	rc = f_open(&file, bootparam->banner, FA_READ);
	if(rc) {
		warning("\nError opening banner");
	}

	while(1) {
		rc = f_read(&file, buf, sizeof(buf), &br); //Read a chunk of the file
		if(rc || !br) break; //Error or EOF
		for(i = 0; i < br; i++) {
			usartWriteChar(USART0, buf[i]);
			if(buf[i] == '\n') {
				usartWriteChar(USART0, '\r');
			}
		}
	}

	rc = f_close(&file);
	if(rc) {
		warning("\nError closing banner");
	}
}


/********************************************************
 * boot
 *
 * Comment:
 * Bootload a file specified by bootparam->bootfile
 * (the bootfile parameter in the bootparm.txt)
 *
 ********************************************************/
int boot(bootparam_t * bootparam)
{
	FRESULT rc;
	FATFS fatfs;    //Filesystem object
	FIL file;       //File object
	UINT br;
	int i;

	unsigned char * sdram;
	BYTE buf[BUFMAXLEN];

	sdram = (unsigned char *)bootparam->loadaddr;

	usartWriteLine(USART0, "\nLoading ");
	usartWriteLine(USART0, bootparam->bootfile);

	f_mount(0, &fatfs);
	rc = f_open(&file, bootparam->bootfile, FA_READ);
	if(rc) {
		warning("\nError opening ");
		warning(bootparam->bootfile);
		return ERROR;
	}

	while(1) {
		rc = f_read(&file, buf, sizeof(buf), &br);
		if(rc) {
			return ERROR;
		}

		if(!br) {
			break;    //EOF
		}

		for(i = 0; i < br; i++) {
			*sdram = buf[i];
			sdram++;
		}
	}
	rc = f_close(&file);
	if(rc) {
		warning("\nError closing ");
		warning(bootparam->bootfile);
	}

	//TODO: Detection of SD-card and reinit SD-card if removed/inserted
	//TODO: Checksum and reload if necessary

	usartWriteLine(USART0, "\nBytes read: ");
	usartWriteValue(USART0, (int)(sdram - SDRAM_BASE));
	usartWriteLine(USART0, "\n");

	if(bootparam->bootdelay) {
		usartWriteLine(USART0, "\nWaiting ");
		usartWriteValue(USART0, bootparam->bootdelay);
		usartWriteLine(USART0, " seconds\n");
		delay_ms(bootparam->bootdelay  * 1000);
	}

	usartWriteLine(USART0, "\nStarting...\n");
	interruptDisable();
	asm("icall %0" : : "r"(bootparam->bootaddr));
	return 0;
}


/********************************************************
 *
 *
 * Comment:
 *
 *
 ********************************************************/
void emblod(bootparam_t * bootparam)
{

}