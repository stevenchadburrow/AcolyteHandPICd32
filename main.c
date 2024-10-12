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
#pragma config FPLLIDIV = DIV_8         // System PLL Input Divider (8x Divider)
#pragma config FPLLRNG = RANGE_21_42_MHZ// System PLL Input Range (21-42 MHz Input)
#pragma config FPLLICLK = PLL_FRC       // System PLL Input Clock Selection (FRC is input to the System PLL)
#pragma config FPLLMULT = MUL_128       // System PLL Multiplier (PLL Multiply by 128)
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
#pragma config TRCEN = ON               // Trace Enable (Trace features in the CPU are enabled)
#pragma config BOOTISA = MIPS32         // Boot ISA Selection (Boot code and Exception code is MIPS32)
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



#define SYS_FREQ 200000000 // Running at 200MHz

unsigned int VirtToPhys(volatile void* p) // changed 'const' to 'volatile'
{
	return (int)p<0?((int)p&0x1fffffffL):(unsigned int)((unsigned char*)p+0x40000000L);
}

void DelayMS(unsigned int s)
{
    // Convert microseconds us into how many clock ticks it will take
	unsigned int count = s * 1000 * SYS_FREQ / 1000000 / 2; // Core Timer updates every 2 ticks
       
    _CP0_SET_COUNT(0); // Set Core Timer count to 0
    
    while (count > _CP0_GET_COUNT()); // Wait until Core Timer count reaches the number we calculated earlier
}

/*
SVGA Signal 800 x 600 @ 60 Hz timing

General timing
Screen refresh rate	60 Hz
Vertical refresh	37.878787878788 kHz
Pixel freq.	40.0 MHz
Horizontal timing (line)
Polarity of horizontal sync pulse is positive.

Scanline part	Pixels	Time [µs]
Visible area	800	20
Front porch	40	1
Sync pulse	128	3.2
Back porch	88	2.2
Whole line	1056	26.4
Vertical timing (frame)
Polarity of vertical sync pulse is positive.

Frame part	Lines	Time [ms]
Visible area	600	15.84
Front porch	1	0.0264
Sync pulse	4	0.1056
Back porch	23	0.6072
Whole frame	628	16.5792
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
Pin98,RD10,KEY-CLK
Pin99,RD11,KEY-DAT
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

// USB
Pin77,D+
Pin76,D-

// Oscillator
Pin71,RC12,OSC1/CLKI
Pin72,RC15,OSC2/CLKO

// UART
Pin69,RD14,U3TX
Pin70,RD15,U3RX

// LED
Pin97,RD9,LED

// Button
Pin27,RJ11,BUTTON
*/



unsigned char screen_buffer[300][400]; // visible portion of screen

unsigned int screen_scanline = 601; // start of vertical blank

unsigned int dma_constant_init = 0x01A3; // turns on DMA channel 1
unsigned int dma_constant_zero = 0x0000; // black during blanking period

void __attribute__((vector(_OUTPUT_COMPARE_3_VECTOR), interrupt(ipl7srs), nomips16)) oc3_handler()
{
    IFS0bits.OC3IF = 0;  // clear interrupt flag
	
	screen_scanline = screen_scanline + 1; // increment scanline
	
	if (screen_scanline >= 600)
	{
		if (screen_scanline == 628) 
		{
			DMACONbits.ON = 1; // enable the DMA controller
			
			screen_scanline = 0;
		}
		else
		{
			DMACONbits.ON = 0; // disable the DMA controller
		}
	}
	else
	{
		DCH1SSA = VirtToPhys(screen_buffer[screen_scanline>>1]);
	}
}

void __attribute__((vector(_CHANGE_NOTICE_D_VECTOR), interrupt(ipl6srs), nomips16)) cnd_handler()
{
	IFS3bits.CNDIF = 0;  // clear interrupt flag
	
	if ((CNFD & 0x0200) == 0x0200) // PS/2 Keyboard Clock
	{
		if ((PORTD & 0x0400) == 0x0400) // high
		{
			
		}
		else // low
		{
			
		}
	}
	
	if ((CNFD & 0x1000) == 0x1000) // PS/2 Mouse Clock
	{
		if ((PORTD & 0x2000) == 0x2000) // high
		{
			
		}
		else // low
		{
			
		}
	}
}

void __attribute__((vector(_UART3_RX_VECTOR), interrupt(ipl5srs), nomips16)) u3rx_handler()
{
	IFS4bits.U3RXIF = 0;  // clear interrupt flag
	
	// check for errors
	if(U1STAbits.FERR == 1)
	{
		return;
	}
		
	// clear overrun error
	if(U1STAbits.OERR == 1)
	{
		U1STAbits.OERR = 0;
		return;
	}
	
	unsigned char data = U1RXREG; // get character
			
	U1TXREG = data; // echo character received
}




// SDcard commands below
// This was used for the Arduino, but has been modified to work here.
unsigned char sdcard_block[512];

void sdcard_longdelay(void)
{
	DelayMS(10); // arbitrary amount of time to delay, should be around 10ms
}

void sdcard_sendbyte(unsigned int value)
{
	unsigned int temp_value = value;
	
	temp_value = (temp_value & 0x00FF);
	
	SPI1BUF = temp_value;
	
	while (SPI1STATbits.SPIRBF == 0) { }
	
	temp_value = SPI1BUF; // dummy read
	
};

unsigned int sdcard_receivebyte(void)
{
	unsigned int temp_value = 0x0000;
	
	SPI1BUF = 0xFFFF; // dummy write
	
	while (SPI1STATbits.SPIRBF == 0) { }
	
	temp_value = SPI1BUF;
	
	temp_value = (temp_value & 0x00FF);
	
	return temp_value;
};


