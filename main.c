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

#include "splash_pointing.c"
#include "splash_brighting.c"
#include "splash_floating.c"
#include "splash_calling.c"
#include "splash_blushing.c"
#include "splash_television.c"


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
Pin4,RE6,RED-BUSB_LED_data
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

#define USB_ENDPOINT_BUFFER_SIZE 64
#define USB_EP0_WAIT_TIMEOUT 40000
#define USB_VBUS_VALID 0x3

#define USB_KEYBOARD_NUM_KEYS 256 //128
#define USB_KEY_NUMLOCK 0x53
#define USB_KEY_CAPSLOCK 0x39
#define USB_KEY_SCROLLLOCK 0x47
#define USB_CTRL_MASK 0x1
#define USB_ALT_MASK 0x4
#define USB_SHIFT_MASK 0x2

#define USB_KEYBOARD_REPEAT_DELAY 250       // 250ms between key repeats, if a key is held down
#define USB_HID_REQUEST_DELAY 25        // 25 USB_device_milliseconds between keyboard report requests

typedef struct
{
    unsigned char pressed;      // Is the key currently pressed?
    unsigned long repeat_time;  // When should this press be repeated (if it's pressed)?
    unsigned char released;     // Was the key released?
    
} USB_HID_KEY;

typedef struct {
  unsigned char  bLength;
  unsigned char  bDescriptorType;
  unsigned short bcdUSB;
  unsigned char  bDeviceClass;
  unsigned char  bDeviceSubClass;
  unsigned char  bDeviceProtocol;
  unsigned char  bMaxPacketSize0;
  unsigned short idVendor;
  unsigned short idProduct;
  unsigned short bcdDevice;
  unsigned char  iManufacturer;
  unsigned char  iProduct;
  unsigned char  iSerialNumber;
  unsigned char  bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

void USB_EP0_send(unsigned char *buffer, uint32_t length);
void USB_EP0_send_setup(unsigned char *buffer, uint32_t length);
void USB_EP0_receive(unsigned char *buffer, uint32_t length);
int USB_EP0_receive_long(unsigned char *buffer, uint32_t length);
void USB_EP1_receive(unsigned char *buffer);
void USB_init_endpoints(int USB_address);
void USB_HOST_reset_device();

unsigned char USB_DEVICE_DESCRIPTOR_REQUEST[8] = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00};
unsigned char USB_DEVICE_CONFIGURATION_REQUEST[8] = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x09, 0x00};
unsigned char USB_DEVICE_STRING_REQUEST[8] = {0x80, 0x06, 0x02, 0x03, 0x09, 0x04, 0x04, 0x00};
unsigned char USB_DEVICE_GET_HID_REPORT_REQUEST[8] = {0x81, 0x06, 0x00, 0x22, 0x00, 0x00, 0x09, 0x00};
unsigned char USB_DEVICE_ASSIGN_ADDRESS[8] = {0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char USB_DEVICE_SET_CONFIGURATION[8] = {0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char USB_DEVICE_SET_INTERFACE[8] = {0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char USB_DEVICE_SET_REPORT[8] = {0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00};
unsigned char USB_DEVICE_SET_IDLE[8] = {0x21, 0x0A, 0x00, 0x7D, 0x00, 0x00, 0x00, 0x00};
unsigned char USB_DEVICE_SET_PROTOCOL[8] = {0x21, 0x0B, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char __attribute__ ((coherent, aligned(8))) USB_EP0_buffer[USB_ENDPOINT_BUFFER_SIZE];
unsigned char __attribute__ ((coherent, aligned(8))) USB_EP1_buffer[USB_ENDPOINT_BUFFER_SIZE];
USB_DEVICE_DESCRIPTOR USB_device_descriptor;
unsigned char USB_device_configuration[255];
unsigned char USB_device_name[255];
unsigned char USB_HID_report[512];

unsigned char USB_NUM_LOCK_STATUS = 0;
unsigned char USB_CAPS_LOCK_STATUS = 0;
unsigned char USB_SCROLL_LOCK_STATUS = 0;
unsigned char USB_SHIFT_PRESSED = 0;
unsigned char USB_CTRL_PRESSED = 0;
unsigned char USB_ALT_PRESSED = 0;
volatile char USB_EP0_RECEIVED = 0;
volatile char USB_EP0_IF = 0;
volatile char USB_EP1_RECEIVED = 0;
volatile char USB_DEVICE_CONNECTED = 0;
volatile char USB_STAGE = 0;
volatile char USB_HID_STAGE = 0;
volatile char USB_HID_BUSY = 0;

int USB_config_length;
USB_HID_KEY USB_device_keys[USB_KEYBOARD_NUM_KEYS];
int USB_DEVICE_ADDRESS;
volatile unsigned long USB_device_millis;
unsigned long USB_device_timer;
char USB_LED_data = 0;
int USB_EP0_LENGTH = 8;




const unsigned char text_bitmap[64*96] = { 
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 32
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0x00,0x00,0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,
	0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0x00,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,
	0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0x00,
	0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
	0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,
	0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0x00,
	0xFF,0xFF,0x00,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

const unsigned char ps2_conversion[256] = 
{
  	0x00,0x16,0x0C,0x0E,0x1E,0x1C,0x1D,0x15,
	0x00,0x18,0x07,0x0F,0x1F,0x09,0x60,0x00,
	0x00,0x00,0x00,0x00,0x00,0x71,0x31,0x00,
	0x00,0x00,0x7A,0x73,0x61,0x77,0x32,0x00,
	0x00,0x63,0x78,0x64,0x65,0x34,0x33,0x00,
	0x00,0x20,0x76,0x66,0x74,0x72,0x35,0x00,
	0x00,0x6E,0x62,0x68,0x67,0x79,0x36,0x00,
	0x00,0x00,0x6D,0x6A,0x75,0x37,0x38,0x00,
	0x00,0x2C,0x6B,0x69,0x6F,0x30,0x39,0x00,
	0x00,0x2E,0x2F,0x6C,0x3B,0x70,0x2D,0x00,
	0x00,0x00,0x27,0x00,0x5B,0x3D,0x00,0x00,
	0x00,0x00,0x0D,0x5D,0x00,0x5C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,
	0x00,0x31,0x00,0x34,0x37,0x00,0x00,0x00,
	0x30,0x2E,0x32,0x35,0x36,0x38,0x1B,0x00,
	0x19,0x2B,0x33,0x2D,0x2A,0x39,0x00,0x00,

	0x00,0x16,0x0C,0x0E,0x1E,0x1C,0x1D,0x15,
	0x00,0x18,0x07,0x0F,0x1F,0x09,0x7E,0x00,
	0x00,0x00,0x00,0x00,0x00,0x51,0x21,0x00,
	0x00,0x00,0x5A,0x53,0x41,0x57,0x40,0x00,
	0x00,0x43,0x58,0x44,0x45,0x24,0x23,0x00,
	0x00,0x20,0x56,0x46,0x54,0x52,0x25,0x00,
	0x00,0x4E,0x42,0x48,0x47,0x59,0x5E,0x00,
	0x00,0x00,0x4D,0x4A,0x55,0x26,0x2A,0x00,
	0x00,0x3C,0x4B,0x49,0x4F,0x29,0x28,0x00,
	0x00,0x3E,0x3F,0x4C,0x3A,0x50,0x5F,0x00,
	0x00,0x00,0x22,0x00,0x7B,0x2B,0x00,0x00,
	0x00,0x00,0x0D,0x7D,0x00,0x7C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,
	0x00,0x03,0x00,0x13,0x02,0x00,0x00,0x00,
	0x1A,0x7F,0x12,0x35,0x14,0x11,0x1B,0x00,
	0x19,0x2B,0x04,0x2D,0x2A,0x01,0x00,0x00
};

const unsigned char usb_conversion[256] = 
{
	0x00,0x00,0x00,0x00,0x61,0x62,0x63,0x64,
	0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,
	0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,
	0x75,0x76,0x77,0x78,0x79,0x7A,0x30,0x31,
	0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
	0x0D,0x1B,0x08,0x09,0x20,0x2D,0x3D,0x5B,
	0x5D,0x5C,0x00,0x3B,0x27,0x60,0x2C,0x2E,
	0x2F,0x00,0x1C,0x1D,0x1E,0x1F,0x0E,0x0F,
	0x17,0x07,0x16,0x18,0x19,0x15,0x00,0x00,
	0x00,0x1A,0x02,0x01,0x7F,0x03,0x04,0x14,
	0x13,0x12,0x11,0x00,0x2F,0x2A,0x2D,0x2B,
	0x0D,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
	0x37,0x38,0x39,0x2E,0x00,0x00,0x00,0x3D,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x41,0x42,0x43,0x44,
	0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,
	0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,
	0x55,0x56,0x57,0x58,0x59,0x5A,0x21,0x40,
	0x23,0x24,0x25,0x5E,0x26,0x2A,0x28,0x29,
	0x0D,0x1B,0x08,0x09,0x20,0x5F,0x2B,0x7B,
	0x7D,0x7C,0x00,0x3A,0x22,0x7E,0x3C,0x3E,
	0x3F,0x00,0x1C,0x1D,0x1E,0x1F,0x0E,0x0F,
	0x17,0x07,0x16,0x18,0x19,0x15,0x00,0x00,
	0x00,0x1A,0x02,0x01,0x7F,0x03,0x04,0x14,
	0x13,0x12,0x11,0x00,0x2F,0x2A,0x2D,0x2B,
	0x0D,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
	0x37,0x38,0x39,0x2E,0x00,0x00,0x00,0x3D,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


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



volatile unsigned char __attribute__((coherent,address(0x80001000))) screen_buffer[384][512]; //screen_buffer[300][400]; // visible portion of screen

volatile unsigned char screen_blank[512]; // black scanline

volatile unsigned int screen_scanline = 771; // start of vertical sync

volatile unsigned char screen_zero[1] = { 0x00 }; // zero value for black

volatile unsigned int audio_counter[2] = { 0, 0 }; // audio duration

volatile unsigned int ps2_port = 0x0000;
volatile unsigned int ps2_flags = 0x0000;

// PS/2 keyboard variables
volatile char ps2_state_array[2][256];
volatile unsigned int ps2_cursor_x[2][256];
volatile unsigned int ps2_cursor_y[2][256];
volatile unsigned char ps2_counter[2] = { 0x00, 0x00 };
volatile unsigned char ps2_buffer[2] = { 0x00, 0x00 };
volatile unsigned char ps2_writepos[2] = { 0x00, 0x00 };
volatile unsigned char ps2_readpos[2] = { 0x00, 0x00 };
volatile unsigned char ps2_sequence[2] = { 0xFF, 0xFF }; // starts with 0xFA acknowledge byte
volatile unsigned char ps2_shift[2] = { 0x00, 0x00 };
volatile unsigned char ps2_release[2] = { 0x00, 0x00 };
volatile unsigned char ps2_extended[2] = { 0x00, 0x00 };
volatile unsigned char ps2_mode[2] = { 0x00, 0x00 };
volatile unsigned char ps2_ready[2] = { 0x00, 0x00 };
volatile unsigned char ps2_check[2] = { 0x00, 0x00 };

volatile char usb_state_array[256];
volatile unsigned int usb_cursor_x[256];
volatile unsigned int usb_cursor_y[256];
volatile unsigned int usb_buttons[256];
volatile unsigned char usb_writepos = 0x00;
volatile unsigned char usb_readpos = 0x00;
volatile unsigned char usb_mode = 0x00;



volatile unsigned int USB_device_millis_delay = 0;

void __attribute__((vector(_OUTPUT_COMPARE_3_VECTOR), interrupt(ipl7srs))) oc3_handler()
{		
    IFS0bits.OC3IF = 0;  // clear interrupt flag
	
	PORTE = 0;
	
	USB_device_millis_delay = USB_device_millis_delay + 1;
	
	if (USB_device_millis_delay >= 56) // should be 56.475 really
	{
		USB_device_millis_delay = 0;
		USB_device_millis++;
	}
	
	screen_scanline = screen_scanline + 1; // increment scanline
	
	if (screen_scanline == 806) screen_scanline = 0;
	
	if (screen_scanline < 768)
	{		
		DCH0INTbits.CHBCIF = 0; // clear transfer complete flag
		DCH0SSA = VirtToPhys(screen_buffer[screen_scanline>>1]); // transfer source physical address
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
						if (ps2_cursor_x[p][(unsigned char)(ps2_writepos[p]-1)] >= 512 - (signed char)(ps2_buffer[p]))
						{
							ps2_cursor_x[p][ps2_writepos[p]] = 512;
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
						if (ps2_cursor_y[p][(unsigned char)(ps2_writepos[p]-1)] >= 384 - (signed char)(ps2_buffer[p]))
						{
							ps2_cursor_y[p][ps2_writepos[p]] = 384;
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
				array[4] = (unsigned int)(384 - ps2_cursor_y[p][ps2_readpos[p]]);

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
			screen_buffer[y+i][(x+j)] = (text_bitmap[pos+i*8+j]);
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
			screen_buffer[y+i][(x+j)] = (unsigned char)((text_bitmap[pos+i*8+j] ^ 0xFF));
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




// USB_host_tasks() is the main "USB Stack" / State machine responsible for enumerating the keyboard and then requesting packets on endpoint 1
void USB_host_tasks()
{
    int received_length;
   // int temp_index;
    
    if (USB_DEVICE_CONNECTED)
    {
        switch (USB_STAGE)
        {
            case 0:
            {
                // Is this an LS device or an FS device?
                if (USBOTGbits.LSDEV)
                {
                    // Low speed device attached
                    USB_EP0_LENGTH = 8; // Set the EP0 length to 8 bytes for now
                }
                else
                {
                    // Full speed device attached
                    USB_EP0_LENGTH = 64; // Set the EP0 length to 64 bytes for now
                }
                // Device just connected, tell it to reset
                // Tell device to reset its stack
                USB_HOST_reset_device();
                USB_STAGE++;
                USB_EP0_IF = 0;
                break;
            }
            case 1:
            {
                if (USB_EP0_IF == 0)
                {
                    // Send device descriptor request
                    USB_EP0_send_setup(USB_DEVICE_DESCRIPTOR_REQUEST, 8);                        
                    USB_STAGE++;
                    USB_EP0_IF = 0;
                }
                break;
            }
            case 2:
            {
                if (USB_EP0_IF)
                {
                    // Send handshake
                    // Sending request packet handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USB_STAGE++;
                    USB_EP0_IF = 0;
                }
                break;
            }
            case 3:
            {
                if (USB_EP0_IF)
                {
					// Receive X bytes
                    received_length = USBE0CSR2bits.RXCNT;
                    USB_EP0_receive((unsigned char *)&USB_device_descriptor, USBE0CSR2bits.RXCNT); //&USB_device_descriptor, USBE0CSR2bits.RXCNT);
                    if (USB_device_descriptor.bMaxPacketSize0 > 0)
                    {
                        //Max endpoint 0 length is X bytes
                        USB_EP0_LENGTH = USB_device_descriptor.bMaxPacketSize0;
                    }
                    // Got USB version
                    USB_STAGE++;
                    USB_EP0_IF = 0;
                }
                break;
            }
            case 4:
            {
                // Send handshake
                // Sending data received handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // STATUS + TXPKTRDY
                USB_STAGE++;
                USB_EP0_IF = 0;
                break;
            }
            case 5:
            {
                // Set device address
                if (USB_EP0_IF)
                {
                    // Assigning device address of 2
                    USB_DEVICE_ASSIGN_ADDRESS[2] = 2;
                    USB_EP0_send_setup(USB_DEVICE_ASSIGN_ADDRESS, 8);
                    USB_STAGE++;
                    USB_EP0_IF = 0;
                }
                break;
            }
            case 6:
            {
                if (USB_EP0_IF)
                {
                    //Delaying 50ms to let the device get ready
                    DelayMS(50);
                    // Sending status transition handshake, no data stage
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60; // Set STATUS and REQPKT, no data stage
                    USB_STAGE++;
                    USB_EP0_IF = 0;
                }
                break;
            }
            case 7:
            {
                if (USB_EP0_IF)
                {
                    *((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // Clear STATUS and RXPKTRDY
                    // Sending empty packet request to force set address
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 8:
            {
                if (USB_EP0_IF)
                {
                    // Should receive 0 bytes
                    received_length = USBE0CSR2bits.RXCNT;
                    USB_EP0_receive(USB_device_configuration, USBE0CSR2bits.RXCNT);
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 9:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;
                USB_EP0_IF = 0;
                USB_STAGE++;
                break;
            }            
            case 10:
            {
                if (USB_EP0_IF)
                {
                    // Set address in hardware, request configuration this time
                    // Initialising endpoint 1
                    USB_DEVICE_ADDRESS = 2;
                    USB_init_endpoints(USB_DEVICE_ADDRESS);
                    // Sending request for short device configuration
                    USB_EP0_send_setup(USB_DEVICE_CONFIGURATION_REQUEST, 8);
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 11:
            {
                if (USB_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 12:
            {
                if (USB_EP0_IF)
                {
					// Receive X bytes
                    received_length = USBE0CSR2bits.RXCNT;
                    USB_EP0_receive(USB_device_configuration, received_length);
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 13:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;
                USB_STAGE++;
                USB_EP0_IF = 0;
                break;
            }
            case 14:
            {
                if (USB_EP0_IF)
                {
					// Sending request for full device configuration
                    USB_config_length = USB_device_configuration[2];
                    USB_DEVICE_CONFIGURATION_REQUEST[6] = USB_device_configuration[2];
                    USB_EP0_send_setup(USB_DEVICE_CONFIGURATION_REQUEST, 8);
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 15:
            {
                if (USB_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 16:
            {
                if (USB_EP0_IF)
                {
					// Receive X bytes
                    received_length = USB_EP0_receive_long(USB_device_configuration, USBE0CSR2bits.RXCNT);
                    USB_EP0_IF = 0;                   
                    USB_STAGE++;
                }
                break;
            }            
            case 17:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // STATUS and TXPKTRDY
                USB_EP0_IF = 0;
                USB_STAGE++;
                break;               
            }
            case 18:
            {
                if (USB_EP0_IF)
                {
                    // Sending request for short device name
                    USB_EP0_send_setup(USB_DEVICE_STRING_REQUEST, 8);
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 19:
            {
                if (USB_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 20:
            {
                if (USB_EP0_IF)
                {
					// Receive X bytes
                    received_length = USBE0CSR2bits.RXCNT;
                    USB_EP0_receive(USB_device_name, received_length);
                    // Use the received length to modify the GET STRING request
                    USB_DEVICE_STRING_REQUEST[6] = USB_device_name[0];
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 21:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;
                
                // Sending request for full device name
                USB_EP0_send_setup(USB_DEVICE_STRING_REQUEST, 8);
                USB_EP0_IF = 0;
                USB_STAGE++;
                break;
            }
            case 22:
            {
                if (USB_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 23:
            {
                if (USB_EP0_IF)
                {
					// Receive device name as string
                    received_length = USB_EP0_receive_long(USB_device_name, USBE0CSR2bits.RXCNT);
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 24:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;                
                USB_EP0_IF = 0;
                USB_STAGE++;
                break;
            }
            case 25:
            {
                if (USB_EP0_IF)
                {
                    // Send full configuration request
                    // Sending SET CONFIGURATION request (configuration 0)
                    USB_EP0_send_setup(USB_DEVICE_SET_CONFIGURATION, 8);
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;                
            }
            case 26:
            {
                if (USB_EP0_IF)
                {
                    // Send handshake
                    // Sending status transition handshake, no data stage
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60;  // Set STATUS and REQPKT, no data stage
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 27:
            {
                if (USB_EP0_IF)
                {
					// Send SET IDLE request
                    *((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // Clear STATUS and RXPKTRDY
                    USB_EP0_send_setup(USB_DEVICE_SET_IDLE, 8);
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 28:
            {
                if (USB_EP0_IF)
                {
                    // Sending status transition handshake, no data stage
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60;
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 29:
            {
                if (USB_EP0_IF)
                {
					// Send SET protocol
                    *((unsigned char*)&USBE0CSR0 + 0x2) &= 0x41; // Clear STATUS and RXPKTRDY                    
                    USB_EP0_send_setup(USB_DEVICE_SET_PROTOCOL, 8);
                    USB_EP0_IF = 0;
                    USB_STAGE++;

                }
                break;
            }
            case 30:
            {
                if (USB_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60;
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 31:
            {
                if (USB_EP0_IF)
                {
                    // Request short 9-byte HID report. Not bothering with requesting the whole thing as we never need it.
                    *((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // Clear STATUS and RXPKTRDY   
                    USB_EP0_send_setup(USB_DEVICE_GET_HID_REPORT_REQUEST, 8);
                    USB_EP0_IF = 0;
                    USB_STAGE++;                    
                }
                break;
            }
            case 32:
            {
                if (USB_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20;
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 33:
            {
                if (USB_EP0_IF)
                {
					// Receive X bytes
                    received_length = USB_EP0_receive_long(USB_HID_report, USBE0CSR2bits.RXCNT);
                    USB_EP0_IF = 0;
                    USB_STAGE++;
                }
                break;
            }
            case 34:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;
                USB_STAGE++;
                USB_EP0_IF = 0;
                break;
            }            
            case 35:
            {                
                if (USB_EP0_IF)
                {
                    // Keyboard is enumerated!
                    USB_device_timer = USB_device_millis + USB_HID_REQUEST_DELAY;
                    USB_STAGE++;
                    USB_EP0_IF = 0;
                }
                break;
            }
            case 36:
            {
                if (USB_device_millis > USB_device_timer)
                {
                    USB_device_timer = USB_device_millis + USB_HID_REQUEST_DELAY;                    
                    USBE1CSR1bits.REQPKT = 1;
                }
                
                break;
            }
        }
    }  
}

// USB_HID_tasks() exists purely for handling the setting of LEDs while not blocking the rest of my code
void USB_HID_tasks()
{
    switch (USB_HID_STAGE)
    {
        case 0:
        {
            // Do nothing            
            break;
        }
        case 1:
        {
            // Send HID SET REPORT
            // Send the report command FIRST
            USB_EP0_send_setup(USB_DEVICE_SET_REPORT, 8);
            
            // Wait for transmission to end
            USB_EP0_IF = 0;
            USB_HID_STAGE++;
            break;
        }
        case 2:
        {
            // Send data
            if (USB_EP0_IF)
            {
                USB_EP0_IF = 0;
                // Now send the actual 1-byte long report, but NOT as a setup packet. Yay
                USB_EP0_buffer[0] = USB_LED_data;
                USB_EP0_send(USB_EP0_buffer, 1);
                USB_HID_STAGE++;
            }

            break;
        }
        case 3:
        {
            if (USB_EP0_IF)
            {
                // Send handshake
                USB_EP0_IF = 0;
                USB_HID_STAGE++;
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60;  // Set STATUS and REQPKT, no data stage
            }

            break;
        }
        case 4:
        {
            if (USB_EP0_IF)
            {
                // Send handshake
                USB_EP0_IF = 0;
                USB_HID_STAGE = 0;
                USB_HID_BUSY = 0;
                *((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // Clear STATUS and RXPKTRDY
            }

            break;            
        }
    }
}

void USB_init_endpoints(int USB_address)
{
    USBE1CSR0bits.MODE = 1;     // EP1 is OUT (OUT from host = in to device = RX)
    
    // Clear all interrupt flags
	USBCSR0bits.EP0IF = 0;
    USBCSR0bits.EP1TXIF = 0;
    USBCSR1bits.EP1RXIF = 0;
	
    // Set the maximum transfer length for each endpoint
	// Configure endpoint 0 first.
    USBE0CSR0bits.TXMAXP = USB_EP0_LENGTH; // Set endpoint 0 buffer size to 16 bytes (can be a maximum of 64 for EP0)

    // And next my custom endpoints
    USBE1CSR0bits.TXMAXP = 64;   // Endpoint 1 - Maximum TX payload / transaction - 512
    USBE1CSR1bits.RXMAXP = 64;   // Endpoint 1 - Maximum TX payload / transaction - 512

	// Specify which kinds of endpoint we will be using
    USBE1CSR2bits.PROTOCOL = 3; // Endpoint 1 - Interrupt mode
            
	// Enable DISNYET
    USBE1CSR1bits.PIDERR = 0; // Clear DISNYET to enable NYET replies
    
    // Set the SPEED for endpoint 1 to Full Speed (2)
    USBE1CSR3bits.SPEED = 2;

    // Set the TEP (transmit endpoint) for endpoint 1 to 1 (why is this necessary again?)
    USBE1CSR3bits.TEP = 1;
    
    // Set RXINTERV (polling interval) to 1, meaning 1ms, for endpoint 1
    USBIENCSR3bits.RXINTERV = 1;
    
	// Set up buffer locations for endpoint 0
    USBCSR3bits.ENDPOINT = 0;
    USBFIFOA = 0x0000;    
    // Clear data toggle
    USBIENCSR0bits.CLRDT = 1;

	// Set up buffer locations for endpoint 1
    USBCSR3bits.ENDPOINT = 1;
    USBFIFOA = 0x00A000A;
    // Clear data toggle
    USBIENCSR0bits.CLRDT = 1;

    // Set buffer sizes for endpoint 1
    USBE1CSR3bits.TXFIFOSZ = 0x6;   // Transmit FIFO Size bits - 64 bytes
    USBE1CSR3bits.RXFIFOSZ = 0x6;   // Receive FIFO Size bits - 64 bytes
	
	// Set maximum size for each packet before splitting occurs
    USBOTGbits.TXFIFOSZ = 0x3; // 64 bytes for transmit
    USBOTGbits.RXFIFOSZ = 0x3; // 64 bytes for receive
    
    USBOTGbits.HOSTMODE = 1;
        
	// Disable Isochronous mode for endpoint 1
    USBE1CSR0bits.ISO = 0;      // Isochronous TX Endpoint Disable bit (Device mode).

    // Set up endpoint interrupts
	// Initially clear all the interrupt enables (IE)
    USBCSR1 = 0;

    USBCSR1bits.EP0IE = 1;      // Endpoint 0 interrupt enable
    USBCSR2bits.EP1RXIE = 1;    // Endpoint 1 RX interrupt enable
    USBCSR2bits.SOFIE = 0;

	// Set current endpoint to EP0
    USBCSR3bits.ENDPOINT = 1;
    
    // Set the receiving address for endpoint 1 to the device address we gave to the keyboard
    USBE1RXAbits.RXFADDR = USB_address;
}

void USB_setup()
{
    volatile uint8_t * usbBaseAddress;
    int cnt;
    
    for (cnt = 0; cnt < USB_KEYBOARD_NUM_KEYS; cnt++)
    {
        USB_device_keys[cnt].pressed = 0;
        USB_device_keys[cnt].released = 0;
        USB_device_keys[cnt].repeat_time = 0;
    }

    USBCRCONbits.USBIE = 1;

    // Reset the USB hardware according to the demonic ritual Microchip demands
    // Shouldn't be attempted on a blood moon is Venus is in retrograde
    usbBaseAddress = (uint8_t *)&USBCSR0;
    *(usbBaseAddress + 0x7F) = 0x3;
    DelayMS(10);
    *(usbBaseAddress + 0x7F) = 0;
    
    USB_DEVICE_ADDRESS = 0; 

    USBCSR2bits.SESSRQIE = 1;
    USBCSR2bits.CONNIE = 1;
    USBCSR2bits.RESETIE = 1;
    USBCSR2bits.VBUSERRIE = 1;
    USBCSR2bits.DISCONIE = 1;
    USBCSR2bits.EP1RXIE = 1;
    USBCSR1bits.EP1TXIE = 1;
    
    USBOTGbits.HOSTMODE = 1;
    
    IEC4bits.USBIE = 0;         // Disable the USB interrupt    
    IFS4bits.USBIF = 0;         // Clear the USB interrupt flag.
    IPC33bits.USBIP = 6;        // USB Interrupt Priority 6
    IPC33bits.USBIS = 0;        // USB Interrupt Sub-Priority 0
    IPC33bits.USBDMAIP = 5;
    IPC33bits.USBDMAIS = 1;
    IFS4bits.USBDMAIF = 0;
    IEC4bits.USBDMAIE = 0;
    
    //USB_init_endpoints();

    USBCSR0bits.HSEN = 0; // 1; // CHANGING THIS TO ZERO!!!
    USBCRCONbits.USBIDOVEN = 1;
    USBCRCONbits.PHYIDEN = 1;

    USBCRCONbits.USBIDVAL = 0; 
    USBCRCONbits.USBIDVAL = 0; 

    IFS4bits.USBIF = 0;         // Clear the USB interrupt flag.
    IEC4bits.USBIE = 1;         // Enable the USB interrupt

    USB_STAGE = 0;
 
    USBOTGbits.SESSION = 1;
}

int USB_EP0_wait_TXRDY()
{
    int timeout;
    
    timeout = 0;
    
    while (USBE0CSR0bits.TXRDY)
    {
        timeout++;
        if (timeout > USB_EP0_WAIT_TIMEOUT) return 1;
    };
    
    return 0;
}

void USB_EP0_send_setup(unsigned char *buffer, uint32_t length)
{	
	int cnt;
    unsigned char *FIFO_buffer;

    FIFO_buffer = (unsigned char *)&USBFIFO0;
	
	USBE0CSR0bits.TXMAXP = USB_EP0_LENGTH;
    
    USBE0CSR2bits.SPEED = 2;
    USBE0TXAbits.TXHUBADD = 0;
    USBE0TXAbits.TXHUBPRT = 0;
    USBE0TXAbits.TXFADDR = USB_DEVICE_ADDRESS;    

    for (cnt = 0; cnt < length; cnt++)
    {
        *FIFO_buffer = *buffer++; // Send the bytes
    }

    *((unsigned char*)&USBE0CSR0 + 0x2) = 0xA;
}

void USB_EP0_send(unsigned char *buffer, uint32_t length)
{
	int cnt;
    unsigned char *FIFO_buffer;

    FIFO_buffer = (unsigned char *)&USBFIFO0;
	
	USBE0CSR0bits.TXMAXP = USB_EP0_LENGTH;
    
    USBE0CSR2bits.SPEED = 2;
    USBE0TXAbits.TXHUBADD = 0;
    USBE0TXAbits.TXHUBPRT = 0;
    USBE0TXAbits.TXFADDR = USB_DEVICE_ADDRESS; 

    for (cnt = 0; cnt < length; cnt++)
    {
        *FIFO_buffer = *buffer++; // Send the bytes
    }

    USBE0CSR0bits.TXRDY = 1;
}

void USB_EP0_receive(unsigned char *buffer, uint32_t length)
{
    int cnt;
    unsigned char *FIFO_buffer;
    
    // Get 8-bit pointer to USB FIFO for endpoint 0
    FIFO_buffer = (unsigned char *)&USBFIFO0;
    
    cnt = 0;
    
    while (cnt < length)
    {
        *buffer++ = *(FIFO_buffer + (cnt & 3));
        cnt++;
    }
    USBE0CSR0bits.RXRDYC = 1;
}

int USB_EP0_receive_long(unsigned char *buffer, uint32_t length)
{
    int total_received;
    int received_length;
    int temp_index;
    
    total_received = length;    
    received_length = length;
    
    USB_EP0_receive(buffer, received_length);

    temp_index = received_length;

    while (received_length >= USB_EP0_LENGTH)
    {
        USB_EP0_IF = 0;
        *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT

        while (USB_EP0_IF == 0);

        received_length = USBE0CSR2bits.RXCNT;
        USB_EP0_receive(&buffer[temp_index], received_length);
        temp_index += received_length;                        
        total_received += received_length;
    }

    USB_EP0_IF = 0;
    
    return total_received;
}

void USB_EP1_receive(unsigned char *buffer)
{	
    unsigned char *FIFO_buffer;
    int cnt;
    int length;
    
    USBE1CSR0bits.MODE = 0;     // EP1 is RX mode
    length = USBE1CSR2bits.RXCNT;
	
    FIFO_buffer = (unsigned char *)&USBFIFO1;
	
    for(cnt = 0; cnt < length; cnt++)
    {
        buffer[cnt] = *(FIFO_buffer + (cnt & 3)); // Store the received bytes in the buffer
		
    }
    
    USBE1CSR1bits.RXPKTRDY = 0;
}

// USB ISR
void __attribute__((vector(_USB_VECTOR), interrupt(ipl6srs), nomips16)) USB_handler()
{
    unsigned char EP0IF, EP1RXIF;
    unsigned char CONNIF;
    unsigned int CSR0, CSR1, CSR2;
    
    // Reading once from USBCSR0, USBCSR1 and USBCSR2 clears them (why?) so store them
    CSR0 = USBCSR0;
    
    EP0IF = (CSR0 & (1<<16)) ? 1 : 0;   
    
    CSR1 = USBCSR1;
    
    EP1RXIF = (CSR1 & (1 << 1)) ? 1 : 0;
    
    CSR2 = USBCSR2;
    
    CONNIF = (CSR2 & (1 << 20)) ? 1 : 0;
    
    if (CONNIF)
    {
        // Now we need to enable reset
        USB_DEVICE_CONNECTED = 1;
    }
    
    // Endpoint 0 Interrupt
    if(EP0IF)
    {
        USBCSR0bits.EP0IF = 0;      // Clear the USB EndPoint 0 Interrupt Flag.
        USB_EP0_IF = 1;             // My own flag for Endpoint 0 Interrupts
    }
      
    // Endpoint 1 Receive Interrupt
    if (EP1RXIF)
    {
        USBCSR1bits.EP1RXIF = 0;    // Clear the USB EndPoint 1 RX Interrupt Flag.
        USB_EP1_RECEIVED++;       // To let the app know Endpoint 1 received something
    }
    
    IFS4bits.USBIF = 0;             // Clear the main interrupt flag!
}

void USB_HOST_reset_device()
{
    // When resetting the attached USB device, we set the RESET signal high, wait 100ms (holding it in reset) and then set it low again
    USBCSR0bits.RESET = 1;
    DelayMS(100);
    USBCSR0bits.RESET = 0;
    DelayMS(100);
}

void USB_keyboard_status_toggle(char keycode)
{
    if (USB_HID_BUSY) return; // We are already processing a status toggle, skip this one
    
    switch (keycode)
    {
        case USB_KEY_NUMLOCK:
        {
            // Flip the bit
            USB_NUM_LOCK_STATUS ^= 1;
            
            // Fill in the LED bit fields            
            if (USB_NUM_LOCK_STATUS)
                USB_LED_data |= 1;
            else
                USB_LED_data &= 0xFE;
                                    
            break;
        }
        case USB_KEY_CAPSLOCK:
        {
            // Flip the bit
            USB_CAPS_LOCK_STATUS ^= 1;
            
            // Fill in the LED bit fields            
            if (USB_CAPS_LOCK_STATUS)
                USB_LED_data |= 2;
            else
                USB_LED_data &= 0xFD;
                                    
            break;
        }
        case USB_KEY_SCROLLLOCK:
        {
            // Flip the bit
            USB_SCROLL_LOCK_STATUS ^= 1;
            
            // Fill in the LED bit fields            
            if (USB_SCROLL_LOCK_STATUS)
                USB_LED_data |= 4;
            else
                USB_LED_data &= 0xFB;
                                    
            break;
        }
        default:
        {
            break;
        }
    }
        
    USB_HID_STAGE = 1;
    USB_HID_BUSY = 1;
}

void USB_HID_process_report()
{
	if (USB_HID_report[1] == 0x01 && USB_HID_report[3] == 0x06) // keyboard
	{
		usb_mode = 0x01; // keyboard
		
		int cnt;
		char current_keys_pressed[USB_KEYBOARD_NUM_KEYS];

		// Clear the current_keys_pressed array first
		//memset(current_keys_pressed, 0, sizeof(current_keys_pressed));
		for (cnt = 0; cnt < USB_KEYBOARD_NUM_KEYS; cnt++)
		{
			current_keys_pressed[cnt] = 0;
		}

		// Find out which keys are currently being pressed by looking at the report
		for (cnt = 0; cnt < 6; cnt++)
		{        
			current_keys_pressed[USB_EP1_buffer[cnt + 2]] = 1;

			USB_keyboard_status_toggle(USB_EP1_buffer[cnt + 2]);
		}

		// Compare this to the current key status
		for (cnt = 0; cnt < USB_KEYBOARD_NUM_KEYS; cnt++)
		{
			// If a key's pressed value is now 0, but it was 1 before, that means it has been released        
			if ((current_keys_pressed[cnt] == 0) && (USB_device_keys[cnt].pressed == 1))
			{
				USB_device_keys[cnt].released = 1; // Key has just been released
				USB_device_keys[cnt].pressed = 0;  // Key is no longer pressed
			}

			// If a key's pressed value is 1 now, but it was 0 before, that means it has just been pressed
			if ((current_keys_pressed[cnt] == 1) && (USB_device_keys[cnt].pressed == 0))
			{
				USB_device_keys[cnt].pressed = 1;
				USB_device_keys[cnt].repeat_time = USB_device_millis; // Ensures that it will be processed immediately
			}
		}

		// Now let's process the modifier keys, which are all stored in different bit fields in byte 0 of the HID report
		USB_CTRL_PRESSED = ((USB_EP1_buffer[0] & USB_CTRL_MASK) == USB_CTRL_MASK);
		USB_ALT_PRESSED = ((USB_EP1_buffer[0] & USB_ALT_MASK) == USB_ALT_MASK);
		USB_SHIFT_PRESSED = ((USB_EP1_buffer[0] & USB_SHIFT_MASK) ==  USB_SHIFT_MASK);

		if ((USB_EP1_buffer[0] > 0) && (USB_EP1_buffer[2] > 0))
		{
			USB_SHIFT_PRESSED = ((USB_EP1_buffer[0] & USB_SHIFT_MASK) ==  USB_SHIFT_MASK);
		}
		
		for (unsigned int i=0; i<USB_KEYBOARD_NUM_KEYS; i++)
		{
			if (USB_device_keys[i].pressed && (USB_device_millis >= USB_device_keys[i].repeat_time))
			{
				USB_device_keys[i].repeat_time = USB_device_millis + USB_KEYBOARD_REPEAT_DELAY;

				if (USB_SHIFT_PRESSED)
				{
					usb_state_array[usb_writepos] = usb_conversion[i+0x80];
					usb_writepos++;
				}
				else
				{
					usb_state_array[usb_writepos] = usb_conversion[i];
					usb_writepos++;
				}
			}
		}
	}
	else if (USB_HID_report[1] == 0x01 && USB_HID_report[3] == 0x02) // mouse
	{	
		usb_mode = 0x02; // mouse
		
		usb_state_array[usb_writepos] = USB_EP1_buffer[0];
		
		if ((signed char)(USB_EP1_buffer[1]) < 0)
		{
			if (usb_cursor_x[(unsigned char)(usb_writepos-1)] < 0 - (signed char)(USB_EP1_buffer[1]))
			{
				usb_cursor_x[usb_writepos] = 0;
			}
			else
			{
				usb_cursor_x[usb_writepos] = 
					usb_cursor_x[(unsigned char)(usb_writepos-1)] + (signed char)(USB_EP1_buffer[1]);
			}
		}
		else if ((signed char)(USB_EP1_buffer[1]) >= 0)
		{
			if (usb_cursor_x[(unsigned char)(usb_writepos-1)] >= 512 - (signed char)(USB_EP1_buffer[1]))
			{
				usb_cursor_x[usb_writepos] = 512;
			}
			else
			{
				usb_cursor_x[usb_writepos] = 
					usb_cursor_x[(unsigned char)(usb_writepos-1)] + (signed char)(USB_EP1_buffer[1]);
			}
		}
		
		if ((signed char)(USB_EP1_buffer[2]) < 0)
		{
			if (usb_cursor_y[(unsigned char)(usb_writepos-1)] < 0 - (signed char)(USB_EP1_buffer[2]))
			{
				usb_cursor_y[usb_writepos] = 0;
			}
			else
			{
				usb_cursor_y[usb_writepos] = 
					usb_cursor_y[(unsigned char)(usb_writepos-1)] + (signed char)(USB_EP1_buffer[2]);
			}
		}
		else if ((signed char)(USB_EP1_buffer[2]) >= 0)
		{
			if (usb_cursor_y[(unsigned char)(usb_writepos-1)] >= 384 - (signed char)(USB_EP1_buffer[2]))
			{
				usb_cursor_y[usb_writepos] = 384;
			}
			else
			{
				usb_cursor_y[usb_writepos] = 
					usb_cursor_y[(unsigned char)(usb_writepos-1)] + (signed char)(USB_EP1_buffer[2]);
			}
		}
		
		usb_writepos++;
	}
	else
	{
		if (USB_EP1_buffer[0] == 0x00 && USB_EP1_buffer[1] == 0x14) // xbox-type controller
		{
			usb_mode = 0x03; // xbox-type controller
			
			// Format: 0xWXYZ
			// W = 1 start, 2 select, 3 both
			// X = 1 up, 2 down, 4 left, 8 right (combinations for multiple)
			// Y = 1 a, 2 b, 3 x, 4 y (combinations for multiple)
			// Z = 1 left bumper, 2 right bumper, 3 both
			usb_buttons[usb_writepos] = (unsigned int)((unsigned int)(USB_EP1_buffer[2] << 8) + USB_EP1_buffer[3]);
			
			usb_writepos++;
		}
		else
		{
			usb_mode = 0x00; // unknown

			/*
			SendHex(USB_EP1_RECEIVED);
			SendChar(' ');

			SendHex(USB_HID_report[1]);
			SendChar(':');

			SendHex(USB_HID_report[3]);
			SendChar(' ');

			for (unsigned char i = 0; i < 16; i++)
			{        
				SendHex(USB_EP1_buffer[i]);
				SendChar('.');
			}

			SendChar('\n');
			SendChar('\r');
			*/
		}
	}
}








// SDcard commands below
// This was used for the Arduino, but has been modified to work here.
volatile unsigned char sdcard_block[512];

void sdcard_longdelay(void)
{
	//DelayMS(1); // arbitrary amount of time to delay, only using 1ms here, should be around 10ms???
	
	// fine tune delay here
	unsigned long count = 0x00000000;
	count = (unsigned long)((unsigned long)((SYS_FREQ / 1000) * 1) / 4); // Convert microseconds us into how many clock ticks it will take
	_CP0_SET_COUNT(0); // Set Core Timer count to 0
	while (count > _CP0_GET_COUNT()); // Wait until Core Timer count reaches the number we calculated earlier
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




#include "ffconf.h"
#include "ff.h"
#include "ff.c"
#include "diskio.h"
#include "diskio.c"


// Global variables
FIL file; // File handle for the file we open
DIR dir; // Directory information for the current directory
FATFS fso; // File System Object for the file system we are reading from

void disk_setup()
{
    // Wait for the disk to initialise
    while(disk_initialize(0));
    // Mount the disk
    f_mount(&fso, "", 0);
    // Change dir to the root directory
    f_chdir("/");
    // Open the directory
    f_opendir(&dir, ".");
};

void disk_exchange()
{	
	// #The SDcard must have been formatted
	// #Check which drive it is, here /dev/sdc
	// sudo fdisk -l
	// sudo umount /dev/sdc
	// sudo mkfs.vfat /dev/sdc
	
	char buffer[16];
	unsigned int bytes;
	unsigned char result;
	
	for (unsigned int i=0; i<16; i++)
	{
		buffer[i] = 0;
	};
	
	// read in some text
	result = f_open(&file, "/INPUT.TXT", FA_READ);
	SendHex(result);
	result = f_read(&file, buffer, 16, &bytes);
	SendHex(result);
	result = f_close(&file);
	SendHex(result);
	
	/// write the same text
	result = f_open(&file, "/OUTPUT.TXT", FA_CREATE_NEW);
	SendHex(result);
	result = f_open(&file, "/OUTPUT.TXT", FA_WRITE);
	SendHex(result);
	result = f_write(&file, buffer, 16, &bytes);
	SendHex(result);
	result = f_close(&file);
	SendHex(result);
};








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
	for (unsigned int i=0; i<8; i++)
	{
		for (unsigned int j=0; j<8; j++)
		{
			screen_buffer[y*8+i][x*8+j] = value;
		}
	}		
};

// block is fancy looking
void tetra_block(unsigned int x, unsigned int y, unsigned char value)
{
	for (unsigned int i=0; i<8; i++)
	{
		for (unsigned int j=0; j<8; j++)
		{
			if (i == 0) screen_buffer[y*8+i][x*8+j] = 0xFF;
			else if (j == 0) screen_buffer[y*8+i][x*8+j] = 0xFF;
			else screen_buffer[y*8+i][x*8+j] = value;
		}
	}	
};

const unsigned int tetra_size_x = 10;
const unsigned int tetra_size_y = 20;
volatile unsigned char tetra_board[2*10*20];

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
	
	volatile unsigned int game_over[2];

	volatile unsigned int lines[2];
	volatile unsigned int timer[2];
	
	volatile int background;
	volatile int background_counter;
	volatile int background_trans;
	volatile int background_delay;
	
	volatile unsigned char joy_curr[2];
	volatile unsigned char joy_prev[2];
	volatile unsigned int joy_delay[2];
	volatile unsigned int joy_button_delay[2];
	
};

struct tetra_struct_vars tetra_vars;

void Tetra()
{	
	unsigned int press_delay = 0x0000;
	unsigned int press_speed = 0x002F;
	unsigned int heights_delay = 0x0000;
	unsigned int heights_speed = 0x002F;
	
	unsigned char directions[4] = { 0, 0, 0, 0 };
	unsigned char buttons[4] = { 2, 2, 2, 2 };
	unsigned int cursors[2] = { 200, 150 };
	unsigned char clicks[2] = { 2, 2 };
	unsigned char heights = 1;
	
	unsigned char key_active = 0;
	unsigned char mouse_active = 0;
	unsigned char mouse_move = 0;
	
	unsigned char pause[2] = { 0, 0 }; // not sure if this is needed any more, it was glitching at some point
	
	unsigned int overall_delay = 0x0000;
	
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
		
		tetra_vars.game_over[z] = 0x01;
		
		tetra_vars.lines[z] = 0;
		tetra_vars.timer[z] = 0;

		tetra_vars.joy_curr[z] = 0x00;
		tetra_vars.joy_prev[z] = 0x00;
		tetra_vars.joy_delay[z] = 0;
		tetra_vars.joy_button_delay[z] = 0;

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
	
	tetra_vars.background = 1;
	tetra_vars.background_counter = 0;
	tetra_vars.background_trans = -1;
	tetra_vars.background_delay = 0;
	
	// set background
	for (unsigned int y=0; y<384; y++)
	{
		for (unsigned int x=0; x<512; x++)
		{
			screen_buffer[y][x] = splash_brighting[y * 512 + x];
		}
	}
	
	volatile unsigned int test = 0;
	volatile unsigned int count = 0;
	
	while (1)
	{		
		if (overall_delay > 0x0000)
		{
			overall_delay--;
			continue;
		}
		
		overall_delay = 0x1FFF;
		
		if (tetra_vars.background_trans >= 0)
		{
			if (tetra_vars.background_delay > 0)
			{
			 	tetra_vars.background_delay--;
			}
			else
			{
				// set background
				for (unsigned int x=0; x<512; x++)
				{
					if (tetra_vars.background == 1) screen_buffer[tetra_vars.background_trans][x] = splash_brighting[tetra_vars.background_trans * 512 + x];
					else if (tetra_vars.background == 2) screen_buffer[tetra_vars.background_trans][x] = splash_floating[tetra_vars.background_trans * 512 + x];
					else if (tetra_vars.background == 3) screen_buffer[tetra_vars.background_trans][x] = splash_calling[tetra_vars.background_trans * 512 + x];
					else if (tetra_vars.background == 4) screen_buffer[tetra_vars.background_trans][x] = splash_blushing[tetra_vars.background_trans * 512 + x];
				}
				
				tetra_vars.background_trans--;
				
				tetra_vars.background_delay = 5;
			}
		}
		
		key_active = 0;
		mouse_active = 0;
		mouse_move = 0;
	
		// PS/2 keyboards and mice
		for (unsigned char p=0; p<2; p++)
		{
			if (ps2_ready[p] == 0x01 && ps2_mode[p] == 0x00) // ready and keyboard
			{
				key_active = 1;
				
				if (ps2_readpos[p] != ps2_writepos[p])
				{
					if (ps2_state_array[p][ps2_readpos[p]] == 0x0B) // release
					{
						ps2_readpos[p]++;

						if (ps2_state_array[p][ps2_readpos[p]] == 0x11)
						{
							directions[0] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x12)
						{
							directions[1] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x13)
						{
							directions[2] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x14)
						{
							directions[3] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x20)
						{
							buttons[0] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x30)
						{
							buttons[1] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x7A ||
							ps2_state_array[p][ps2_readpos[p]] == 0x5A)
						{
							buttons[2] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x78 ||
							ps2_state_array[p][ps2_readpos[p]] == 0x58)
						{
							buttons[3] = 0;
						}

						ps2_readpos[p]++;
					}
					else
					{
						if (ps2_state_array[p][ps2_readpos[p]] == 0x11)
						{
							directions[0] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x12)
						{
							directions[1] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x13)
						{
							directions[2] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x14)
						{
							directions[3] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x20)
						{
							buttons[0] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x30)
						{
							buttons[1] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x78 ||
							ps2_state_array[p][ps2_readpos[p]] == 0x58)
						{
							buttons[2] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x7A ||
							ps2_state_array[p][ps2_readpos[p]] == 0x5A)
						{
							buttons[3] = 1;
						}

						ps2_readpos[p]++;
					}

					break;
				}
			}
			else if (ps2_ready[p] == 0x01 && ps2_mode[p] == 0x01) // ready and mouse
			{
				mouse_active = 1;
				
				if (ps2_readpos[p] != ps2_writepos[p])
				{	
					mouse_move = 1;
					
					if ((ps2_state_array[p][ps2_readpos[p]] & 0x01) == 0x01) // left click
					{
						if (clicks[1] == 0) clicks[1] = 1;
					}
					else clicks[1] = 0;
					
					if ((ps2_state_array[p][ps2_readpos[p]] & 0x02) == 0x02) // right click
					{
						if (clicks[0] == 0) clicks[0] = 1;
						else clicks[0] = 2;
					}
					else clicks[0] = 0;
					
					cursors[0] = ps2_cursor_x[p][ps2_readpos[p]];
					cursors[1] = ps2_cursor_y[p][ps2_readpos[p]];
					
					ps2_readpos[p]++;	
				}
			}
		}
		
		// if device connected...
		if (USB_DEVICE_CONNECTED)
		{
			USB_host_tasks();
			USB_HID_tasks();
			if (USB_EP1_RECEIVED > 0)
			{
				USB_EP1_receive(USB_EP1_buffer);
				USB_HID_process_report();
				USB_EP1_RECEIVED--;
			}
			
			if (key_active == 0)
			{
				directions[0] = 0;
				directions[1] = 0;
				directions[2] = 0;
				directions[3] = 0;
			
				if (usb_mode == 0x01) // keyboard
				{
					key_active = 1;

					if (press_delay == 0x0000)
					{			
						if (USB_device_keys[0x52].pressed)
						{
							directions[0] = 1;
						}
						else if (USB_device_keys[0x51].pressed)
						{
							directions[1] = 1;
						}
						else if (USB_device_keys[0x50].pressed)
						{
							directions[2] = 1;
						}
						else if (USB_device_keys[0x4F].pressed)
						{
							directions[3] = 1;
						}
					}

					if (USB_device_keys[0x2C].pressed)
					{
						if (buttons[0] == 0) buttons[0] = 1;
						else buttons[0] = 2;
					}
					else buttons[0] = 0;
					
					if (USB_device_keys[0x1B].pressed)
					{
						if (buttons[2] == 0) buttons[2] = 1;
						else buttons[2] = 2;
					}
					else buttons[2] = 0;

					if (USB_device_keys[0x62].pressed)
					{
						if (buttons[1] == 0) buttons[1] = 1;
						else buttons[1] = 2;
					}
					else buttons[1] = 0;
					
					if (USB_device_keys[0x1D].pressed)
					{
						if (buttons[3] == 0) buttons[3] = 1;
						else buttons[3] = 2;
					}
					else buttons[3] = 0;
				}
			}
			
			if (mouse_active == 0)
			{
				if (usb_mode == 0x02) // mouse
				{	
					mouse_active = 1;
					
					if (usb_readpos != usb_writepos)
					{	
						mouse_move = 1;

						if ((usb_state_array[usb_readpos]) == 0x01) // left click
						{
							if (clicks[1] == 0) clicks[1] = 1;
						}
						else clicks[1] = 0;

						if ((usb_state_array[usb_readpos]) == 0x02) // right click
						{
							if (clicks[0] == 0) clicks[0] = 1;
							else clicks[0] = 2;
						}
						else clicks[0] = 0;

						cursors[0] = (unsigned int)(usb_cursor_x[usb_readpos]);
						cursors[1] = (unsigned int)(384 - usb_cursor_y[usb_readpos]);

						usb_readpos++;	
					}
				}
			}
		}
		
		if (key_active > 0)
		{
			if (press_delay > 0x0000) press_delay--;
		}
		
		if (mouse_active > 0)
		{
			if (heights_delay > 0x0000) heights_delay--;
		}
		
		tetra_vars.joy_prev[0] = tetra_vars.joy_curr[0];
		tetra_vars.joy_curr[0] = 0xFF; 
		
		if (pause[0] == 0)
		{
			if (PORTJbits.RJ0 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0x7F);
			if (PORTJbits.RJ1 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xBF);
			if (PORTJbits.RJ2 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xDF);
			if (PORTJbits.RJ3 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xEF);
			if (PORTJbits.RJ4 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xF7);
			if (PORTJbits.RJ5 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xFB);
			
			// change joystick select pin here if you want
		}
		else pause[0]--;
		
		if (usb_mode != 0x03) // xbox-type controller
		{
			tetra_vars.joy_prev[1] = tetra_vars.joy_curr[1];
			tetra_vars.joy_curr[1] = 0xFF; 

			if (pause[1] == 0)
			{
				if (PORTJbits.RJ6 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0x7F);
				if (PORTJbits.RJ7 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xBF);
				if (PORTJbits.RJ10 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xDF);
				if (PORTJbits.RJ12 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xEF);
				if (PORTJbits.RJ13 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xF7);
				if (PORTJbits.RJ14 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xFB);
				
				// change joystick select pin here if you want
			}
			else pause[1]--;
		}
		else
		{	
			tetra_vars.joy_prev[1] = tetra_vars.joy_curr[1];
			
			if (pause[1] == 0)
			{	
				while (usb_readpos != usb_writepos)
				{	
					tetra_vars.joy_curr[1] = 0xFF;

					if ((usb_buttons[usb_readpos] & 0x0100) == 0x0100) // up
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0x7F);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x80);
					}

					if ((usb_buttons[usb_readpos] & 0x0200) == 0x0200) // down
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xBF);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x40);
					}

					if ((usb_buttons[usb_readpos] & 0x0400) == 0x0400) // left
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xDF);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x20);
					}

					if ((usb_buttons[usb_readpos] & 0x0800) == 0x0800) // down
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xEF);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x10);
					}

					if ((usb_buttons[usb_readpos] & 0x0010) == 0x0010) // A
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xF7);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x08);
					}

					if ((usb_buttons[usb_readpos] & 0x0020) == 0x0020) // B
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xFB);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x04);
					}

					usb_readpos++;
				}
			}
			else pause[1]--;
		}
		
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

			if (tetra_vars.timer[z] > 288 - 16 * tetra_vars.speed[z]) // determines how fast it falls
			{
				tetra_vars.timer[z] = 0;
			}

			tetra_vars.joy_delay[z]++;

			if (tetra_vars.joy_delay[z] > 40) // determines button turbo speed
			{
				tetra_vars.joy_delay[z] = 0;
				tetra_vars.joy_prev[z] = tetra_vars.joy_prev[z] | 0xF0;
			}
			
			if (tetra_vars.joy_button_delay[z] > 0) tetra_vars.joy_button_delay[z]--;
			
			// mice
			if (z == 1)
			{
				if (mouse_active == 1 && mouse_move == 1)
				{
					tetra_vars.new_pos_x[z] = (unsigned int)(cursors[0] / (unsigned int)(512 / (tetra_size_x+2)));

					if (cursors[1] < 10) heights = 0;
					else if (cursors[1] > 374) heights = 2;
					else heights = 1;
				}
			}

			if ((((tetra_vars.joy_curr[z] & 0x80) == 0x00) && ((tetra_vars.joy_prev[z] & 0x80) == 0x80)) ||
				(press_delay == 0x0000 && directions[0] == 1 && z == 0) ||
				(heights_delay == 0x0000 && heights == 2 && z == 1)) // up
			{
				tetra_vars.timer[z] = 1; // not zero
				tetra_vars.joy_delay[z] = 0;
				
				if (z == 0) press_delay = press_speed;
				if (z == 1) heights_delay = heights_speed;
			}
			else if ((((tetra_vars.joy_curr[z] & 0x40) == 0x00) && ((tetra_vars.joy_prev[z] & 0x40) == 0x40)) ||
				(press_delay == 0x0000 && directions[1] == 1 && z == 0) ||
				(heights_delay == 0x0000 && heights == 0 && z == 1)) // down
			{
				tetra_vars.timer[z] = 0;
				tetra_vars.joy_delay[z] = 0;
				
				if (z == 0) press_delay = press_speed;
				if (z == 1) heights_delay = heights_speed;
			}
			else if ((((tetra_vars.joy_curr[z] & 0x20) == 0x00) && ((tetra_vars.joy_prev[z] & 0x20) == 0x20)) ||
				(press_delay == 0x0000 && directions[2] == 1 && z == 0)) // left
			{
				tetra_vars.new_pos_x[z]--;
				tetra_vars.joy_delay[z] = 0;
				
				if (z == 0) press_delay = press_speed;
			}
			else if ((((tetra_vars.joy_curr[z] & 0x10) == 0x00) && ((tetra_vars.joy_prev[z] & 0x10) == 0x10)) ||
				(press_delay == 0x0000 && directions[3] == 1 && z == 0)) // right
			{
				tetra_vars.new_pos_x[z]++;
				tetra_vars.joy_delay[z] = 0;
				
				if (z == 0) press_delay = press_speed;
			}
			else if ((((tetra_vars.joy_curr[z] & 0x08) == 0x00) && ((tetra_vars.joy_prev[z] & 0x08) == 0x08) && tetra_vars.joy_button_delay[z] == 0) ||
				(press_delay == 0x0000 && (buttons[0] == 1 || buttons[2] == 1) && z == 0) ||
				(clicks[0] == 1 && z == 1)) // button 1
			{
				tetra_vars.joy_button_delay[z] = 0x003F;
				
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
					tetra_vars.new_rot[z]++;
					if (tetra_vars.new_rot[z] == 4) tetra_vars.new_rot[z] = 0;
					tetra_vars.joy_delay[z] = 0;
				}
				
				if (z == 0)
				{
					press_delay = press_speed;
					if (buttons[0] == 1) buttons[0] = 2;
					if (buttons[2] == 1) buttons[2] = 2;
				}
				if (z == 1)
				{
					if (clicks[0] == 1) clicks[0] = 2;
				}
			}
			else if ((((tetra_vars.joy_curr[z] & 0x04) == 0x00) && ((tetra_vars.joy_prev[z] & 0x04) == 0x04) && tetra_vars.joy_button_delay[z] == 0) ||
				(press_delay == 0x0000 && (buttons[1] == 1 || buttons[3] == 1) && z == 0) ||
				(clicks[1] == 1 && z == 1)) // button 2
			{
				tetra_vars.joy_button_delay[z] = 0x003F;
				
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
				
				if (z == 0)
				{
					press_delay = press_speed;
					if (buttons[1] == 1) buttons[1] = 2;
					if (buttons[3] == 1) buttons[3] = 2;
				}
				if (z == 1)
				{
					if (clicks[1] == 1) clicks[1] = 2;
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
					music_note(523, 250, 0);

					if (tetra_vars.pos_y[z] == 4)
					{
						tetra_vars.game_over[z] = 1;
						
						// SOUND HERE
						music_note(262, 250, 0);
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
								
								tetra_vars.background_counter++;
								
								if (tetra_vars.background_counter >= 4 && tetra_vars.background_trans < 0)
								{
									tetra_vars.background_counter = 0;
									
									tetra_vars.background++;
									
									if (tetra_vars.background >= 5) tetra_vars.background = 1;
									
									tetra_vars.background_trans = 384;
								}
								
								pause[0] = 0x3F;
								pause[1] = 0x3F;
								
								// SOUND HERE
								music_note(1047, 250, 0);
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
		
		unsigned int vert = 0x06;
		unsigned int horz = 0x04;
		
		for (unsigned int i=0; i<(tetra_size_y-1)*8; i++)
		{
			for (unsigned int j=0; j<8; j++)
			{
				screen_buffer[i+(vert+2)*8][j+horz*8-8] = 0x92; // light grey
				screen_buffer[i+(vert+2)*8][j+horz*8+tetra_size_x*8] = 0x92; // light grey
			}
		}
		
		for (unsigned int i=0; i<4; i++)
		{
			for (unsigned int j=0; j<(tetra_size_x+2)*8; j++)
			{
				screen_buffer[i+(vert+2)*8-4][j+horz*8-8] = 0x92; // light grey
				screen_buffer[i+(vert+2)*8+(tetra_size_y-1)*8][j+horz*8-8] = 0x92; // light grey
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
						else if (tetra_vars.piece[0] == 2) tetra_block(j + horz, i + 0x02 + vert, 0xF0); // orange
						else if (tetra_vars.piece[0] == 3) tetra_block(j + horz, i + 0x02 + vert, 0xFC); // yellow
						else if (tetra_vars.piece[0] == 4) tetra_block(j + horz, i + 0x02 + vert, 0x1C); // green
						else if (tetra_vars.piece[0] == 5) tetra_block(j + horz, i + 0x02 + vert, 0xE3); // magenta
						else if (tetra_vars.piece[0] == 6) tetra_block(j + horz, i + 0x02 + vert, 0xE0); // red
						break;
					}
					case '#':
					{
						tetra_block(j + horz, i + 0x02 + vert, 0x49); // dark grey
						break;
					}
				}
			}
		}

		display_decimal((unsigned int)((horz + 0x05) * 8), vert * 8, tetra_vars.lines[0]);

		if (tetra_vars.new_piece[0] == 0) display_character(horz * 8, vert * 8, 'I');
		else if (tetra_vars.new_piece[0] == 1) display_character(horz * 8, vert * 8, 'J');
		else if (tetra_vars.new_piece[0] == 2) display_character(horz * 8, vert * 8, 'L');
		else if (tetra_vars.new_piece[0] == 3) display_character(horz * 8, vert * 8, 'O');
		else if (tetra_vars.new_piece[0] == 4) display_character(horz * 8, vert * 8, 'S');
		else if (tetra_vars.new_piece[0] == 5) display_character(horz * 8, vert * 8, 'T');
		else if (tetra_vars.new_piece[0] == 6) display_character(horz * 8, vert * 8, 'Z');

		if (tetra_vars.game_over[0] != 0x00)
		{
			display_string((unsigned int)((horz + 0x02) * 8), (unsigned int)(0xB0 + vert * 8), "Press \\");
			display_string((unsigned int)((horz + 0x02) * 8), (unsigned int)(0xB8 + vert * 8), "Button\\");
		}
		else
		{
			display_string((unsigned int)((horz + 0x02) * 8), (unsigned int)(0xB0 + vert * 8), "      \\");
			display_string((unsigned int)((horz + 0x02) * 8), (unsigned int)(0xB8 + vert * 8), "      \\");
		}
		
		vert = 0x06;
		horz = 0x32;
		
		for (unsigned int i=0; i<(tetra_size_y-1)*8; i++)
		{
			for (unsigned int j=0; j<8; j++)
			{
				screen_buffer[i+(vert+2)*8][j+horz*8-8] = 0x92; // light grey
				screen_buffer[i+(vert+2)*8][j+horz*8+tetra_size_x*8] = 0x92; // light grey
			}
		}
		
		for (unsigned int i=0; i<4; i++)
		{
			for (unsigned int j=0; j<(tetra_size_x+2)*8; j++)
			{
				screen_buffer[i+(vert+2)*8-4][j+horz*8-8] = 0x92; // light grey
				screen_buffer[i+(vert+2)*8+(tetra_size_y-1)*8][j+horz*8-8] = 0x92; // light grey
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
						if (tetra_vars.piece[1] == 0) tetra_block(j + horz, i + 0x02 + vert, 0x1F); // cyan
						else if (tetra_vars.piece[1] == 1) tetra_block(j + horz, i + 0x02 + vert, 0x03); // blue
						else if (tetra_vars.piece[1] == 2) tetra_block(j + horz, i + 0x02 + vert, 0xF0); // orange
						else if (tetra_vars.piece[1] == 3) tetra_block(j + horz, i + 0x02 + vert, 0xFC); // yellow
						else if (tetra_vars.piece[1] == 4) tetra_block(j + horz, i + 0x02 + vert, 0x1C); // green
						else if (tetra_vars.piece[1] == 5) tetra_block(j + horz, i + 0x02 + vert, 0xE3); // magenta
						else if (tetra_vars.piece[1] == 6) tetra_block(j + horz, i + 0x02 + vert, 0xE0); // red
						break;
					}
					case '#':
					{
						tetra_block(j + horz, i + 0x02 + vert, 0x49); // mid grey
						break;
					}
				}
			}
		}

		display_decimal((unsigned int)((horz + 0x05) * 8), vert * 8, tetra_vars.lines[1]);

		if (tetra_vars.new_piece[1] == 0) display_character(horz * 8, vert * 8, 'I');
		else if (tetra_vars.new_piece[1] == 1) display_character(horz * 8, vert * 8, 'J');
		else if (tetra_vars.new_piece[1] == 2) display_character(horz * 8, vert * 8, 'L');
		else if (tetra_vars.new_piece[1] == 3) display_character(horz * 8, vert * 8, 'O');
		else if (tetra_vars.new_piece[1] == 4) display_character(horz * 8, vert * 8, 'S');
		else if (tetra_vars.new_piece[1] == 5) display_character(horz * 8, vert * 8, 'T');
		else if (tetra_vars.new_piece[1] == 6) display_character(horz * 8, vert * 8, 'Z');

		if (tetra_vars.game_over[1] != 0x00)
		{
			display_string((unsigned int)((horz + 0x02) * 8), (unsigned int)(0xB0 + vert * 8), "Press \\");
			display_string((unsigned int)((horz + 0x02) * 8), (unsigned int)(0xB8 + vert * 8), "Button\\");
		}
		else
		{
			display_string((unsigned int)((horz + 0x02) * 8), (unsigned int)(0xB0 + vert * 8), "      \\");
			display_string((unsigned int)((horz + 0x02) * 8), (unsigned int)(0xB8 + vert * 8), "      \\");
		}
	}
};




void BadApple()
{ 
	while (1)
	{
		for (unsigned int y=0; y<384; y++)
		{
			for (unsigned int x=0; x<512; x++)
			{
				screen_buffer[y][x] = splash_television[y * 512 + x];
				//screen_buffer[y][x] = 0x25; // grey?
			}
		}

		int test = 0;

		for (int i=0; i<5; i++)
		{
			test = sdcard_initialize();

			if (test == 1) break;
		}

		if (test == 1)
		{
			// move on
		}
		else
		{
			// lock up
			while (1)
			{
				for (unsigned int i=0; i<384; i++)
				{
					for (unsigned int j=0; j<512; j++)
					{
						screen_buffer[i][j] = 0x00;
					}
				}

				DelayMS(100);
				DelayMS(100);
				DelayMS(100);
				DelayMS(100);

				for (unsigned int i=0; i<384; i++)
				{
					for (unsigned int j=0; j<512; j++)
					{
						screen_buffer[i][j] = 0xFF;
					}
				}

				DelayMS(100);
				DelayMS(100);
				DelayMS(100);
				DelayMS(100);
			}
		}  

		unsigned int address_high = 0x0000, address_low = 0x0000;

		unsigned int x = 0x0000;
		unsigned int y = 0x0000;
		unsigned char value = 0x00;

		unsigned char temp_value = 0x00;

		unsigned int frames = 0;

		while (frames < 13138) // total frames in the video
		{
			frames += 2; // only 30 FPS
			
			x = 88;
			y = 104;

			for (unsigned int l=0; l<12; l++)
			{
				//sdcard_readblock(address_high, (unsigned int)(address_low + (j * 2)));

				sdcard_disable();
				sdcard_pump();
				sdcard_longdelay(); // this is probably not needed
				sdcard_enable();
				sdcard_sendbyte(0x51); // CMD17 = 0x40 + 0x11 (17 in hex)
				sdcard_sendbyte((unsigned char)((address_high)&0x00FF));
				sdcard_sendbyte((unsigned char)(((address_low)&0xFF00) >> 8));
				sdcard_sendbyte((unsigned char)((address_low)&0x00FE)); // only blocks of 512 bytes
				sdcard_sendbyte(0x00);
				sdcard_sendbyte(0x01); // CRC (general)
				temp_value = sdcard_waitresult(); // command response
				if (temp_value == 0xFF) { break; }
				else if (temp_value != 0x00) { break; } // expecting 0x00
				temp_value = sdcard_waitresult(); // data packet starts with 0xFE
				if (temp_value == 0xFF) { break; }
				else if (temp_value != 0xFE) { break; }

				for (unsigned int i=0; i<16; i++)
				{
					for (unsigned int j=0; j<32; j++) // packet of 512 bytes
					{					
						// get value from SDcard
						value = sdcard_receivebyte();

						for (unsigned int k=0; k<8; k++)
						{
							if ((value & 0x01) == 0x01)
							{
								screen_buffer[y][x] = 0xFF;
							}
							else
							{
								screen_buffer[y][x] = 0x00;
							}

							value = (unsigned int)(value >> 1);

							x += 1;
						}
					}

					x = 88;
					y += 1;
				}

				temp_value = sdcard_receivebyte(); // data packet ends with 0x55 then 0xAA
				temp_value = sdcard_receivebyte(); // ignore here
				sdcard_disable();

				if (address_low == 0xFFFE)
				{
					address_low = 0;
					
					address_high += 1; 
				}
				else
				{
					address_low += 2;
				}
			}

			// uncomment if you want to skip next frame entirely!
			//for (unsigned int l=0; l<12; l++)
			//{
			//	if (address_low == 0xFFFE)
			//	{
			//		address_low = 0;
			//		
			//		address_high += 1; 
			//	}
			//	else
			//	{
			//		address_low += 2;
			//	}
			//}
			
			// fine tune delay here
			unsigned long count = 0x00000000;
			count = (unsigned long)(((unsigned long)((SYS_FREQ / 1000) * 17) / 5) / 2); // Convert microseconds us into how many clock ticks it will take
			_CP0_SET_COUNT(0); // Set Core Timer count to 0
			while (count > _CP0_GET_COUNT()); // Wait until Core Timer count reaches the number we calculated earlier
		}
	}
}


volatile char scratchpad_buffer[64][48];

void Scratchpad()
{
	char key_value = 0x00;
	char key_prev = '*';
	
	char test = 0x00;
	
	unsigned int pos_x = 0x00;
	unsigned int pos_y = 0x00;
	
	unsigned int mouse_state[5] = { 0, 0, 0, 0, 0 };
	
	unsigned char joy_curr[2] = { 0xFF, 0xFF };
	unsigned int joy_delay = 0x0000;
	unsigned int joy_speed = 0x01FF;
	
	unsigned int overall_delay = 0x0000;
	
	for (unsigned int y=0; y<384; y++)
	{
		for (unsigned int x=0; x<512; x++)
		{
			screen_buffer[y][x] = 0x00; // black
		}
	}
	
	for (unsigned int y=0; y<48; y++)
	{
		for (unsigned int x=0; x<64; x++)
		{
			scratchpad_buffer[x][y] = ' ';
		}
	}
	
	display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
	
	while (1)
	{
		if (overall_delay > 0x0000)
		{
			overall_delay--;
			continue;
		}
		
		overall_delay = 0x1FFF;
		
		key_value = input_ps2_keyboard();
		
		// if device connected...
		if (USB_DEVICE_CONNECTED)
		{
			USB_host_tasks();
			USB_HID_tasks();
			if (USB_EP1_RECEIVED > 0)
			{
				USB_EP1_receive(USB_EP1_buffer);
				USB_HID_process_report();
				USB_EP1_RECEIVED--;
			}
			
			if (key_value == 0x00)
			{
				key_value = input_usb_keyboard();
			}
		}
		
		joy_curr[0] = 0xFF; 
		
		if (PORTJbits.RJ0 == 0) joy_curr[0] = (joy_curr[0] & 0x7F);
		if (PORTJbits.RJ1 == 0) joy_curr[0] = (joy_curr[0] & 0xBF);
		if (PORTJbits.RJ2 == 0) joy_curr[0] = (joy_curr[0] & 0xDF);
		if (PORTJbits.RJ3 == 0) joy_curr[0] = (joy_curr[0] & 0xEF);
		if (PORTJbits.RJ4 == 0) joy_curr[0] = (joy_curr[0] & 0xF7);
		if (PORTJbits.RJ5 == 0) joy_curr[0] = (joy_curr[0] & 0xFB);
		
		if (usb_mode != 0x03) // xbox-type controller
		{
			joy_curr[1] = 0xFF; 
			
			if (PORTJbits.RJ6 == 0) joy_curr[1] = (joy_curr[1] & 0x7F);
			if (PORTJbits.RJ7 == 0) joy_curr[1] = (joy_curr[1] & 0xBF);
			if (PORTJbits.RJ10 == 0) joy_curr[1] = (joy_curr[1] & 0xDF);
			if (PORTJbits.RJ12 == 0) joy_curr[1] = (joy_curr[1] & 0xEF);
			if (PORTJbits.RJ13 == 0) joy_curr[1] = (joy_curr[1] & 0xF7);
			if (PORTJbits.RJ14 == 0) joy_curr[1] = (joy_curr[1] & 0xFB);
		}
		else
		{
			while (usb_readpos != usb_writepos)
			{
				joy_curr[1] = 0xFF;
				
				if ((usb_buttons[usb_readpos] & 0x0100) == 0x0100) // up
				{
					joy_curr[1] = (joy_curr[1] & 0x7F);
				}
				else
				{
					joy_curr[1] = (joy_curr[1] | 0x80);
				}
				
				if ((usb_buttons[usb_readpos] & 0x0200) == 0x0200) // down
				{
					joy_curr[1] = (joy_curr[1] & 0xBF);
				}
				else
				{
					joy_curr[1] = (joy_curr[1] | 0x40);
				}
				
				if ((usb_buttons[usb_readpos] & 0x0400) == 0x0400) // left
				{
					joy_curr[1] = (joy_curr[1] & 0xDF);
				}
				else
				{
					joy_curr[1] = (joy_curr[1] | 0x20);
				}
				
				if ((usb_buttons[usb_readpos] & 0x0800) == 0x0800) // down
				{
					joy_curr[1] = (joy_curr[1] & 0xEF);
				}
				else
				{
					joy_curr[1] = (joy_curr[1] | 0x10);
				}
				
				if ((usb_buttons[usb_readpos] & 0x0010) == 0x0010) // A
				{
					joy_curr[1] = (joy_curr[1] & 0xF7);
				}
				else
				{
					joy_curr[1] = (joy_curr[1] | 0x08);
				}
				
				if ((usb_buttons[usb_readpos] & 0x0020) == 0x0020) // B
				{
					joy_curr[1] = (joy_curr[1] & 0xFB);
				}
				else
				{
					joy_curr[1] = (joy_curr[1] | 0x04);
				}
				
				usb_readpos++;
			}
		}
		
		if (key_value == 0x00)
		{
			for (unsigned char z=0; z<2; z++)
			{
				if (joy_delay == 0x0000)
				{
					if ((joy_curr[z] & 0x08) == 0x00) //&& (joy_prev[z] & 0x08) == 0x08) // button 1
					{
						scratchpad_buffer[pos_x/8][pos_y/8] = ' ';
						display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);	
						joy_delay = joy_speed;
					}
					else if ((joy_curr[z] & 0x04) == 0x00) //&& (joy_prev[z] & 0x04) == 0x04) // button 2
					{
						scratchpad_buffer[pos_x/8][pos_y/8] = key_prev;
						display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);	
						joy_delay = joy_speed;
					}
					
					if ((joy_curr[z] & 0x80) == 0x00) //&& (joy_prev[z] & 0x80) == 0x80) // up
					{
						key_value = 0x11;
						joy_delay = joy_speed;
					}
					else if ((joy_curr[z] & 0x40) == 0x00) //&& (joy_prev[z] & 0x40) == 0x40) // down
					{
						key_value = 0x12;
						joy_delay = joy_speed;
					}
					else if ((joy_curr[z] & 0x20) == 0x00) //&& (joy_prev[z] & 0x20) == 0x20) // left
					{
						key_value = 0x13;
						joy_delay = joy_speed;
					}
					else if ((joy_curr[z] & 0x10) == 0x00) //&& (joy_prev[z] & 0x10) == 0x10) // right
					{
						key_value = 0x14;
						joy_delay = joy_speed;
					}
				}
				else joy_delay--;
			}
		}
		
		if (key_value != 0x00)
		{
			if (key_value == 0x1B) // escape
			{
				music_note(262, 250, 0);
				
				pos_x = 0x00;
				pos_y = 0x00;
	
				for (unsigned int y=0; y<384; y++)
				{
					for (unsigned int x=0; x<512; x++)
					{
						screen_buffer[y][x] = 0x00; // black
					}
				}

				for (unsigned int y=0; y<48; y++)
				{
					for (unsigned int x=0; x<64; x++)
					{
						scratchpad_buffer[x][y] = ' ';
					}
				}
	
				display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
			}
			else if (key_value == 0x0D) // return
			{
				display_character(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
				
				pos_x = 0x00;
				pos_y += 8;
				
				if (pos_y >= 376)
				{
					for (unsigned int y=0; y<376; y++)
					{
						for (unsigned int x=0; x<512; x++)
						{
							screen_buffer[y][x] = screen_buffer[y+8][x];
						}
					}
					
					for (unsigned int y=376; y<384; y++)
					{
						for (unsigned int x=0; x<512; x++)
						{
							screen_buffer[y][x] = 0x00; // black
						}
					}
					
					for (unsigned int y=0; y<47; y++)
					{
						for (unsigned int x=0; x<64; x++)
						{
							scratchpad_buffer[x][y] = scratchpad_buffer[x][y+1];
						}
					}
					
					for (unsigned int x=0; x<64; x++)
					{
						scratchpad_buffer[x][47] = ' ';
					}
					
					pos_y -= 8;
				}
				
				display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
			}
			else if (key_value == 0x08) // backspace
			{
				display_character(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
				
				if (pos_x >= 8)
				{
					pos_x -= 8;
				}
				
				scratchpad_buffer[pos_x/8][pos_y/8] = ' ';
				
				display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
			}
			else if (key_value == 0x11) // up
			{
				display_character(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
				
				if (pos_y >= 8)
				{
					pos_y -= 8;
				}
				
				display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
			}
			else if (key_value == 0x12) // down
			{
				display_character(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
				
				if (pos_y < 368)
				{
					pos_y += 8;
				}
				
				display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
			}
			else if (key_value == 0x13) // left
			{
				display_character(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
				
				if (pos_x >= 8)
				{
					pos_x -= 8;
				}
				
				display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
			}
			else if (key_value == 0x14) // right
			{
				display_character(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
				
				if (pos_x < 496)
				{
					pos_x += 8;
				}
				
				display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
			}
			else if (key_value >= 32)
			{
				key_prev = key_value;
				
				scratchpad_buffer[pos_x/8][pos_y/8] = key_value;
				
				display_character(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
				
				if (pos_x < 496)
				{
					pos_x += 0x08;
				}
				
				display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
			}
		}
		
		test = input_ps2_mouse((unsigned int *)mouse_state);
		
		if (test == 0x00) test = input_usb_mouse((unsigned int *)mouse_state);
		
		if (test != 0x00)
		{
			display_character(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);
			
			pos_x = (unsigned int)(mouse_state[3] / 8) * 8;
			
			if (pos_x > 496) pos_x = 496;
			
			pos_y = (unsigned int)(mouse_state[4] / 8) * 8;
			
			if (pos_y > 368) pos_y = 368;
			
			if (mouse_state[0] != 0x0000) // left
			{
				scratchpad_buffer[pos_x/8][pos_y/8] = key_prev;
			}
			else if (mouse_state[1] != 0x0000) // right
			{
				scratchpad_buffer[pos_x/8][pos_y/8] = ' ';
			}
			
			display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);	
		}
	}
}



volatile unsigned char menu_pos = 0;
volatile unsigned char menu_max = 1;
volatile unsigned char menu_loop = 1;
volatile unsigned char menu_key = 0;
volatile unsigned int menu_joy = 0xFFFF;
volatile unsigned int menu_mouse[5] = { 0, 0, 0, 0, 0 };
volatile unsigned char menu_up = 0;
volatile unsigned char menu_down = 0;
volatile unsigned int menu_delay = 0x0000;

int main()
{
	char test = 0x00;
	
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
	
	CNPDFbits.CNPDF3 = 1; // Pull down to ensure host mode
    LATFbits.LATF3 = 0; // USB host mode

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

    // Set up caching
    unsigned int cp0 = _mfc0(16, 0);
    cp0 &= ~0x07;
    cp0 |= 0b011; // K0 = Cacheable, non-coherent, write-back, write allocate
    _mtc0(16, 0, cp0);  
	
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
	DCH0SSIZ = 512; //400; // source size
	DCH0DSIZ = 1; // dst size 
	DCH0CSIZ = 512; //400; // 1 byte per event

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
	// Timer for 'USB_device_millis' used with USB
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
	
    // Init USB_device_millis
    USB_device_millis = 0;
	
	
	
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
	for (unsigned int y=0; y<384; y++)
	{
		for (unsigned int x=0; x<512; x++)
		{
			screen_buffer[y][x] = splash_pointing[y * 512 + x];
			//screen_buffer[y][x] = 0x25; // grey?
			//screen_buffer[y][x] = (unsigned char)((x + y) % 256); // test pattern
			//if (x % 2 == 0) screen_buffer[y][x] = 0xFF; // white
			//else screen_buffer[y][x] = 0x1F; // cyan
		}
	}
	
	// set black scanline
	for (unsigned int x=0; x<512; x++) screen_blank[x] = 0x00;
	
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
	//while (U3STAbits.UTXBF == 1) { }
	U3TXREG = '*';
	
	// turn on video timers
	T4CONbits.ON = 1; // turn on TMR4 (independent of others)
	T3CONbits.ON = 1; // turn on TMR3 (cycle offset pre-calculated above)
	T5CONbits.ON = 1; // turn on TMR5 (cycle offset pre-calculated above)
	T2CONbits.ON = 1; // turn on TMR2 (cycle offset pre-calculated above)
	
	
	USB_setup(); // initialize USB
	
	DelayMS(1000); // settling delay, avoid garbage characters
	
       

	
	display_string(24, 16, "Acolyte Hand PIC'd 32\\");
	
	display_string(24, 300, " Tetra     \\");
	display_string(24, 308, " Bad Apple \\");
	display_string(24, 316, " Scratchpad\\");
	display_string(24, 324, "           \\");
	
	menu_max = 4; // number of menu items, change accordingly
	
	display_character(24, 300, '>');
	
	

	
	
	while (menu_loop > 0)
	{
		if (menu_up == 1)
		{
			menu_up = 0;
			
			if (menu_pos > 0)
			{
				display_character(24, 300 + menu_pos * 8, ' ');
					
				menu_pos--;
					
				display_character(24, 300 + menu_pos * 8, '>');
				
				music_note(523, 250, 0);
			}
		}
		
		if (menu_down == 1)
		{
			menu_down = 0;
			
			if (menu_pos < menu_max-1)
			{
				display_character(24, 300 + menu_pos * 8, ' ');

				menu_pos++;

				display_character(24, 300 + menu_pos * 8, '>');
				
				music_note(523, 250, 0);
			}
		}	
	
		if (menu_delay > 0x0000)
		{
			menu_delay--;
			continue;
		}
		
		menu_delay = 0x7FFF;
		
		menu_key = input_ps2_keyboard();
		
		// if device connected...
		if (USB_DEVICE_CONNECTED)
		{
			USB_host_tasks();
			USB_HID_tasks();
			if (USB_EP1_RECEIVED > 0)
			{	
				USB_EP1_receive(USB_EP1_buffer);
				USB_HID_process_report();
				USB_EP1_RECEIVED--;
			}
		
			if (menu_key == 0x00)
			{
				menu_key = input_usb_keyboard();
			}
		}
		
		if (menu_key != 0x00)
		{
			if (menu_key == 0x0D || menu_key == 0x20) // enter or space
			{
				menu_loop = 0;
			}
			else if (menu_key == 0x11) // up
			{
				menu_up = 1;
			}
			else if (menu_key == 0x12) // down
			{
				menu_down = 1;
			}
		}
		
		if (PORTJbits.RJ0 == 0) // up
		{
			if ((menu_joy & 0x0001) == 0x0000)
			{
				menu_joy = (menu_joy | 0x0001);
				
				menu_up = 1;
			}
		}
		else menu_joy = (menu_joy & 0xFFFE);
		
		if (PORTJbits.RJ1 == 0) // down
		{
			if ((menu_joy & 0x0002) == 0x0000)
			{
				menu_joy = (menu_joy | 0x0002);
				
				menu_down = 1;
			}
		}
		else menu_joy = (menu_joy & 0xFFFD);
		
		if (PORTJbits.RJ4 == 0 || PORTJbits.RJ5 == 0) // either button
		{
			menu_loop = 0;
		}
		
		if (usb_mode != 0x03) // xbox-type controller
		{
			if (PORTJbits.RJ6 == 0) // up
			{
				if ((menu_joy & 0x0040) == 0x0000)
				{
					menu_joy = (menu_joy | 0x0040);

					menu_up = 1;
				}
			}
			else menu_joy = (menu_joy & 0xFFBF);

			if (PORTJbits.RJ7 == 0) // down
			{
				if ((menu_joy & 0x0080) == 0x0000)
				{
					menu_joy = (menu_joy | 0x0080);

					menu_down = 1;
				}
			}
			else menu_joy = (menu_joy & 0xFF7F);
		
			if (PORTJbits.RJ13 == 0 || PORTJbits.RJ14 == 0) // either button
			{
				menu_loop = 0;
			}
		}
		else
		{
			while (usb_readpos != usb_writepos)
			{
				if ((usb_buttons[usb_readpos] & 0x0100) == 0x0100) // up
				{
					menu_up = 1;
				}
				
				if ((usb_buttons[usb_readpos] & 0x0200) == 0x0200) // down
				{
					menu_down = 1;
				}
				
				if ((usb_buttons[usb_readpos] & 0x0010) == 0x0010) // A
				{
					menu_loop = 0;
				}
				
				if ((usb_buttons[usb_readpos] & 0x0020) == 0x0020) // B
				{
					menu_loop = 0;
				}
				
				usb_readpos++;
			}
		}
		
		test = input_ps2_mouse((unsigned int *)menu_mouse);
		
		if (test == 0x00) test = input_usb_mouse((unsigned int *)menu_mouse);
		
		if (test != 0x00)
		{	
			for (unsigned char i=0; i<menu_max; i++)
			{
				if (menu_mouse[4] <= (unsigned int)(i * 384 / menu_max))
				{
					display_character(24, 300 + menu_pos * 8, ' ');

					menu_pos = i;

					display_character(24, 300 + menu_pos * 8, '>');

					music_note(523, 250, 0);
					
					break;
				}
			}
			
			if (menu_mouse[0] != 0x0000)
			{
				menu_loop = 0;
			}
		}
	}

	music_note(1047, 250, 0);

	

	
	if (menu_pos == 0) Tetra();
	else if (menu_pos == 1) BadApple();
	else if (menu_pos == 2) Scratchpad();
	else if (menu_pos == 3) { }
	
	
	
	
	disk_setup(); // initialize disk
	disk_exchange(); // read INPUT.TXT and write OUTPUT.TXT
	
	
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
		while (U3STAbits.UTXBF == 1) { }
		U3TXREG = '@';
*/
/*		
		unsigned char dummy = 0x00;
		
		for (unsigned int i=0; i<10000; i++)
		{
			for (unsigned int j=0; j<1000; j++)
			{
				dummy++;
			}
		}
		
		PORTDbits.RD11 = 0;
		
		for (unsigned int i=0; i<10000; i++)
		{
			for (unsigned int j=0; j<1000; j++)
			{
				dummy++;
			}
		}
		
		PORTDbits.RD11 = 1;
*/
		//asm("WAIT");
