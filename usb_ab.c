

unsigned char usb_class = 0; // 0 = none, 1 = host, 2 = device


#define USBA_ENDPOINT_BUFFER_SIZE 64
#define USBA_EP0_WAIT_TIMEOUT 40000
#define USBA_VBUS_VALID 0x3

#define USBA_KEYBOARD_NUM_KEYS 256 //128
#define USBA_KEY_NUMLOCK 0x53
#define USBA_KEY_CAPSLOCK 0x39
#define USBA_KEY_SCROLLLOCK 0x47
#define USBA_CTRL_MASK 0x1
#define USBA_ALT_MASK 0x4
#define USBA_SHIFT_MASK 0x2

#define USBA_KEYBOARD_REPEAT_DELAY 250       // 250ms between key repeats, if a key is held down
#define USBA_HID_REQUEST_DELAY 25        // 25 USBA_device_milliseconds between keyboard report requests

typedef struct
{
    unsigned char pressed;      // Is the key currently pressed?
    unsigned long repeat_time;  // When should this press be repeated (if it's pressed)?
    unsigned char released;     // Was the key released?
    
} USBA_HID_KEY;

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
} USBA_DEVICE_DESCRIPTOR;

void USBA_EP0_send(unsigned char *buffer, uint32_t length);
void USBA_EP0_send_setup(unsigned char *buffer, uint32_t length);
void USBA_EP0_receive(unsigned char *buffer, uint32_t length);
int USBA_EP0_receive_long(unsigned char *buffer, uint32_t length);
void USBA_EP1_receive(unsigned char *buffer);
void USBA_init_endpoints(int USBA_address);
void USBA_HOST_reset_device();