void sdcard_pump(void)
{	
	PORTDbits.RD4 = 1; // CS is high, must disable the device
	
	sdcard_longdelay();
	
	for (unsigned int i=0; i<128; i++)
	{
		sdcard_sendbyte(0xFFFF); // MOSI must be high!
	}
};

void sdcard_enable(void)
{
	PORTDbits.RD4 = 0; // CS is low
}

void sdcard_disable(void)
{
	PORTDbits.RD4 = 1; // CS is high
	
	sdcard_sendbyte(0xFFFF);
}

unsigned int sdcard_waitresult(void)
{
	unsigned int temp_value = 0x00FF;

	for (unsigned int i=0; i<65000; i++) // arbitrary wait time
	{
		temp_value = sdcard_receivebyte();

		if (temp_value != 0x00FF)
		{
			return temp_value;
		}
	}

	return 0xFF;
}

unsigned int sdcard_initialize(void)
{
	unsigned int temp_value = 0x0000;	
	
	sdcard_disable();
	sdcard_pump();
	sdcard_longdelay();
	sdcard_enable();
	sdcard_sendbyte(0x0040); // CMD0 = 0x40 + 0x00 (0 in hex)
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0095); // CRC for CMD0
	temp_value = sdcard_waitresult(); // command response
	if (temp_value == 0x00FF) { return 0; }	
	sdcard_disable();
	if (temp_value != 0x0001) { return 0; } // expecting 0x01	
	sdcard_longdelay();
	sdcard_pump();
	sdcard_enable();
	sdcard_sendbyte(0x0048); // CMD8 = 0x40 + 0x08 (8 in hex)
	sdcard_sendbyte(0x0000); // CMD8 needs 0x000001AA argument
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0001);
	sdcard_sendbyte(0x00AA); 
	sdcard_sendbyte(0x0087); // CRC for CMD8
	temp_value = sdcard_waitresult(); // command response
	if (temp_value == 0x00FF) { return 0; }
	sdcard_disable();
	if (temp_value != 0x0001) { return 0; } // expecting 0x01
	sdcard_enable();
	temp_value = sdcard_receivebyte(); // 32-bit return value, ignore
	temp_value = sdcard_receivebyte();
	temp_value = sdcard_receivebyte();
	temp_value = sdcard_receivebyte();
	sdcard_disable();
	do {
		sdcard_pump();
		sdcard_longdelay();
		sdcard_enable();
		sdcard_sendbyte(0x0077); // CMD55 = 0x40 + 0x37 (55 in hex)
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0001); // CRC (general)
		temp_value = sdcard_waitresult(); // command response
		if (temp_value == 0x00FF) { return 0; }
		sdcard_disable();
		if (temp_value != 0x0001) { break; } // expecting 0x01, but if not it might already be 'initialized'?
		sdcard_pump();
		sdcard_longdelay();
		sdcard_enable();
		sdcard_sendbyte(0x0069); // CMD41 = 0x40 + 0x29 (41 in hex)
		sdcard_sendbyte(0x0040); // needed for CMD41?
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0001); // CRC (general)
		temp_value = sdcard_waitresult(); // command response
		if (temp_value == 0x00FF) { return 0; }
		sdcard_disable();
		if (temp_value != 0x0000 && temp_value != 0x0001) { return 0; } // expecting 0x00, if 0x01 try again
		sdcard_longdelay();
	} while (temp_value == 0x0001);

	return 1;
}

unsigned int sdcard_readblock(unsigned int high, unsigned int low)
{
	unsigned char temp_value = 0x00;

	sdcard_disable();
	sdcard_pump();
	sdcard_longdelay(); // this is probably not needed
	sdcard_enable();
	sdcard_sendbyte(0x51); // CMD17 = 0x40 + 0x11 (17 in hex)
	sdcard_sendbyte((high&0x00FF));
	sdcard_sendbyte(((low&0xFF00) >> 8));
	sdcard_sendbyte((low&0x00FE)); // only blocks of 512 bytes
	sdcard_sendbyte(0x00);
	sdcard_sendbyte(0x01); // CRC (general)
	temp_value = sdcard_waitresult(); // command response
	if (temp_value == 0xFF) { return 0; }
	else if (temp_value != 0x00) { return 0; } // expecting 0x00
	temp_value = sdcard_waitresult(); // data packet starts with 0xFE
	if (temp_value == 0xFF) { return 0; }
	else if (temp_value != 0xFE) { return 0; }
	
	for (unsigned int i=0; i<512; i++)
	{
		sdcard_block[i] = sdcard_receivebyte();
	}
	
	temp_value = sdcard_receivebyte(); // data packet ends with 0x55 then 0xAA
	temp_value = sdcard_receivebyte(); // ignore here
	sdcard_disable();

	return 1;
}

unsigned int sdcard_writeblock(unsigned int high, unsigned int low)
{
	unsigned char temp_value = 0x00;

	sdcard_disable();
	sdcard_pump();
	sdcard_longdelay(); // this is probably not needed
	sdcard_enable();
	sdcard_sendbyte(0x58); // CMD24 = 0x40 + 0x18 (24 in hex)
	sdcard_sendbyte((high&0x00FF));
	sdcard_sendbyte(((low&0xFF00)>>8));
	sdcard_sendbyte((low&0x00FE)); // only blocks of 512 bytes
	sdcard_sendbyte(0x00);
	sdcard_sendbyte(0x01); // CRC (general)
	temp_value = sdcard_waitresult(); // command response
	if (temp_value == 0xFF) { return 0; }
	else if (temp_value != 0x00) { return 0; } // expecting 0x00
	sdcard_sendbyte(0xFE); // data packet starts with 0xFE

	for (unsigned int i=0; i<512; i++)
	{
		sdcard_sendbyte(sdcard_block[i]);
	}
	
	sdcard_sendbyte(0x55); // data packet ends with 0x55 then 0xAA
	sdcard_sendbyte(0xAA);
	temp_value = sdcard_receivebyte(); // toggle clock 8 times
	sdcard_disable();

	return 1;
}




