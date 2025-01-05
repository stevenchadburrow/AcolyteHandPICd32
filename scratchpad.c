
#define CHARS_WIDE 90
#define CHARS_HIGH 64

volatile char __attribute__((coherent)) scratchpad_buffer[CHARS_WIDE][CHARS_HIGH];

void Scratchpad()
{
	TRISKbits.TRISK6 = 1; // float joy-select (pulled high)
	
	display_foreground_color = 0xFF; // change color of text!
	display_background_color = 0x25; 
	
	char key_value = 0x00;
	char key_prev = '*';
	
	char test = 0x00;
	
	unsigned int pos_x = 0x00;
	unsigned int pos_y = 0x00;
	
	unsigned int mouse_state[5] = { 0, 0, 0, 0, 0 };
	
	unsigned char joy_curr[2] = { 0xFF, 0xFF };
	unsigned int joy_delay = 0x0000;
	unsigned int joy_speed = 0x01FF;
	
	unsigned char joy_toggle = 1;
	
	unsigned int overall_delay = 0x0000;
	
	for (unsigned int y=0; y<SCREEN_Y; y++)
	{
		for (unsigned int x=0; x<SCREEN_X; x++)
		{
			screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
		}
	}
	
	for (unsigned int y=0; y<CHARS_HIGH; y++)
	{
		for (unsigned int x=0; x<CHARS_WIDE; x++)
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
		
		USBHostTasks();
		
		key_value = input_ps2_keyboard();
		
		if (key_value == 0x00)
		{
			key_value = input_usb_keyboard();
		}
		
		if (joy_toggle == 1)
		{
			joy_curr[0] = joy_curr[0] | 0xFC; 

			if (PORTKbits.RK0 == 0) joy_curr[0] = (joy_curr[0] & 0x7F);
			if (PORTKbits.RK1 == 0) joy_curr[0] = (joy_curr[0] & 0xBF);
			if (PORTKbits.RK2 == 0) joy_curr[0] = (joy_curr[0] & 0xDF);
			if (PORTKbits.RK3 == 0) joy_curr[0] = (joy_curr[0] & 0xEF);
			if (PORTKbits.RK4 == 0) joy_curr[0] = (joy_curr[0] & 0xF7);
			if (PORTKbits.RK5 == 0) joy_curr[0] = (joy_curr[0] & 0xFB);
		}
		else
		{
			joy_curr[0] = joy_curr[0] | 0x03; 

			if (PORTKbits.RK4 == 0) joy_curr[0] = (joy_curr[0] & 0xFD);
			if (PORTKbits.RK5 == 0) joy_curr[0] = (joy_curr[0] & 0xFE);
		}
		
		if (usb_mode != 0x02) // xbox-type controller
		{
			if (joy_toggle == 1)
			{
				joy_curr[1] = joy_curr[1] | 0xFC; 

				if (PORTFbits.RF0 == 0) joy_curr[1] = (joy_curr[1] & 0x7F);
				if (PORTFbits.RF1 == 0) joy_curr[1] = (joy_curr[1] & 0xBF);
				if (PORTFbits.RF2 == 0) joy_curr[1] = (joy_curr[1] & 0xDF);
				if (PORTFbits.RF4 == 0) joy_curr[1] = (joy_curr[1] & 0xEF);
				if (PORTFbits.RF5 == 0) joy_curr[1] = (joy_curr[1] & 0xF7);
				if (PORTFbits.RF8 == 0) joy_curr[1] = (joy_curr[1] & 0xFB);
			}
			else
			{
				joy_curr[1] = joy_curr[1] | 0x03; 

				if (PORTFbits.RF5 == 0) joy_curr[1] = (joy_curr[1] & 0xFD);
				if (PORTFbits.RF8 == 0) joy_curr[1] = (joy_curr[1] & 0xFE);
			}
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
		
		joy_toggle = 1 - joy_toggle;
		
		if (joy_toggle == 1)
		{
			TRISKbits.TRISK6 = 1; // float joy-select (pulled high)
		}
		else
		{
			PORTKbits.RK6 = 0;
			TRISKbits.TRISK6 = 0; // ground joy-select
		}
		
		if (key_value == 0x00)
		{
			for (unsigned char z=0; z<2; z++)
			{
				if (joy_delay == 0x0000)
				{
					if ((joy_curr[z] & 0x08) == 0x00 || (joy_curr[z] & 0x01) == 0x00) //&& (joy_prev[z] & 0x08) == 0x08) // button 1
					{
						scratchpad_buffer[pos_x/8][pos_y/8] = ' ';
						display_inverse(pos_x+4, pos_y, scratchpad_buffer[pos_x/8][pos_y/8]);	
						joy_delay = joy_speed;
					}
					else if ((joy_curr[z] & 0x04) == 0x00 || (joy_curr[z] & 0x02) == 0x00) //&& (joy_prev[z] & 0x04) == 0x04) // button 2
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
	
				for (unsigned int y=0; y<SCREEN_Y; y++)
				{
					for (unsigned int x=0; x<SCREEN_X; x++)
					{
						screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
					}
				}

				for (unsigned int y=0; y<CHARS_HIGH; y++)
				{
					for (unsigned int x=0; x<CHARS_WIDE; x++)
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
				
				if (pos_y >= SCREEN_Y-8)
				{
					for (unsigned int y=0; y<SCREEN_Y-8; y++)
					{
						for (unsigned int x=0; x<SCREEN_X; x++)
						{
							screen_buffer[y*SCREEN_X+x] = screen_buffer[(y+8)*SCREEN_X+x];
						}
					}
					
					for (unsigned int y=SCREEN_Y-8; y<SCREEN_Y; y++)
					{
						for (unsigned int x=0; x<SCREEN_X; x++)
						{
							screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
						}
					}
					
					for (unsigned int y=0; y<CHARS_HIGH-1; y++)
					{
						for (unsigned int x=0; x<CHARS_WIDE; x++)
						{
							scratchpad_buffer[x][y] = scratchpad_buffer[x][y+1];
						}
					}
					
					for (unsigned int x=0; x<CHARS_WIDE; x++)
					{
						scratchpad_buffer[x][CHARS_HIGH-1] = ' ';
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
				
				if (pos_y < SCREEN_Y-16)
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
				
				if (pos_x < SCREEN_X-16)
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
				
				if (pos_x < SCREEN_X-16)
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
			
			if (pos_x > SCREEN_X-16) pos_x = SCREEN_X-16;
			
			pos_y = (unsigned int)(mouse_state[4] / 8) * 8;
			
			if (pos_y > SCREEN_Y-16) pos_y = SCREEN_Y-16;
			
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