unsigned char USBA_DEVICE_DESCRIPTOR_REQUEST[8] = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00};
unsigned char USBA_DEVICE_CONFIGURATION_REQUEST[8] = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x09, 0x00};
unsigned char USBA_DEVICE_STRING_REQUEST[8] = {0x80, 0x06, 0x02, 0x03, 0x09, 0x04, 0x04, 0x00};
unsigned char USBA_DEVICE_GET_HID_REPORT_REQUEST[8] = {0x81, 0x06, 0x00, 0x22, 0x00, 0x00, 0x09, 0x00};
unsigned char USBA_DEVICE_ASSIGN_ADDRESS[8] = {0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char USBA_DEVICE_SET_CONFIGURATION[8] = {0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char USBA_DEVICE_SET_INTERFACE[8] = {0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char USBA_DEVICE_SET_REPORT[8] = {0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00};
unsigned char USBA_DEVICE_SET_IDLE[8] = {0x21, 0x0A, 0x00, 0x7D, 0x00, 0x00, 0x00, 0x00};
unsigned char USBA_DEVICE_SET_PROTOCOL[8] = {0x21, 0x0B, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char __attribute__ ((coherent, aligned(8))) USBA_EP0_buffer[USBA_ENDPOINT_BUFFER_SIZE];
unsigned char __attribute__ ((coherent, aligned(8))) USBA_EP1_buffer[USBA_ENDPOINT_BUFFER_SIZE];
USBA_DEVICE_DESCRIPTOR USBA_device_descriptor;
unsigned char USBA_device_configuration[255];
unsigned char USBA_device_name[255];
unsigned char USBA_HID_report[512];

unsigned char USBA_NUM_LOCK_STATUS = 0;
unsigned char USBA_CAPS_LOCK_STATUS = 0;
unsigned char USBA_SCROLL_LOCK_STATUS = 0;
unsigned char USBA_SHIFT_PRESSED = 0;
unsigned char USBA_CTRL_PRESSED = 0;
unsigned char USBA_ALT_PRESSED = 0;
volatile char USBA_EP0_RECEIVED = 0;
volatile char USBA_EP0_IF = 0;
volatile char USBA_EP1_RECEIVED = 0;
volatile char USBA_DEVICE_CONNECTED = 0;
volatile char USBA_STAGE = 0;
volatile char USBA_HID_STAGE = 0;
volatile char USBA_HID_BUSY = 0;

int USBA_config_length;
USBA_HID_KEY USBA_device_keys[USBA_KEYBOARD_NUM_KEYS];
int USBA_DEVICE_ADDRESS;
volatile unsigned long USBA_device_millis;
volatile unsigned int USBA_device_millis_delay = 0;
unsigned long USBA_device_timer;
char USBA_LED_data = 0;
int USBA_EP0_LENGTH = 8;




#define USBB_MAX_BUFFER_SIZE 512
#define USBB_EP0_WAIT_TIMEOUT 1000

typedef struct
{
    int baud;
    short stop_bits;
    short parity_bits;
    short data_bits;
} USBB_LINE_CODING;

typedef struct
{
    volatile unsigned short rx_num_bytes;
    volatile unsigned short tx_num_bytes;
    volatile unsigned char tx_buffer[USBB_MAX_BUFFER_SIZE];
    volatile unsigned char rx_buffer[USBB_MAX_BUFFER_SIZE];
} USBB_ENDPOINT;

typedef struct
{
    volatile unsigned char bmRequestType;
    volatile unsigned char bmRequest;
    volatile unsigned short wValue;
    volatile unsigned short wIndex;
    volatile unsigned short wLength;
} USBB_TRANSACTION;

unsigned char USBB_device_description[] = {
/* Descriptor Length						*/ 18, /* Always 18 or 0x12 */
/* DescriptorType: DEVICE					*/ 0x01,
/* bcdUSB (ver 2.0)							*/ 0x00,0x02,
/* bDeviceClass								*/ 0x00,
/* bDeviceSubClass							*/ 0x00,
/* bDeviceProtocol							*/ 0x00,
/* bMaxPacketSize0							*/ 0x40, /* Always 64 or 0x40 for High Speed USB */
/* idVendor									*/ 0xD8,0x04, /* e.g. - 0x04D8 - Microchip VID */
/* idProduct								*/ 0x0A,0x00, /* e.g. - 0x000A */
/* bcdDevice								*/ 0x00,0x02, /* e.g. - 02.00 */
/* iManufacturer							*/ 0x01,
/* iProduct									*/ 0x02,
/* iSerialNumber							*/ 0x03, 
/* bNumConfigurations						*/ 0x01
};

unsigned char USBB_address = 0;
volatile char USBB_set_address = 0;
volatile char USBB_line_control_state = 0;

USBB_ENDPOINT USBB_EP[3];
USBB_TRANSACTION USBB_transaction;
USBB_LINE_CODING USBB_line_coding;

void USBB_init(void *receive_callback);
void USBB_connect();
void USBB_EP2_send(volatile unsigned char *buffer, int size);
void (*USBB_RECEIVE_CALLBACK)(volatile unsigned char *buffer, int length);


unsigned char USBB_config_descriptor[] = {
// Referenced from: https://gist.github.com/tai/acd59b125a007ad47767
/*********************************************************************
 Configuration Descriptors 
 *********************************************************************/
/*  bLength (Descriptor Length)             */ 9, 
/*  bDescriptorType: CONFIG					*/ 0x02,
/*  wTotalLength							*/ 0x43,0x00,
/*  bNumInterfaces							*/ 2, 
/*  bConfigurationValue						*/ 1, 
/*  iConfiguration							*/ 0, 
/*  bmAttributes							*/ 0x80, /* bit 6 set = bus powered = 0x80, 0xC0 is for self powered */
/*  bMaxPower								*/ 0x32, /* Value x 2mA, set to 0 for self powered, was 0x32 */
/*********************************************************************
  Interface 0 - Communications Class
 *********************************************************************/                                               
/* bLength                                  */ 0x09,
/* bDescriptorType: INTERFACE               */ 0x04,
/* bInterfaceNumber                         */ 0x00,
/* bAlternateSetting                        */ 0x00,
/* bNumEndpoints: 0 endpoint(s)             */ 0x01,
/* bInterfaceClass: CDC                     */ 0x02,
/* bInterfaceSubclass:Abstract Control Model*/ 0x02,
/* bInterfaceProtocol:AT Commands V.25ter   */ 0x01,
/* iInterface                               */ 0x00,
/*********************************************************************
 Header Functional Descriptor
 *********************************************************************/                                               
/* bLength                                  */ 0x05,
/* bDescriptorType: CS_INTERFACE            */ 0x24,
/* bDescriptorSubtype: HEADER FD            */ 0x00,
/* bcdADC                                   */ 0x20,0x01,
/*********************************************************************
 Abstract Control Model Functional Descriptor
 *********************************************************************/                                               
/* bLength                                  */ 0x04,
/* bDescriptorType: CS_INTERFACE            */ 0x24,
/* bDescriptorSubtype: ACM-FD               */ 0x02,
/* bmCapabilities                           */ 0x02,
/*********************************************************************
 Union Functional Descriptor
 *********************************************************************/                                               
/* bLength                                  */ 0x05,
/* bDescriptorType: CS_INTERFACE            */ 0x24,
/* bDescriptorSubtype: Union FD             */ 0x06,
/* bControlInterface                        */ 0x00,
/* bSubordinateInterface                    */ 0x01,
/*********************************************************************
 Call Management Functional Descriptor
 *********************************************************************/
/* bLength                                  */ 0x05,
/* bDescriptorType: CS_INTERFACE            */ 0x24,
/* bDescriptorSubtype: CM-FD                */ 0X01,
/* bmCapabilities                           */ 0x00,
/* bDataInterface                           */ 0x01,
/*********************************************************************
 Interrupt IN Endpoint - 1
 *********************************************************************/                                               
/* bLength                                  */ 0x07,
/* bDescriptorType: ENDPOINT                */ 0x05,
/* bEndpointAddress: IN Endpoint 1          */ 0x81, 
/* bmAttributes: INTERRUPT                  */ 0x03, 
/* max packet size (LSB)                    */ 0x10,
/* max packet size (MSB)                    */ 0x00,
/* polling interval                         */ 0x2, 
/*********************************************************************
 Interface 1 - Data Class Interface
 *********************************************************************/                                               
/* bLength                                  */ 0x09,
/* bDescriptorType: INTERFACE               */ 0x04,
/* interface index                          */ 0x01,
/* alt setting index                        */ 0x00,
/* bNumEndpoints: 2 endpoint(s)             */ 0x02,
/* bInterfaceClass: CDC-Data                */ 0x0A,
/* bInterfaceSubclass: unused               */ 0x00,
/* bInterfaceProtocol: None                 */ 0x00,
/* iInterface                               */ 0x00,
/*********************************************************************
 Endpoint 2 (Bulk IN)
 *********************************************************************/
/* bLength                                  */ 0x07,
/* bDescriptorType: ENDPOINT                */ 0x05,
/* bEndpointAddress: IN Endpoint 3          */ 0x02, 
/* bmAttributes: BULK                       */ 0x02,
/* max packet size (LSB)                    */ 0x00,
/* max packet size (MSB)                    */ 0x02,
/* bInterval: None for BULK                 */ 0x00,
/*********************************************************************
 Endpoint 2 (Bulk OUT)
 *********************************************************************/                                               
/* bLength                                  */ 0x07,
/* bDescriptorType: ENDPOINT                */ 0x05,
/* bEndpointAddress: OUT Endpoint 2         */ 0x82,
/* bmAttributes: BULK                       */ 0x02,
/* max packet size (LSB)                    */ 0x00,
/* max packet size (MSB)                    */ 0x02,
/* bInterval: None for BULK                 */ 0x00
};

#define	USBB_STR_DESC(l)	(((((l))<<1) & 0xFF) | 0x0300)
// String descriptors. All have a common format: [byte] string_length, [byte] data_type (3 = string), UTF-16 encoded string, hence all the 0's between each 8-byte character

// Language description
unsigned char USBB_string0[] =  { 4, 0x03, 0x09, 0x04}; // 0x0409 = English

// Vendor description
unsigned short USBB_string1[] = { USBB_STR_DESC(9),
                            'A','n','o','n','y','m','o','u','s'};   

// Product description
unsigned short USBB_string2[] = { USBB_STR_DESC(10),
                            'C','D','C',' ','D','e','v','i','c','e'};
 
// Serial number description
unsigned short USBB_string3[] = { USBB_STR_DESC(7),
                            'A','B','C','-','1','2','3'};

// Configuration description
unsigned short USBB_string4[] = { USBB_STR_DESC(5),
                            'U','L','T','R','A'};
// Interface description 
unsigned short USBB_string5[] = { USBB_STR_DESC(7),
                            'C','D','C',' ','A','C','M'};





// USBA_host_tasks() is the main "USB Stack" / State machine responsible for enumerating the keyboard and then requesting packets on endpoint 1
void USBA_host_tasks()
{
    int received_length;
   // int temp_index;
    
    if (USBA_DEVICE_CONNECTED)
    {
        switch (USBA_STAGE)
        {
            case 0:
            {
                // Is this an LS device or an FS device?
                if (USBOTGbits.LSDEV)
                {
                    // Low speed device attached
                    USBA_EP0_LENGTH = 8; // Set the EP0 length to 8 bytes for now
                }
                else
                {
                    // Full speed device attached
                    USBA_EP0_LENGTH = 64; // Set the EP0 length to 64 bytes for now
                }
                // Device just connected, tell it to reset
                // Tell device to reset its stack
                USBA_HOST_reset_device();
                USBA_STAGE++;
                USBA_EP0_IF = 0;
                break;
            }
            case 1:
            {
                if (USBA_EP0_IF == 0)
                {
                    // Send device descriptor request
                    USBA_EP0_send_setup(USBA_DEVICE_DESCRIPTOR_REQUEST, 8);                        
                    USBA_STAGE++;
                    USBA_EP0_IF = 0;
                }
                break;
            }
            case 2:
            {
                if (USBA_EP0_IF)
                {
                    // Send handshake
                    // Sending request packet handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USBA_STAGE++;
                    USBA_EP0_IF = 0;
                }
                break;
            }
            case 3:
            {
                if (USBA_EP0_IF)
                {
					// Receive X bytes
                    received_length = USBE0CSR2bits.RXCNT;
                    USBA_EP0_receive((unsigned char *)&USBA_device_descriptor, USBE0CSR2bits.RXCNT); //&USBA_device_descriptor, USBE0CSR2bits.RXCNT);
                    if (USBA_device_descriptor.bMaxPacketSize0 > 0)
                    {
                        //Max endpoint 0 length is X bytes
                        USBA_EP0_LENGTH = USBA_device_descriptor.bMaxPacketSize0;
                    }
                    // Got USB version
                    USBA_STAGE++;
                    USBA_EP0_IF = 0;
                }
                break;
            }
            case 4:
            {
                // Send handshake
                // Sending data received handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // STATUS + TXPKTRDY
                USBA_STAGE++;
                USBA_EP0_IF = 0;
                break;
            }
            case 5:
            {
                // Set device address
                if (USBA_EP0_IF)
                {
                    // Assigning device address of 2
                    USBA_DEVICE_ASSIGN_ADDRESS[2] = 2;
                    USBA_EP0_send_setup(USBA_DEVICE_ASSIGN_ADDRESS, 8);
                    USBA_STAGE++;
                    USBA_EP0_IF = 0;
                }
                break;
            }
            case 6:
            {
                if (USBA_EP0_IF)
                {
                    //Delaying 50ms to let the device get ready
                    DelayMS(50);
                    // Sending status transition handshake, no data stage
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60; // Set STATUS and REQPKT, no data stage
                    USBA_STAGE++;
                    USBA_EP0_IF = 0;
                }
                break;
            }
            case 7:
            {
                if (USBA_EP0_IF)
                {
                    *((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // Clear STATUS and RXPKTRDY
                    // Sending empty packet request to force set address
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 8:
            {
                if (USBA_EP0_IF)
                {
                    // Should receive 0 bytes
                    received_length = USBE0CSR2bits.RXCNT;
                    USBA_EP0_receive(USBA_device_configuration, USBE0CSR2bits.RXCNT);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 9:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;
                USBA_EP0_IF = 0;
                USBA_STAGE++;
                break;
            }            
            case 10:
            {
                if (USBA_EP0_IF)
                {
                    // Set address in hardware, request configuration this time
                    // Initialising endpoint 1
                    USBA_DEVICE_ADDRESS = 2;
                    USBA_init_endpoints(USBA_DEVICE_ADDRESS);
                    // Sending request for short device configuration
                    USBA_EP0_send_setup(USBA_DEVICE_CONFIGURATION_REQUEST, 8);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 11:
            {
                if (USBA_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 12:
            {
                if (USBA_EP0_IF)
                {
					// Receive X bytes
                    received_length = USBE0CSR2bits.RXCNT;
                    USBA_EP0_receive(USBA_device_configuration, received_length);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 13:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;
                USBA_STAGE++;
                USBA_EP0_IF = 0;
                break;
            }
            case 14:
            {
                if (USBA_EP0_IF)
                {
					// Sending request for full device configuration
                    USBA_config_length = USBA_device_configuration[2];
                    USBA_DEVICE_CONFIGURATION_REQUEST[6] = USBA_device_configuration[2];
                    USBA_EP0_send_setup(USBA_DEVICE_CONFIGURATION_REQUEST, 8);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 15:
            {
                if (USBA_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 16:
            {
                if (USBA_EP0_IF)
                {
					// Receive X bytes
                    received_length = USBA_EP0_receive_long(USBA_device_configuration, USBE0CSR2bits.RXCNT);
                    USBA_EP0_IF = 0;                   
                    USBA_STAGE++;
                }
                break;
            }            
            case 17:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // STATUS and TXPKTRDY
                USBA_EP0_IF = 0;
                USBA_STAGE++;
                break;               
            }
            case 18:
            {
                if (USBA_EP0_IF)
                {
                    // Sending request for short device name
                    USBA_EP0_send_setup(USBA_DEVICE_STRING_REQUEST, 8);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 19:
            {
                if (USBA_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 20:
            {
                if (USBA_EP0_IF)
                {
					// Receive X bytes
                    received_length = USBE0CSR2bits.RXCNT;
                    USBA_EP0_receive(USBA_device_name, received_length);
                    // Use the received length to modify the GET STRING request
                    USBA_DEVICE_STRING_REQUEST[6] = USBA_device_name[0];
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 21:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;
                
                // Sending request for full device name
                USBA_EP0_send_setup(USBA_DEVICE_STRING_REQUEST, 8);
                USBA_EP0_IF = 0;
                USBA_STAGE++;
                break;
            }
            case 22:
            {
                if (USBA_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 23:
            {
                if (USBA_EP0_IF)
                {
					// Receive device name as string
                    received_length = USBA_EP0_receive_long(USBA_device_name, USBE0CSR2bits.RXCNT);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 24:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;                
                USBA_EP0_IF = 0;
                USBA_STAGE++;
                break;
            }
            case 25:
            {
                if (USBA_EP0_IF)
                {
                    // Send full configuration request
                    // Sending SET CONFIGURATION request (configuration 0)
                    USBA_EP0_send_setup(USBA_DEVICE_SET_CONFIGURATION, 8);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;                
            }
            case 26:
            {
                if (USBA_EP0_IF)
                {
                    // Send handshake
                    // Sending status transition handshake, no data stage
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60;  // Set STATUS and REQPKT, no data stage
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 27:
            {
                if (USBA_EP0_IF)
                {
					// Send SET IDLE request
                    *((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // Clear STATUS and RXPKTRDY
                    USBA_EP0_send_setup(USBA_DEVICE_SET_IDLE, 8);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 28:
            {
                if (USBA_EP0_IF)
                {
                    // Sending status transition handshake, no data stage
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60;
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 29:
            {
                if (USBA_EP0_IF)
                {
					// Send SET protocol
                    *((unsigned char*)&USBE0CSR0 + 0x2) &= 0x41; // Clear STATUS and RXPKTRDY                    
                    USBA_EP0_send_setup(USBA_DEVICE_SET_PROTOCOL, 8);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;

                }
                break;
            }
            case 30:
            {
                if (USBA_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60;
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 31:
            {
                if (USBA_EP0_IF)
                {
                    // Request short 9-byte HID report. Not bothering with requesting the whole thing as we never need it.
                    *((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // Clear STATUS and RXPKTRDY   
                    USBA_EP0_send_setup(USBA_DEVICE_GET_HID_REPORT_REQUEST, 8);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;                    
                }
                break;
            }
            case 32:
            {
                if (USBA_EP0_IF)
                {
                    // Send handshake
                    *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20;
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 33:
            {
                if (USBA_EP0_IF)
                {
					// Receive X bytes
                    received_length = USBA_EP0_receive_long(USBA_HID_report, USBE0CSR2bits.RXCNT);
                    USBA_EP0_IF = 0;
                    USBA_STAGE++;
                }
                break;
            }
            case 34:
            {
                // Send handshake
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x42;
                USBA_STAGE++;
                USBA_EP0_IF = 0;
                break;
            }            
            case 35:
            {                
                if (USBA_EP0_IF)
                {
                    // Keyboard is enumerated!
                    USBA_device_timer = USBA_device_millis + USBA_HID_REQUEST_DELAY;
                    USBA_STAGE++;
                    USBA_EP0_IF = 0;
                }
                break;
            }
            case 36:
            {
                if (USBA_device_millis > USBA_device_timer)
                {
                    USBA_device_timer = USBA_device_millis + USBA_HID_REQUEST_DELAY;                    
                    USBE1CSR1bits.REQPKT = 1;
                }
                
                break;
            }
        }
    }  
}

// USBA_HID_tasks() exists purely for handling the setting of LEDs while not blocking the rest of my code
void USBA_HID_tasks()
{
    switch (USBA_HID_STAGE)
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
            USBA_EP0_send_setup(USBA_DEVICE_SET_REPORT, 8);
            
            // Wait for transmission to end
            USBA_EP0_IF = 0;
            USBA_HID_STAGE++;
            break;
        }
        case 2:
        {
            // Send data
            if (USBA_EP0_IF)
            {
                USBA_EP0_IF = 0;
                // Now send the actual 1-byte long report, but NOT as a setup packet. Yay
                USBA_EP0_buffer[0] = USBA_LED_data;
                USBA_EP0_send(USBA_EP0_buffer, 1);
                USBA_HID_STAGE++;
            }

            break;
        }
        case 3:
        {
            if (USBA_EP0_IF)
            {
                // Send handshake
                USBA_EP0_IF = 0;
                USBA_HID_STAGE++;
                *((unsigned char*)&USBE0CSR0 + 0x2) = 0x60;  // Set STATUS and REQPKT, no data stage
            }

            break;
        }
        case 4:
        {
            if (USBA_EP0_IF)
            {
                // Send handshake
                USBA_EP0_IF = 0;
                USBA_HID_STAGE = 0;
                USBA_HID_BUSY = 0;
                *((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // Clear STATUS and RXPKTRDY
            }

            break;            
        }
    }
}

void USBA_init_endpoints(int USBA_address)
{
    USBE1CSR0bits.MODE = 1;     // EP1 is OUT (OUT from host = in to device = RX)
    
    // Clear all interrupt flags
	USBCSR0bits.EP0IF = 0;
    USBCSR0bits.EP1TXIF = 0;
    USBCSR1bits.EP1RXIF = 0;
	
    // Set the maximum transfer length for each endpoint
	// Configure endpoint 0 first.
    USBE0CSR0bits.TXMAXP = USBA_EP0_LENGTH; // Set endpoint 0 buffer size to 16 bytes (can be a maximum of 64 for EP0)

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
    USBE1RXAbits.RXFADDR = USBA_address;
}

void USBA_setup()
{
	usb_class = 1; // host
	
    volatile uint8_t * usbBaseAddress;
    int cnt;
    
    for (cnt = 0; cnt < USBA_KEYBOARD_NUM_KEYS; cnt++)
    {
        USBA_device_keys[cnt].pressed = 0;
        USBA_device_keys[cnt].released = 0;
        USBA_device_keys[cnt].repeat_time = 0;
    }

    USBCRCONbits.USBIE = 1;

    // Reset the USB hardware according to the demonic ritual Microchip demands
    // Shouldn't be attempted on a blood moon is Venus is in retrograde
    usbBaseAddress = (uint8_t *)&USBCSR0;
    *(usbBaseAddress + 0x7F) = 0x3;
    DelayMS(10);
    *(usbBaseAddress + 0x7F) = 0;
    
    USBA_DEVICE_ADDRESS = 0; 

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
    
    //USBA_init_endpoints();

    USBCSR0bits.HSEN = 0; // 1; // CHANGING THIS TO ZERO!!!
    USBCRCONbits.USBIDOVEN = 1;
    USBCRCONbits.PHYIDEN = 1;

    USBCRCONbits.USBIDVAL = 0; 
    USBCRCONbits.USBIDVAL = 0; 

    IFS4bits.USBIF = 0;         // Clear the USB interrupt flag.
    IEC4bits.USBIE = 1;         // Enable the USB interrupt

	CNPDFbits.CNPDF3 = 1; // Pull down to ensure host mode
    LATFbits.LATF3 = 0; // USB host mode
	
    USBA_STAGE = 0;
 
    USBOTGbits.SESSION = 1;
}

int USBA_EP0_wait_TXRDY()
{
    int timeout;
    
    timeout = 0;
    
    while (USBE0CSR0bits.TXRDY)
    {
        timeout++;
        if (timeout > USBA_EP0_WAIT_TIMEOUT) return 1;
    };
    
    return 0;
}

void USBA_EP0_send_setup(unsigned char *buffer, uint32_t length)
{	
	int cnt;
    unsigned char *FIFO_buffer;

    FIFO_buffer = (unsigned char *)&USBFIFO0;
	
	USBE0CSR0bits.TXMAXP = USBA_EP0_LENGTH;
    
    USBE0CSR2bits.SPEED = 2;
    USBE0TXAbits.TXHUBADD = 0;
    USBE0TXAbits.TXHUBPRT = 0;
    USBE0TXAbits.TXFADDR = USBA_DEVICE_ADDRESS;    

    for (cnt = 0; cnt < length; cnt++)
    {
        *FIFO_buffer = *buffer++; // Send the bytes
    }

    *((unsigned char*)&USBE0CSR0 + 0x2) = 0xA;
}

void USBA_EP0_send(unsigned char *buffer, uint32_t length)
{
	int cnt;
    unsigned char *FIFO_buffer;

    FIFO_buffer = (unsigned char *)&USBFIFO0;
	
	USBE0CSR0bits.TXMAXP = USBA_EP0_LENGTH;
    
    USBE0CSR2bits.SPEED = 2;
    USBE0TXAbits.TXHUBADD = 0;
    USBE0TXAbits.TXHUBPRT = 0;
    USBE0TXAbits.TXFADDR = USBA_DEVICE_ADDRESS; 

    for (cnt = 0; cnt < length; cnt++)
    {
        *FIFO_buffer = *buffer++; // Send the bytes
    }

    USBE0CSR0bits.TXRDY = 1;
}

void USBA_EP0_receive(unsigned char *buffer, uint32_t length)
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

int USBA_EP0_receive_long(unsigned char *buffer, uint32_t length)
{
    int total_received;
    int received_length;
    int temp_index;
    
    total_received = length;    
    received_length = length;
    
    USBA_EP0_receive(buffer, received_length);

    temp_index = received_length;

    while (received_length >= USBA_EP0_LENGTH)
    {
        USBA_EP0_IF = 0;
        *((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // REQPKT

        while (USBA_EP0_IF == 0);

        received_length = USBE0CSR2bits.RXCNT;
        USBA_EP0_receive(&buffer[temp_index], received_length);
        temp_index += received_length;                        
        total_received += received_length;
    }

    USBA_EP0_IF = 0;
    
    return total_received;
}

void USBA_EP1_receive(unsigned char *buffer)
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

void USBA_HOST_reset_device()
{
    // When resetting the attached USB device, we set the RESET signal high, wait 100ms (holding it in reset) and then set it low again
    USBCSR0bits.RESET = 1;
    DelayMS(100);
    USBCSR0bits.RESET = 0;
    DelayMS(100);
}

void USBA_keyboard_status_toggle(char keycode)
{
    if (USBA_HID_BUSY) return; // We are already processing a status toggle, skip this one
    
    switch (keycode)
    {
        case USBA_KEY_NUMLOCK:
        {
            // Flip the bit
            USBA_NUM_LOCK_STATUS ^= 1;
            
            // Fill in the LED bit fields            
            if (USBA_NUM_LOCK_STATUS)
                USBA_LED_data |= 1;
            else
                USBA_LED_data &= 0xFE;
                                    
            break;
        }
        case USBA_KEY_CAPSLOCK:
        {
            // Flip the bit
            USBA_CAPS_LOCK_STATUS ^= 1;
            
            // Fill in the LED bit fields            
            if (USBA_CAPS_LOCK_STATUS)
                USBA_LED_data |= 2;
            else
                USBA_LED_data &= 0xFD;
                                    
            break;
        }
        case USBA_KEY_SCROLLLOCK:
        {
            // Flip the bit
            USBA_SCROLL_LOCK_STATUS ^= 1;
            
            // Fill in the LED bit fields            
            if (USBA_SCROLL_LOCK_STATUS)
                USBA_LED_data |= 4;
            else
                USBA_LED_data &= 0xFB;
                                    
            break;
        }
        default:
        {
            break;
        }
    }
        
    USBA_HID_STAGE = 1;
    USBA_HID_BUSY = 1;
}

void USBA_HID_process_report()
{
	if (USBA_HID_report[1] == 0x01 && USBA_HID_report[3] == 0x06) // keyboard
	{
		usb_mode = 0x01; // keyboard
		
		int cnt;
		char current_keys_pressed[USBA_KEYBOARD_NUM_KEYS];

		// Clear the current_keys_pressed array first
		//memset(current_keys_pressed, 0, sizeof(current_keys_pressed));
		for (cnt = 0; cnt < USBA_KEYBOARD_NUM_KEYS; cnt++)
		{
			current_keys_pressed[cnt] = 0;
		}

		// Find out which keys are currently being pressed by looking at the report
		for (cnt = 0; cnt < 6; cnt++)
		{        
			current_keys_pressed[USBA_EP1_buffer[cnt + 2]] = 1;

			USBA_keyboard_status_toggle(USBA_EP1_buffer[cnt + 2]);
		}

		// Compare this to the current key status
		for (cnt = 0; cnt < USBA_KEYBOARD_NUM_KEYS; cnt++)
		{
			// If a key's pressed value is now 0, but it was 1 before, that means it has been released        
			if ((current_keys_pressed[cnt] == 0) && (USBA_device_keys[cnt].pressed == 1))
			{
				USBA_device_keys[cnt].released = 1; // Key has just been released
				USBA_device_keys[cnt].pressed = 0;  // Key is no longer pressed
			}

			// If a key's pressed value is 1 now, but it was 0 before, that means it has just been pressed
			if ((current_keys_pressed[cnt] == 1) && (USBA_device_keys[cnt].pressed == 0))
			{
				USBA_device_keys[cnt].pressed = 1;
				USBA_device_keys[cnt].repeat_time = USBA_device_millis; // Ensures that it will be processed immediately
			}
		}

		// Now let's process the modifier keys, which are all stored in different bit fields in byte 0 of the HID report
		USBA_CTRL_PRESSED = ((USBA_EP1_buffer[0] & USBA_CTRL_MASK) == USBA_CTRL_MASK);
		USBA_ALT_PRESSED = ((USBA_EP1_buffer[0] & USBA_ALT_MASK) == USBA_ALT_MASK);
		USBA_SHIFT_PRESSED = ((USBA_EP1_buffer[0] & USBA_SHIFT_MASK) ==  USBA_SHIFT_MASK);

		if ((USBA_EP1_buffer[0] > 0) && (USBA_EP1_buffer[2] > 0))
		{
			USBA_SHIFT_PRESSED = ((USBA_EP1_buffer[0] & USBA_SHIFT_MASK) ==  USBA_SHIFT_MASK);
		}
		
		for (unsigned int i=0; i<USBA_KEYBOARD_NUM_KEYS; i++)
		{
			if (USBA_device_keys[i].pressed && (USBA_device_millis >= USBA_device_keys[i].repeat_time))
			{
				USBA_device_keys[i].repeat_time = USBA_device_millis + USBA_KEYBOARD_REPEAT_DELAY;

				if (USBA_SHIFT_PRESSED)
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
	else if (USBA_HID_report[1] == 0x01 && USBA_HID_report[3] == 0x02) // mouse
	{	
		usb_mode = 0x02; // mouse
		
		usb_state_array[usb_writepos] = USBA_EP1_buffer[0];
		
		if ((signed char)(USBA_EP1_buffer[1]) < 0)
		{
			if (usb_cursor_x[(unsigned char)(usb_writepos-1)] < 0 - (signed char)(USBA_EP1_buffer[1]))
			{
				usb_cursor_x[usb_writepos] = 0;
			}
			else
			{
				usb_cursor_x[usb_writepos] = 
					usb_cursor_x[(unsigned char)(usb_writepos-1)] + (signed char)(USBA_EP1_buffer[1]);
			}
		}
		else if ((signed char)(USBA_EP1_buffer[1]) >= 0)
		{
			if (usb_cursor_x[(unsigned char)(usb_writepos-1)] >= SCREEN_X - (signed char)(USBA_EP1_buffer[1]))
			{
				usb_cursor_x[usb_writepos] = SCREEN_X;
			}
			else
			{
				usb_cursor_x[usb_writepos] = 
					usb_cursor_x[(unsigned char)(usb_writepos-1)] + (signed char)(USBA_EP1_buffer[1]);
			}
		}
		
		if ((signed char)(USBA_EP1_buffer[2]) < 0)
		{
			if (usb_cursor_y[(unsigned char)(usb_writepos-1)] < 0 - (signed char)(USBA_EP1_buffer[2]))
			{
				usb_cursor_y[usb_writepos] = 0;
			}
			else
			{
				usb_cursor_y[usb_writepos] = 
					usb_cursor_y[(unsigned char)(usb_writepos-1)] + (signed char)(USBA_EP1_buffer[2]);
			}
		}
		else if ((signed char)(USBA_EP1_buffer[2]) >= 0)
		{
			if (usb_cursor_y[(unsigned char)(usb_writepos-1)] >= SCREEN_Y - (signed char)(USBA_EP1_buffer[2]))
			{
				usb_cursor_y[usb_writepos] = SCREEN_Y;
			}
			else
			{
				usb_cursor_y[usb_writepos] = 
					usb_cursor_y[(unsigned char)(usb_writepos-1)] + (signed char)(USBA_EP1_buffer[2]);
			}
		}
		
		usb_writepos++;
	}
	else
	{
		if (USBA_EP1_buffer[0] == 0x00 && USBA_EP1_buffer[1] == 0x14) // xbox-type controller
		{
			usb_mode = 0x03; // xbox-type controller
			
			// Format: 0xWXYZ
			// W = 1 start, 2 select, 3 both
			// X = 1 up, 2 down, 4 left, 8 right (combinations for multiple)
			// Y = 1 a, 2 b, 3 x, 4 y (combinations for multiple)
			// Z = 1 left bumper, 2 right bumper, 3 both
			usb_buttons[usb_writepos] = (unsigned int)((unsigned int)(USBA_EP1_buffer[2] << 8) + USBA_EP1_buffer[3]);
			
			usb_writepos++;
		}
		else
		{
			usb_mode = 0x00; // unknown

			/*
			SendHex(USBA_EP1_RECEIVED);
			SendChar(' ');

			SendHex(USBA_HID_report[1]);
			SendChar(':');

			SendHex(USBA_HID_report[3]);
			SendChar(' ');

			for (unsigned char i = 0; i < 16; i++)
			{        
				SendHex(USBA_EP1_buffer[i]);
				SendChar('.');
			}

			SendChar('\n');
			SendChar('\r');
			*/
		}
	}
}



void USBB_init_endpoints()
{
    USBE1CSR0bits.MODE = 0;     // EP1 is OUT (OUT from host = in to device = RX)
    USBE2CSR0bits.MODE = 0;     // EP2 is OUT (for now, will be IN at times)
    
    // Clear all interrupt flags
	USBCSR0bits.EP0IF = 0;
    USBCSR0bits.EP1TXIF = 0;
    USBCSR0bits.EP2TXIF = 0;
    USBCSR1bits.EP1RXIF = 0;
    USBCSR1bits.EP2RXIF = 0;
	
    // Set the maximum transfer length for each endpoint
	// Configure endpoint 0 first.
    USBE0CSR0bits.TXMAXP = 16; // Set endpoint 0 buffer size to 16 bytes (can be a maximum of 64 for EP0)

    // And next my custom endpoints
    USBE1CSR0bits.TXMAXP = 16;   // Endpoint 1 - Maximum TX payload / transaction (can be a maximum of 64 for CDC "Interrupt IN endpoint"
    USBE2CSR0bits.TXMAXP = 512;   // Endpoint 2 - Maximum TX payload / transaction (512 is the maximum, set to 512)
    USBE2CSR1bits.RXMAXP = 512;   // Endpoint 2 - Maximum RX payload / transaction (512 is the maximum, set to 512)

	// Specify which kinds of endpoint we will be using
    USBE1CSR2bits.PROTOCOL = 3; // Endpoint 1 - Interrupt mode
    USBE2CSR2bits.PROTOCOL = 2; // Endpoint 2 TX - Bulk mode
    USBE2CSR3bits.PROTOCOL = 2; // Endpoint 2 RX - Bulk mode

	// Enable DISNYET
    USBE1CSR1bits.PIDERR = 0; // Clear DISNYET to enable NYET replies
    USBE2CSR1bits.PIDERR = 0; // Clear DISNYET to enable NYET replies

	// Set up buffer location for endpoint 1
    USBCSR3bits.ENDPOINT = 1;
    USBFIFOA = 0x08;
    USBIENCSR0bits.CLRDT = 1;

	// Set up buffer locations for endpoint 2
    USBCSR3bits.ENDPOINT = 2;
    USBFIFOA = 0x000A004A;
    USBIENCSR0bits.CLRDT = 1;
    USBE2CSR3bits.TXFIFOSZ = 0x9;   // Transmit FIFO Size bits - 512 bytes
    USBE2CSR3bits.RXFIFOSZ = 0x9;   // Receive FIFO Size bits - 512 bytes
	
	// Set maximum size for each packet before splitting occurs
    USBOTGbits.RXFIFOSZ = 0b0110; // 0b0110 = 512 bytes
    USBOTGbits.TXFIFOSZ = 0b0110; // 0b0110 = 512 bytes

	// Set receive endpoint 2 to High Speed
    USBE2CSR3bits.SPEED = 1;
    
	// Disable Isochronous mode for endpoints 1 and 2
    USBE1CSR0bits.ISO = 0;      // Isochronous TX Endpoint Disable bit (Device mode).
    USBE2CSR0bits.ISO = 0;      // Isochronous TX Endpoint Disable bit (Device mode).

    // Set up endpoint interrupts
	// Initially clear all the interrupt enables (IE)
    USBCSR1 = 0;
    USBCSR2 = 0;
    
    USBCSR1bits.EP0IE = 1;      // Endpoint 0 interrupt enable
    USBCSR1bits.EP1TXIE = 1;    // Endpoint 1 TX interrupt enable
    USBCSR2bits.EP1RXIE = 1;    // Endpoint 1 RX interrupt enable
    USBCSR1bits.EP2TXIE = 1;    // Endpoint 2 TX interrupt enable
    USBCSR2bits.EP2RXIE = 1;    // Endpoint 2 RX interrupt enable
    USBCSR2bits.RESETIE = 1;
    USBCSR2bits.RESUMEIE = 1;
    USBCSR2bits.SOFIE = 1;

	// Set current endpoint to EP2
    USBCSR3bits.ENDPOINT = 2;    
}

void USBB_setup(void *receive_callback)
{
	usb_class = 2; // device
	
    USBB_RECEIVE_CALLBACK = receive_callback;

    USBCSR0bits.SOFTCONN = 0;   // Initially disable it while we go into setup

    USBB_address = 0;			// We haven't been given an address yet    
    USBCSR0bits.FUNC = 0;       // Initially set the USB address to 0 until later when the host assigns us an address

    USBCSR2bits.RESETIE = 1;    // Enable the reset interrupt
    IEC4bits.USBIE = 1;         // Enable the USB interrupt
    USBCRCONbits.USBIE = 1;     // Enable USB module interrupt
    IFS4bits.USBIF = 0;         // Clear the USB interrupt flag.
    IPC33bits.USBIP = 6;        // USB Interrupt Priority 6
    IPC33bits.USBIS = 0;        // USB Interrupt Sub-Priority 0

    USBCSR0bits.HSEN = 0;// 1; // CHANGING THIS TO ZERO!!!
    
    USBB_line_coding.baud = 9600;
    USBB_line_coding.data_bits = 8;
    USBB_line_coding.parity_bits = 0;
    USBB_line_coding.stop_bits = 0;
	
	LATFbits.LATF3 = 1; // USB device mode
    CNPUFbits.CNPUF3 = 1; // Pull up to ensure device mode
	
	USBCSR0bits.SOFTCONN = 1; // connect!
}

int USBB_EP0_Wait_TXRDY()
{	
    int timeout;
    
    timeout = 0;
    
    while (USBE0CSR0bits.TXRDY)
    {
        timeout++;
        if (timeout > USBB_EP0_WAIT_TIMEOUT) return 1;
    };
    
    return 0;
}

// Send EP[0].tx_num_bytes from EP[0].tx_buffer on endpoint 0
void USBB_EP0_send()
{
    int cnt;
    unsigned char *FIFO_buffer;
        
    FIFO_buffer = (unsigned char *)&USBFIFO0;
    
    if (USBB_EP0_Wait_TXRDY()) return;
    
    cnt = 0;
    
    while (cnt < USBB_EP[0].tx_num_bytes)
    {
        *FIFO_buffer = USBB_EP[0].tx_buffer[cnt]; // Send the bytes
        cnt++;
		
        // Have we sent 64 bytes?
        if ((cnt > 0) && (cnt % 64 == 0))
        {
            // Set TXRDY and wait for it to be cleared before sending any more bytes
            USBE0CSR0bits.TXRDY = 1;            
            if (USBB_EP0_Wait_TXRDY()) return;
        }
    }
    
    USBE0CSR0bits.TXRDY = 1;            
}

void USBB_EP0_receive(int length)
{
    int cnt;
    unsigned char *FIFO_buffer;
    
    // Store number of bytes received
    USBB_EP[0].rx_num_bytes = USBE0CSR2bits.RXCNT;
                
    // Get 8-bit pointer to USB FIFO for endpoint 0
    FIFO_buffer = (unsigned char *)&USBFIFO0;
    
    for(cnt = 0; cnt < length; cnt++)
    {
        // Read in one byte at a time
        USBB_EP[0].rx_buffer[cnt] = *(FIFO_buffer + (cnt & 3));
    }
        
    USBE0CSR0bits.RXRDYC = 1;
}

void USBB_EP0_queue(unsigned char *buffer, int size, int max_size)
{
    int cnt;
    
    if (max_size < size)
        size = max_size;
    
    USBB_EP[0].tx_num_bytes = size;
    
    for (cnt = 0; cnt < size; cnt++)
        USBB_EP[0].tx_buffer[cnt] = buffer[cnt];

    USBB_EP0_send();
}

void USBB_EP2_send(volatile unsigned char *buffer, int size)
{
    int cnt;
    unsigned char *FIFO_buffer;
    
    USBE2CSR0bits.MODE = 1;     // EP2 is TX

    FIFO_buffer = (unsigned char *)&USBFIFO2;
    
    for (cnt = 0; cnt < size; cnt++)
    {
        *FIFO_buffer = buffer[cnt]; // Send the bytes
    }

    USBE2CSR0bits.TXPKTRDY = 1;
}

int USBB_EP2_receive()
{
    unsigned char *FIFO_buffer;
    int cnt;
    int bytes_received;
    
    USBE2CSR0bits.MODE = 0;     // EP2 is RX

    bytes_received = USBE2CSR2bits.RXCNT;
    
    FIFO_buffer = (unsigned char *)&USBFIFO2;
    
    for(cnt = 0; cnt < bytes_received; cnt++)
    {
        USBB_EP[2].rx_buffer[cnt] = *(FIFO_buffer + (cnt & 3)); // Store the received bytes in array ep0data[].
    }
    
    USBE2CSR1bits.RXPKTRDY = 0;
    
    return bytes_received;
}

void USBB_EP0_handle_transaction()
{
    //int cnt;
    unsigned char temp_data[7];
    //USBB_LINE_CODING new_line_coding;
	
    // We're not going to bother with whether bmRequestType is IN or OUT for the most part
    switch (USBB_transaction.bmRequest)
    {
        case 0xC:
        {
            USBE0CSR0bits.STALL = 1;
            break;
            
        }
        case 0x0: 
        {
            if (USBB_transaction.bmRequestType == 0x80) // Get status
                USBB_EP0_queue((unsigned char *)USBB_device_description, 0, 0);
            if (USBB_transaction.bmRequestType == 0x00) // Select function
                USBB_EP0_queue((unsigned char *)USBB_device_description, 0, 0);
            break;            
        }
        
        case 0x5: // Set USB address
        {
            USBE0CSR0bits.RXRDYC = 1;
            USBB_address = USBB_EP[0].rx_buffer[2];
            USBB_set_address = 1;
            break;
        }

        case 0x6: // Get descriptor
        {
            switch (USBB_transaction.wValue >> 8)
            {
                case 0x1: // Get device descriptor
                {
                    USBB_EP0_queue((unsigned char *)USBB_device_description, sizeof(USBB_device_description), USBB_transaction.wLength);
                    break;
                }
                
                case 0x2: // Get configuration descriptor
                {
                    USBB_EP0_queue((unsigned char *)USBB_config_descriptor, sizeof(USBB_config_descriptor), USBB_transaction.wLength);
                    break;
                }
                
                case 0x3: // Get string descriptors
                {
                    switch (USBB_transaction.wValue & 0xFF)
                    {
                        case 0x0: // String 0 - Language ID
                        {
                            USBB_EP0_queue((unsigned char *)USBB_string0, sizeof(USBB_string0), USBB_transaction.wLength);
                            break;
                        }
                        case 0x1: // String 1 - Vendor
                        {
                            USBB_EP0_queue((unsigned char *)USBB_string1, sizeof(USBB_string1), USBB_transaction.wLength);
                            break;
                        }
                        case 0x2: // String 2 - Product name
                        {
                            USBB_EP0_queue((unsigned char *)USBB_string2, sizeof(USBB_string2), USBB_transaction.wLength);
                            break;
                        }
                        case 0x3: // String 3 - Serial number
                        {
                            USBB_EP0_queue((unsigned char *)USBB_string3, sizeof(USBB_string3), USBB_transaction.wLength);
                            break;
                        }
                        case 0x4: // String 4 - Configuration
                        {
                            USBB_EP0_queue((unsigned char *)USBB_string4, sizeof(USBB_string4), USBB_transaction.wLength);
                            break;
                        }
                        case 0x5: // String 5 - Interface
                        {
                            USBB_EP0_queue((unsigned char *)USBB_string5, sizeof(USBB_string5), USBB_transaction.wLength);
                            break;
                        }
                        default: break;
                    }
                }
                default: 
                {
                    break;
                }
            }
            break;
        }
        
        case 0x9: // Set interface
        {
            // Zegads we have enumeration!
            break;
        }
        
        case 0x20:
        {
            if (USBB_transaction.bmRequestType == 0x21)
            {
                // Set line coding
                // Read (wLength) bytes from USB Endpoint 0
                USBB_EP0_receive(USBB_transaction.wLength);
                
                USBB_line_coding.baud = ((int)USBB_EP[0].rx_buffer[3] << 24) | 
					((int)USBB_EP[0].rx_buffer[2] << 16) | 
					((int)USBB_EP[0].rx_buffer[1] << 8) | 
					USBB_EP[0].rx_buffer[0];
                USBB_line_coding.parity_bits = USBB_EP[0].rx_buffer[4];
                USBB_line_coding.stop_bits = USBB_EP[0].rx_buffer[5];
                USBB_line_coding.data_bits = USBB_EP[0].rx_buffer[6];
            }
            break;
           
        }
        
        case 0x21:
        {
            // Get line coding
            temp_data[0] = USBB_line_coding.baud & 0xFF;
            temp_data[1] = (USBB_line_coding.baud >> 8) & 0xFF;
            temp_data[2] = (USBB_line_coding.baud >> 16) & 0xFF;
            temp_data[3] = USBB_line_coding.baud >> 24;
            temp_data[4] = USBB_line_coding.stop_bits;
            temp_data[5] = USBB_line_coding.parity_bits;
            temp_data[6] = USBB_line_coding.data_bits;
            USBB_EP0_queue(temp_data, sizeof(temp_data), USBB_transaction.wLength);
            break;
        }
        
        case 0x22:
        {
            if (USBB_transaction.bmRequestType == 0x21)
            {
                 // Set line control state
                USBB_line_control_state = USBB_transaction.wValue >> 8;
                
                if (USBB_line_control_state != 0)
                {
                    // We're connected now
                }
            }
            break;
        }
        
        default: 
        {
            USBE0CSR0bits.STALL = 1;
            break;
        }
    }   
}

// Use the command:
// sudo picocom /dev/ttyACM0
// This is how you receive data
void USBB_data_received(volatile unsigned char *data, int length)
{
	// This is how you send data back
    USBB_EP2_send(data, length); // echo
}


// USB ISR
void __attribute__((vector(_USB_VECTOR), interrupt(ipl6srs), nomips16)) usb_handler()
{		
	if (usb_class == 1) // host
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
			USBA_DEVICE_CONNECTED = 1;
		}

		// Endpoint 0 Interrupt
		if(EP0IF)
		{
			USBCSR0bits.EP0IF = 0;      // Clear the USB EndPoint 0 Interrupt Flag.
			USBA_EP0_IF = 1;             // My own flag for Endpoint 0 Interrupts
		}

		// Endpoint 1 Receive Interrupt
		if (EP1RXIF)
		{
			USBCSR1bits.EP1RXIF = 0;    // Clear the USB EndPoint 1 RX Interrupt Flag.
			USBA_EP1_RECEIVED++;       // To let the app know Endpoint 1 received something
		}
	}
	else if (usb_class == 2) // device
	{
		//int cnt;
		//unsigned char *FIFO_data;

		if(USBCSR2bits.RESETIF)
		{
			// So when this is set, the USB bus has been reset and we actually need to set up all the USB endpoints now

			USBB_init_endpoints();

			USBCSR2bits.RESETIF = 0;      

		}

		/* Endpoint 0 Interrupt Hanler */
		if(USBCSR0bits.EP0IF == 1)
		{
			if (USBB_set_address) // Do we need the set the USB address?
			{
				USBCSR0bits.FUNC = USBB_address & 0x7F;
				USBB_set_address = 0;
			}

			if(USBE0CSR0bits.RXRDY)
			{
				USBB_EP0_receive(USBE0CSR2bits.RXCNT);

				USBB_transaction.bmRequestType = USBB_EP[0].rx_buffer[0];
				USBB_transaction.bmRequest = USBB_EP[0].rx_buffer[1];
				USBB_transaction.wValue = (int)(USBB_EP[0].rx_buffer[3] << 8) | USBB_EP[0].rx_buffer[2];
				USBB_transaction.wIndex = (int)(USBB_EP[0].rx_buffer[5] << 8) | USBB_EP[0].rx_buffer[4];
				USBB_transaction.wLength = (int)(USBB_EP[0].rx_buffer[7] << 8) | USBB_EP[0].rx_buffer[6];

				USBB_EP0_handle_transaction();

				if (USBB_transaction.wLength == 0) USBE0CSR0bits.DATAEND = 1; // End of Data Control bit (Device mode) 
			}

			if (USBE0CSR0bits.SETEND) 
			{
				USBE0CSR0bits.SETENDC = 1;
			}

			USBCSR0bits.EP0IF = 0;  // Clear the USB EndPoint 0 Interrupt Flag.
		}

		if(USBCSR0bits.EP1TXIF == 1)
		{ 
			USBCSR0bits.EP1TXIF = 0;        // Supposedly Cleared By Hardware (Clear Just In Case).
		} 

		if(USBE2CSR1bits.RXPKTRDY)
		{
			USBB_EP[2].rx_num_bytes = USBB_EP2_receive();
			if (USBB_RECEIVE_CALLBACK) USBB_RECEIVE_CALLBACK(USBB_EP[2].rx_buffer, USBB_EP[2].rx_num_bytes);       
	//        USBE2CSR1bits.RXPKTRDY = 0;
			USBCSR1bits.EP2RXIF = 0;
		}  

		if (USBCSR2bits.SOFIF)
		{
			USBCSR2bits.SOFIF = 0;
		}
	}
    
    IFS4bits.USBIF = 0;             // Clear the main interrupt flag!
}







