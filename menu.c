

const unsigned int menu_x = 24;
const unsigned int menu_y = 300;

volatile unsigned char menu_pos = 0;
volatile unsigned char menu_max = 1;
volatile unsigned char menu_loop = 1;
volatile unsigned char menu_key = 0;
volatile unsigned int menu_joy = 0xFFFF;
volatile unsigned int menu_mouse[5] = { 0, 0, 0, 0, 0 };
volatile unsigned char menu_up = 0;
volatile unsigned char menu_down = 0;
volatile unsigned int menu_delay = 0x0000;


unsigned char Menu()
{
	char test = 0;
	
	display_string(menu_x, 16, "Acolyte Hand PIC'd 32\\");

	display_string(menu_x, menu_y,		" Tetra     \\");
	display_string(menu_x, menu_y+8,	" Scratchpad\\");
	display_string(menu_x, menu_y+16,	" Bad Apple \\");
	display_string(menu_x, menu_y+24,	" User Code \\");
	display_string(menu_x, menu_y+32,	" Reprogram \\");
	display_string(menu_x, menu_y+40,	"           \\");

	menu_max = 6; // number of menu items, change accordingly

	display_character(menu_x, menu_y, '>');

	while (menu_loop > 0)
	{
		if (menu_up == 1)
		{
			menu_up = 0;
			
			if (menu_pos > 0)
			{
				display_character(menu_x, menu_y + menu_pos * 8, ' ');
					
				menu_pos--;
					
				display_character(menu_x, menu_y + menu_pos * 8, '>');
				
				music_note(523, 250, 0);
			}
		}
		
		if (menu_down == 1)
		{
			menu_down = 0;
			
			if (menu_pos < menu_max-1)
			{
				display_character(menu_x, menu_y + menu_pos * 8, ' ');

				menu_pos++;

				display_character(menu_x, menu_y + menu_pos * 8, '>');
				
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
		if (USBA_DEVICE_CONNECTED)
		{
			USBA_host_tasks();
			USBA_HID_tasks();
			if (USBA_EP1_RECEIVED > 0)
			{	
				USBA_EP1_receive(USBA_EP1_buffer);
				USBA_HID_process_report();
				USBA_EP1_RECEIVED--;
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
				if (menu_mouse[4] <= (unsigned int)(i * SCREEN_Y / menu_max))
				{
					display_character(menu_x, menu_y + menu_pos * 8, ' ');

					menu_pos = i;

					display_character(menu_x, menu_y + menu_pos * 8, '>');

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

	return menu_pos;
}
