


void BadApple()
{ 
	while (1)
	{
		for (unsigned int y=0; y<SCREEN_Y; y++)
		{
			for (unsigned int x=0; x<SCREEN_X; x++)
			{
#ifdef SPLASH
				screen_buffer[y*SCREEN_X+x] = splash_television[y * SCREEN_X + x];
#else
				screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
#endif
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
			// lock up
			while (1)
			{
				for (unsigned int i=0; i<SCREEN_Y; i++)
				{
					for (unsigned int j=0; j<SCREEN_X; j++)
					{
						screen_buffer[i*SCREEN_X+j] = 0x00;
					}
				}

				DelayMS(100);
				DelayMS(100);
				DelayMS(100);
				DelayMS(100);

				for (unsigned int i=0; i<SCREEN_Y; i++)
				{
					for (unsigned int j=0; j<SCREEN_X; j++)
					{
						screen_buffer[i*SCREEN_X+j] = 0xFF;
					}
				}

				DelayMS(100);
				DelayMS(100);
				DelayMS(100);
				DelayMS(100);
			}
		}  

		unsigned int address_high = 0x0000, address_low = 0x0000;

		unsigned int x = 0x0000;
		unsigned int y = 0x0000;
		unsigned char value = 0x00;

		unsigned char temp_value = 0x00;

		unsigned int frames = 0;

		while (frames < 13138) // total frames in the video
		{
			frames += 2; // only 30 FPS
			
			x = 88;
			y = 104;

			for (unsigned int l=0; l<12; l++)
			{
				//sdcard_readblock(address_high, (unsigned int)(address_low + (j * 2)));

				sdcard_disable();
				sdcard_pump();
				sdcard_longdelay(); // this is probably not needed
				sdcard_enable();
				sdcard_sendbyte(0x51); // CMD17 = 0x40 + 0x11 (17 in hex)
				sdcard_sendbyte((unsigned char)((address_high)&0x00FF));
				sdcard_sendbyte((unsigned char)(((address_low)&0xFF00) >> 8));
				sdcard_sendbyte((unsigned char)((address_low)&0x00FE)); // only blocks of 512 bytes
				sdcard_sendbyte(0x00);
				sdcard_sendbyte(0x01); // CRC (general)
				temp_value = sdcard_waitresult(); // command response
				if (temp_value == 0xFF) { break; }
				else if (temp_value != 0x00) { break; } // expecting 0x00
				temp_value = sdcard_waitresult(); // data packet starts with 0xFE
				if (temp_value == 0xFF) { break; }
				else if (temp_value != 0xFE) { break; }

				for (unsigned int i=0; i<16; i++)
				{
					for (unsigned int j=0; j<32; j++) // packet of 512 bytes
					{					
						// get value from SDcard
						value = sdcard_receivebyte();

						for (unsigned int k=0; k<8; k++)
						{
							if ((value & 0x01) == 0x01)
							{
								screen_buffer[y*SCREEN_X+x] = 0xFF;
							}
							else
							{
								screen_buffer[y*SCREEN_X+x] = 0x00;
							}

							value = (unsigned int)(value >> 1);

							x += 1;
						}
					}

					x = 88;
					y += 1;
				}

				temp_value = sdcard_receivebyte(); // data packet ends with 0x55 then 0xAA
				temp_value = sdcard_receivebyte(); // ignore here
				sdcard_disable();

				if (address_low == 0xFFFE)
				{
					address_low = 0;
					
					address_high += 1; 
				}
				else
				{
					address_low += 2;
				}
			}

			// uncomment if you want to skip next frame entirely!
			//for (unsigned int l=0; l<12; l++)
			//{
			//	if (address_low == 0xFFFE)
			//	{
			//		address_low = 0;
			//		
			//		address_high += 1; 
			//	}
			//	else
			//	{
			//		address_low += 2;
			//	}
			//}
			
			// fine tune delay here
			unsigned long count = 0x00000000;
			count = (unsigned long)(((unsigned long)((SYS_FREQ / 1000) * 17) / 5) / 2); // Convert microseconds us into how many clock ticks it will take
			_CP0_SET_COUNT(0); // Set Core Timer count to 0
			while (count > _CP0_GET_COUNT()); // Wait until Core Timer count reaches the number we calculated earlier
		}
	}
}


