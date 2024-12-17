// Hand PIC'd 32
// By: Professor Steven Chad Burrow


// comment out if you want to program the PIC32 faster
#define SPLASH


/*
VESA Signal 800 x 600 @ 72 Hz timing

Interested in easy to use VGA solution for embedded applications? Click here!

General timing
Screen refresh rate	72 Hz
Vertical refresh	48.076923076923 kHz
Pixel freq.	50.0 MHz
Horizontal timing (line)
Polarity of horizontal sync pulse is positive.

Scanline part	Pixels	Time [µs]
Visible area	800	16
Front porch	56	1.12
Sync pulse	120	2.4
Back porch	64	1.28
Whole line	1040	20.8
Vertical timing (frame)
Polarity of vertical sync pulse is positive.

Frame part	Lines	Time [ms]
Visible area	600	12.48
Front porch	37	0.7696
Sync pulse	6	0.1248
Back porch	23	0.4784
Whole frame	666	13.8528
*/

/*	
Interrupt from Visible Timer at beginning of scanline, DMA0 to start.
DMA0 transfers single byte to DMA1 config to enable it.
DMA1 transfers ~400 pixels to PMP on Pixel Timer.
DMA1 chains to DMA2 which transfers a zero to PMP or PORTx.
All of this is independent of sync signals and timers!
Increment line number and change addresses on separate interrupt!
*/ 

/*
// Video Color Signals
Pin5,RE7,RED-A
Pin4,RE6,RED-B
Pin3,RE5,RED-C
Pin144,RE4,GREEN-A
Pin143,RE3,GREEN-B
Pin142,RE2,GREEN-C
Pin138,RE1,BLUE-A
Pin135,RE0,BLUE-B

// Video Clock Signals
Pin104,RD0,PIXEL-CLOCK
Pin119,RD5,VISIBLE-CLOCK
Pin110,RD2,H-SYNC
Pin111,RD3,V-SYNC

// Audio
Pin23,RE8,AUDIO-A
Pin24,RE9,AUDIO-B

// SPI SDcard Adapter
Pin120,RD6,MISO
Pin121,RD7,MOSI
Pin109,RD1,SCK1
Pin118,RD4,CS

// PS/2 Keyboard/Mouse
Pin97,RD9,KEY-CLK
Pin98,RD10,KEY-DAT
Pin112,RD12,MOUSE-CLK
Pin113,RD13,MOUSE-DAT

// Joysticks
Pin114,RJ0,JOY-A-UP
Pin115,RJ1,JOY-A-DOWN
Pin116,RJ2,JOY-A-LEFT
Pin117,RJ3,JOY-A-RIGHT
Pin131,RJ4,JOY-A-BUTTON1
Pin132,RJ5,JOY-A-BUTTON2
Pin133,RJ6,JOY-B-UP
Pin134,RJ7,JOY-B-DOWN
Pin10,RJ10,JOY-B-LEFT
Pin9,RJ12,JOY-B-RIGHT
Pin28,RJ13,JOY-B-BUTTON1
Pin29,RJ14,JOY-B-BUTTON2
Pin30,RJ15,JOY-SELECT

// Programmer
Pin20,MCLR
Pin36,RB0,PGED1
Pin35,RB1,PGEC1

// USB0xFF, 0xFF, 0xFF, 0xFF,
Pin77,D+
Pin76,D-Pin29,RJ14,JOY

// Oscillator
Pin71,RC12,OSC1/CLKI
Pin72,RC15,OSC2/CLKO

// UART
Pin69,RD14,U3TX
Pin70,RD15,U3RX

// LED
Pin99,RD11,LED

// Button
Pin27,RJ11,BUTTON
*/




// PIC32MZ2048EFM144 Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
#pragma config USERID = 0xFFFF          // Enter Hexadecimal value (Enter Hexadecimal value)
#pragma config FMIIEN = ON              // Ethernet RMII/MII Enable (MII Enabled)
#pragma config FETHIO = OFF             // Ethernet I/O Pin Select (Alternate Ethernet I/O)
#pragma config PGL1WAY = OFF            // Permission Group Lock One Way Configuration (Allow multiple reconfigurations)
#pragma config PMDL1WAY = OFF           // Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
#pragma config FUSBIDIO = OFF           // USB USBID Selection (Controlled by Port Function)

