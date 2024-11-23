
unsigned int NVMUnlock(unsigned int nvmop)
{
	// Suspend or Disable all Interrupts
	asm("di");
	// Enable Flash Write/Erase Operations and Select
	// Flash operation to perform
	NVMCON = nvmop;
	// Write Keys
	NVMKEY = 0xAA996655;
	NVMKEY = 0x556699AA;
	// Start the operation using the Set Register
	NVMCONSET = 0x8000;
	// Wait for operation to complete
	while (NVMCON & 0x8000);
	// Restore Interrupts
	asm("ei");
	// Disable NVM write enable
	NVMCONCLR = 0x0004000;
	// Return WRERR and LVDERR Error Status Bits
	return (NVMCON & 0x3000);
}

unsigned int NVMErasePage(unsigned long address)
{
	unsigned int res;
	// Load address to program into NVMADDR register
	NVMADDR = (unsigned long) address;
	// Unlock and Erase Page
	res = NVMUnlock (0x4004);
	// Return Result
	return res;
}

unsigned int NVMEraseAll()
{
	unsigned int res;
	// Unlock and Erase All
	res = NVMUnlock (0x4007);
	// Return Result
	return res;
}

unsigned int NVMWriteSingleWord(unsigned long address, unsigned long data0)
{	
	unsigned int res;
	// Load data into NVMDATA register
	NVMDATA0 = data0;
	// Load address to program into NVMADDR register
	NVMADDR = (unsigned long) address;
	// Unlock and Write Single Word
	res = NVMUnlock (0x4001); // ECC must be turned off!
	// Return Result
	return res;
}

unsigned int NVMWriteQuadWord(unsigned long address, 
	unsigned long data0, unsigned long data1, unsigned long data2, unsigned long data3)
{
	unsigned int res;
	// Load data into NVMDATA register
	NVMDATA0 = data0;
	NVMDATA1 = data1;
	NVMDATA2 = data2;
	NVMDATA3 = data3;
	// Load address to program into NVMADDR register
	NVMADDR = (unsigned long) address;
	// Unlock and Write Quad Word
	res = NVMUnlock (0x4002);
	// Return Result
	return res;
}

#ifdef USER_SPACE

void ReprogramCode()
{	
	// #The SDcard must have been formatted
	// #Check which drive it is, here /dev/sdc
	// sudo fdisk -l
	// sudo umount /dev/sdc
	// sudo mkfs.vfat /dev/sdc
	
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	
	// Wait for the disk to initialise
    while(disk_initialize(0));
    // Mount the disk
    f_mount(&fso, "", 0);
    // Change dir to the root directory
    f_chdir("/");
    // Open the directory
    f_opendir(&dir, ".");
	
	char buffer[1];
	char line[256];
	unsigned int pos = 0;
	unsigned int bytes = 1;
	unsigned int result = 0;
	
	unsigned long high_address = 0x0;
	unsigned long low_address = 0x0;
	unsigned char single = 0x0;
	unsigned long word[4] = { 0x0, 0x0, 0x0, 0x0 };
	unsigned char num = 0;
	
	for (unsigned int i=0; i<256; i++) line[i] = '0';
	pos = 0;
	
	result = f_open(&file, "/CODE.HEX", FA_READ);
	
	SendString("\n\r\\");
	
	if (result == 0)
	{
		SendString("Found CODE.HEX, reprogramming now\n\r\\");
		
		//NVMEraseAll();
		
		for (unsigned long i=REPROGRAM_BEGIN; i<REPROGRAM_END; i+=0x00001000) // ???
		{
			NVMErasePage(i);
		}
		
		buffer[0] = 0;
		
		while (bytes > 0 && buffer[0] != ':')
		{
			while (f_read(&file, buffer, 1, &bytes) == 1) { } // colon
		}
		
		while (bytes > 0)
		{
			while (f_read(&file, buffer, 1, &bytes) == 1) { }
			
			if (bytes > 0)
			{
				if (buffer[0] == ':')
				{	
					low_address = 0x0000;
					
					for (unsigned int i=0; i<4; i++)
					{
						low_address = (low_address << 4);
						
						if (line[i+2] >= 'a') low_address += (unsigned int)(line[i+2] - 'a' + 10);
						else low_address += (unsigned int)(line[i+2] - '0');
					}
					
					if (line[6] == '0' && line[7] == '0') // data
					{	
						for (unsigned char i=0; i<4; i++) word[i] = 0x00000000;
						
						if (line[0] == '1' && line[1] == '0') num = 4;
						else if (line[0] == '0' && line[1] == 'C') num = 3;
						else if (line[0] == '0' && line[1] == '8') num = 2;
						else if (line[0] == '0' && line[1] == '4') num = 1;
						
						for (unsigned char i=0; i<num; i++)
						{
							for (unsigned char j=0; j<8; j+=2)
							{
								word[i] = (word[i] >> 8);
								
								single = 0x00;
								
								if (line[i*8+j+8] >= 'a') single += (unsigned long)(line[i*8+j+8] - 'a' + 10) * 0x10;
								else single += (unsigned long)(line[i*8+j+8] - '0') * 0x10;
								
								if (line[i*8+j+1+8] >= 'a') single += (unsigned long)(line[i*8+j+1+8] - 'a' + 10) * 0x01;
								else single += (unsigned long)(line[i*8+j+1+8] - '0') * 0x01;
								
								word[i] += single * 0x01000000;
							}
						}
						
						if (high_address >= (REPROGRAM_BEGIN >> 16) && high_address < (REPROGRAM_END >> 16)) // range where it will re-program flash
						{
							SendChar('.');

							//NVMWriteQuadWord((unsigned long)((high_address << 16) + low_address), 
							//	word[0], word[1], word[2], word[3]);
							
							for (int i=0; i<num; i++)
							{
								NVMWriteSingleWord((unsigned long)((high_address << 16) + low_address), word[i]);
								
								low_address += 4;
							}
						}
					}
					else if (line[6] == '0' && line[7] == '1') // eof
					{
						
					}
					else if (line[6] == '0' && line[7] == '4') // high address
					{	
						high_address = 0x0000;
						
						for (unsigned int i=0; i<4; i++)
						{
							high_address = (high_address << 4);
							
							if (line[i+8] >= 'a') high_address += (unsigned int)(line[i+8] - 'a' + 10);
							else high_address += (unsigned int)(line[i+8] - '0');
						}
					}
					else
					{
				
					}
					
					for (unsigned int i=0; i<256; i++) line[i] = '0';
					pos = 0;
				}
				else
				{
					line[pos] = buffer[0];
					pos++;
				}
			}
		}

		while (f_close(&file) == 1) { }
		
		SendString("\r\n\\");
		
		SendString("Please reset computer now\r\n\\");
	}
	else
	{
		SendString("Could not find CODE.HEX\r\n\\");
	}
	
	DelayMS(1000);
	
	// soft reset system
	SYSKEY = 0x0; // reset
	SYSKEY = 0xAA996655; // unlock key #1
	SYSKEY = 0x556699AA; // unlock key #2
	RSWRST = 1; // set bit to reset of system
	SYSKEY = 0x0; // re-lock
	RSWRST; // read from register to reset
	while (1) { } // wait until reset occurs
};

#else

void ReprogramCode()
{
	while (1) { }
};

#endif