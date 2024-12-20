

// use VideoExtractor.cpp to convert!

void AudioVideoDemo()
{ 
	unsigned int high_block = 0x0040; // default starting at 1 GB into the SDcard
	unsigned int low_block = 0x0000;
	
	menu_x = 484;
	menu_y = 16;
	menu_pos = 0;
	menu_max = 5;
	
	display_string(menu_x, menu_y,		" *** Select Start\\");
	display_string(menu_x, menu_y+8,	" Seek +0.0 GB    \\");
	display_string(menu_x, menu_y+16,	" Seek +0.5 GB    \\");
	display_string(menu_x, menu_y+24,	" Seek +1.0 GB    \\");
	display_string(menu_x, menu_y+32,	" Seek +1.5 GB    \\");

	unsigned char pos = 0;
	
	while (pos == 0)
	{
		pos = Menu();
	}
	
	if (pos == 1)
	{
		high_block = 0x0000; // 0 MB into SDcard
		low_block = 0x0000;
	}
	else if (pos == 2)
	{
		high_block = 0x0020; // 500 MB into SDcard
		low_block = 0x0000;
	}
	else if (pos == 3)
	{
		high_block = 0x0040; // 1000 MB into SDcard
		low_block = 0x0000;
	}
	else if (pos == 4)
	{
		high_block = 0x0060; // 1500 MB into SDcard
		low_block = 0x0000;
	}
	
	while (1)
	{
		for (unsigned int y=0; y<SCREEN_Y; y++)
		{
			for (unsigned int x=0; x<SCREEN_X; x++)
			{
				screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
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
			return;
		} 

		unsigned int x = 0x0000;
		unsigned int y = 0x0000;
		unsigned char value = 0x00;

		unsigned long frames = 0;
		unsigned char loop = 1;
		
		unsigned char temp_value = 0x00;

		//unsigned long count = 0;

		sdcard_disable();
		sdcard_pump();
		sdcard_longdelay(); // this is probably not needed
		sdcard_enable();
		sdcard_sendbyte(0x52); // CMD18 = 0x40 + 0x12 (18 in hex)
		sdcard_sendbyte((high_block&0x00FF));
		sdcard_sendbyte(((low_block&0xFF00) >> 8));
		sdcard_sendbyte((low_block&0x00FE)); // only blocks of 512 bytes
		sdcard_sendbyte(0x00);
		sdcard_sendbyte(0x01); // CRC (general)
		temp_value = sdcard_waitresult(); // command response
		if (temp_value == 0xFF) { return; }
		else if (temp_value != 0x00) { return; } // expecting 0x00
		temp_value = sdcard_waitresult(); // data packet starts with 0xFE
		if (temp_value == 0xFF) { return; }
		else if (temp_value != 0xFE) { return; }

		// I don't know why I need to do this?
		for (unsigned int i=0; i<49631; i++) sdcard_receivebyte(); // ???, 49631 works with color
		 
		while (loop > 0) // total frames in the video
		{
			frames++;
			
			x = 80;
			y = 48;

			for (unsigned int i=0; i<96; i++)
			{
				if (frames != 1)
				{
					sdcard_receivebyte(); // data packet begins with 0xFE
				}

				for (unsigned int j=0; j<2; j++)
				{
					for (unsigned int j=0; j<4; j++)
					{	
						for (unsigned int k=0; k<64; k++)
						{
							// get value from SDcard
							value = sdcard_receivebyte();

							if (k % 64 == 60) // audio (unused)
							{
								if (value == 0x00 && frames > 360) loop = 0; // exit loop if zero
							}
							else if (k % 64 == 61 || k % 64 == 62)  // audio (unused)
							{
								// do nothing
							}
							else if (k % 64 == 63) // audio
							{
								// Should add 0x07 before shifting in case of rounding???
								PORTH = (unsigned char)(((value + 0x80) >> 2) & 0x3F); 
							}
							else // comment the 'else' out to show the audio
							{				
								screen_buffer[y*SCREEN_X+x] = value;
								screen_buffer[y*SCREEN_X+x+1] = value;
								screen_buffer[(y+1)*SCREEN_X+x] = value;
								screen_buffer[(y+1)*SCREEN_X+x+1] = value;
						
								x += 2;
							}

							/*
							// fine tune delay here
							count = (unsigned long)(96);
							_CP0_SET_COUNT(0);
							while (count > _CP0_GET_COUNT());
							*/
						}
					}
					
					y += 2;
					x = 80;
				}

				sdcard_receivebyte();
				sdcard_receivebyte(); // data packet ends with 0x55 then 0xAA

				sdcard_receivebyte(); // ???
			}			
		}
	}
}