// DEVCFG2
#pragma config FPLLIDIV = DIV_3         // System PLL Input Divider (3x Divider)
#pragma config FPLLRNG = RANGE_5_10_MHZ// System PLL Input Range (5-10 MHz Input)
#pragma config FPLLICLK = PLL_POSC       // System PLL Input Clock Selection (FRC is input to the System PLL)
#pragma config FPLLMULT = MUL_50       // System PLL Multiplier (PLL Multiply by 50)
#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (2x Divider)
#pragma config UPLLFSEL = FREQ_24MHZ    // USB PLL Input Frequency Selection (USB PLL input is 24 MHz)

// DEVCFG1
#pragma config FNOSC = FRCDIV           // Oscillator Selection Bits (Fast RC Osc w/Div-by-N (FRCDIV))
#pragma config DMTINTV = WIN_127_128    // DMT Count Window Interval (Window/Interval value is 127/128 counter value)
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disable SOSC)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = ON            // CLKO Output Signal Active on the OSCO Pin (Enabled)
#pragma config FCKSM = CSECME           // Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WDTSPGM = STOP           // Watchdog Timer Stop During Flash Programming (WDT stops during Flash programming)
#pragma config WINDIS = NORMAL          // Watchdog Timer Window Mode (Watchdog Timer is in non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled)
#pragma config FWDTWINSZ = WINSZ_50     // Watchdog Timer Window Size (Window size is 50%)
#pragma config DMTCNT = DMT31           // Deadman Timer Count Selection (2^31 (2147483648))
#pragma config FDMTEN = OFF             // Deadman Timer Enable (Deadman Timer is disabled)

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config TRCEN = OFF               // Trace Disable (Trace features in the CPU are disabled)
#pragma config BOOTISA = MIPS32         // Boot ISA Selection (Boot code and Exception code is MIPS32)
//#pragma config FECCCON = OFF_UNLOCKED   // Dynamic Flash ECC Configuration (ECC and Dynamic ECC are disabled (ECCCON bits are writable))
#pragma config FECCCON = ON             // Dynamic Flash ECC Configuration (Flash ECC is enabled (ECCCON bits are locked))
#pragma config FSLEEP = VREGS           // Flash Sleep Mode (Flash power down is controlled by the VREGS bit)
#pragma config DBGPER = PG_ALL          // Debug Mode CPU Access Permission (Allow CPU access to all permission regions)
#pragma config SMCLR = MCLR_NORM        // Soft Master Clear Enable bit (MCLR pin generates a normal system Reset)
#pragma config SOSCGAIN = GAIN_2X       // Secondary Oscillator Gain Control bits (2x gain setting)
#pragma config SOSCBOOST = OFF          // Secondary Oscillator Boost Kick Start Enable bit (Normal start of the oscillator)
#pragma config POSCGAIN = GAIN_2X       // Primary Oscillator Gain Control bits (2x gain setting)
#pragma config POSCBOOST = OFF          // Primary Oscillator Boost Kick Start Enable bit (Normal start of the oscillator)
#pragma config EJTAGBEN = NORMAL        // EJTAG Boot (Normal EJTAG functionality)

// DEVCP0
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

// SEQ3
#pragma config TSEQ = 0xFFFF            // Boot Flash True Sequence Number (Enter Hexadecimal value)
#pragma config CSEQ = 0xFFFF            // Boot Flash Complement Sequence Number (Enter Hexadecimal value)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>


#pragma region name="gb_rom" origin=0x9D100000 size=0x00100000


#define SYS_FREQ 150000000 // Running at 150MHz

unsigned long VirtToPhys(volatile void* p) // changed 'const' to 'volatile'
{
	return (long)p<0?((long)p&0x1fffffffL):(unsigned long)((unsigned long*)p+0x40000000L);
}

