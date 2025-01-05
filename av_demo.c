

// use VideoExtractor.cpp to convert!

void AudioVideoDemo()
{ 
	unsigned int high_block = 0x0040; // default starting at 1 GB into the SDcard
	unsigned int low_block = 0x0000;
	
	menu_x = 544;
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
	
	
	screen_mode = 1; // 65K-color mode
	
	
	// center on the screen horizontally
	while (TMR3 < 0x01E0 || TMR4 > 0x0E60) { }
	TMR3 = TMR3 - 0x01E0; //0x0CC0 to 0x0AE0;
	TMR4 = TMR3 + 0x01E0; //0x0180 to 0x0360;
	
	
	// DMA setup
	IEC4bits.DMA0IE = 0; // disable interrupts
	IFS4bits.DMA0IF = 0; // clear flags
	IEC4bits.DMA1IE = 0; // disable interrupts
	IFS4bits.DMA1IF = 0; // clear flags
	DMACONbits.ON = 1; // enable the DMA controller
	
	DCH0CONbits.CHEN = 0; // disable channel
	DCH0ECONbits.CHSIRQ = 12; // start on Output Compare 2 interrupt
	DCH0ECONbits.SIRQEN = 1; // enable start interrupt
	DCH0INT = 0x0000; // clear all interrupts
	DCH0INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH0INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH0CONbits.CHCHN = 0; // disallow chaining
	DCH0CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH0CONbits.CHPRI = 0x3; // highest priority
	DCH0SSA = VirtToPhys(screen_zero); // transfer source physical address
	//DCH0DSA = VirtToPhys(((unsigned char*)&PORTH + 1)); // transfer destination physical address
	DCH0DSA = VirtToPhys(&PORTH); // transfer destination physical address
	DCH0SSIZ = 2; // source size
	DCH0DSIZ = 2; // dst size 
	DCH0CSIZ = 2; // 2 byte per event

	DCH1CONbits.CHEN = 0; // disable channel
	DCH1ECONbits.CHSIRQ = 14; // start on Timer 3 interrupt
	DCH1ECONbits.SIRQEN = 1; // enable start interrupt
	DCH1INT = 0x0000; // clear all interrupts
	DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH1INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH1CONbits.CHCHN = 1; // allow chaining
	DCH1CONbits.CHCHNS = 0; // chain from higher channel
	DCH1CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH1CONbits.CHPRI = 0x3; // highest priority
	//DCH1DSA = VirtToPhys(((unsigned char*)&PORTH + 1)); // transfer destination physical address
	DCH1DSA = VirtToPhys(&PORTH); // transfer destination physical address
	DCH1SSIZ = HI_SCREEN_X*2; // source size
	DCH1DSIZ = 2; // dst size 
	DCH1CSIZ = HI_SCREEN_X*2; // X byte per event
	
	DCH2CONbits.CHEN = 0; // disable channel
	DCH2ECONbits.CHSIRQ = 19; // start on Timer 4 interrupt
	DCH2ECONbits.SIRQEN = 1; // enable start interrupt
	DCH2INT = 0x0000; // clear all interrupts
	DCH2INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH2INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH2CONbits.CHCHN = 1; // allow chaining
	DCH2CONbits.CHCHNS = 0; // chain from higher channel
	DCH2CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH2CONbits.CHPRI = 0x3; // highest priority
	DCH2SSA = VirtToPhys(screen_zero); // transfer source physical address
	//DCH2DSA = VirtToPhys(((unsigned char*)&PORTH + 1)); // transfer destination physical address
	DCH2DSA = VirtToPhys(&PORTH); // transfer destination physical address
	DCH2SSIZ = 2; // source size
	DCH2DSIZ = 2; // dst size 
	DCH2CSIZ = 2; // 2 byte per event
	
	  
	audio_switch = 0;
	
	unsigned short *av_screen_buffer = (unsigned short *)screen_buffer;
	
	for (unsigned short i=0; i<HI_SCREEN_Y; i++)
	{
		for (unsigned short j=0; j<HI_SCREEN_X; j++)
		{
			av_screen_buffer[i*HI_SCREEN_X+j] = 0x0000;
		}
	}
	
	while (1)
	{
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
		unsigned char value1 = 0x00;
		unsigned char value2 = 0x00;
		unsigned short color = 0x0000;

		unsigned long frames = 0;
		unsigned char loop = 1;
		
		unsigned char temp_value = 0x00;

		unsigned long count = 0;

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
		for (unsigned int i=0; i<191; i++) sdcard_receivebyte(); // ???, 49631 works with color
		 
		while (loop > 0) // total frames in the video
		{
			frames++;
			
			x = 0;
			y = 0;

			for (unsigned int i=0; i<192; i++)
			{
				if (frames != 1)
				{
					sdcard_receivebyte(); // data packet begins with 0xFE
				}

				for (unsigned int j=0; j<16; j++)
				{
					for (unsigned int k=0; k<16; k++)
					{
						// get value from SDcard
						value1 = sdcard_receivebyte();
						
						// get value from SDcard
						value2 = sdcard_receivebyte();

						if (k == 15) // audio
						{
							PORTJ = (unsigned short)(((unsigned short)(value2) + 0x80) << 8) + (unsigned short)((value1) + 0x80);
						}
						else // comment the 'else' out to show the audio
						{		
							color = ((unsigned short)(value2) << 8) + (unsigned short)(value1);
							
							av_screen_buffer[y*HI_SCREEN_X+x] = color;
							av_screen_buffer[y*HI_SCREEN_X+x+1] = color;
							av_screen_buffer[(y+1)*HI_SCREEN_X+x] = color;
							av_screen_buffer[(y+1)*HI_SCREEN_X+x+1] = color;

							x += 2;
						}

						// fine tune delay here
						count = (unsigned long)(8);
						_CP0_SET_COUNT(0);
						while (count > _CP0_GET_COUNT());
					}
				}
				
				y += 2;
				x = 0;

				sdcard_receivebyte();
				sdcard_receivebyte(); // data packet ends with 0x55 then 0xAA

				sdcard_receivebyte(); // ???
			}

			if (color != 0x0000) loop = 0; // if last pixel wasn't black, end of loop
		}
	}
}