const unsigned char tetra_random[224] = 
{
	0, 4, 3, 6, 2, 5, 1,
	4, 1, 3, 5, 2, 0, 6,
	6, 5, 2, 0, 4, 1, 3,
	5, 4, 6, 0, 3, 1, 2,
	
	6, 0, 1, 5, 3, 4, 2,
	2, 4, 0, 5, 1, 6, 3,
	3, 1, 6, 2, 4, 0, 5,
	4, 5, 1, 6, 0, 2, 3,
	
	6, 5, 0, 1, 4, 3, 2,
	3, 5, 2, 6, 1, 0, 4,
	1, 3, 6, 5, 4, 2, 0,
	2, 4, 6, 0, 1, 3, 5,
	
	5, 2, 6, 0, 4, 1, 3,
	5, 0, 2, 4, 6, 1, 3,
	1, 4, 2, 6, 0, 3, 5,
	6, 0, 3, 2, 5, 4, 1,
	
	
	2, 6, 3, 5, 4, 0, 1,
	1, 3, 2, 6, 5, 0, 4,
	6, 4, 0, 3, 5, 2, 1,
	2, 5, 3, 4, 6, 0, 1,
	
	4, 5, 0, 6, 2, 1, 3,
	2, 1, 3, 0, 4, 5, 6,
	2, 0, 5, 4, 6, 1, 3,
	6, 2, 3, 0, 5, 4, 1,
	
	0, 6, 5, 3, 1, 4, 2,
	4, 5, 1, 2, 0, 6, 3,
	4, 1, 6, 3, 0, 2, 5,
	6, 0, 2, 5, 4, 3, 1,
	
	1, 3, 5, 2, 6, 0, 4,
	3, 1, 2, 6, 4, 0, 5,
	4, 2, 0, 1, 6, 3, 5,
	4, 3, 0, 5, 1, 6, 2,
};

const char tetra_map[448] =
{
    // I
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	
	' ', ' ', ' ', ' ',
	'*', '*', '*', '*',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	
	' ', ' ', ' ', ' ',
	'*', '*', '*', '*',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
  
	// J
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	' ', '*', ' ', ' ',
	' ', '*', '*', '*',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	' ', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', '*', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	'*', '*', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', ' ', ' ',
	 
	// L
	' ', '*', ' ', ' ',
	' ', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	' ', '*', '*', '*',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	
	' ', ' ', ' ', ' ',
	' ', ' ', '*', ' ',
	'*', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	
	// O
	' ', '*', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	// S
	' ', ' ', '*', '*',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', '*', '*',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', ' ', ' ',
	
	// T
	' ', '*', ' ', ' ',
	'*', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	'*', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', ' ', ' ',
	'*', '*', ' ', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	// Z
	'*', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', '*', ' ',
	' ', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	'*', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', '*', ' ',
	' ', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
};

// solid is one single value
void tetra_solid(unsigned int x, unsigned int y, unsigned char value)
{	
	for (unsigned int i=y*8; i<y*8+8; i++)
	{
		for (unsigned int j=x*8; j<x*8+8; j++)
		{
			screen_buffer[i][j] = value;
		}
	}		
};

// block is fancy looking
void tetra_block(unsigned int x, unsigned int y, unsigned char value)
{
	for (unsigned int i=y*8; i<y*8+8; i++)
	{
		for (unsigned int j=x*8; j<x*8+8; j++)
		{
			if (i == y*8) screen_buffer[i][j] = 0xFF;
			else if (j == x*8) screen_buffer[i][j] = 0xFF;
			else screen_buffer[i][j] = value;
		}
	}	
};

void tetra_character(unsigned int x, unsigned int y, unsigned char value)
{
	/*
	unsigned int pos = (unsigned int)(value - 32) * 16;
  
	for (unsigned int i=y; i<y+8; i++)
	{		
		screen_buffer[i][x] = character_bitmap[pos];		
		screen_buffer[i][x+1] = character_bitmap[pos+1];
		
		pos += 2;
	}
	*/
};

void tetra_decimal(unsigned char x, unsigned char y, unsigned int value)
{
	/*
	volatile unsigned int temp = 0;
	volatile unsigned int next = 0;
	
	next = value;
	
	temp = next / 10000;
	tetra_character(x, y, (unsigned char)(temp + '0'));
	next = next - 10000 * temp;
	
	temp = next / 1000;
	tetra_character(x + 0x02, y, (unsigned char)(temp + '0'));
	next = next - 1000 * temp;
	
	temp = next / 100;
	tetra_character(x + 0x04, y, (unsigned char)(temp + '0'));
	next = next - 100 * temp;
	
	temp = next / 10;
	tetra_character(x + 0x06, y, (unsigned char)(temp + '0'));
	next = next - 10 * temp;
	
	temp = next;
	tetra_character(x + 0x08, y, (unsigned char)(temp + '0'));
	*/
};

void tetra_string(unsigned char x, unsigned char y, char *value)
{
	/*
	volatile unsigned char pos = 0;
	
	while (value[pos] != '\\')
	{
		tetra_character(x + pos * 2, y, value[pos]);
		
		pos++;
	}
	*/
};

const unsigned int tetra_size_x = 10;
const unsigned int tetra_size_y = 18;
volatile unsigned char tetra_board[2*10*18];

struct tetra_struct_vars
{
	volatile unsigned int pos_x[2]; // start at 4
	volatile unsigned int pos_y[2]; // start at 4
	volatile unsigned int rot[2];
	volatile unsigned int new_pos_x[2];
	volatile unsigned int new_rot[2];
	volatile unsigned int piece[2];
	volatile unsigned int new_piece[2];