void DelayMS(unsigned int s)
{
	unsigned long count = 0x00000000;
	
	count = (unsigned long)((unsigned long)((SYS_FREQ / 1000) * s) / 2); // Convert microseconds us into how many clock ticks it will take
       
	_CP0_SET_COUNT(0); // Set Core Timer count to 0
    
	while (count > _CP0_GET_COUNT()); // Wait until Core Timer count reaches the number we calculated earlier
}

// if using UART, use command 'sudo picocom /dev/ttyUSB0' in Linux
void SendChar(char value)
{
	while (U3STAbits.UTXBF == 1) { }
	U3TXREG = (char)(value);
}

void SendString(char *value)
{
	for (unsigned int i=0; i<256; i++)
	{
		if (value[i] == '\\') break;
		else
		{
			while (U3STAbits.UTXBF == 1) { }
			U3TXREG = (char)(value[i]);
		}
	}
}

void SendHex(unsigned char value)
{
	while (U3STAbits.UTXBF == 1) { }
	if (value/16 >= 10) U3TXREG = (char)(value/16 + 'A' - 10);
	else U3TXREG = (char)(value/16 + '0');
	while (U3STAbits.UTXBF == 1) { }
	if (value%16 >= 10) U3TXREG = (char)(value%16 + 'A' - 10);
	else U3TXREG = (char)(value%16 + '0');
}

void SendLongHex(unsigned long value)
{
	SendHex((unsigned char)(value >> 24));
	SendHex((unsigned char)(value >> 16));
	SendHex((unsigned char)(value >> 8));
	SendHex((unsigned char)(value));
}

#define SCREEN_X 640
#define SCREEN_Y 480

// most important arrays
volatile unsigned char __attribute__((coherent,address(0x8002B000))) screen_buffer[SCREEN_Y*SCREEN_X]; // visible portion of screen
volatile unsigned char __attribute__((coherent,address(0x80076000))) audio_buffer[8192];
volatile unsigned int audio_position = 0;
volatile unsigned int audio_switch = 0;
volatile unsigned int frame_position = 0;

// variable arrays
volatile char __attribute__((coherent)) ps2_state_array[2][256];
volatile unsigned int __attribute__((coherent)) ps2_cursor_x[2][256];
volatile unsigned int __attribute__((coherent)) ps2_cursor_y[2][256];
volatile unsigned char __attribute__((coherent)) ps2_mode[2] = { 0x00, 0x00 };
volatile unsigned char __attribute__((coherent)) ps2_writepos[2] = { 0x00, 0x00 };
volatile unsigned char __attribute__((coherent)) ps2_readpos[2] = { 0x00, 0x00 };

volatile char __attribute__((coherent)) usb_state_array[256];
volatile unsigned int __attribute__((coherent)) usb_cursor_x[256];
volatile unsigned int __attribute__((coherent)) usb_cursor_y[256];
volatile unsigned int __attribute__((coherent)) usb_buttons[256];
volatile unsigned char __attribute__((coherent)) usb_mode = 0x00;
volatile unsigned char __attribute__((coherent)) usb_writepos = 0x00;
volatile unsigned char __attribute__((coherent)) usb_readpos = 0x00;

// additional variables
volatile unsigned char __attribute__((coherent)) screen_blank[SCREEN_X]; // black scanline
volatile unsigned int screen_scanline = 637; // start of vertical sync
volatile unsigned char screen_zero[1] = { 0x00 }; // zero value for black

// PS/2 keyboard variables
volatile unsigned char ps2_counter[2] = { 0x00, 0x00 };
volatile unsigned char ps2_buffer[2] = { 0x00, 0x00 };
volatile unsigned char ps2_sequence[2] = { 0xFF, 0xFF }; // starts with 0xFA acknowledge byte
volatile unsigned char ps2_shift[2] = { 0x00, 0x00 };
volatile unsigned char ps2_release[2] = { 0x00, 0x00 };
volatile unsigned char ps2_extended[2] = { 0x00, 0x00 };
volatile unsigned char ps2_ready[2] = { 0x00, 0x00 };
volatile unsigned char ps2_check[2] = { 0x00, 0x00 };
volatile unsigned int ps2_port = 0x0000;
volatile unsigned int ps2_flags = 0x0000;




