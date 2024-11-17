


// SDcard commands below
// This was used for the Arduino, but has been modified to work here.
volatile unsigned char sdcard_block[512];

void sdcard_longdelay(void)
{
	//DelayMS(1); // arbitrary amount of time to delay, only using 1ms here, should be around 10ms???
	
	// fine tune delay here
	unsigned long count = 0x00000000;
	count = (unsigned long)((unsigned long)((SYS_FREQ / 1000) * 1) / 4); // Convert microseconds us into how many clock ticks it will take
	_CP0_SET_COUNT(0); // Set Core Timer count to 0
	while (count > _CP0_GET_COUNT()); // Wait until Core Timer count reaches the number we calculated earlier
}

void sdcard_sendbyte(unsigned int value)
{
	unsigned int temp_value = value;
	
	temp_value = (temp_value & 0x00FF);
	
	SPI1BUF = temp_value;
	
	while (SPI1STATbits.SPIRBF == 0) { }
	
	temp_value = SPI1BUF; // dummy read
	
};

unsigned int sdcard_receivebyte(void)
{
	unsigned int temp_value = 0x0000;
	
	SPI1BUF = 0xFFFF; // dummy write
	
	while (SPI1STATbits.SPIRBF == 0) { }
	
	temp_value = SPI1BUF;
	
	temp_value = (temp_value & 0x00FF);
	
	return temp_value;
};


void sdcard_pump(void)
{	
	PORTDbits.RD4 = 1; // CS is high, must disable the device
	
	sdcard_longdelay();
	
	for (unsigned int i=0; i<128; i++)
	{
		sdcard_sendbyte(0xFFFF); // MOSI must be high!
	}
};

void sdcard_enable(void)
{
	PORTDbits.RD4 = 0; // CS is low
}

void sdcard_disable(void)
{
	PORTDbits.RD4 = 1; // CS is high
	
	sdcard_sendbyte(0xFFFF);
}

unsigned int sdcard_waitresult(void)
{
	unsigned int temp_value = 0x00FF;

	for (unsigned int i=0; i<65000; i++) // arbitrary wait time
	{
		temp_value = sdcard_receivebyte();

		if (temp_value != 0x00FF)
		{
			return temp_value;
		}
	}

	return 0xFF;
}

unsigned int sdcard_initialize(void)
{
	unsigned int temp_value = 0x0000;	
	
	sdcard_disable();
	sdcard_pump();
	sdcard_longdelay();
	sdcard_enable();
	sdcard_sendbyte(0x0040); // CMD0 = 0x40 + 0x00 (0 in hex)
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0095); // CRC for CMD0
	temp_value = sdcard_waitresult(); // command response
	if (temp_value == 0x00FF) { return 0; }	
	sdcard_disable();
	if (temp_value != 0x0001) { return 0; } // expecting 0x01	
	sdcard_longdelay();
	sdcard_pump();
	sdcard_enable();
	sdcard_sendbyte(0x0048); // CMD8 = 0x40 + 0x08 (8 in hex)
	sdcard_sendbyte(0x0000); // CMD8 needs 0x000001AA argument
	sdcard_sendbyte(0x0000);
	sdcard_sendbyte(0x0001);
	sdcard_sendbyte(0x00AA); 
	sdcard_sendbyte(0x0087); // CRC for CMD8
	temp_value = sdcard_waitresult(); // command response
	if (temp_value == 0x00FF) { return 0; }
	sdcard_disable();
	if (temp_value != 0x0001) { return 0; } // expecting 0x01
	sdcard_enable();
	temp_value = sdcard_receivebyte(); // 32-bit return value, ignore
	temp_value = sdcard_receivebyte();
	temp_value = sdcard_receivebyte();
	temp_value = sdcard_receivebyte();
	sdcard_disable();
	do {
		sdcard_pump();
		sdcard_longdelay();
		sdcard_enable();
		sdcard_sendbyte(0x0077); // CMD55 = 0x40 + 0x37 (55 in hex)
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0001); // CRC (general)
		temp_value = sdcard_waitresult(); // command response
		if (temp_value == 0x00FF) { return 0; }
		sdcard_disable();
		if (temp_value != 0x0001) { break; } // expecting 0x01, but if not it might already be 'initialized'?
		sdcard_pump();
		sdcard_longdelay();
		sdcard_enable();
		sdcard_sendbyte(0x0069); // CMD41 = 0x40 + 0x29 (41 in hex)
		sdcard_sendbyte(0x0040); // needed for CMD41?
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0000);
		sdcard_sendbyte(0x0001); // CRC (general)
		temp_value = sdcard_waitresult(); // command response
		if (temp_value == 0x00FF) { return 0; }
		sdcard_disable();
		if (temp_value != 0x0000 && temp_value != 0x0001) { return 0; } // expecting 0x00, if 0x01 try again
		sdcard_longdelay();
	} while (temp_value == 0x0001);

	return 1;
}

unsigned int sdcard_readblock(unsigned int high, unsigned int low)
{
	unsigned char temp_value = 0x00;

	sdcard_disable();
	sdcard_pump();
	sdcard_longdelay(); // this is probably not needed
	sdcard_enable();
	sdcard_sendbyte(0x51); // CMD17 = 0x40 + 0x11 (17 in hex)
	sdcard_sendbyte((high&0x00FF));
	sdcard_sendbyte(((low&0xFF00) >> 8));
	sdcard_sendbyte((low&0x00FE)); // only blocks of 512 bytes
	sdcard_sendbyte(0x00);
	sdcard_sendbyte(0x01); // CRC (general)
	temp_value = sdcard_waitresult(); // command response
	if (temp_value == 0xFF) { return 0; }
	else if (temp_value != 0x00) { return 0; } // expecting 0x00
	temp_value = sdcard_waitresult(); // data packet starts with 0xFE
	if (temp_value == 0xFF) { return 0; }
	else if (temp_value != 0xFE) { return 0; }
	
	for (unsigned int i=0; i<512; i++)
	{
		sdcard_block[i] = sdcard_receivebyte();
	}
	
	temp_value = sdcard_receivebyte(); // data packet ends with 0x55 then 0xAA
	temp_value = sdcard_receivebyte(); // ignore here
	sdcard_disable();

	return 1;
}

unsigned int sdcard_writeblock(unsigned int high, unsigned int low)
{
	unsigned char temp_value = 0x00;

	sdcard_disable();
	sdcard_pump();
	sdcard_longdelay(); // this is probably not needed
	sdcard_enable();
	sdcard_sendbyte(0x58); // CMD24 = 0x40 + 0x18 (24 in hex)
	sdcard_sendbyte((high&0x00FF));
	sdcard_sendbyte(((low&0xFF00)>>8));
	sdcard_sendbyte((low&0x00FE)); // only blocks of 512 bytes
	sdcard_sendbyte(0x00);
	sdcard_sendbyte(0x01); // CRC (general)
	temp_value = sdcard_waitresult(); // command response
	if (temp_value == 0xFF) { return 0; }
	else if (temp_value != 0x00) { return 0; } // expecting 0x00
	sdcard_sendbyte(0xFE); // data packet starts with 0xFE

	for (unsigned int i=0; i<512; i++)
	{
		sdcard_sendbyte(sdcard_block[i]);
	}
	
	sdcard_sendbyte(0x55); // data packet ends with 0x55 then 0xAA
	sdcard_sendbyte(0xAA);
	temp_value = sdcard_receivebyte(); // toggle clock 8 times
	sdcard_disable();

	return 1;
}



