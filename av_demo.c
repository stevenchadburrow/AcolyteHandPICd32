


void AudioVideoDemo()
{ 
	audio_switch = 0; // turn off audio_buffer[]
	
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

		unsigned long count = 0;

		sdcard_disable();
		sdcard_pump();
		sdcard_longdelay(); // this is probably not needed
		sdcard_enable();
		sdcard_sendbyte(0x52); // CMD18 = 0x40 + 0x12 (18 in hex)
		sdcard_sendbyte((0x0000&0x00FF));
		sdcard_sendbyte(((0x0000&0xFF00) >> 8));
		sdcard_sendbyte((0x0000&0x00FE)); // only blocks of 512 bytes
		sdcard_sendbyte(0x00);
		sdcard_sendbyte(0x01); // CRC (general)
		temp_value = sdcard_waitresult(); // command response
		if (temp_value == 0xFF) { return; }
		else if (temp_value != 0x00) { return; } // expecting 0x00
		temp_value = sdcard_waitresult(); // data packet starts with 0xFE
		if (temp_value == 0xFF) { return; }
		else if (temp_value != 0xFE) { return; }

		// I don't know why I need to do this?
		for (unsigned int i=0; i<12407; i++) sdcard_receivebyte(); // ???
		 
		while (loop > 0) // total frames in the video
		{
			frames++;
			
			x = 80;
			y = 48;

			for (unsigned int i=0; i<24; i++)
			{
				if (frames != 1)
				{
					sdcard_receivebyte(); // data packet begins with 0xFE
				}

				for (unsigned int j=0; j<8; j++)
				{	
					for (unsigned int k=0; k<64; k++)
					{
						// get value from SDcard
						value = sdcard_receivebyte();

						if (k == 30 || k == 62) // audio (unused)
						{
							if (value == 0x00 && frames > 360) loop = 0;
						}
						else if (k == 31 || k == 63) // audio
						{
							// Should add 0x07 before shifting in case of rounding???
							PORTH = (((value >> 4) + 0x08) & 0x0F); 
						}
						else // comment the 'else' out to show the audio
						{				
							for (unsigned int l=0; l<4; l++)
							{
								switch ((value & 0x03))
								{
									case 0x00: // black
									{
										screen_buffer[y*SCREEN_X+x] = 0x00;
										screen_buffer[y*SCREEN_X+x+1] = 0x00;
										screen_buffer[(y+1)*SCREEN_X+x] = 0x00;
										screen_buffer[(y+1)*SCREEN_X+x+1] = 0x00;
										break;
									}
									case 0x01: // dark-grey
									{
										screen_buffer[y*SCREEN_X+x] = 0x49;
										screen_buffer[y*SCREEN_X+x+1] = 0x49;
										screen_buffer[(y+1)*SCREEN_X+x] = 0x49;
										screen_buffer[(y+1)*SCREEN_X+x+1] = 0x49;
										break;
									}
									case 0x02: // light-grey
									{
										screen_buffer[y*SCREEN_X+x] = 0x92;
										screen_buffer[y*SCREEN_X+x+1] = 0x92;
										screen_buffer[(y+1)*SCREEN_X+x] = 0x92;
										screen_buffer[(y+1)*SCREEN_X+x+1] = 0x92;
										break;
									}
									case 0x03: // white
									{
										screen_buffer[y*SCREEN_X+x] = 0xFF;
										screen_buffer[y*SCREEN_X+x+1] = 0xFF;
										screen_buffer[(y+1)*SCREEN_X+x] = 0xFF;
										screen_buffer[(y+1)*SCREEN_X+x+1] = 0xFF;
										break;
									}
								}
									
								value = (unsigned int)(value >> 2);

								x += 2;
							}
						}

						
						// fine tune delay here
						count = (unsigned long)(96);
						_CP0_SET_COUNT(0);
						while (count > _CP0_GET_COUNT());
						
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