void ps2_command(unsigned char command, unsigned char port)
{
	CNCONDbits.ON = 0; // turn off interrupt-on-change
	
	unsigned char copy = command;
	
	unsigned char parity_count = 0x00;
	
	for (unsigned char i=0; i<8; i++)
	{
		if ((copy & 0x01) == 0x01) parity_count++;
		
		copy = (copy >> 1);
	}
	
	if (port == 0) // primary port
	{
		PORTDbits.RD9 = 0; // when not floating, ground them
		PORTDbits.RD10 = 0;
		TRISDbits.TRISD9 = 1; // normally they are both floating though
		TRISDbits.TRISD10 = 1; 

		DelayMS(1000); // wait a while for mouse to send it's initial code

		TRISDbits.TRISD9 = 0; // ground clock

		DelayMS(1000); // delay for at least 100 ms

		TRISDbits.TRISD10 = 0; // ground data
		TRISDbits.TRISD9 = 1; // release clock

		while (PORTDbits.RD9 == 0) { } // wait until clock is high

		while (PORTDbits.RD9 == 1) { } // wait until clock is low

		for (unsigned char i=0; i<8; i++)
		{
			if ((command & 0x01) == 0x00)
			{
				TRISDbits.TRISD10 = 0; // ground data
			}
			else
			{
				TRISDbits.TRISD10 = 1; // float data to high
			}

			command = (command >> 1);

			while (PORTDbits.RD9 == 0) { } // wait until clock is high

			while (PORTDbits.RD9 == 1) { } // wait until clock is low
		}

		if (parity_count % 2 == 1) TRISDbits.TRISD10 = 0; // ground data (for parity)
		else TRISDbits.TRISD10 = 1; // float data high (for parity)

		while (PORTDbits.RD9 == 0) { } // wait until clock is high

		while (PORTDbits.RD9 == 1) { } // wait until clock is low

		TRISDbits.TRISD10 = 1; // release data line

		while (PORTDbits.RD10 == 0) { } // wait until data is high

		while (PORTDbits.RD10 == 1) { } // wait until data is low

		while (PORTDbits.RD9 == 1) { } // wait until clock is low

		while (PORTDbits.RD10 == 0) { } // wait until data is high

		while (PORTDbits.RD9 == 0) { } // wait until clock is high
	}
	else if (port == 1) // secondary port (from splitter)
	{
		PORTDbits.RD12 = 0; // when not floating, ground them
		PORTDbits.RD13 = 0;
		TRISDbits.TRISD12 = 1; // normally they are both floating though
		TRISDbits.TRISD13 = 1; 

		DelayMS(1000); // wait a while for mouse to send it's initial code

		TRISDbits.TRISD12 = 0; // ground clock

		DelayMS(1000); // delay for at least 100 ms

		TRISDbits.TRISD13 = 0; // ground data
		TRISDbits.TRISD12 = 1; // release clock

		while (PORTDbits.RD12 == 0) { } // wait until clock is high

		while (PORTDbits.RD12 == 1) { } // wait until clock is low

		for (unsigned char i=0; i<8; i++)
		{
			if ((command & 0x01) == 0x00)
			{
				TRISDbits.TRISD13 = 0; // ground data
			}
			else
			{
				TRISDbits.TRISD13 = 1; // float data to high
			}

			command = (command >> 1);

			while (PORTDbits.RD12 == 0) { } // wait until clock is high

			while (PORTDbits.RD12 == 1) { } // wait until clock is low
		}
		
		if (parity_count % 2 == 1) TRISDbits.TRISD13 = 0; // ground data (for parity)
		else TRISDbits.TRISD13 = 1; // float data high (for parity)

		while (PORTDbits.RD12 == 0) { } // wait until clock is high

		while (PORTDbits.RD12 == 1) { } // wait until clock is low

		TRISDbits.TRISD13 = 1; // release data line

		while (PORTDbits.RD13 == 0) { } // wait until data is high

		while (PORTDbits.RD13 == 1) { } // wait until data is low

		while (PORTDbits.RD12 == 1) { } // wait until clock is low

		while (PORTDbits.RD13 == 0) { } // wait until data is high

		while (PORTDbits.RD12 == 0) { } // wait until clock is high
	}
	
	CNCONDbits.ON = 1; // turn on interrupt-on-change
	
	return;
}



