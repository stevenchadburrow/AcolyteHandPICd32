
// my own USB code, heavily influenced by Aidan Mocke's code

unsigned char usbhost_connected = 0;
unsigned char usbhost_address = 0;
unsigned char usbhost_stage = 0;
unsigned char usbhost_hid_stage = 0;
unsigned char usbhost_ep0_interrupt = 0;
unsigned char usbhost_ep1_interrupt = 0;
unsigned int usbhost_ep0_length = 8;
unsigned long usbhost_device_delay = 0;
unsigned long usbhost_device_millis = 0;
unsigned long usbhost_device_millis_previous = 0;

unsigned char __attribute__((coherent, aligned(8))) usbhost_device_descriptor[32];
unsigned char usbhost_device_descriptor_request[8] = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00};
unsigned char usbhost_device_assign_address[8] = {0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char __attribute__((coherent, aligned(8))) usbhost_device_configuration[256];
unsigned char usbhost_device_configuration_request[8] = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x09, 0x00};
unsigned char __attribute__((coherent, aligned(8))) usbhost_device_name[256];
unsigned char usbhost_device_name_request[8] = {0x80, 0x06, 0x02, 0x03, 0x09, 0x04, 0x04, 0x00};
unsigned char usbhost_device_set_configuration[8] = {0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char usbhost_device_set_idle[8] = {0x21, 0x0A, 0x00, 0x7D, 0x00, 0x00, 0x00, 0x00};
unsigned char usbhost_device_set_protocol[8] = {0x21, 0x0B, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char __attribute__((coherent, aligned(8))) usbhost_device_hid_report[512];
unsigned char usbhost_device_hid_report_request[8] = {0x81, 0x06, 0x00, 0x22, 0x00, 0x00, 0x09, 0x00};

unsigned char __attribute__((coherent, aligned(8))) usbhost_ep1_buffer[64];

unsigned char usbhost_keyboard_previous[8];
unsigned int usbhost_keyboard_repeat;


void __attribute__((optimize("O1"))) usbhost_initialize_endpoints(int address)
{
	USBCSR3bits.ENDPOINT = 0; // EP0 register select
	USBFIFOA = 0x00000000; // set EP0 buffer location
	USBCSR0bits.EP0IF = 0; // clear flags
	USBCSR0bits.EP1TXIF = 0;
	USBCSR1bits.EP1RXIF = 0;
	USBE0CSR0bits.TXMAXP = usbhost_ep0_length; // set buffer size to packet size
	USBIENCSR0bits.CLRDT = 1; // clear data toggle

	USBCSR3bits.ENDPOINT = 1; // EP1 register select
	USBFIFOA = 0x000A000A;
	USBIENCSR0bits.MODE = 1; // set EP1 to OUT
	USBIENCSR0bits.TXMAXP = 64; // max EP1 buffer size
	USBIENCSR1bits.RXMAXP = 64;
	USBIENCSR1bits.PIDERR = 0; // clear PID error status bit
	USBIENCSR2bits.PROTOCOL = 3; // interrupt protocol for TX
	USBIENCSR3bits.SPEED = 2; // full-speed or low-speed for RX
	USBIENCSR3bits.TEP = 1; // transmit endpoint for EP1
	USBIENCSR3bits.RXINTERV = 1; // polling interval of 1 ms
	USBIENCSR3bits.TXFIFOSZ = 6; // max FIFO size of 64 bytes
	USBIENCSR3bits.RXFIFOSZ = 6;
	USBIENCSR0bits.ISO = 0; // disable iscochronous transfers
	USBE1RXAbits.RXFADDR = address; // set receiving address for EP1
	USBIENCSR0bits.CLRDT = 1; // clear data toggle

	USBOTGbits.TXFIFOSZ = 3; // max FIFO size of 64 bytes
	USBOTGbits.RXFIFOSZ = 3;
	
	USBCSR1 = 0; // clear all interrupt enables

	USBCSR1bits.EP0IE = 1; // EP0 interrupt enable
	USBCSR2bits.EP1RXIE = 1; // EP1 RX interrupt enable
	USBCSR2bits.SOFIE = 0; // start of frame interrupt disabled

	USBCSR3bits.ENDPOINT = 1; // set current endpoint to EP1
}

void __attribute__((optimize("O1"))) usbhost_ep0_send_setup(unsigned char *buffer, unsigned long length)
{
	unsigned char *fifo = (unsigned char *)&USBFIFO0;
	
	USBE0CSR0bits.TXMAXP = usbhost_ep0_length;

	USBE0CSR2bits.SPEED = 2; // full-speed / low-speed
	USBE0TXAbits.TXHUBADD = 0; // set hub address
	USBE0TXAbits.TXHUBPRT = 0; // set port number
	USBE0TXAbits.TXFADDR = usbhost_address; // set functional address
	
	for (unsigned int i=0; i<length; i++)
	{
		*fifo = *buffer++; // send bytes
	}

	*((unsigned char*)&USBE0CSR0 + 0x2) = 0xA; // set both SETUPPKT and TXPKTRDY at the same time	
}

void __attribute__((optimize("O1"))) usbhost_ep0_send(unsigned char *buffer, unsigned long length)
{
	unsigned char *fifo = (unsigned char *)&USBFIFO0;
	
	USBE0CSR0bits.TXMAXP = usbhost_ep0_length;

	USBE0CSR2bits.SPEED = 2; // full-speed / low-speed
	USBE0TXAbits.TXHUBADD = 0; // set hub address
	USBE0TXAbits.TXHUBPRT = 0; // set port number
	USBE0TXAbits.TXFADDR = usbhost_address; // set functional address
	
	for (unsigned int i=0; i<length; i++)
	{
		*fifo = *buffer++; // send bytes
	}

	USBE0CSR0bits.TXRDY = 1; // data packet is now loaded
}

void __attribute__((optimize("O1"))) usbhost_ep0_receive(unsigned char *buffer, unsigned long length)
{
	
	unsigned char *fifo = (unsigned char *)&USBFIFO0;
	
	for (unsigned int i=0; i<length; i++)
	{
		*buffer++ = *(fifo + (i & 3)); // send bytes
	}

	USBE0CSR0bits.RXRDY = 1; // data packet has been received
}

unsigned int __attribute__((optimize("O1"))) usbhost_ep0_receive_long(unsigned char *buffer, unsigned long length)
{
	unsigned int total = length;
	
	unsigned int received = length;
	unsigned int index = 0;

	usbhost_ep0_receive((unsigned char *)&buffer, length); // get data

	index = length; // increment position in buffer

	while (received >= usbhost_ep0_length)
	{
		usbhost_ep0_interrupt = 0; // clear flag
		
		*((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // request IN transaction

		while (usbhost_ep0_interrupt == 0) { } // wait for interrupt
	 
		received = USBE0CSR2bits.RXCNT; // get amount received

		usbhost_ep0_receive((unsigned char *)&buffer[index], length); // get more data
		
		index += length; // increment position in buffer

		total += length; // increment total received
	}

	usbhost_ep0_interrupt = 0; // clear flag
	
	return total;
}

void __attribute__((optimize("O1"))) usbhost_ep1_receive(unsigned char *buffer)
{
	USBCSR3bits.ENDPOINT = 1; // set current endpoint to EP1 (just in case)

	unsigned char *fifo = (unsigned char *)&USBFIFO1;
	
	unsigned int length = USBIENCSR2bits.RXCNT; // get length of packet

	USBIENCSR0bits.MODE = 0; // EP1 is in RX mode
	
	for (unsigned int i=0; i<length; i++)
	{
		buffer[i] = *(fifo + (i & 3)); // store data
	}

	USBIENCSR1bits.RXPKTRDY = 0; // packet has been unloaded
}

void __attribute__((optimize("O1"))) usbhost_reset()
{
	USBCSR0bits.RESET = 1;
	DelayMS(100);
	USBCSR0bits.RESET = 0;
	DelayMS(100);
}

// public, before loop
void __attribute__((optimize("O1"))) USBHostSetup()
{
	usb_mode = 0xFF; // no device
	
	USBCRCONbits.USBIE = 1; // enable interrupts

	*((unsigned char*)&USBEOFRST + 0x3) = 0x03; // reset usb clock hardware
	DelayMS(100);
	*((unsigned char*)&USBEOFRST + 0x3) = 0x00;
	  
	usbhost_address = 0; // start with default address

	USBCSR2bits.SESSRQIE = 1; // enable interrupts
	USBCSR2bits.CONNIE = 1;
	USBCSR2bits.RESETIE = 1;
	USBCSR2bits.VBUSERRIE = 1;
	USBCSR2bits.DISCONIE = 1;
	USBCSR2bits.EP1RXIE = 1;
	USBCSR1bits.EP1TXIE = 1;
	
	USBOTGbits.HOSTMODE = 1; // put in host mode
	
	IEC4bits.USBIE = 0; // disable usb interrupt
	IFS4bits.USBIF = 0; // clear flag
	IPC33bits.USBIP = 6; // priority level 6
	IPC33bits.USBIS = 0; // sub-priority level 0

	IEC4bits.USBDMAIE = 0; // disable usb dma interrupt
	IFS4bits.USBDMAIF = 0; // clear flag
	IPC33bits.USBDMAIP = 5; // priority level 5
	IPC33bits.USBDMAIS = 0; // sub-priority level 0

	USBCSR0bits.HSEN = 0; // disable high-speed
	USBCRCONbits.USBIDOVEN = 1; // enable ID override
	USBCRCONbits.PHYIDEN = 1; // enable ID monitoring

	USBCRCONbits.USBIDVAL = 0; // set ID override value to 0
	
	IFS4bits.USBIF = 0; // clear flags again
	IEC4bits.USBIE = 1; // enable usb interrupt

	CNPDFbits.CNPDF3 = 1; // pull down USBID pin for host mode
	LATFbits.LATF3 = 0; // then ground USBID pin for host mode

	usbhost_stage = 0;

	USBOTGbits.SESSION = 1; // start usb session
}

// public, in loop
void __attribute__((optimize("O1"))) USBHostTasks()
{
	if (usbhost_connected > 0)
	{
		//SendHex(usbhost_stage);
		//SendChar('.');
		
		switch (usbhost_stage)
		{
			case 0:
			{
				if (USBOTGbits.LSDEV > 0) // low-speed device?
				{
					usbhost_ep0_length = 8; // set EP0 length to 8 bytes
				}
				else
				{
					usbhost_ep0_length = 64; // set EP0 length to 64 bytes for now
				}
			
				usbhost_reset();

				usbhost_ep0_send_setup(usbhost_device_descriptor_request, 8);

				usbhost_stage++;
				usbhost_ep0_interrupt = 0;

				break;
			}
			case 1:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // request packet
					
					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 2:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_ep0_receive(usbhost_device_descriptor, USBE0CSR2bits.RXCNT);
					
					//SendString("Device Descriptor \\");
					//for (unsigned int i=0; i<32; i++) SendHex(usbhost_device_descriptor[i]);
					//SendString("\r\n\\");
					
					if (usbhost_device_descriptor[8] > 0)
					{
						usbhost_ep0_length = usbhost_device_descriptor[8];
					}

					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // set both STATPKT and TXPKTRDY for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 3:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_device_assign_address[2] = 2; // set as address 2

					usbhost_ep0_send_setup(usbhost_device_assign_address, 8); // assign address to device
					
					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}				

				break;
			}
			case 4:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					DelayMS(50); // delay to let device get ready

					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x60; // set both STATUS and REQPKT for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}				

				break;
			}	
			case 5:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // clear STATUS and REQPKT

					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // send empty packet request

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 6:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_ep0_receive(usbhost_device_configuration, USBE0CSR2bits.RXCNT); // should receive 0 bytes
					
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // set both STATPKT and TXPKTRDY for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 7:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_address = 2; // for EP1
				
					usbhost_initialize_endpoints(usbhost_address); // initialize endpoints

					usbhost_ep0_send_setup(usbhost_device_configuration_request, 8); // send request for short configuration

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 8:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // send empty packet request (handshake)

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 9:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_ep0_receive(usbhost_device_configuration, USBE0CSR2bits.RXCNT); // receive X bytes
					
					//SendString("Device Configuration \\");
					//for (unsigned int i=0; i<32; i++) SendHex(usbhost_device_configuration[i]);
					//SendString("\r\n\\");
					
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // set both STATPKT and TXPKTRDY for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 10:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_device_configuration_request[6] = usbhost_device_configuration[2];
					
					usbhost_ep0_send_setup(usbhost_device_configuration_request, 8); // send request for long configuration

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 11:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // send empty packet request (handshake)

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 12:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					//usbhost_ep0_receive_long(usbhost_device_configuration, USBE0CSR2bits.RXCNT); // receive X bytes
					usbhost_ep0_receive(usbhost_device_configuration, USBE0CSR2bits.RXCNT); // receive X bytes
					
					//SendString("Device Configuration \\");
					//for (unsigned int i=0; i<32; i++) SendHex(usbhost_device_configuration[i]);
					//SendString("\r\n\\");
					
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // set both STATPKT and TXPKTRDY for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 13:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_ep0_send_setup(usbhost_device_name_request, 8); // send request for short device name

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 14:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // send empty packet request (handshake)

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 15:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_ep0_receive(usbhost_device_name, USBE0CSR2bits.RXCNT); // receive X bytes
					
					//SendString("Device Name \\");
					//for (unsigned int i=0; i<32; i++) SendHex(usbhost_device_name[i]);
					//SendString("\r\n\\");
					
					usbhost_device_name_request[6] = usbhost_device_name[0];

					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // set both STATPKT and TXPKTRDY for handshake

					usbhost_ep0_send_setup(usbhost_device_name_request, 8); // send request for full device name

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 16:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // send empty packet request (handshake)

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 17:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					//usbhost_ep0_receive_long(usbhost_device_name, USBE0CSR2bits.RXCNT); // receive X bytes
					usbhost_ep0_receive(usbhost_device_name, USBE0CSR2bits.RXCNT); // receive X bytes
					
					//SendString("Device Name \\");
					//for (unsigned int i=0; i<32; i++) SendHex(usbhost_device_name[i]);
					//SendString("\r\n\\");
					
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // set both STATPKT and TXPKTRDY for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 18:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_ep0_send_setup(usbhost_device_set_configuration, 8); // send configuration request

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 19:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x60; // set both STATPKT and REQPKT for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 20:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // clear STATUS and REQPKT

					usbhost_ep0_send_setup(usbhost_device_set_idle, 8); // send idle request
					
					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 21:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x60; // set both STATPKT and REQPKT for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 22:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // clear STATUS and REQPKT

					usbhost_ep0_send_setup(usbhost_device_set_protocol, 8); // send protocol request

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 23:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x60; // set both STATPKT and REQPKT for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 24:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) &= ~0x41; // clear STATUS and REQPKT

					usbhost_ep0_send_setup(usbhost_device_hid_report_request, 8); // send hid report request

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 25:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x20; // send empty packet request (handshake)

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 26:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					//usbhost_ep0_receive_long(usbhost_device_hid_report, USBE0CSR2bits.RXCNT);
					usbhost_ep0_receive(usbhost_device_hid_report, USBE0CSR2bits.RXCNT);

					//SendString("Device HID Report \\");
					//for (unsigned int i=0; i<32; i++) SendHex(usbhost_device_hid_report[i]);
					//SendString("\r\n\\");
					
					*((unsigned char*)&USBE0CSR0 + 0x2) = 0x42; // set both STATPKT and TXPKTRDY for handshake

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			case 27:
			{
				if (usbhost_ep0_interrupt > 0)
				{
					usbhost_device_millis = 0; // hid request delay

					usbhost_stage++;
					usbhost_ep0_interrupt = 0;
				}

				break;
			}
			default:
			{	
				if (usb_mode == 0x00) // keyboard
				{
					if (usbhost_keyboard_repeat <= usbhost_device_millis - usbhost_device_millis_previous)
					{
						for (unsigned char i=2; i<8; i++)
						{
							if (usbhost_keyboard_previous[i] != 0x00)
							{
								usb_state_array[usb_writepos] = usb_conversion[usbhost_keyboard_previous[i] +
									(usbhost_keyboard_previous[0] == 0x00 ? 0x00 : 0x80)];
								usb_writepos++;
							}	
						}

						usbhost_keyboard_repeat = 250; // repeat every 250ms
					}
					else
					{
						usbhost_keyboard_repeat -= usbhost_device_millis - usbhost_device_millis_previous;
					}
				}

				if (usbhost_device_millis > 25)
				{
					usbhost_device_millis = 0; // hid request delay
					
					USBCSR3bits.ENDPOINT = 1; // set current endpoint to EP1 (just in case)
					USBIENCSR1bits.REQPKT = 1; // request packet
				}
				
				usbhost_device_millis_previous = usbhost_device_millis;

				if (usbhost_ep1_interrupt > 0)
				{	
					usbhost_ep1_receive(usbhost_ep1_buffer);
					
					//for (int i=0; i<8; i++) { SendHex(usbhost_ep1_buffer[i]); SendChar('.'); }
					//SendString("\r\n\\");
					
					if (usbhost_device_hid_report[1] == 0x01 && usbhost_device_hid_report[3] == 0x06) // keyboard
					{
						usb_mode = 0x00;
					}
					else if (usbhost_device_hid_report[1] == 0x01 && usbhost_device_hid_report[3] == 0x02) // mouse
					{	
						usb_mode = 0x01;
					}
					else
					{
						if (usbhost_ep1_buffer[0] == 0x00 && usbhost_ep1_buffer[1] == 0x14) // xbox-type controller
						{
							usb_mode = 0x02;
						}
						else
						{
							usb_mode = 0xFF; // unknown
						}
					}
					
					if (usb_mode == 0x00) // keyboard
					{	
						char test = 1;

						for (unsigned char i=0; i<8; i++)
						{
							if (usbhost_keyboard_previous[i] != usbhost_ep1_buffer[i])
							{
								test = 0;
							}
						}

						if (test == 0)
						{
							for (unsigned char i=2; i<8; i++)
							{
								test = 0;

								for (unsigned char j=2; j<8; j++)
								{
									if (usbhost_keyboard_previous[i] == usbhost_ep1_buffer[j])
									{
										test = 1;
									}
								}

								if (test == 0) // release
								{
									usb_state_array[usb_writepos] = 0x0B;
									usb_writepos++;

									usb_state_array[usb_writepos] = usb_conversion[usbhost_keyboard_previous[i] + 
										(usbhost_keyboard_previous[0] == 0x00 ? 0x00 : 0x80)];
									usb_writepos++;
								}
							}

							for (unsigned char i=2; i<8; i++)
							{
								test = 0;

								for (unsigned char j=2; j<8; j++)
								{
									if (usbhost_ep1_buffer[i] == usbhost_keyboard_previous[j])
									{
										test = 1;
									}
								}

								if (test == 0) // press
								{
									usbhost_keyboard_repeat = 1000; // delay until repeat

									usb_state_array[usb_writepos] = usb_conversion[usbhost_ep1_buffer[i] +
										(usbhost_ep1_buffer[0] == 0x00 ? 0x00 : 0x80)];
									usb_writepos++;
								}
							}

							for (unsigned char i=0; i<8; i++)
							{
								usbhost_keyboard_previous[i] = usbhost_ep1_buffer[i];
							}
						}
					}
					else if (usb_mode == 0x01) // mouse
					{
						usb_state_array[usb_writepos] = usbhost_ep1_buffer[0];
		
						if ((signed char)(usbhost_ep1_buffer[1]) < 0)
						{
							if (usb_cursor_x[(unsigned char)(usb_writepos-1)] < 0 - (signed char)(usbhost_ep1_buffer[1]))
							{
								usb_cursor_x[usb_writepos] = 0;
							}
							else
							{
								usb_cursor_x[usb_writepos] = 
									usb_cursor_x[(unsigned char)(usb_writepos-1)] + (signed char)(usbhost_ep1_buffer[1]);
							}
						}
						else if ((signed char)(usbhost_ep1_buffer[1]) >= 0)
						{
							if (usb_cursor_x[(unsigned char)(usb_writepos-1)] >= SCREEN_X - (signed char)(usbhost_ep1_buffer[1]))
							{
								usb_cursor_x[usb_writepos] = SCREEN_X;
							}
							else
							{
								usb_cursor_x[usb_writepos] = 
									usb_cursor_x[(unsigned char)(usb_writepos-1)] + (signed char)(usbhost_ep1_buffer[1]);
							}
						}

						if ((signed char)(usbhost_ep1_buffer[2]) < 0)
						{
							if (usb_cursor_y[(unsigned char)(usb_writepos-1)] < 0 - (signed char)(usbhost_ep1_buffer[2]))
							{
								usb_cursor_y[usb_writepos] = 0;
							}
							else
							{
								usb_cursor_y[usb_writepos] = 
									usb_cursor_y[(unsigned char)(usb_writepos-1)] + (signed char)(usbhost_ep1_buffer[2]);
							}
						}
						else if ((signed char)(usbhost_ep1_buffer[2]) >= 0)
						{
							if (usb_cursor_y[(unsigned char)(usb_writepos-1)] >= SCREEN_Y - (signed char)(usbhost_ep1_buffer[2]))
							{
								usb_cursor_y[usb_writepos] = SCREEN_Y;
							}
							else
							{
								usb_cursor_y[usb_writepos] = 
									usb_cursor_y[(unsigned char)(usb_writepos-1)] + (signed char)(usbhost_ep1_buffer[2]);
							}
						}

						usb_writepos++;
					}
					else if (usb_mode == 0x02) // xbox controller
					{
						// Format: 0xWXYZ
						// W = 1 start, 2 select, 3 both
						// X = 1 up, 2 down, 4 left, 8 right (combinations for multiple)
						// Y = 1 a, 2 b, 3 x, 4 y (combinations for multiple)
						// Z = 1 left bumper, 2 right bumper, 3 both
						usb_buttons[usb_writepos] = (unsigned int)((unsigned int)(usbhost_ep1_buffer[2] << 8) + usbhost_ep1_buffer[3]);
			
						usb_writepos++;
					}

					usbhost_ep1_interrupt--;
				}
			}
		}
	}
}