	volatile unsigned int seed;

	volatile unsigned int bag[14];
	volatile unsigned int bag_pos[2];

	volatile unsigned int speed[2];

	volatile unsigned int lines[2];
	volatile unsigned int timer[2];
	
	volatile unsigned char joy_curr[2];
	volatile unsigned char joy_prev[2];
	volatile unsigned int joy_delay[2];

	volatile unsigned int game_over[2];
};

struct tetra_struct_vars tetra_vars;

void tetra()
{	
	for (volatile unsigned int z=0; z<2; z++)
	{	
		tetra_vars.pos_x[z] = 7; // start at 4
		tetra_vars.pos_y[z] = 4; // start at 4
		tetra_vars.rot[z] = 0;
		tetra_vars.new_pos_x[z] = 7;
		tetra_vars.new_rot[z] = 0;
		tetra_vars.piece[z] = 0;
		tetra_vars.new_piece[z] = 1;
		
		for (volatile unsigned int i=0; i<7; i++)
		{
			tetra_vars.bag[i + z * 7] = i;
		}
		
		tetra_vars.bag_pos[z] = 1;
		
		tetra_vars.speed[z] = 0; // max of 13

		tetra_vars.seed = 0;
		
		tetra_vars.lines[z] = 0;
		tetra_vars.timer[z] = 0;

		tetra_vars.joy_curr[z] = 0x00;
		tetra_vars.joy_prev[z] = 0x00;
		tetra_vars.joy_delay[z] = 0;
		
		tetra_vars.game_over[z] = 0x01;

		for (volatile unsigned int i=0; i<tetra_size_y; i++)
		{
			for (volatile unsigned int j=0; j<tetra_size_x; j++)
			{
				if (i != tetra_size_y - 1)
				{
					tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = ' '; // empty
				}
				else
				{
					tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = '_'; // permanent bottom
				}
			}
		}
	}
	
	volatile unsigned int test = 0;
	volatile unsigned int count = 0;
	
	while (1)
	{		
		tetra_vars.joy_prev[0] = tetra_vars.joy_curr[0];
		tetra_vars.joy_curr[0] = 0xFF; 
		
		if (PORTJbits.RJ0 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0x7F);
		if (PORTJbits.RJ1 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xBF);
		if (PORTJbits.RJ2 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xDF);
		if (PORTJbits.RJ3 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xEF);
		if (PORTJbits.RJ4 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xF7);
		if (PORTJbits.RJ5 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xFB);
		
		tetra_vars.joy_prev[1] = tetra_vars.joy_curr[1];
		tetra_vars.joy_curr[1] = 0xFF; 
		
		if (PORTJbits.RJ6 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0x7F);
		if (PORTJbits.RJ7 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xBF);
		if (PORTJbits.RJ10 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xDF);
		if (PORTJbits.RJ12 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xEF);
		if (PORTJbits.RJ13 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xF7);
		if (PORTJbits.RJ14 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xFB);
		
		// change joystick select pin here if you want
		
		tetra_vars.seed++; // random 
		
		// DELAY HERE!!!
		
		for (volatile unsigned int z=0; z<2; z++)
		{
			if (tetra_vars.game_over[z] == 0)
			{
				for (volatile unsigned int i=0; i<tetra_size_y; i++)
				{
					for (volatile unsigned int j=0; j<tetra_size_x; j++)
					{
						if (tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] == '*') 
						{
							tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = ' ';
						}
					}
				}
			}

			tetra_vars.new_pos_x[z] = tetra_vars.pos_x[z];
			tetra_vars.new_rot[z] = tetra_vars.rot[z];

			tetra_vars.timer[z]++;

			if (tetra_vars.timer[z] > 15 - tetra_vars.speed[z]) //30 - 2 * tetra_vars.speed[z]) // determines how fast it falls
			{
				tetra_vars.timer[z] = 0;
			}

			tetra_vars.joy_delay[z]++;

			if (tetra_vars.joy_delay[z] > 1) //4) // determines button turbo speed
			{
				tetra_vars.joy_delay[z] = 0;
				tetra_vars.joy_prev[z] = tetra_vars.joy_prev[z] | 0xF0;
			}

			if (((tetra_vars.joy_curr[z] & 0x80) == 0x00) && ((tetra_vars.joy_prev[z] & 0x80) == 0x80)) // up
			{
				tetra_vars.timer[z] = 1; // not zero
				tetra_vars.joy_delay[z] = 0;
			}
			else if (((tetra_vars.joy_curr[z] & 0x40) == 0x00) && ((tetra_vars.joy_prev[z] & 0x40) == 0x40)) // down
			{
				tetra_vars.timer[z] = 0;
				tetra_vars.joy_delay[z] = 0;
			}
			else if (((tetra_vars.joy_curr[z] & 0x20) == 0x00) && ((tetra_vars.joy_prev[z] & 0x20) == 0x20)) // left
			{
				tetra_vars.new_pos_x[z]--;
				tetra_vars.joy_delay[z] = 0;
			}
			else if (((tetra_vars.joy_curr[z] & 0x10) == 0x00) && ((tetra_vars.joy_prev[z] & 0x10) == 0x10)) // right
			{
				tetra_vars.new_pos_x[z]++;
				tetra_vars.joy_delay[z] = 0;
			}
			else if (((tetra_vars.joy_curr[z] & 0x08) == 0x00) && ((tetra_vars.joy_prev[z] & 0x08) == 0x08)) // button 1
			{
				if (tetra_vars.game_over[z] != 0)
				{
					tetra_vars.game_over[z] = 2; // exit game
					
					tetra_vars.speed[z] = 0;
					
					tetra_vars.lines[z] = 0;
					
					for (volatile unsigned int i=0; i<tetra_size_y; i++)
					{
						for (volatile unsigned int j=0; j<tetra_size_x; j++)
						{
							if (i != tetra_size_y - 1)
							{
								tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = ' '; // empty
							}
							else
							{
								tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = '_'; // permanent bottom
							}
						}
					}
				}
				else
				{  
					tetra_vars.new_rot[z]++;
					if (tetra_vars.new_rot[z] == 4) tetra_vars.new_rot[z] = 0;
					tetra_vars.joy_delay[z] = 0;
				}
			}
			else if (((tetra_vars.joy_curr[z] & 0x04) == 0x00) && ((tetra_vars.joy_prev[z] & 0x04) == 0x04)) // button 2
			{
				if (tetra_vars.game_over[z] != 0)
				{
					tetra_vars.game_over[z] = 0; // play again
					
					tetra_vars.speed[z] = 0;
					
					tetra_vars.lines[z] = 0;
					
					for (volatile unsigned int i=0; i<tetra_size_y; i++)
					{
						for (volatile unsigned int j=0; j<tetra_size_x; j++)
						{
							if (i != tetra_size_y - 1)
							{
								tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = ' '; // empty
							}
							else
							{
								tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = '_'; // permanent bottom
							}
						}
					}
				}
				else
				{
					if (tetra_vars.new_rot[z] == 0) tetra_vars.new_rot[z] = 3;
					else tetra_vars.new_rot[z]--;
					tetra_vars.joy_delay[z] = 0;
				}
			}
			
			if (tetra_vars.game_over[z] != 0) continue;

			for (volatile unsigned int k=0; k<4; k++)
			{
				for (volatile unsigned int i=0; i<4; i++)
				{
					for (volatile unsigned int j=0; j<4; j++)
					{
						if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.new_rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] != ' ')
						{
							if (tetra_vars.new_pos_x[z] + j >= 0x80 || tetra_vars.new_pos_x[z] + j <= 0x03)
							{	
								tetra_vars.new_pos_x[z]++;
								i = 5;
								j = 5;
							}
							else if (tetra_vars.new_pos_x[z] + j >= tetra_size_x + 4)
							{
								tetra_vars.new_pos_x[z]--;
								i = 5;
								j = 5;
							}
						}
					}
				}
			}
			
			test = 0;

			for (volatile unsigned int i=0; i<4; i++)
			{
				for (volatile unsigned int j=0; j<4; j++)
				{
					if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.new_rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] == '*')
					{
						if (tetra_board[(tetra_vars.new_pos_x[z] + j - 4) + (tetra_vars.pos_y[z] + i - 4) * tetra_size_x + z * tetra_size_x * tetra_size_y] != ' ')
						{
							test = 1;
							i = 5;
							j = 5;
						}
					}
				}
			}

			if (test == 0)
			{
				tetra_vars.pos_x[z] = tetra_vars.new_pos_x[z];
				tetra_vars.rot[z] = tetra_vars.new_rot[z];
			}

			if (tetra_vars.timer[z] == 0)
			{
				tetra_vars.pos_y[z]++;

				test = 0;

				for (volatile unsigned int i=0; i<4; i++)
				{
					for (volatile unsigned int j=0; j<4; j++)
					{
						if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] == '*')
						{
							if (tetra_board[(tetra_vars.pos_x[z] + j - 4) + (tetra_vars.pos_y[z] + i - 4) * tetra_size_x + z * tetra_size_x * tetra_size_y] != ' ')
							{
								test = 1;
								i = 5;
								j = 5;
							}
						}
					}
				}

				if (test == 1)
				{	
					tetra_vars.pos_y[z]--;

					for (volatile unsigned int i=0; i<4; i++)
					{
						for (volatile unsigned int j=0; j<4; j++)
						{
							if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] == '*')
							{
								tetra_board[(tetra_vars.pos_x[z] + j - 4) + (tetra_vars.pos_y[z] + i - 4) * tetra_size_x + z * tetra_size_x * tetra_size_y] = '#';
							}
						}
					}
					
					// SOUND HERE

					if (tetra_vars.pos_y[z] == 4)
					{
						tetra_vars.game_over[z] = 1;
						
						// SOUND HERE
					}
					else
					{
						count = 0;

						for (volatile unsigned int i=tetra_size_y-1; i>=1; i--)
						{
							test = 0;

							for (volatile unsigned int j=0; j<tetra_size_x; j++)
							{
								if (tetra_board[j + (i-1) * tetra_size_x + z * tetra_size_x * tetra_size_y] == ' ') test = 1;
							}

							if (test == 0)
							{
								for (volatile unsigned int j=i; j>=2; j--)
								{
									for (volatile unsigned int k=0; k<tetra_size_x; k++)
									{
										tetra_board[k + (j-1) * tetra_size_x + z * tetra_size_x * tetra_size_y] = 
											tetra_board[k + (j-2) * tetra_size_x + z * tetra_size_x * tetra_size_y];
									}
								}

								i++; // test that row again

								tetra_vars.lines[z]++;
								count++;
								
								// SOUND HERE
							}
						}

						if (tetra_vars.game_over[1-z] != 0) count = 0;

						if (count >= 1) count--;

						for (volatile unsigned int i=0; i<count; i++)
						{
							for (volatile unsigned int j=0; j<tetra_size_y-1; j++)
							{
								for (volatile unsigned int k=0; k<tetra_size_x; k++)
								{
									if (j < tetra_size_y-2)
									{
										tetra_board[k + j * tetra_size_x + (1-z) * tetra_size_x * tetra_size_y] = 
											tetra_board[k + (j+1) * tetra_size_x + (1-z) * tetra_size_x * tetra_size_y];
									}
									else
									{
										if (k == tetra_vars.seed % 10) tetra_board[k + j * tetra_size_x + (1-z) * tetra_size_x * tetra_size_y] = ' ';
										else tetra_board[k + j * tetra_size_x + (1-z) * tetra_size_x * tetra_size_y] = '#';
									}
								}
							}
						}
						
						tetra_vars.pos_y[1-z] -= count;
						if (tetra_vars.pos_y[1-z] < 4) tetra_vars.pos_y[1-z] = 4;
					
						tetra_vars.speed[z] = (unsigned int)(tetra_vars.lines[z] / 10);
						if (tetra_vars.speed[z] > 9) tetra_vars.speed[z] = 13; // max

						tetra_vars.pos_x[z] = 7; // start at 4
						tetra_vars.pos_y[z] = 4; // start at 4
						tetra_vars.rot[z] = 0;
						tetra_vars.piece[z] = tetra_vars.new_piece[z];

						tetra_vars.bag_pos[z]++;

						if (tetra_vars.bag_pos[z] == 7)
						{
							tetra_vars.bag_pos[z] = 0;

							for (volatile unsigned int i=0; i<7; i++)
							{
								tetra_vars.bag[(i + ((tetra_vars.seed & 0xF0) >> 4)) % 7 + z * 7] = tetra_random[i + (tetra_vars.seed & 0x1F) * 7];
							}
						}

						tetra_vars.new_piece[z] = tetra_vars.bag[tetra_vars.bag_pos[z] + z * 7];
					}
				}
			}

			for (volatile unsigned int i=0; i<4; i++)
			{
				for (volatile unsigned int j=0; j<4; j++)
				{
					if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] == '*')
					{
						tetra_board[(tetra_vars.pos_x[z] + j - 4) + (tetra_vars.pos_y[z] + i - 4) * tetra_size_x + z * tetra_size_x * tetra_size_y] = 
							tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64];
					}
				}
			}
		}
		
		unsigned int horz = 0x02;
		unsigned int vert = 0x00;
		
		for (unsigned int i=0; i<256; i++)
		{
			for (unsigned int j=0; j<200; j++)
			{
				if (i % 2 == 0) screen_buffer[i][j] = 0x6F; // speckled background
				else screen_buffer[i][j] = 0x25;
			}
		}

		for (volatile unsigned int i=0; i<tetra_size_y-1; i++)
		{
			for (volatile unsigned int j=0; j<tetra_size_x; j++)
			{
				switch (tetra_board[j + i * tetra_size_x])
				{
					case ' ':
					{
						tetra_solid(j + horz, i + 0x02 + vert, 0x00); // black
						break;
					}
					case '*':
					{
						if (tetra_vars.piece[0] == 0) tetra_block(j + horz, i + 0x02 + vert, 0x1F); // cyan
						else if (tetra_vars.piece[0] == 1) tetra_block(j + horz, i + 0x02 + vert, 0x03); // blue
						else if (tetra_vars.piece[0] == 2) tetra_block(j + horz, i + 0x02 + vert, 0x6F); // grey
						else if (tetra_vars.piece[0] == 3) tetra_block(j + horz, i + 0x02 + vert, 0xFC); // yellow
						else if (tetra_vars.piece[0] == 4) tetra_block(j + horz, i + 0x02 + vert, 0x1C); // green
						else if (tetra_vars.piece[0] == 5) tetra_block(j + horz, i + 0x02 + vert, 0xE3); // magenta
						else if (tetra_vars.piece[0] == 6) tetra_block(j + horz, i + 0x02 + vert, 0xE0); // red
						break;
					}
					case '#':
					{
						tetra_block(j + horz, i + 0x02 + vert, 0x25); // dark grey
						break;
					}
				}
			}
		}

		tetra_decimal(horz * 8 + 0x0A, vert * 8, tetra_vars.lines[0]);

		if (tetra_vars.new_piece[0] == 0) tetra_character(horz * 8, vert * 8, 'I');
		else if (tetra_vars.new_piece[0] == 1) tetra_character(horz * 8, vert * 8, 'J');
		else if (tetra_vars.new_piece[0] == 2) tetra_character(horz * 8, vert * 8, 'L');
		else if (tetra_vars.new_piece[0] == 3) tetra_character(horz * 8, vert * 8, 'O');
		else if (tetra_vars.new_piece[0] == 4) tetra_character(horz * 8, vert * 8, 'S');
		else if (tetra_vars.new_piece[0] == 5) tetra_character(horz * 8, vert * 8, 'T');
		else if (tetra_vars.new_piece[0] == 6) tetra_character(horz * 8, vert * 8, 'Z');

		if (tetra_vars.game_over[0] != 0x00)
		{
			tetra_string(horz * 8 + 0x08, 0x40 + vert * 8, "Press\\");
			tetra_string(horz * 8 + 0x08, 0x48 + vert * 8, "Button\\");
		}
		
		horz = 0x82;
		
		for (unsigned int i=0; i<256; i++)
		{
			for (unsigned int j=0; j<200; j++)
			{
				if (i % 2 == 0) screen_buffer[i][j] = 0x6F; // speckled background
				else screen_buffer[i][j] = 0x25;
			}
		}

		for (volatile unsigned int i=0; i<tetra_size_y-1; i++)
		{
			for (volatile unsigned int j=0; j<tetra_size_x; j++)
			{
				switch (tetra_board[j + i * tetra_size_x + tetra_size_x * tetra_size_y])
				{
					case ' ':
					{
						tetra_solid(j + horz, i + 0x02 + vert, 0x00); // black
						break;
					}
					case '*':
					{
						if (tetra_vars.piece[0] == 0) tetra_block(j + horz, i + 0x02 + vert, 0x1F); // cyan
						else if (tetra_vars.piece[0] == 1) tetra_block(j + horz, i + 0x02 + vert, 0x03); // blue
						else if (tetra_vars.piece[0] == 2) tetra_block(j + horz, i + 0x02 + vert, 0x6F); // grey
						else if (tetra_vars.piece[0] == 3) tetra_block(j + horz, i + 0x02 + vert, 0xFC); // yellow
						else if (tetra_vars.piece[0] == 4) tetra_block(j + horz, i + 0x02 + vert, 0x1C); // green
						else if (tetra_vars.piece[0] == 5) tetra_block(j + horz, i + 0x02 + vert, 0xE3); // magenta
						else if (tetra_vars.piece[0] == 6) tetra_block(j + horz, i + 0x02 + vert, 0xE0); // red
						break;
					}
					case '#':
					{
						tetra_block(j + horz, i + 0x02 + vert, 0x25); // dark grey
						break;
					}
				}
			}
		}

		tetra_decimal(horz * 8 + 0x0A, vert * 8, tetra_vars.lines[1]);

		if (tetra_vars.new_piece[1] == 0) tetra_character(horz * 8, vert * 8, 'I');
		else if (tetra_vars.new_piece[1] == 1) tetra_character(horz * 8, vert * 8, 'J');
		else if (tetra_vars.new_piece[1] == 2) tetra_character(horz * 8, vert * 8, 'L');
		else if (tetra_vars.new_piece[1] == 3) tetra_character(horz * 8, vert * 8, 'O');
		else if (tetra_vars.new_piece[1] == 4) tetra_character(horz * 8, vert * 8, 'S');
		else if (tetra_vars.new_piece[1] == 5) tetra_character(horz * 8, vert * 8, 'T');
		else if (tetra_vars.new_piece[1] == 6) tetra_character(horz * 8, vert * 8, 'Z');

		if (tetra_vars.game_over[1] != 0x00)
		{
			tetra_string(horz * 8 + 0x08, 0x40 + vert * 8, "Press\\");
			tetra_string(horz * 8 + 0x08, 0x48 + vert * 8, "Button\\");
		}
	}
};