#ifdef SPLASH
#include "splash_default.c"
#endif

#include "tables.c"
#include "usbhost.c"
#include "interrupts.c"
#include "sdcard.c" 

#include "ffconf.h"
#include "ff.h"
#include "ff.c"
#include "diskio.h"
#include "diskio.c"



// just an example of how to read and write from files
void EchoFile()
{
	// #The SDcard must have been formatted
	// #Check which drive it is, here /dev/sdc
	// sudo fdisk -l
	// sudo umount /dev/sdc
	// sudo mkfs.vfat /dev/sdc
	
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	
	// Wait for the disk to initialise
    while(disk_initialize(0));
    // Mount the disk
    f_mount(&fso, "", 0);
    // Change dir to the root directory
    f_chdir("/");
    // Open the directory
    f_opendir(&dir, ".");
 
	char buffer[16];
	unsigned int bytes;
	unsigned int result;
	
	for (unsigned int i=0; i<16; i++)
	{
		buffer[i] = ' ';
	};
	
	SendString("\n\r\\");
	
	// read in some text
	result = f_open(&file, "/INPUT.TXT", FA_READ);
	if (result == 0)
	{
		SendString("Found INPUT.TXT, copying now\n\r\\");
		
		while (f_read(&file, buffer, 16, &bytes) != 0) { }
		while (f_close(&file) != 0) { }
	}
	else
	{
		SendString("Could not find INPUT.TXT\n\r\\");
	}
	
	// write the same text back
	result = f_open(&file, "/OUTPUT.TXT", FA_WRITE);
	if (result != 0)
	{
		result = f_open(&file, "/OUTPUT.TXT", FA_CREATE_NEW);
		if (result == 0)
		{
			while (f_close(&file) != 0) { }
			result = f_open(&file, "/OUTPUT.TXT", FA_WRITE);
		}
	}
	if (result == 0)
	{	
		while (f_write(&file, buffer, 16, &bytes) != 0) { }
		while (f_close(&file) != 0) { }
		
		SendString("Copied to OUTPUT.TXT\n\r\\");
	}
	else
	{
		SendString("Could not copy to OUTPUT.TXT\n\r\\");
	}
};






// frequency in Hz, plays for a short duration
void music_note(unsigned int frequency, unsigned int duration, unsigned char channel)
{
	
	return;
};

char input_ps2_keyboard()
{
	char value = 0x00;
	
	for (unsigned char p=0; p<2; p++)
	{
		if (ps2_ready[p] == 0x01 && ps2_mode[p] == 0x00) // ready and keyboard
		{
			if (ps2_readpos[p] != ps2_writepos[p])
			{
				if (ps2_state_array[p][ps2_readpos[p]] == 0x0B) // release
				{
					ps2_readpos[p]++;
					
					while (ps2_readpos[p] == ps2_writepos[p]) { }
					
					ps2_readpos[p]++; // skip
				}
				else
				{
					value = ps2_state_array[p][ps2_readpos[p]]; // press

					ps2_readpos[p]++;
				}
				
				break;
			}
		}
	}
	
	return value;
};

char input_ps2_mouse(unsigned int *array)
{
	char value = 0x00;
	
	array[0] = 0;
	array[1] = 0;
	array[2] = 0;
	array[3] = 0;
	array[4] = 0;
	
	for (unsigned char p=0; p<2; p++)
	{
		if (ps2_ready[p] == 0x01 && ps2_mode[p] == 0x01) // ready and mouse
		{
			if (ps2_readpos[p] != ps2_writepos[p])
			{
				value = 0x01;
				
				if ((ps2_state_array[p][ps2_readpos[p]] & 0x01) == 0x01) array[0] = 0x0001;
				
				if ((ps2_state_array[p][ps2_readpos[p]] & 0x02) == 0x02) array[1] = 0x0001;
				
				if ((ps2_state_array[p][ps2_readpos[p]] & 0x04) == 0x04) array[2] = 0x0001;
				
				array[3] = (unsigned int)(ps2_cursor_x[p][ps2_readpos[p]]);
				array[4] = (unsigned int)(SCREEN_Y - ps2_cursor_y[p][ps2_readpos[p]]);

				ps2_readpos[p]++;
				
				break;
			}
		}
	}
	
	return value;
};

