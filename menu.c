

unsigned int menu_x = 16;
unsigned int menu_y = 16;

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
	menu_delay = 0x0000;
	menu_up = 0;
	menu_down = 0;
	menu_loop = 1;
	
	unsigned char exit = 0;
	
	char test = 0;
	
	unsigned char gamepad_flag = 0;

	display_character(menu_x, menu_y+8*menu_pos, '>');

	while (menu_loop > 0)
	{
		USBHostTasks();
		
		if (menu_up == 1)
		{	
			menu_up = 0;
			
			if (menu_pos > 0)
			{
				display_character(menu_x, menu_y + menu_pos * 8, ' ');
					
				menu_pos--;
					
				display_character(menu_x, menu_y + menu_pos * 8, '>');
				
				//music_note(523, 500, 0);
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
				
				//music_note(523, 500, 0);
			}
		}	

		if (menu_delay > 0x0000)
		{
			menu_delay--;
			continue;
		}
		
		menu_delay = 0x7FFF;
		
		menu_key = input_ps2_keyboard();
		
		if (menu_key == 0x00)
		{
			menu_key = input_usb_keyboard();
		}
		
		if (menu_key != 0x00)
		{
			if (menu_key == 0x0D || menu_key == 0x20) // enter or space
			{
				//menu_loop = 0;
				exit = 1;
				
				DelayMS(100);
			}
			else
			{
				if (exit == 1) exit = 255;
			}
			
			if (menu_key == 0x11) // up
			{
				menu_up = 1;
			}
			else if (menu_key == 0x12) // down
			{
				menu_down = 1;
			}
		}
		
		if (gamepad_flag == 0)
		{
			if (PORTJbits.RJ0 == 0) // up
			{
				if ((menu_joy & 0x0001) == 0x0000)
				{
					menu_joy = (menu_joy | 0x0001);

					menu_up = 1;
				}
			}
			else menu_joy = (menu_joy & 0xFFFE);
		}
			
		if (gamepad_flag == 0)
		{
			if (PORTJbits.RJ1 == 0) // down
			{
				if ((menu_joy & 0x0002) == 0x0000)
				{
					menu_joy = (menu_joy | 0x0002);

					menu_down = 1;
				}
			}
			else menu_joy = (menu_joy & 0xFFFD);
		}
		
		if (PORTJbits.RJ4 == 0 || PORTJbits.RJ5 == 0) // either button
		{
			//menu_loop = 0;
			exit = 2;
			
			DelayMS(100);
		}
		else
		{
			if (exit == 2) exit = 255;
		}
		
		if (gamepad_flag == 0)
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
		}

		if (gamepad_flag == 0)
		{
			if (PORTJbits.RJ7 == 0 && gamepad_flag == 0) // down
			{
				if ((menu_joy & 0x0080) == 0x0000)
				{
					menu_joy = (menu_joy | 0x0080);

					menu_down = 1;
				}
			}
			else menu_joy = (menu_joy & 0xFF7F);
		}

		if (PORTJbits.RJ13 == 0 || PORTJbits.RJ14 == 0) // either button
		{
			//menu_loop = 0;
			exit = 3;
			
			DelayMS(100);
		}
		else
		{
			if (exit == 3) exit = 255;
		}
		
		if (usb_mode == 0x02) // xbox controller
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
				
				if ((usb_buttons[usb_readpos] & 0x0010) == 0x0010 ||
					(usb_buttons[usb_readpos] & 0x0020) == 0x0020) // A
				{
					//menu_loop = 0;
					exit = 4;
					
					DelayMS(100);
				}
				else
				{
					if (exit == 4) exit = 255;
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

					//music_note(523, 500, 0);
					
					break;
				}
			}
			
			if (menu_mouse[0] != 0x0000)
			{
				//menu_loop = 0;
				exit = 5;
				
				DelayMS(100);
			}
			else
			{
				if (exit == 5) exit = 255;
			}
		}
		
		gamepad_flag = 1 - gamepad_flag;

		if (gamepad_flag == 0)
		{
			TRISJbits.TRISJ15 = 1; // float joy-select (pulled high) for next frame
		}
		else
		{
			PORTJbits.RJ15 = 0;
			TRISJbits.TRISJ15 = 0; // ground joy-select for next frame
		}
		
		if (exit == 255) menu_loop = 0;
	}
	
	TRISJbits.TRISJ15 = 1; // float joy-select (pulled high)

	//music_note(1047, 500, 0);
	
	display_character(menu_x, menu_y + menu_pos * 8, ' ');

	return menu_pos;
}
