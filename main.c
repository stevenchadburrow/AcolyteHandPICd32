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
#pragma config FECCCON = OFF_UNLOCKED //ON             // Dynamic Flash ECC Configuration (Flash ECC is enabled (ECCCON bits are locked))
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



// comment out if you want to program the PIC32 faster
#define SPLASH

#ifdef SPLASH
#include "splash_default.c"
#endif



#define SYS_FREQ 150000000 // Running at 150MHz

unsigned long VirtToPhys(volatile void* p) // changed 'const' to 'volatile'
{
	return (long)p<0?((long)p&0x1fffffffL):(unsigned long)((unsigned char*)p+0x40000000L);
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





#define SCREEN_X 512
#define SCREEN_Y 384

// globally accessable variables
volatile unsigned char __attribute__((coherent,address(0x80004000))) screen_buffer[SCREEN_Y*SCREEN_X]; //screen_buffer[300][400]; // visible portion of screen
volatile char ps2_state_array[2][256];
volatile unsigned int ps2_cursor_x[2][256];
volatile unsigned int ps2_cursor_y[2][256];
volatile unsigned char ps2_mode[2] = { 0x00, 0x00 };
volatile unsigned char ps2_writepos[2] = { 0x00, 0x00 };
volatile unsigned char ps2_readpos[2] = { 0x00, 0x00 };

volatile char usb_state_array[256];
volatile unsigned int usb_cursor_x[256];
volatile unsigned int usb_cursor_y[256];
volatile unsigned int usb_buttons[256];
volatile unsigned char usb_mode = 0x00;
volatile unsigned char usb_writepos = 0x00;
volatile unsigned char usb_readpos = 0x00;


// additional variables
volatile unsigned char screen_blank[SCREEN_X]; // black scanline
volatile unsigned int screen_scanline = 771; // start of vertical sync
volatile unsigned char screen_zero[1] = { 0x00 }; // zero value for black

volatile unsigned int audio_counter[2] = { 0, 0 }; // audio duration

volatile unsigned int ps2_port = 0x0000;
volatile unsigned int ps2_flags = 0x0000;

// PS/2 keyboard variables
volatile unsigned char ps2_counter[2] = { 0x00, 0x00 };
volatile unsigned char ps2_buffer[2] = { 0x00, 0x00 };
volatile unsigned char ps2_sequence[2] = { 0xFF, 0xFF }; // starts with 0xFA acknowledge byte
volatile unsigned char ps2_shift[2] = { 0x00, 0x00 };
volatile unsigned char ps2_release[2] = { 0x00, 0x00 };
volatile unsigned char ps2_extended[2] = { 0x00, 0x00 };
volatile unsigned char ps2_ready[2] = { 0x00, 0x00 };
volatile unsigned char ps2_check[2] = { 0x00, 0x00 };


#include "tables.c"

#include "usb_ab.c"





/*
VESA Signal 1024 x 768 @ 70 Hz timing

General timing
Screen refresh rate	70 Hz
Vertical refresh	56.475903614458 kHz
Pixel freq.	75.0 MHz
Horizontal timing (line)
Polarity of horizontal sync pulse is negative.

Scanline part	Pixels	Time [µs]
Visible area	1024	13.653333333333
Front porch	24	0.32
Sync pulse	136	1.8133333333333
Back porch	144	1.92
Whole line	1328	17.706666666667
Vertical timing (frame)
Polarity of vertical sync pulse is negative.

Frame part	Lines	Time [ms]
Visible area	768	13.59872
Front porch	3	0.05312
Sync pulse	6	0.10624
Back porch	29	0.51349333333333
Whole frame	806	14.271573333333
*/


/*	
Interrupt from Visible Timer at beginning of scanline, DMA0 to start.
DMA0 transfers single byte to DMA1 config to enable it.
DMA1 transfers ~400 pixels to PMP on Pixel Timer.
DMA1 chains to DMA2 which transfers a zero to PMP or PORTx.
All of this is independent of sync signals and timers!
Increment line number and change addresses on separate interrupt!
This setup *must* have a 74HCT273 and the pixel clock is also using an OC
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


//#include <string.h>



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


void __attribute__((vector(_OUTPUT_COMPARE_3_VECTOR), interrupt(ipl7srs))) oc3_handler()
{		
    IFS0bits.OC3IF = 0;  // clear interrupt flag
	
	PORTE = 0;
	
	USBA_device_millis_delay = USBA_device_millis_delay + 1;
	
	if (USBA_device_millis_delay >= 56) // should be 56.475 really
	{
		USBA_device_millis_delay = 0;
		USBA_device_millis++;
	}
	
	screen_scanline = screen_scanline + 1; // increment scanline
	
	if (screen_scanline == 806) screen_scanline = 0;
	
	if (screen_scanline < 768)
	{		
		DCH0INTbits.CHBCIF = 0; // clear transfer complete flag
		DCH0SSA = VirtToPhys(screen_buffer + (screen_scanline>>1)*SCREEN_X); // transfer source physical address
		DCH0CONbits.CHEN = 1; // enable channel
	}
	else if (screen_scanline == 805)
	{
		DCH0INTbits.CHBCIF = 0; // clear transfer complete flag
		DCH0SSA = VirtToPhys(screen_blank); // transfer source physical address
		DCH0CONbits.CHEN = 1; // enable channel
	}
	
	return;
}

void __attribute__((vector(_CHANGE_NOTICE_D_VECTOR), interrupt(ipl5srs))) cnd_handler()
{
	IFS3bits.CNDIF = 0;  // clear interrupt flag
	
	ps2_port = PORTD;
	ps2_flags = CNFD;
	
	CNFD = 0x0000; // must clear out the flags when done
	
	if (((ps2_flags & 0x0200) == 0x0200)) // PS/2 Port 0
	{	
		if (ps2_counter[0] < 0x09)
		{
			ps2_buffer[0] = ps2_buffer[0] >> 1;
			
			if ((ps2_port & 0x0400) == 0x0400)
			{
				ps2_buffer[0] = ps2_buffer[0] | 0x80;
			} 
		}

		ps2_counter[0] = ps2_counter[0] + 1;
	}
	
	if (((ps2_flags & 0x1000) == 0x1000)) // PS/2 Port 1
	{
		if (ps2_counter[1] < 0x09)
		{
			ps2_buffer[1] = ps2_buffer[1] >> 1;
			
			if ((ps2_port & 0x2000) == 0x2000)
			{
				ps2_buffer[1] = ps2_buffer[1] | 0x80;
			} 
		}

		ps2_counter[1] = ps2_counter[1] + 1;
	}
		
	for (unsigned char p=0; p<2; p++)
	{
		if (ps2_counter[p] == 0x0B)
		{
			//SendHex((unsigned char)ps2_buffer[p]); // TEMPORARY!!!
			
			ps2_counter[p] = 0x00;

			if (ps2_ready[p] == 0x00)
			{
				ps2_cursor_x[p][0xFF] = 0x0000; // set initial cursor position
				ps2_cursor_y[p][0xFF] = 0x0000;
				
				if (ps2_buffer[p] == 0xAA)
				{
					ps2_ready[p] = 0x01; // should be 0xAA byte
				}
				else if (ps2_buffer[p] == 0xFA) // acknowledge
				{
					// do nothing
				}
				else
				{
					ps2_command(0xFF, p); // reset device with 0xFF command
				}
			}
			else if (ps2_mode[p] == 0x00) // keyboard routines
			{
				if (ps2_check[p] == 0x00) // check if keyboard or mouse
				{
					if (ps2_buffer[p] == 0x00) // if 0x00, it is a mouse
					{	
						ps2_command(0xF4, p); // enable output command 0xF4
						
						ps2_mode[p] = 0x01; // make it a mouse
					}
					
					ps2_check[p] = 0x01;
				}
				
				if (ps2_buffer[p] == 0x00) // null character, probably from mouse
				{
					// do nothing
				}
				else if (ps2_buffer[p] == 0xF0) // release
				{
					ps2_release[p] = 0xF0;
				}
				else if (ps2_buffer[p] == 0xE0) // extended
				{
					ps2_extended[p] = 0xE0;
				}
				else // normal
				{
					if (ps2_release[p] == 0x00)
					{
						if (ps2_extended[p] == 0xE0)
						{
							ps2_state_array[p][ps2_writepos[p]] = ps2_conversion[(unsigned char)(ps2_buffer[p] + ps2_shift[p] + 0x80)];

							ps2_writepos[p] = ps2_writepos[p] + 1;

							ps2_extended[p] = 0x00;
						}
						else
						{
							if (ps2_buffer[p] == 0x12 || ps2_buffer[p] == 0x59)
							{
								ps2_shift[p] = 0x80;
							}
							else
							{
								ps2_state_array[p][ps2_writepos[p]] = ps2_conversion[(unsigned char)(ps2_buffer[p] + ps2_shift[p])];

								ps2_writepos[p] = ps2_writepos[p] + 1;
							}
						}
					}
					else
					{
						if (ps2_extended[p] == 0xE0)
						{
							ps2_state_array[p][ps2_writepos[p]] = 0x0B; // release character, ignore next character if needed
							ps2_writepos[p] = ps2_writepos[p] + 1;
							
							ps2_state_array[p][ps2_writepos[p]] = ps2_conversion[(unsigned char)(ps2_buffer[p] + ps2_shift[p] + 0x80)];
							ps2_writepos[p] = ps2_writepos[p] + 1;

							ps2_extended[p] = 0x00;
						}
						else
						{
							if (ps2_buffer[p] == 0x12 || ps2_buffer[p] == 0x59)
							{
								ps2_shift[p] = 0x00;
							}
							else
							{
								ps2_state_array[p][ps2_writepos[p]] = 0x0B; // release character, ignore next character if needed
								ps2_writepos[p] = ps2_writepos[p] + 1;
								
								ps2_state_array[p][ps2_writepos[p]] = ps2_conversion[(unsigned char)(ps2_buffer[p] + ps2_shift[p])];
								ps2_writepos[p] = ps2_writepos[p] + 1;
							}
						}

						ps2_release[p] = 0x00;
						ps2_extended[p] = 0x00;
					}
				}
			}
			else if (ps2_mode[p] == 0x01) // mouse
			{
				if (ps2_sequence[p] == 0xFF) // acknowledge 0xFA
				{
					ps2_sequence[p] = 0x00;
				}
				else if (ps2_sequence[p] == 0x00) // buttons
				{
					ps2_state_array[p][ps2_writepos[p]] = ps2_buffer[p];

					ps2_sequence[p] = 0x01;
				}
				else if (ps2_sequence[p] == 0x01) // x movement
				{
					if ((signed char)(ps2_buffer[p]) < 0)
					{
						if (ps2_cursor_x[p][(unsigned char)(ps2_writepos[p]-1)] < 0 - (signed char)(ps2_buffer[p]))
						{
							ps2_cursor_x[p][ps2_writepos[p]] = 0;
						}
						else
						{
							ps2_cursor_x[p][ps2_writepos[p]] = 
								ps2_cursor_x[p][(unsigned char)(ps2_writepos[p]-1)] + (signed char)(ps2_buffer[p]);
						}
					}
					else if ((signed char)(ps2_buffer[p]) >= 0)
					{
						if (ps2_cursor_x[p][(unsigned char)(ps2_writepos[p]-1)] >= SCREEN_X - (signed char)(ps2_buffer[p]))
						{
							ps2_cursor_x[p][ps2_writepos[p]] = SCREEN_X;
						}
						else
						{
							ps2_cursor_x[p][ps2_writepos[p]] = 
								ps2_cursor_x[p][(unsigned char)(ps2_writepos[p]-1)] + (signed char)(ps2_buffer[p]);
						}
					}

					ps2_sequence[p] = 0x02;
				}
				else if (ps2_sequence[p] == 0x02) // y movement
				{
					if ((signed char)(ps2_buffer[p]) < 0)
					{
						if (ps2_cursor_y[p][(unsigned char)(ps2_writepos[p]-1)] < 0 - (signed char)(ps2_buffer[p]))
						{
							ps2_cursor_y[p][ps2_writepos[p]] = 0;
						}
						else
						{
							ps2_cursor_y[p][ps2_writepos[p]] = 
								ps2_cursor_y[p][(unsigned char)(ps2_writepos[p]-1)] + (signed char)(ps2_buffer[p]);
						}
					}
					else if ((signed char)(ps2_buffer[p]) >= 0)
					{
						if (ps2_cursor_y[p][(unsigned char)(ps2_writepos[p]-1)] >= SCREEN_Y - (signed char)(ps2_buffer[p]))
						{
							ps2_cursor_y[p][ps2_writepos[p]] = SCREEN_Y;
						}
						else
						{
							ps2_cursor_y[p][ps2_writepos[p]] = 
								ps2_cursor_y[p][(unsigned char)(ps2_writepos[p]-1)] + (signed char)(ps2_buffer[p]);
						}
					}

					ps2_sequence[p] = 0x00;

					ps2_writepos[p] = ps2_writepos[p] + 1;
				}
			}
		}		
	}
	
	return;
}

void __attribute__((vector(_UART3_RX_VECTOR), interrupt(ipl4srs))) u3rx_handler() //, nomips16)) u3rx_handler()
{	
	IFS4bits.U3RXIF = 0;  // clear interrupt flag
	
	// check for errors
	if(U3STAbits.FERR == 1)
	{
		return;
	}
		
	// clear overrun error
	if(U3STAbits.OERR == 1)
	{
		U3STAbits.OERR = 0;
		return;
	}
	
	// check if there is a character ready to read
	if(U3STAbits.URXDA == 1)
	{
		unsigned char data = U3RXREG; // get character
		
		if (data == '*') // debug?
		{
			
		}
		else
		{
			U3TXREG = data; // echo character received
		}
	}
	
	return;
}

void __attribute__((vector(_TIMER_8_VECTOR), interrupt(ipl3srs))) t8_handler()
{
	IFS1bits.T8IF = 0;  // clear interrupt flag
	
	if (audio_counter[0] > 0)
	{
		audio_counter[0] = audio_counter[0] - 1;
	}
	else
	{
		T6CONbits.ON = 0; // turn off timers
		T8CONbits.ON = 0; // turn off timers
		OC8CONbits.ON = 0; // turn OC8 off
	}
}

void __attribute__((vector(_TIMER_9_VECTOR), interrupt(ipl3srs))) t9_handler()
{
	IFS1bits.T9IF = 0;  // clear interrupt flag
	
	if (audio_counter[1] > 0)
	{
		audio_counter[1] = audio_counter[1] - 1;
	}
	else
	{
		T7CONbits.ON = 0; // turn off timers
		T9CONbits.ON = 0; // turn off timers
		OC9CONbits.ON = 0; // turn OC9 off
	}
}


// frequency in Hz, plays for a short duration
void music_note(unsigned int frequency, unsigned int duration, unsigned char channel)
{
	unsigned int period = (unsigned int)(SYS_FREQ / (64 * frequency));
	
	if (channel == 0)
	{
		T6CONbits.ON = 0; // turn off timers
		T8CONbits.ON = 0; // turn off timers

		TMR6 = 0;
		PR6 = period;
		TMR8 = 0;
		PR8 = (unsigned int)(SYS_FREQ / 256000);

		audio_counter[0] = duration;

		T6CONbits.ON = 1; // turn on timers
		T8CONbits.ON = 1; // turn on timers

		OC8CONbits.ON = 1; // turn OC8 on
	}
	else if (channel == 1)
	{
		T7CONbits.ON = 0; // turn off timers
		T9CONbits.ON = 0; // turn off timers

		TMR7 = 0;
		PR7 = period;
		TMR9 = 0;
		PR9 = (unsigned int)(SYS_FREQ / 256000);

		audio_counter[1] = duration;

		T7CONbits.ON = 1; // turn on timers
		T9CONbits.ON = 1; // turn on timers

		OC9CONbits.ON = 1; // turn OC8 on
	}
	
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
					ps2_readpos[p] += 2; // skip
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
	
	if (usb_mode == 0x01) // keyboard
	{
		if (usb_readpos != usb_writepos)
		{
			value = usb_state_array[usb_readpos];
			usb_readpos++;
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
	
	if (usb_mode == 0x02) // mouse
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

void display_character(unsigned int x, unsigned int y, unsigned char value)
{
	unsigned int pos = (unsigned int)(value - 32) * 64;
  
	for (unsigned int i=0; i<8; i++)
	{		
		for (unsigned int j=0; j<8; j++)
		{
			screen_buffer[(y+i)*SCREEN_X+(x+j)] = (text_bitmap[pos+i*8+j]);
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
			screen_buffer[(y+i)*SCREEN_X+(x+j)] = (unsigned char)((text_bitmap[pos+i*8+j] ^ 0xFF));
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











// must come before the ff.c and diskio.c files
#include "sdcard.c"


#include "ffconf.h"
#include "ff.h"
#include "ff.c"
#include "diskio.h"
#include "diskio.c"


// Global variables
FIL file; // File handle for the file we open
DIR dir; // Directory information for the current directory
FATFS fso; // File System Object for the file system we are reading from

void EchoFile()
{
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
		
		while (f_read(&file, buffer, 16, &bytes) == 1) { }
		while (f_close(&file) == 1) { }
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
			while (f_close(&file) == 1) { }
			result = f_open(&file, "/OUTPUT.TXT", FA_WRITE);
		}
	}
	if (result == 0)
	{	
		while (f_write(&file, buffer, 16, &bytes) == 1) { }
		while (f_close(&file) == 1) { }
		
		SendString("Copied to OUTPUT.TXT\n\r\\");
	}
	else
	{
		SendString("Could not copy to OUTPUT.TXT\n\r\\");
	}
};

unsigned int NVMUnlock(unsigned int nvmop)
{
	// Suspend or Disable all Interrupts
	asm("di");
	// Enable Flash Write/Erase Operations and Select
	// Flash operation to perform
	NVMCON = nvmop;
	// Write Keys
	NVMKEY = 0xAA996655;
	NVMKEY = 0x556699AA;
	// Start the operation using the Set Register
	NVMCONSET = 0x8000;
	// Wait for operation to complete
	while (NVMCON & 0x8000);
	// Restore Interrupts
	asm("ei");
	// Disable NVM write enable
	NVMCONCLR = 0x0004000;
	// Return WRERR and LVDERR Error Status Bits
	return (NVMCON & 0x3000);
}

unsigned int NVMErasePage(unsigned long address)
{
	unsigned int res;
	// Load address to program into NVMADDR register
	NVMADDR = (unsigned long) address;
	// Unlock and Erase Page
	res = NVMUnlock (0x4004);
	// Return Result
	return res;
}

unsigned int NVMEraseAll()
{
	unsigned int res;
	// Unlock and Erase All
	res = NVMUnlock (0x4007);
	// Return Result
	return res;
}

unsigned int NVMWriteSingleWord(unsigned long address, unsigned long data0)
{	
	unsigned int res;
	// Load data into NVMDATA register
	NVMDATA0 = data0;
	// Load address to program into NVMADDR register
	NVMADDR = (unsigned long) address;
	// Unlock and Write Single Word
	res = NVMUnlock (0x4001); // ECC must be turned off!
	// Return Result
	return res;
}

unsigned int NVMWriteQuadWord(unsigned long address, 
	unsigned long data0, unsigned long data1, unsigned long data2, unsigned long data3)
{
	unsigned int res;
	// Load data into NVMDATA register
	NVMDATA0 = data0;
	NVMDATA1 = data1;
	NVMDATA2 = data2;
	NVMDATA3 = data3;
	// Load address to program into NVMADDR register
	NVMADDR = (unsigned long) address;
	// Unlock and Write Quad Word
	res = NVMUnlock (0x4002);
	// Return Result
	return res;
}

#pragma region name="user_ram" origin=0x80040000 size=0x00040000 
#pragma region name="user_rom" origin=0x9D080100 size=0x0017FF00
#define USER_RAM __attribute__((region("user_ram")))
#define USER_ROM __attribute__((region("user_rom")))
#define USER_JUMP __attribute__((address(0x9D080000)))
#define REPROGRAM_BEGIN 0x1D080000
#define REPROGRAM_END 0x1D200000

void ReprogramCode()
{	
	// #The SDcard must have been formatted
	// #Check which drive it is, here /dev/sdc
	// sudo fdisk -l
	// sudo umount /dev/sdc
	// sudo mkfs.vfat /dev/sdc
	
	// Wait for the disk to initialise
    while(disk_initialize(0));
    // Mount the disk
    f_mount(&fso, "", 0);
    // Change dir to the root directory
    f_chdir("/");
    // Open the directory
    f_opendir(&dir, ".");
	
	char buffer[1];
	char line[256];
	unsigned int pos = 0;
	unsigned int bytes = 1;
	unsigned int result = 0;
	
	unsigned long high_address = 0x0;
	unsigned long low_address = 0x0;
	unsigned char single = 0x0;
	unsigned long word[4] = { 0x0, 0x0, 0x0, 0x0 };
	unsigned char num = 0;
	
	for (unsigned int i=0; i<256; i++) line[i] = '0';
	pos = 0;
	
	result = f_open(&file, "/CODE.HEX", FA_READ);
	
	SendString("\n\r\\");
	
	if (result == 0)
	{
		SendString("Found CODE.HEX, reprogramming now\n\r\\");
		
		//NVMEraseAll();
		
		for (unsigned long i=REPROGRAM_BEGIN; i<REPROGRAM_END; i+=0x00001000) // ???
		{
			NVMErasePage(i);
		}
		
		buffer[0] = 0;
		
		while (bytes > 0 && buffer[0] != ':')
		{
			while (f_read(&file, buffer, 1, &bytes) == 1) { } // colon
		}
		
		while (bytes > 0)
		{
			while (f_read(&file, buffer, 1, &bytes) == 1) { }
			
			if (bytes > 0)
			{
				if (buffer[0] == ':')
				{	
					low_address = 0x0000;
					
					for (unsigned int i=0; i<4; i++)
					{
						low_address = (low_address << 4);
						
						if (line[i+2] >= 'a') low_address += (unsigned int)(line[i+2] - 'a' + 10);
						else low_address += (unsigned int)(line[i+2] - '0');
					}
					
					if (line[6] == '0' && line[7] == '0') // data
					{	
						for (unsigned char i=0; i<4; i++) word[i] = 0x00000000;
						
						if (line[0] == '1' && line[1] == '0') num = 4;
						else if (line[0] == '0' && line[1] == 'C') num = 3;
						else if (line[0] == '0' && line[1] == '8') num = 2;
						else if (line[0] == '0' && line[1] == '4') num = 1;
						
						for (unsigned char i=0; i<num; i++)
						{
							for (unsigned char j=0; j<8; j+=2)
							{
								word[i] = (word[i] >> 8);
								
								single = 0x00;
								
								if (line[i*8+j+8] >= 'a') single += (unsigned long)(line[i*8+j+8] - 'a' + 10) * 0x10;
								else single += (unsigned long)(line[i*8+j+8] - '0') * 0x10;
								
								if (line[i*8+j+1+8] >= 'a') single += (unsigned long)(line[i*8+j+1+8] - 'a' + 10) * 0x01;
								else single += (unsigned long)(line[i*8+j+1+8] - '0') * 0x01;
								
								word[i] += single * 0x01000000;
							}
						}
						
						if (high_address >= (REPROGRAM_BEGIN >> 16) && high_address < (REPROGRAM_END >> 16)) // range where it will re-program flash
						{
							SendChar('.');

							//NVMWriteQuadWord((unsigned long)((high_address << 16) + low_address), 
							//	word[0], word[1], word[2], word[3]);
							
							for (int i=0; i<num; i++)
							{
								NVMWriteSingleWord((unsigned long)((high_address << 16) + low_address), word[i]);
								
								low_address += 4;
							}
						}
					}
					else if (line[6] == '0' && line[7] == '1') // eof
					{
						
					}
					else if (line[6] == '0' && line[7] == '4') // high address
					{	
						high_address = 0x0000;
						
						for (unsigned int i=0; i<4; i++)
						{
							high_address = (high_address << 4);
							
							if (line[i+8] >= 'a') high_address += (unsigned int)(line[i+8] - 'a' + 10);
							else high_address += (unsigned int)(line[i+8] - '0');
						}
					}
					else
					{
				
					}
					
					for (unsigned int i=0; i<256; i++) line[i] = '0';
					pos = 0;
				}
				else
				{
					line[pos] = buffer[0];
					pos++;
				}
			}
		}

		while (f_close(&file) == 1) { }
		
		SendString("\r\n\\");
		
		SendString("Please reset computer now\r\n\\");
	}
	else
	{
		SendString("Could not find CODE.HEX\r\n\\");
	}
	
	DelayMS(1000);
	
	// soft reset system
	SYSKEY = 0x0; // reset
	SYSKEY = 0xAA996655; // unlock key #1
	SYSKEY = 0x556699AA; // unlock key #2
	RSWRST = 1; // set bit to reset of system
	SYSKEY = 0x0; // re-lock
	RSWRST; // read from register to reset
	while (1) { } // wait until reset occurs
};





void USER_ROM testing_hex(unsigned char value)
{
	while (U3STAbits.UTXBF == 1) { }
	if (value/16 >= 10) U3TXREG = (char)(value/16 + 'A' - 10);
	else U3TXREG = (char)(value/16 + '0');
	while (U3STAbits.UTXBF == 1) { }
	if (value%16 >= 10) U3TXREG = (char)(value%16 + 'A' - 10);
	else U3TXREG = (char)(value%16 + '0');
};

unsigned int USER_RAM testing_value;

void USER_ROM testing_function()
{
	testing_value = 0x4000;
	
	for (unsigned int i=0; i<256; i++) testing_value += i;
	
	testing_hex((unsigned char)(testing_value / 256));
	
	testing_hex((unsigned char)(testing_value % 256));
}

// must be a very small function
void USER_JUMP UserCode()
{
	testing_function();
};





#include "tetra.c"
#include "badapple.c"
#include "scratchpad.c"
#include "menu.c"



int main()
{
	char dummy = 0x00;
	
	// turn off analog
	ANSELA = 0x0;
	ANSELB = 0x0;
	ANSELC = 0x0;
	ANSELD = 0x0;
	ANSELE = 0x0;
	ANSELF = 0x0;
	ANSELG = 0x0;
	ANSELH = 0x0;
	//ANSELI = 0x0;
	ANSELJ = 0x0;
	//ANSELK = 0x0;
	
	// make everything output
	TRISA = 0x0;
	TRISB = 0x0;
	TRISC = 0x0;
	TRISD = 0x0;
	TRISE = 0x0;
	TRISF = 0x0;
	TRISG = 0x0;
	TRISH = 0x0;
	//TRISI = 0x0;
	TRISJ = 0x0;
	TRISK = 0x0;
	
	// set all ports to ground
	PORTA = 0x0;
	PORTB = 0x0;
	PORTC = 0x0;
	PORTD = 0x0;
	PORTE = 0x0;
	PORTF = 0x0;
	PORTG = 0x0;
	PORTH = 0x0;
	//PORTI = 0x0;
	PORTJ = 0x0;
	PORTK = 0x0;
	
	// disable all interrupts
	IEC0 = 0x0;
	IEC1 = 0x0;
	IEC2 = 0x0;
	IEC3 = 0x0;
	IEC4 = 0x0;
	IEC5 = 0x0;
	IEC6 = 0x0;
	//IEC7 = 0x0;
	
	// clear all flags
	IFS0 = 0x0;
	IFS1 = 0x0;
	IFS2 = 0x0;
	IFS3 = 0x0;
	IFS4 = 0x0;
	IFS5 = 0x0;
	IFS6 = 0x0;
	//IFS7 = 0x0;
	
	
	// Set up caching
	unsigned int cp0 = _mfc0(16, 0);
	cp0 &= ~0x07;
	cp0 |= 0b011; // K0 = Cacheable, non-coherent, write-back, write allocate
	//cp0 &= ~0x03;
	//cp0 |= 0x02; // K0 = Uncachable
	_mtc0(16, 0, cp0);  
	
	 
	// change tri-state on pins
	TRISD = 0xB640; // MISO, KEY, LED, MOUSE, and UART
	TRISJ = 0xFCFF; // JOY-A, JOY-B, and BUTTON
	CNPUD = 0xC000; // pull-ups for UART

	// set oscillator and timers
	SYSKEY = 0x0; // reset
	SYSKEY = 0xAA996655; // unlock key #1
	SYSKEY = 0x556699AA; // unlock key #2
	
	CFGCONbits.DMAPRI = 1; // DMA does have highest priority (?)
	CFGCONbits.CPUPRI = 0; // CPU does not have highest priority (?)
	CFGCONbits.OCACLK = 1; // use alternate OC/TMR table
	
	PB1DIV = 0x00008001; // divide by 2
	PB2DIV = 0x00008007; //0x00008003; // change PB2 clock to 150 / 8 = 18.75 MHz for SPI and UART
	PB3DIV = 0x00008000; // set OC and TMR clock division by 1
	PB4DIV = 0x00008001; // divide by 2
	PB5DIV = 0x00008001; // divide by 2
	//PB6DIV = 0x00008001; // divide by 2
	PB7DIV = 0x00008000; // CPU clock divide by 1
	SPLLCON = 0x01310301; //0x02270203; // use PLL to bring external 24 MHz into 150 MHz
	
	// PRECON - Set up prefetch
	PRECONbits.PFMSECEN = 0; // Flash SEC Interrupt Enable (Do not generate an interrupt when the PFMSEC bit is set)
	PRECONbits.PREFEN = 0b11; // Predictive Prefetch Enable (Enable predictive prefetch for any address)
	PRECONbits.PFMWS = 0b010; // PFM Access Time Defined in Terms of SYSCLK Wait States (Two wait states)
    
	CFGCONbits.USBSSEN = 1; // USB?

	
	OSCCONbits.SLPEN = 0; // WAIT instruction puts CPU into idle mode
	OSCCONbits.NOSC = 0x1; // switch to SPLL
	OSCCONbits.OSWEN = 1; // enable the switch
	SYSKEY = 0x0; // re-lock
	
	while (OSCCONbits.OSWEN != 0) { } // wait for clock switch to complete

	// set PPS
	SYSKEY = 0x0; // reset
	SYSKEY = 0xAA996655; // unlock key #1
	SYSKEY = 0x556699AA; // unlock key #2
	CFGCONbits.IOLOCK = 0; // PPS is unlocked
	RPD0R = 0xC; // OC1 on pin RPD0 (pixel clock)
	RPD5R = 0xB; // OC2 on pin RPD5 (visible clock)
	RPD2R = 0xB; // OC3 on pin RPD2 (h-sync)
	RPD3R = 0xB; // OC4 on pin RPD3 (v-sync)
	SDI1R = 0xE; // SDI1 on pin RPD6 (miso)
	RPD7R = 0x5; // SDO1 on pin RPD7 (mosi)
	// SCK1 on RD1 (sclk)
	// CS1 on RD4 (cs)
	RPE9R = 0xC; // OC8 on pin RPE9 (audio 1)
	RPE8R = 0xD; // OC9 on pin RPE8 (audio 2)
	RPD14R = 0x1; // U3TX on pin RPD14 (uart tx)
	U3RXR = 0xB; // U3RX on pin RPD15 (uart rx)
	CFGCONbits.IOLOCK = 1; // PPS is locked
	SYSKEY = 0x0; // re-lock
	
	// set OC1 and TMR4, pixel clock
	// I've had to adjust these values many times...
	OC1CON = 0x0; // reset OC1
	OC1CON = 0x00000003; // toggle, use Timer4
	OC1R = 0x0001; //0x0000; // pixel-sync rise (adjust)
	OC1RS = 0x0001; //0x0000; // pixel-sync fall (adjust)
	T4CON = 0x0; // rest Timer4, prescale of 1:1
	TMR4 = 0x0; // zero out counter
	PR4 = 0x01; //0x01; // pixel-reset (minus one)
	
	// set OC2 and OC3 and TMR5, horizontal visible and sync clocks
	OC2CON = 0x0; // reset OC2
	OC2CON = 0x0000000D; // toggle, use Timer5
	OC2R = 0x0230; // h-visible rise
	OC2RS = 0x0A30; //0x0640; // h-blank fall
	OC3CON = 0x0; // reset OC3
	OC3CON = 0x0000000D; // toggle, use Timer5
	OC3R = 0x0110; //0x0690; // h-sync rise
	OC3RS = 0x0A5F; //0x0789; // h-sync fall
	T5CON = 0x0000; // reset Timer5, prescale of 1:1
	TMR5 = 0x0000; // zero out counter (offset some cycles)
	PR5 = 0x0A5F; //0x083F; // h-reset (minus one)
	
	// set OC4 and TMR2, vertical sync clock
	OC4CON = 0x0; // reset OC4
	OC4CON = 0x00000005; // toggle, use Timer2
	OC4R = 0x00F9; // v-sync rise
	OC4RS = 0x82A8; //0x0042; // v-sync fall
	T2CON = 0x0060; //0x0050; // prescale of 1:64
	TMR2 = 0x0000; // zero out counter (offset some cycles)
	PR2 = 0x82A8; //0xA1E7; // v-reset (minus one)
	
	// TMR3, start of scanline
	T3CON = 0x0000; // reset Timer3, prescale of 1:1
	TMR3 = 0x0830; // zero out counter (offset some cycles)
	PR3 = 0x0A5F; //0xA1E7; // v-reset (minus one)
	
	// set OC8 and OC9 and TMR6/7, audio channels
	OC8CON = 0x0; // reset OC2
	OC8CON = 0x00000003; // toggle on compare, use Timer6
	OC8R = 0x0000; // always at zero
	OC9CON = 0x0; // reset OC3
	OC9CON = 0x0000000B; // toggle on compare, use Timer7
	OC9R = 0x0000; // always at zero
	T6CON = 0x0060; // reset Timer6, prescale of 1:64
	TMR6 = 0x0000; // zero out counter
	PR6 = 0x0001; // determines audio frequency
	T7CON = 0x0060; // reset Timer7, prescale of 1:64
	TMR7 = 0x0000; // zero out counter
	PR7 = 0x0001; // determines audio frequency
	T8CON = 0x0070; // reset Timer8, prescale of 1:256
	TMR8 = 0x0000; // zero out counter
	PR8 = 0x0001; // determines audio duration
	T9CON = 0x0070; // reset Timer9, prescale of 1:256
	TMR9 = 0x0000; // zero out counter
	PR9 = 0x0001; // determines audio duration
	
	IPC4bits.OC3IP = 0x7; // interrupt priority 7
	IPC4bits.OC3IS = 0x0; // interrupt sub-priority 0
	IFS0bits.OC3IF = 0; // OC3 clear flag
	IEC0bits.OC3IE = 1; // OC3 interrupt on (set priority here?)
	
	OC1CONbits.ON = 1; // turn OC1 on
	OC2CONbits.ON = 1; // turn OC2 on
	OC3CONbits.ON = 1; // turn OC3 on
	OC4CONbits.ON = 1; // turn OC4 on
	OC5CONbits.ON = 1; // turn OC5 on
	
	
	IPC9bits.T8IP = 0x3; // audio interrupt priority 3
	IPC9bits.T8IS = 0x1; // sub-priority 1
	IFS1bits.T8IF = 0; // clear flags
	IEC1bits.T8IE = 1; // enable interrupts
	IPC10bits.T9IP = 0x3; // audio interrupt priority 3
	IPC10bits.T9IS = 0x0; // sub-priority 0
	IFS1bits.T9IF = 0; // clear flags
	IEC1bits.T9IE = 1; // enable interrupts

	// DMA setup
	IEC4bits.DMA0IE = 0; // disable interrupts
	IFS4bits.DMA0IF = 0; // clear flags
	IEC4bits.DMA1IE = 0; // disable interrupts
	IFS4bits.DMA1IF = 0; // clear flags
	DMACONbits.ON = 1; // enable the DMA controller
	
	DCH0CONbits.CHEN = 0; // disable channel
	DCH0ECONbits.CHSIRQ = 14; // start on Timer 3 interrupt
	DCH0ECONbits.SIRQEN = 1; // enable start interrupt
	DCH0INT = 0x0000; // clear all interrupts
	DCH0INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH0INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH0CONbits.CHCHN = 0; // disallow chaining
	DCH0CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH0CONbits.CHPRI = 0x3; // highest priority
	DCH0DSA = VirtToPhys(&PORTE); // transfer destination physical address
	DCH0SSIZ = SCREEN_X; //400; // source size
	DCH0DSIZ = 1; // dst size 
	DCH0CSIZ = SCREEN_X; //400; // 1 byte per event

	DCH1CONbits.CHEN = 0; // disable channel
	DCH1ECONbits.CHSIRQ = 19; // start on Timer 4 interrupt
	DCH1ECONbits.SIRQEN = 1; // enable start interrupt
	DCH1INT = 0x0000; // clear all interrupts
	DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH1INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH1CONbits.CHCHN = 1; // allow chaining
	DCH1CONbits.CHCHNS = 0; // chain from higher channel
	DCH1CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH1CONbits.CHPRI = 0x3; // highest priority
	DCH1SSA = VirtToPhys(screen_zero); // transfer source physical address
	DCH1DSA = VirtToPhys(&PORTE); // transfer destination physical address
	DCH1SSIZ = 1; // source size
	DCH1DSIZ = 1; // dst size 
	DCH1CSIZ = 1; // 1 byte per event
	
	
	// set up PS/2 Keyboard and Mouse on PORTD (RD9-RD10,RD12-RD13)
	CNCONDbits.ON = 1; // turn on interrupt-on-change
	CNCONDbits.EDGEDETECT = 1; // edge detect, not mismatch
	CNNED = 0x1200; // negative edge on RD9 and RD12
	CNFD = 0x0000; // clear flags
	
	IPC30bits.CNDIP = 0x5; // interrupt priority 5
	IPC30bits.CNDIS = 0x0; // interrupt sub-priority 0
	IFS3bits.CNDIF = 0;  // clear interrupt flag
	IEC3bits.CNDIE = 1; // enable interrupts

	// set up UART here
	U3BRG = 0x0079; // 18.75 MHz to 9600 baud = 18750000/(16*9600)-1 = 121.07 = 0x0079
	
	U3MODEbits.STSEL = 0; // 1-Stop bit
	U3MODEbits.PDSEL = 0; // No Parity, 8-Data bits
	U3MODEbits.BRGH = 0; // Standard-Speed mode
	U3MODEbits.ABAUD = 0; // Auto-Baud disabled
	
	//U3MODEbits.URXINV = 1; // reverse polarity
	//U3STAbits.UTXINV = 1; // reverse polarity
	
	U3MODEbits.UEN = 0x0; // just use TX and RX
	
	U3STAbits.UTXISEL = 0x0; // Interrupt after one TX character is transmitted
	U3STAbits.URXISEL = 0x0; // interrupt after one RX character is received
	
	U3STAbits.URXEN = 1; // enable RX
	U3STAbits.UTXEN = 1; // enable TX
	
	IPC39bits.U3RXIP = 0x4; // U3RX interrupt priority level 4
	IPC39bits.U3RXIS = 0x0; // U3RX interrupt sub-priority level 0
	//IPC39bits.U3EIP = 0x5; // U3E interrupt priority level 5
	//IPC39bits.U3EIS = 0x3; // U3E interrupt sub-priority level 3
	IFS4bits.U3RXIF = 0;  // clear interrupt flag
	IEC4bits.U3RXIE = 1; // U3RX interrupt on (set priority here?)
	
	U3MODEbits.ON = 1; // turn UART on
	
	// set up SPI here
	PORTDbits.RD4 = 1; // CS is high
	PORTDbits.RD7 = 1; // MOSI is high
	PORTDbits.RD1 = 0; // CLK is low	
	
	SPI1CONbits.ON = 0; // turn SPI module off
	SPI1CON2 = 0; // disable audio, etc.
	SPI1CONbits.DISSDI = 0; // SDI controlled by module
	SPI1CONbits.DISSDO = 0; // SDO controlled by module
	SPI1CONbits.MODE16 = 0x0; // 8-bit mode
	SPI1CONbits.MSTEN = 1; // host mode
	SPI1CONbits.MSSEN = 0; // client SS disabled
	SPI1CONbits.SSEN = 0; // client SS disabled
	SPI1CONbits.SMP = 0; // input sampled in middle of output time
	SPI1CONbits.CKE = 1; // output changes on idle-to-active clock
	SPI1CONbits.CKP = 0; // clock is idle low, active high
	SPI1CONbits.ON = 1; // turn SPI module on
	
	SPI1BUF = 0xFFFF; // dummy write
	while (SPI1STATbits.SPIRBF == 0) { } // wait
	sdcard_block[0] = SPI1BUF; // dummy read
	
	/*
	// Timer for 'USBA_device_millis' used with USB
	T1CON = 0x0; // disable timer 1
	TMR1 = 0; // clear timer 1
	IEC0bits.T1IE = 1; // Enable interrupt for timer 1
	IFS0bits.T1IF = 0; // Clear interrupt flag for timer 1
	IPC1bits.T1IP = 2; // Interrupt priority 2
	IPC1bits.T1IS = 0; // Sub-priority 0
	PR1 = (unsigned int)(SYS_FREQ / 1000 / 24); // Set up the period
	T1CONbits.TCKPS = 0b10; // pre-scale of 4
	T1CONbits.TON = 1; // Turn on the timer 1
	*/
	
	// Init USBA_device_millis
	// now using H-SYNC interrupt for millis timing
	USBA_device_millis = 0;
	
	
	
	// set shadow register priorities???
	PRISS = 0x76543210;
	
	// enable multi-vector interrupts???
	INTCONSET = _INTCON_MVEC_MASK;
	
	// IDK, just turn on interrupts globally???
	__builtin_enable_interrupts();
	

	// default to black screen
	PORTE = 0;
	
	// turn LED off by default
	PORTDbits.RD11 = 1;
	
	// wait some time
	DelayMS(1000);
	
	
	
	// set display buffer
	for (unsigned int y=0; y<SCREEN_Y; y++)
	{
		for (unsigned int x=0; x<SCREEN_X; x++)
		{
#ifdef SPLASH
			screen_buffer[y*SCREEN_X+x] = splash_default[y * SCREEN_X + x];
#else
			screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
			//screen_buffer[y*SCREEN_X+x] = (unsigned char)((x + y) % 256); // test pattern
			//if (x % 2 == 0) screen_buffer[y*SCREEN_X+x] = 0xFF; // white
			//else screen_buffer[y*SCREEN_X+x] = 0x1F; // cyan
#endif
		}
	}
	
	// set black scanline
	for (unsigned int x=0; x<SCREEN_X; x++) screen_blank[x] = 0x00;
	
	// clear ps2 buffers
	for (unsigned int i=0; i<256; i++)
	{
		ps2_state_array[0][i] = 0x00;
		ps2_state_array[1][i] = 0x00;
		ps2_cursor_x[0][i] = 0x0000;
		ps2_cursor_x[1][i] = 0x0000;
		ps2_cursor_y[0][i] = 0x0000;
		ps2_cursor_y[1][i] = 0x0000;
		
		usb_state_array[i] = 0x00;
		usb_cursor_x[i] = 0x0000;
		usb_cursor_y[i] = 0x0000;
	}
	
	
	// just a 'hello world' over the UART
	SendChar('*');
	
	// turn on video timers
	T4CONbits.ON = 1; // turn on TMR4 (independent of others)
	T3CONbits.ON = 1; // turn on TMR3 (cycle offset pre-calculated above)
	T5CONbits.ON = 1; // turn on TMR5 (cycle offset pre-calculated above)
	T2CONbits.ON = 1; // turn on TMR2 (cycle offset pre-calculated above)
	
	
	
	// need to change the jumpers on the board
	
	if (PORTJbits.RJ11 == 0) // if pressing (or jumpering) the 'button'
	{
		USBB_setup(USBB_data_received); // initialize USB device
	}
	else
	{
		USBA_setup(); // initialize USB host
	}
	
	DelayMS(1000); // settling delay, avoid garbage characters
	
	
	

	dummy = (char)Menu();
	
	if (dummy == 0) Tetra();
	else if (dummy == 1) BadApple();
	else if (dummy == 2) Scratchpad();
	else if (dummy == 3) UserCode();
	else if (dummy == 4) ReprogramCode();
	else if (dummy == 5) { }
	
	
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