char input_usb_keyboard()
{
	char value = 0x00;
	
	if (usb_mode == 0x00) // keyboard
	{
		if (usb_readpos != usb_writepos)
		{
			if (usb_state_array[usb_readpos] == 0x0B) // release
			{
				usb_readpos++;
					
				while (usb_readpos == usb_writepos) { }
					
				usb_readpos++; // skip
			}
			else
			{
				value = usb_state_array[usb_readpos]; // press

				usb_readpos++;
			}
		}
	}
	
	return value;
};

char input_usb_mouse(unsigned int *array)
{
	char value = 0x00;
	
	array[0] = 0;
	array[1] = 0;
	array[2] = 0;
	array[3] = 0;
	array[4] = 0;
	
	if (usb_mode == 0x01) // mouse
	{	
		if (usb_readpos != usb_writepos)
		{
			value = 0x01;

			if ((usb_state_array[usb_readpos]) == 0x01) array[0] = 0x0001;

			if ((usb_state_array[usb_readpos]) == 0x02) array[1] = 0x0001;
			
			if ((usb_state_array[usb_readpos]) == 0x03)
			{
				array[0] = 0x0001;
				array[1] = 0x0001;
			}

			if ((usb_state_array[usb_readpos]) == 0x04) array[2] = 0x0001;

			array[3] = (unsigned int)(usb_cursor_x[usb_readpos]);
			array[4] = (unsigned int)(usb_cursor_y[usb_readpos]); //(unsigned int)(384 - usb_cursor_y[usb_readpos]);

			usb_readpos++;
		}
	}
	
	return value;
};

unsigned char display_foreground_color = 0xFF;
unsigned char display_background_color = 0x00;

void display_character(unsigned int x, unsigned int y, unsigned char value)
{
	unsigned int pos = (unsigned int)(value - 32) * 64;
  
	for (unsigned int i=0; i<8; i++)
	{		
		for (unsigned int j=0; j<8; j++)
		{
			if (text_bitmap[pos+i*8+j] == 0x00)
			{
				screen_buffer[(y+i)*SCREEN_X+(x+j)] = display_background_color;
			}
			else
			{
				screen_buffer[(y+i)*SCREEN_X+(x+j)] = display_foreground_color;
			}
		}
	}
};

void display_inverse(unsigned int x, unsigned int y, unsigned char value)
{
	unsigned int pos = (unsigned int)(value - 32) * 64;
  
	for (unsigned int i=0; i<8; i++)
	{		
		for (unsigned int j=0; j<8; j++)
		{
			if (text_bitmap[pos+i*8+j] == 0x00)
			{
				screen_buffer[(y+i)*SCREEN_X+(x+j)] = display_foreground_color;
			}
			else
			{
				screen_buffer[(y+i)*SCREEN_X+(x+j)] = display_background_color;
			}
		}
	}
};
	
void display_decimal(unsigned int x, unsigned int y, unsigned int value)
{
	volatile unsigned int temp = 0;
	volatile unsigned int next = 0;
	
	next = value;
	
	temp = next / 10000;
	display_character(x, y, (unsigned char)(temp + '0'));
	next = next - 10000 * temp;
	
	temp = next / 1000;
	display_character(x + 0x08, y, (unsigned char)(temp + '0'));
	next = next - 1000 * temp;
	
	temp = next / 100;
	display_character(x + 0x10, y, (unsigned char)(temp + '0'));
	next = next - 100 * temp;
	
	temp = next / 10;
	display_character(x + 0x18, y, (unsigned char)(temp + '0'));
	next = next - 10 * temp;
	
	temp = next;
	display_character(x + 0x20, y, (unsigned char)(temp + '0'));
};

