


volatile void __attribute__((optimize("00"),vector(_OUTPUT_COMPARE_3_VECTOR), interrupt(ipl7srs))) oc3_handler()
{		
    IFS0bits.OC3IF = 0;  // clear interrupt flag
	
	PORTH = 0;
	
	screen_scanline = screen_scanline + 1; // increment scanline
	
	if (screen_scanline == 1066)
	{
		screen_scanline = 0;
	}
	
	if (screen_scanline == 0)
	{
		DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
		DCH1SSA = VirtToPhys(screen_line); // transfer source physical address
		DCH1SSIZ = SCREEN_X; // source size
		DCH1DSIZ = 1; // dst size 
		DCH1CSIZ = SCREEN_X; // X byte per event
		DCH1CONbits.CHEN = 1; // enable channel
	}
	else if (screen_scanline < SCREEN_Y*4)
	{	
		DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
		DCH1SSA = VirtToPhys(screen_buffer + SCREEN_X*SCREEN_Y*screen_frame + SCREEN_X*((screen_scanline)>>2)); // transfer source physical address
		DCH1SSIZ = SCREEN_X; // source size
		DCH1DSIZ = 1; // dst size 
		DCH1CSIZ = SCREEN_X; // X byte per event
		DCH1CONbits.CHEN = 1; // enable channel
	}
	else if (screen_scanline == SCREEN_Y*4)
	{
		DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
		DCH1SSA = VirtToPhys(screen_line); // transfer source physical address
		DCH1SSIZ = SCREEN_X; // source size
		DCH1DSIZ = 1; // dst size 
		DCH1CSIZ = SCREEN_X; // X byte per event
		DCH1CONbits.CHEN = 1; // enable channel
	}
	else if (screen_scanline == SCREEN_Y*4+1)
	{
		if (controller_enable > 0)
		{
			if (TRISKbits.TRISK6 > 0)
			{
				controller_status_1 = (controller_status_1 & 0x0C);

				controller_status_1 = controller_status_1 | 
					((!PORTKbits.RK0) << 4) | // up
					((!PORTKbits.RK1) << 5) | // down
					((!PORTKbits.RK2) << 6) | // left
					((!PORTKbits.RK3) << 7) | // right
					((!PORTKbits.RK4) << 1) | // B
					((!PORTKbits.RK5)); // A

				controller_status_2 = (controller_status_2 & 0x0C);

				controller_status_2 = controller_status_2 | 
					((!PORTFbits.RF0) << 4) | // up
					((!PORTFbits.RF1) << 5) | // down
					((!PORTFbits.RF2) << 6) | // left
					((!PORTFbits.RF4) << 7) | // right
					((!PORTFbits.RF5) << 1) | // B
					((!PORTFbits.RF8)); // A

				PORTKbits.RK6 = 0; // ground when not floating
				TRISKbits.TRISK6 = 0;
			}
			else
			{
				controller_status_1 = (controller_status_1 & 0xF3);

				controller_status_1 = controller_status_1 |
					((!PORTKbits.RK4) << 2) | // select
					((!PORTKbits.RK5) << 3); // start

				controller_status_2 = (controller_status_2 & 0xF3);

				controller_status_2 = controller_status_2 |
					((!PORTFbits.RF5) << 2) | // select
					((!PORTFbits.RF8) << 3); // start

				PORTKbits.RK6 = 0;
				TRISKbits.TRISK6 = 1; // high when floating
			}
			
			
			if (ps2_ready[0] == 0x01 && ps2_mode[0] == 0x00) // ready and keyboard
			{
				char value = 0x00;
				
				while (ps2_readpos[0] != ps2_writepos[0])
				{
					if (ps2_state_array[0][ps2_readpos[0]] == 0x0B) // release
					{
						ps2_readpos[0]++;

						while (ps2_readpos[0] == ps2_writepos[0]) { }

						value = (char)ps2_state_array[0][ps2_readpos[0]];
						
						if (value == 'X' || value == 'x')
						{
							controller_status_3 = (controller_status_3 & 0xFE);
						}
						else if (value == 'Z' || value == 'z')
						{
							controller_status_3 = (controller_status_3 & 0xFD);
						}
						else if (value == 0x20) // space
						{
							controller_status_3 = (controller_status_3 & 0xFB);
						}
						else if (value == 0x0D) // return
						{
							controller_status_3 = (controller_status_3 & 0xF7);
						}
						else if (value == 0x11) // up
						{
							controller_status_3 = (controller_status_3 & 0xEF);
						}
						else if (value == 0x12) // down
						{
							controller_status_3 = (controller_status_3 & 0xDF);
						}
						else if (value == 0x13) // left
						{
							controller_status_3 = (controller_status_3 & 0xBF);
						}
						else if (value == 0x14) // right
						{
							controller_status_3 = (controller_status_3 & 0x7F);
						}
						
						ps2_readpos[0]++;
					}
					else
					{
						value = (char)ps2_state_array[0][ps2_readpos[0]];
						
						if (value == 'X' || value == 'x')
						{
							controller_status_3 = (controller_status_3 | 0x01);
						}
						else if (value == 'Z' || value == 'z')
						{
							controller_status_3 = (controller_status_3 | 0x02);
						}
						else if (value == 0x20) // space
						{
							controller_status_3 = (controller_status_3 | 0x04);
						}
						else if (value == 0x0D) // return
						{
							controller_status_3 = (controller_status_3 | 0x08);
						}
						else if (value == 0x11) // up
						{
							controller_status_3 = (controller_status_3 | 0x10);
						}
						else if (value == 0x12) // down
						{
							controller_status_3 = (controller_status_3 | 0x20);
						}
						else if (value == 0x13) // left
						{
							controller_status_3 = (controller_status_3 | 0x40);
						}
						else if (value == 0x14) // right
						{
							controller_status_3 = (controller_status_3 | 0x80);
						}
						
						ps2_readpos[0]++;
					}
				}
			}
			
			if (ps2_ready[1] == 0x01 && ps2_mode[1] == 0x00) // ready and keyboard
			{
				char value = 0x00;
				
				while (ps2_readpos[1] != ps2_writepos[1])
				{
					if (ps2_state_array[1][ps2_readpos[1]] == 0x0B) // release
					{
						ps2_readpos[1]++;

						while (ps2_readpos[1] == ps2_writepos[1]) { }

						value = (char)ps2_state_array[1][ps2_readpos[1]];
						
						if (value == 'X' || value == 'x')
						{
							controller_status_4 = (controller_status_4 & 0xFE);
						}
						else if (value == 'Z' || value == 'z')
						{
							controller_status_4 = (controller_status_4 & 0xFD);
						}
						else if (value == 0x20) // space
						{
							controller_status_4 = (controller_status_4 & 0xFB);
						}
						else if (value == 0x0D) // return
						{
							controller_status_4 = (controller_status_4 & 0xF7);
						}
						else if (value == 0x11) // up
						{
							controller_status_4 = (controller_status_4 & 0xEF);
						}
						else if (value == 0x12) // down
						{
							controller_status_4 = (controller_status_4 & 0xDF);
						}
						else if (value == 0x13) // left
						{
							controller_status_4 = (controller_status_4 & 0xBF);
						}
						else if (value == 0x14) // right
						{
							controller_status_4 = (controller_status_4 & 0x7F);
						}
						
						ps2_readpos[1]++;
					}
					else
					{
						value = (char)ps2_state_array[1][ps2_readpos[1]];
						
						if (value == 'X' || value == 'x')
						{
							controller_status_4 = (controller_status_4 | 0x01);
						}
						else if (value == 'Z' || value == 'z')
						{
							controller_status_4 = (controller_status_4 | 0x02);
						}
						else if (value == 0x20) // space
						{
							controller_status_4 = (controller_status_4 | 0x04);
						}
						else if (value == 0x0D) // return
						{
							controller_status_4 = (controller_status_4 | 0x08);
						}
						else if (value == 0x11) // up
						{
							controller_status_4 = (controller_status_4 | 0x10);
						}
						else if (value == 0x12) // down
						{
							controller_status_4 = (controller_status_4 | 0x20);
						}
						else if (value == 0x13) // left
						{
							controller_status_4 = (controller_status_4 | 0x40);
						}
						else if (value == 0x14) // right
						{
							controller_status_4 = (controller_status_4 | 0x80);
						}
						
						ps2_readpos[1]++;
					}
				}
			}
		}
		else
		{
			PORTKbits.RK6 = 0;
			TRISKbits.TRISK6 = 1; // high when floating
		}
	}
	
	return;
}

volatile void __attribute__((optimize("00"),vector(_CHANGE_NOTICE_D_VECTOR),interrupt(ipl5srs))) cnd_handler()
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

volatile void __attribute__((optimize("O0"),vector(_UART3_RX_VECTOR), interrupt(ipl4srs))) u3rx_handler() //, nomips16)) u3rx_handler()
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

