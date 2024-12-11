

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

// new audio timer
void __attribute__((vector(_TIMER_8_VECTOR), interrupt(ipl3srs))) t8_handler()
{
	IFS1bits.T8IF = 0;  // clear interrupt flag
	
	PORTH = (unsigned char)(((audio_buffer[audio_position+1] >> 4) + 0x08) & 0x0F); // 4-bit unsigned audio from 16-bit signed audio, this seems to help
	audio_position += 4; // mono channel so skip every other other one
	
	if (audio_position >= 8184)
	{
		for (unsigned int i=0; i<8192; i++) audio_buffer[i] = 0;
		audio_position = 0;
	}
}

// new frame timer
void __attribute__((vector(_TIMER_9_VECTOR), interrupt(ipl2srs))) t9_handler()
{
	IFS1bits.T9IF = 0;  // clear interrupt flag
	
	T9CONbits.ON = 0; // turn off timer
	
	frame_trigger = 1;
}