void display_string(unsigned int x, unsigned int y, char *value)
{
	volatile unsigned char pos = 0;
	
	while (value[pos] != '\\')
	{
		display_character(x + pos * 8, y, value[pos]);
		
		pos++;
	}
};



#include "menu.c"
#include "tetra.c"
#include "badapple.c"
#include "scratchpad.c"
#include "peanut_gb.c"
#include "setup.c" 



int main()
{
	Setup();
	
	DelayMS(1000); // wait some time
	
	SendChar('*'); // just a 'hello world' over the UART
	
	USBHostSetup();
	
	DelayMS(1000); // settling delay, avoid garbage characters
	
	menu_x = 24;
	menu_y = 400;
	menu_pos = 0;
	menu_max = 6; // number of menu items, change accordingly
	
	display_string(menu_x, 16, "Acolyte Hand PIC'd 32\\");

	display_string(menu_x, menu_y,		" Tetra     \\");
	display_string(menu_x, menu_y+8,	" Scratchpad\\");
	display_string(menu_x, menu_y+16,	" Bad Apple \\");
	display_string(menu_x, menu_y+24,	" GB Emu    \\");
	display_string(menu_x, menu_y+32,	" GB Burn   \\");
	display_string(menu_x, menu_y+40,	" Blink Test\\");
	
	char dummy = (char)Menu();
	
	if (dummy == 0) Tetra();
	else if (dummy == 1) Scratchpad();
	else if (dummy == 2) BadApple();
	else if (dummy == 3) PeanutGB();
	else if (dummy == 4) BurnROM();
	else if (dummy == 5) { }

	
	
	/*
	// sox OriginalBadApple.wav -c 1 -b 32 -r 56476 -e signed-integer Test.wav
	// sudo fdisk -l
	// *** Make sure it is /dev/sdc you want, change according to what 'fdisk' told you! ***
	// sudo dd if=Test.wav of=/dev/sdc bs=100M conv=fsync
	
	sdcard_initialize();
	
	int test = 0;
	int prev = 0;

	for (int i=0; i<5; i++)
	{
		test = sdcard_initialize();

		if (test == 1) break;
	}

	if (test > 0)
	{
		TRISH = 0x0000;
		PORTH = 0x0000;
		
		unsigned char temp_value = 0x00;

		sdcard_disable();
		sdcard_pump();
		sdcard_longdelay(); // this is probably not needed
		sdcard_enable();
		sdcard_sendbyte(0x52); // CMD18 = 0x40 + 0x12 (18 in hex)
		sdcard_sendbyte((0x0000&0x00FF));
		sdcard_sendbyte(((0x0000&0xFF00) >> 8));
		sdcard_sendbyte((0x0000&0x00FE)); // only blocks of 512 bytes
		sdcard_sendbyte(0x00);
		sdcard_sendbyte(0x01); // CRC (general)
		temp_value = sdcard_waitresult(); // command response
		if (temp_value == 0xFF) { return 0; }
		else if (temp_value != 0x00) { return 0; } // expecting 0x00
		temp_value = sdcard_waitresult(); // data packet starts with 0xFE
		if (temp_value == 0xFF) { return 0; }
		else if (temp_value != 0xFE) { return 0; }
		
		while (1)
		{
			if (screen_scanline != prev)
			{
				prev = screen_scanline;
				
				// 32-bit signed 1-channel
				sdcard_receivebyte();
				sdcard_receivebyte();
				sdcard_receivebyte();
	 			// Should add 0x08 before shifting in case of rounding???
				PORTH = (((sdcard_receivebyte() >> 4) + 0x08) & 0x0F); 
				
				// 16-bit signed 2-channel
				//sdcard_receivebyte();
				//PORTH = (((sdcard_receivebyte() >> 4) + 0x08) & 0x0F);
				//sdcard_receivebyte();
				//sdcard_receivebyte();
				
				// 8-bit unsigned 2-channel
				//temp_value = sdcard_receivebyte();
				//PORTH = ((((unsigned int)(temp_value + sdcard_receivebyte()) >> 5) + 0x0000) & 0x000F);
			}
		}	
	}
	*/
	
	
	
	
	
	while (1)
	{
		// blink LED
		PORTDbits.RD11 = 0;
		DelayMS(100);
		DelayMS(100);
		DelayMS(100);
		DelayMS(100);
		DelayMS(100);

		PORTDbits.RD11 = 1;
		DelayMS(100);
		DelayMS(100);
		DelayMS(100);
		DelayMS(100);
		DelayMS(100);
	}
}

