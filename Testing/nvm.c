

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

unsigned int NVMBurnROM(char *filename)
{
	for (unsigned long i=0x1D100000; i<0x1D200000; i+=0x00001000) // pages are 0x1000
	{
		NVMErasePage(i);
	}
	
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
 
	unsigned int bytes;
	unsigned char buffer[4][1];
	unsigned long word[4];
	unsigned int result;
	unsigned long addr;
	
	result = f_open(&file, filename, FA_READ);
	if (result == 0)
	{
		for (addr=0; addr<0x00100000; addr+=16) // up to 1MB
		{		
			for (unsigned int pos=0; pos<4; pos++)
			{
				while (f_read(&file, &buffer[0], 1, &bytes) != 0) { } // MUST READ ONE BYTE AT A TIME!!!
				while (f_read(&file, &buffer[1], 1, &bytes) != 0) { } // MUST READ ONE BYTE AT A TIME!!!
				while (f_read(&file, &buffer[2], 1, &bytes) != 0) { } // MUST READ ONE BYTE AT A TIME!!!
				while (f_read(&file, &buffer[3], 1, &bytes) != 0) { } // MUST READ ONE BYTE AT A TIME!!!

				word[pos] = (buffer[3][0] << 24) + (buffer[2][0] << 16) + (buffer[1][0] << 8) + (buffer[0][0]);
			}
			
			if (bytes > 0) 
			{
				NVMWriteQuadWord(addr+0x1D100000, word[0], word[1], word[2], word[3]);
			}
			else break;	
		}

		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }
		
		SendString("Finished rom at \\");
		SendLongHex(addr);
		SendString("\r\n\\");
	}
	else
	{
		SendString("Could not find rom file\n\r\\");
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
	
	return 1;
}
