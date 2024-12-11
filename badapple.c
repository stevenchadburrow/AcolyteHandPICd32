


void BadApple()
{ 
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
			while (1) { } // lock up
		} 

		T8CONbits.ON = 0; // turn off TMR8 
		T9CONbits.ON = 0; // turn off TMR9

		unsigned int x = 0x0000;
		unsigned int y = 0x0000;
		unsigned char value = 0x00;

		unsigned int frames = 0;

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

		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???
		sdcard_receivebyte(); // ???

		while (frames < 13138) // total frames in the video
		{
			frames += 1; // 60 FPS
			
			//x = 88;
			//y = 104;
			x = 16;
			y = 0;

			for (unsigned int i=0; i<12; i++)
			{
				if (frames != 1)
				{
					sdcard_receivebyte(); // data packet begins with 0xFE
				}

				for (unsigned int j=0; j<16; j++)
				{	
					for (unsigned int k=0; k<32; k++)
					{
						// get value from SDcard
						value = sdcard_receivebyte();

						if (k == 15 || k == 31) // audio
						{
							// Should add 0x07 before shifting in case of rounding???
							PORTH = (((value >> 4) + 0x08) & 0x0F); 
						}
						else // comment the 'else' out to show the audio
						{				
							for (unsigned int l=0; l<8; l++)
							{
								if ((value & 0x01) == 0x01)
								{
									screen_buffer[y*SCREEN_X+x] = 0xFF;
									screen_buffer[y*SCREEN_X+x+1] = 0xFF;
									screen_buffer[(y+1)*SCREEN_X+x] = 0xFF;
									screen_buffer[(y+1)*SCREEN_X+x+1] = 0xFF;
								}
								else
								{
									screen_buffer[y*SCREEN_X+x] = 0x00;
									screen_buffer[y*SCREEN_X+x+1] = 0x00;
									screen_buffer[(y+1)*SCREEN_X+x] = 0x00;
									screen_buffer[(y+1)*SCREEN_X+x+1] = 0x00;
								}

								value = (unsigned int)(value >> 1);

								x += 2;
							}
						}

						// fine tune delay here
						count = (unsigned long)(25); // 63 for smaller screen
						_CP0_SET_COUNT(0);
						while (count > _CP0_GET_COUNT());
					}

					y += 2;
				
					//x = 88;
					x = 16;
				}

				sdcard_receivebyte();
				sdcard_receivebyte(); // data packet ends with 0x55 then 0xAA

				sdcard_receivebyte(); // ???
			}			
		}
	}
}