// Unused code below
/*
   char dummy = 0x00;

	// infinite loop
	while (1)
	{
		dummy = input_ps2_keyboard();
		
		if (dummy != 0x00)
		{
			while (U3STAbits.UTXBF == 1) { }
			U3TXREG = dummy;
		}
	}
*/
/*	
	unsigned int tune[16*2] = {
		0x063A,	500,
		0x0768,	500,
		0x06FD,	250,
		0x058C,	250,
		0x058C,	500,
		0x058C,	1000,
		0x058C,	1000,
		0x063A,	250,
		0x0850,	250,
		0x06FD,	250,
		0x058C,	250,
		0x058C,	500,
		0x058C,	250,
		0x05FD,	250,
		0x058C,	250,
		0x04F1,	250,
	};
	
	unsigned int pos = 0;
	unsigned int dummy = 0;
	
	while (1)
	{
		music_note(tune[pos], tune[pos+1] - 50, 0);
		
		while (audio_counter[0] > 0) { }
		
		music_note(0xFFFF, 50, 0);
		
		while (audio_counter[0] > 0) { }
		
		pos += 2;
		
		if (pos >= 32)
		{
			for (unsigned int i=0; i<10000; i++)
			{
				for (unsigned int j=0; j<10000; j++)
				{
					dummy++;
				}
			}
			
			pos = 0;
		}
	}
*/	
/*		
		// check if character is ready
		if(U3STAbits.URXDA == 1)
		{
			char dummy = U3RXREG; // get character
			
			U3TXREG = dummy; // echo character
			
			// blink LED
			//PORTDbits.RD11 = 0;
			//DelayMS(250);
			//PORTDbits.RD11 = 1;
		}
*/
/*
		// blink LED
		PORTDbits.RD11 = 0;
		DelayMS(100);

		PORTDbits.RD11 = 1;
		DelayMS(100);
*/
/*
    // Wait for the disk to initialise
    while(disk_initialize(0));
    // Mount the disk
    f_mount(&fso, "", 0);
    // Change dir to the root directory
    f_chdir("/");
    // Open the directory
    f_opendir(&dir, ".");
 
	char buffer[16];
	unsigned int bytes;
	unsigned int result;
	
	for (unsigned int i=0; i<16; i++)
	{
		buffer[i] = 0;
	};
	
	// read in some text
	result = f_open(&file, "/INPUT.TXT", FA_READ);
	if (result == 0)
	{
		while (f_read(&file, buffer, 16, &bytes) == 1) { }
		while (f_close(&file) == 1) { }
	}
	
	// write the same text back
	result = f_open(&file, "/OUTPUT.TXT", FA_WRITE);
	if (result != 0)
	{
		result = f_open(&file, "/OUTPUT.TXT", FA_CREATE_NEW);
		if (result == 0)
		{
			while (f_close(&file) == 1) { }
			result = f_open(&file, "/OUTPUT.TXT", FA_WRITE);
		}
	}
	if (result == 0)
	{
		while (f_write(&file, buffer, 16, &bytes) == 1) { }
		while (f_close(&file) == 1) { }
	}
*/
		/*
		for (unsigned int i=0; i<0x8000; i++)
		{
			if (i % 16 == 0)
			{
				SendLongHex(i);
				SendChar(':');
			}
			
			SendHex(cart_ram[i]);
			SendChar(' ');
			
			if (i % 16 == 15)
			{
				SendChar('\r');
				SendChar('\n');
			}
		}
		*/