int main()
{
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
	
	// change tri-state on pins
	TRISD = 0xB640; // MISO, KEY, LED, MOUSE, and UART
	TRISJ = 0xFCFF; // JOY-A, JOY-B, and BUTTON
	CNPUD = 0xC000; // pull-ups for UART
	
	// change system and peripheral clocks
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	OSCCON = 0x0; // unlock clock and PLL
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	SPLLCON = 0x01330085; // use PLL to bring internal 8 MHz into 200 MHz
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	OSCCON = 0x8; // lock clock and PLL
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	PB3DIV = 0x00008800; // set OC and TMR clock to no division
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	OSCCON = 0x00001000; // set new clock setting to use PLL
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	OSCCON = 0x00000001; // use new clock setting
	SYSKEY = 0x0;
	
	while (OSCCONbits.OSWEN != 0) { } // wait for clock switch to complete
	
	// change PB clocks
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	PB2DIV = 0x00008007; // change PB2 clock to 200 / 8 = 25 MHz for SPI and UART
	SYSKEY = 0x0;
	
	
	// set CPU and DMA priority for timer 2
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	CFGCONbits.DMAPRI = 1; // DMA does have highest priority (?)
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	CFGCONbits.CPUPRI = 0; // CPU does not have highest priority (?)
	SYSKEY = 0x0;
	
	// set OC pins
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	CFGCONbits.OCACLK = 1; // use alternate OC/TMR table
	SYSKEY = 0x0;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	CFGCONbits.IOLOCK = 1; // PPS is unlocked
	SYSKEY = 0x0;
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
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
	CFGCONbits.IOLOCK = 0; // PPS is locked
	SYSKEY = 0x0;

	// set OC1 and TMR4, pixel clock
	OC1CON = 0x0; // reset OC1
	OC1CON = 0x00000005; // toggle, use Timer4
	OC1R = 0x0007; // pixel-sync rise (adjust)
	OC1RS = 0x0009; // pixel-sync fall (adjust)
	T4CON = 0x0; // reset Timer4
	T4CON = 0x0; // prescale of 1:1
	TMR4 = 0x0; // zero out counter
	PR4 = 0x09; // pixel-reset (minus one)
	
	// set OC2 and OC3 and TMR5, horizontal visible and sync clocks
	OC2CON = 0x0; // reset OC2
	OC2CON = 0x0000000D; // toggle, use Timer5
	OC2R = 0x0000; // h-visible rise
	OC2RS = 0x0FA0; // h-blank fall
	OC3CON = 0x0; // reset OC3
	OC3CON = 0x0000000D; // toggle, use Timer5
	OC3R = 0x1068; // h-sync rise
	OC3RS = 0x12D8; // h-sync fall
	T5CON = 0x0; // reset Timer5, prescale of 1:1
	TMR5 = 0x0000; // zero out counter (offset some cycles)
	PR5 = 0x149F; // h-reset (minus one)
	
	// set OC4 and TMR2, vertical sync clock
	OC4CON = 0x0; // reset OC4
	OC4CON = 0x00000005; // toggle, use Timer2
	OC4R = 0x0000; // v-sync rise
	OC4RS = 0x00A5; // v-sync fall
	T2CON = 0x0; // reset Timer2
	T3CON = 0x0; // reset Timer3
	T2CON = 0x00000058; // prescale of 1:32, 32-bit mode
	TMR2 = 0x00000000; // zero out counter (offset some cycles)
	PR2 = 0x000194C3; // v-reset (minus one)
	
	// set OC8 and OC9 and TMR6/7, audio channels
	OC8CON = 0x0; // reset OC2
	OC8CON = 0x00000003; // toggle on compare, use Timer6
	OC8R = 0x0000; // always at zero
	OC9CON = 0x0; // reset OC3
	OC9CON = 0x0000000B; // toggle on compare, use Timer7
	OC9R = 0x0000; // always at zero
	T6CON = 0x0050; // reset Timer6, prescale of 1:32
	TMR6 = 0x0000; // zero out counter
	PR6 = 0x0001; // determines audio frequency
	T7CON = 0x0050; // reset Timer7, prescale of 1:32
	TMR7 = 0x0000; // zero out counter
	PR7 = 0x0001; // determines audio frequency
	
	IEC0bits.OC3IE = 1; // OC3 interrupt on (set priority here?)
	
	OC1CONbits.ON = 1; // turn OC1 on
	OC2CONbits.ON = 1; // turn OC2 on
	OC3CONbits.ON = 1; // turn OC3 on
	OC4CONbits.ON = 1; // turn OC4 on
	OC8CONbits.ON = 1; // turn OC8 on
	OC9CONbits.ON = 1; // turn OC9 on
	
	// set up PS/2 Keyboard and Mouse on PORTD (RD9-RD10,RD12-RD13)
	CNCONDbits.EDGEDETECT = 1; // edge detect, not mismatch
	CNNED = 0x1200; // negative edge on RD9 and RD12
	CNCONDbits.ON = 1; // turn on interrupt-on-change
	
	IEC3bits.CNDIE = 1; // enable interrupts (set priority here?)
	
	// set up DMA here
	DMACONbits.ON = 0; // disable the DMA controller
	
	DCH0CON = 0x0013; // channel 0 off, priority 3, automatic enable, no chaining	
	DCH1CON = 0x0123; // channel 1 off, priority 3, no automatic enable, chaining to channel 2
	DCH2CON = 0x0013; // channel 2 off, priority 3, automatic enable, no chaining
	
	DCH0ECONbits.CHSIRQ = 24; // start on Timer 5 interrupt
	DCH0ECONbits.SIRQEN = 1; // enable start interrupt
	DCH0INT = 0x0000; // clear all interrupts
	DCH0INTbits.CHSDIE = 0; // source done interrupt disabled
	
	DCH1ECONbits.CHSIRQ = 9; // start on Timer 2 interrupt
	DCH1ECONbits.SIRQEN = 1; // enable start interrupt
	DCH1INT = 0x0000; // clear all interrupts
	DCH1INTbits.CHSDIE = 1; // source done interrupt enabled
	// called by delay_ms()
	DCH2ECONbits.CHSIRQ = 135; // start on DMA Channel 1 interrupt
	DCH2ECONbits.SIRQEN = 1; // enable start interrupt
	DCH2INT = 0x0000; // clear all interrupts
	DCH2INTbits.CHSDIE = 0; // source done interrupt disabled
	
	DCH0SSA = VirtToPhys(&dma_constant_init); // transfer source physical address
	DCH0DSA = VirtToPhys(&DCH1CON); // transfer destination physical address
	DCH0SSIZ = 1; // source size is 1 byte
	DCH0DSIZ = 1; // dst size is 1 byte
	DCH0CSIZ = 1; // 1 byte per event
	
	DCH1SSA = VirtToPhys(screen_buffer[screen_scanline>>1]); // transfer source physical address
	DCH1DSA = VirtToPhys(&PORTE); // transfer destination physical address
	DCH1SSIZ = 400; // source size is 400 bytes
	DCH1DSIZ = 1; // dst size is 1 byte
	DCH1CSIZ = 1; // 1 byte per event
	
	DCH2SSA = VirtToPhys(&dma_constant_zero); // transfer source physical address
	DCH2DSA = VirtToPhys(&PORTE); // transfer destination physical address
	DCH2SSIZ = 1; // source size is 1 byte
	DCH2DSIZ = 1; // dst size is 1 byte
	DCH2CSIZ = 1; // 1 byte per event

	// set up UART here
	U3BRG = 0x00A2; // 25 MHz to 9600 baud = 25000000/(16*9600)-1 = 161.76 = 162
	
	U3MODEbits.STSEL = 0; // 1-Stop bit
	U3MODEbits.PDSEL = 0; // No Parity, 8-Data bits
	U3MODEbits.BRGH = 0; // Standard-Speed mode
	U3MODEbits.ABAUD = 0; // Auto-Baud disabled
	
	//U3MODEbits.URXINV = 1; // reverse polarity
	//U3STAbits.UTXINV = 1; // reverse polarity
	
	U3MODEbits.UEN = 0x0; // just use TX and RX
	
	U3STAbits.UTXISEL = 0x0; // Interrupt after one TX character is transmitted
	U3STAbits.URXISEL = 0; // interrupt after one RX character is received
	
	U3STAbits.URXEN = 1; // enable RX
	U3STAbits.UTXEN = 1; // enable TX
	
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
	
	
	
	
	// set display buffer
	for (unsigned int y=0; y<300; y++)
	{
		for (unsigned int x=0; x<400; x++)
		{
			screen_buffer[y][x] = (unsigned char)((x + y) % 256); // test pattern
		}
	}
	
	// turn DMA channels 0 and 2 on, keep DMA channel 1 off
	DCH2CONbits.CHEN = 1;
	DCH1CONbits.CHEN = 0;
	DCH0CONbits.CHEN = 1;
	
	// turn on video timers
	T4CONbits.ON = 1; // turn on TMR4 (independent of others)
	T5CONbits.ON = 1; // turn on TMR5 (cycle offset pre-calculated above)
	T2CONbits.ON = 1; // turn on TMR2/TMR3 (cycle offset pre-calculated above)
	
	// turn on audio timers
	T6CONbits.ON = 1; // turn on TMR6 (audio channel)
	T7CONbits.ON = 1; // turn on TMR7 (audio channel)
	
	// infinite loop
	while (1)
	{
		// blink LED
		PORTDbits.RD11 = 0;
		DelayMS(500);
		PORTDbits.RD11 = 1;
		DelayMS(500);
	}
}
