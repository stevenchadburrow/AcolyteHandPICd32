/**
 * MIT License
 * Copyright (c) 2018-2023 Mahyar Koshkouei
 *
 * An example of using the peanut_gb.h library. This example application uses
 * SDL2 to draw the screen and get input.
 */

#include <stdint.h>
#include <stdlib.h>

#include "minigb_apu.h"
#include "minigb_apu.c"

#include "peanut_gb.h"


struct priv_t
{
	uint8_t dummy;
};


uint8_t frame_count = 0;

char *cart_rom = (char *)0x9D100000;

volatile uint8_t __attribute__((coherent,address(0x80078000))) cart_ram[32768];
uint8_t __attribute__((coherent)) boot_rom[256];
uint8_t selected_palette[3][4];
uint8_t custom_palette[3][4];

uint8_t cart_bank = 0; // up to 4 banks (for now)

volatile unsigned char sound_toggle = 1; // sound on by default
volatile unsigned char screen_toggle = 2; // screen large by default
volatile unsigned char frame_toggle = 1; // frame half by default
volatile unsigned char skip_toggle = 1; // skipping off by default

char game_name[8];


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
	for (unsigned long i=0x1D100000; i<0x1D200000; i+=0x00001000) // not sure if pages are 0x1000 or 0x2000
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

// Returns a byte from the ROM file at the given address.
uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr)
{
	return cart_rom[addr];
}

// Returns a byte from the cartridge RAM at the given address.
uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr)
{
	return cart_ram[addr];
}

// Writes a given byte to the cartridge RAM at the given address.
void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr,
		       const uint8_t val)
{
	cart_ram[addr] = val;
}

uint8_t gb_boot_rom_read(struct gb_s *gb, const uint_fast16_t addr)
{
	return boot_rom[addr];
}

void read_cart_ram_file(char save_file_name[16], uint8_t *dest,
			const size_t len)
{
	display_string(8, 464, "Reading Cart RAM from File\\");
	
	char title_file_name[16];
	
	for (int i=0; i<16; i++)
	{
		if (save_file_name[i] <= 0x20 || save_file_name[i] >= 0x7F)
		{
			save_file_name[i] = 0;
		}
		
		title_file_name[i] = save_file_name[i];
	}
	
	title_file_name[8] = 'P';
	title_file_name[9] = 'A';
	title_file_name[10] = 'L';
	
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	
	char buffer[1];
	unsigned int bytes;
	unsigned int result;
	
	if(len != 0)
	{
		// Wait for the disk to initialise
		while(disk_initialize(0));
		// Mount the disk
		f_mount(&fso, "", 0);
		// Change dir to the root directory
		f_chdir("/");
		// Open the directory
		f_opendir(&dir, ".");

		result = f_open(&file, save_file_name, FA_READ);
		if (result == 0)
		{	
			for (unsigned int i=0; i<0x8000; i++)
			{
				while (f_read(&file, &buffer, 1, &bytes) != 0) { }

				cart_ram[i] = buffer[0];
			}

			while (f_sync(&file) != 0) { }
			while (f_close(&file) != 0) { }

			SendString("Read cart ram from file\n\r\\");
		}
		else
		{
			SendString("Could not read cart ram from file\n\r\\");
		}
	}
	
	// Wait for the disk to initialise
    while(disk_initialize(0));
    // Mount the disk
    f_mount(&fso, "", 0);
    // Change dir to the root directory
    f_chdir("/");
    // Open the directory
    f_opendir(&dir, ".");

	result = f_open(&file, title_file_name, FA_READ);
	if (result == 0)
	{
		for (unsigned int i=0; i<12; i++)
		{
			while (f_read(&file, &buffer, 1, &bytes) != 0) { }

			custom_palette[i/4][i%4] = ((unsigned char)(buffer[0]>='A'?buffer[0]-'A'+10:buffer[0]-'0') << 4);
			
			while (f_read(&file, &buffer, 1, &bytes) != 0) { }

			custom_palette[i/4][i%4] += (unsigned char)(buffer[0]>='A'?buffer[0]-'A'+10:buffer[0]-'0');
		}
		
		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }
		
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<4; j++)
			{
				selected_palette[i][j] = custom_palette[i][j];
			}
		}
		
		SendString("Read cart palette from file\n\r\\");
	}
	else
	{
		SendString("Could not read cart palette from file\n\r\\");
	}
	
	return;
}

void write_cart_ram_file(char save_file_name[16], uint8_t *dest,
			 const size_t len)
{
	display_string(8, 464, "Writing Cart RAM to File\\");
	
	char title_file_name[16];
	
	for (int i=0; i<16; i++)
	{
		if (save_file_name[i] <= 0x20 || save_file_name[i] >= 0x7F)
		{
			save_file_name[i] = 0;
		}
		
		title_file_name[i] = save_file_name[i];
	}
	
	title_file_name[8] = 'N';
	title_file_name[9] = 'A';
	title_file_name[10] = 'M';
	
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
 
	unsigned char buffer[1];
	unsigned int bytes;
	unsigned int result;
	
	result = f_open(&file, save_file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if (result == 0)
	{		
		for (unsigned int i=0; i<0x8000; i++)
		{
			buffer[0] = cart_ram[i];
			
			while (f_write(&file, buffer, 1, &bytes) != 0) { }
		}
		
		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }
		
		SendString("Wrote cart ram to file\n\r\\");
	}
	else
	{
		SendHex(result);
		
		SendString("Could not write cart ram to file\n\r\\");
	}
	
	// Wait for the disk to initialise
    while(disk_initialize(0));
    // Mount the disk
    f_mount(&fso, "", 0);
    // Change dir to the root directory
    f_chdir("/");
    // Open the directory
    f_opendir(&dir, ".");
	
	result = f_open(&file, title_file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if (result == 0)
	{	
		for (unsigned int i=0; i<8; i++)
		{
			buffer[0] = game_name[i];
			
			while (f_write(&file, buffer, 1, &bytes) != 0) { }
		}
		
		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }
		
		SendString("Wrote cart name to file\n\r\\");
	}
	else
	{
		SendHex(result);
		
		SendString("Could not write cart name to file\n\r\\");
	}
	
	title_file_name[8] = 'P';
	title_file_name[9] = 'A';
	title_file_name[10] = 'L';
	
	// Wait for the disk to initialise
    while(disk_initialize(0));
    // Mount the disk
    f_mount(&fso, "", 0);
    // Change dir to the root directory
    f_chdir("/");
    // Open the directory
    f_opendir(&dir, ".");
	
	result = f_open(&file, title_file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if (result == 0)
	{	
		for (unsigned int i=0; i<12; i++)
		{
			buffer[0] = (((custom_palette[i/4][i%4]&0xF0)>>4)>=10?((custom_palette[i/4][i%4]&0xF0)>>4)+'A'-10:((custom_palette[i/4][i%4]&0xF0)>>4)+'0');
			
			while (f_write(&file, buffer, 1, &bytes) != 0) { }
			
			buffer[0] = ((custom_palette[i/4][i%4]&0x0F)>=10?(custom_palette[i/4][i%4]&0x0F)+'A'-10:(custom_palette[i/4][i%4]&0x0F)+'0');
			
			while (f_write(&file, buffer, 1, &bytes) != 0) { }
		}
		
		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }
		
		SendString("Wrote cart palette to file\n\r\\");
	}
	else
	{
		SendHex(result);
		
		SendString("Could not write cart palette to file\n\r\\");
	}
	
	return;
}

/**
 * Handles an error reported by the emulator. The emulator context may be used
 * to better understand why the error given in gb_err was reported.
 */
void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t addr)
{
	const char* gb_err_str[GB_INVALID_MAX] = {
		"UNKNOWN\\",
		"INVALID OPCODE\\",
		"INVALID READ\\",
		"INVALID WRITE\\",
		"HALT FOREVER\\",
		"INVALID_SIZE\\"
	};
	uint8_t instr_byte;

	instr_byte = __gb_read(gb, addr);
	
	unsigned long long_addr = addr;

	if(addr >= 0x4000 && addr < 0x8000)
	{
		long_addr = (uint32_t)addr * (uint32_t)gb->selected_rom_bank;
	}

	SendString("Error \\");
	SendString((char *)gb_err_str[gb_err]);
	SendChar(' ');
	SendLongHex(long_addr);
	SendChar(' ');
	SendHex(instr_byte);
	SendString("\r\n\\");

	while (1) { }
}

/**
 * Automatically assigns a colour palette to the game using a given game
 * checksum.
 * TODO: Not all checksums are programmed in yet because I'm lazy.
 */
void auto_assign_palette(uint8_t game_checksum)
{
	if (game_checksum == 0x00)
	{
		for (int i=0; i<3; i++)
		{
			selected_palette[i][0] = 0xFF;
			selected_palette[i][1] = 0x92;
			selected_palette[i][2] = 0x49;
			selected_palette[i][3] = 0x00;
		}
	}
	else
	{
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<4; j++)
			{
				selected_palette[i][j] = gb_conversion[gb_lookup[game_checksum]][i*4+j];
			}
		}
	}
}


#if ENABLE_LCD
/**
 * Draws scanline into framebuffer.
 */
void lcd_draw_line(struct gb_s *gb, const uint8_t pixels[160],
		   const uint_fast8_t line)
{
	char test = 0;
	
	if (skip_toggle == 1)
	{
		if (frame_toggle == 0) test = 1;
		else if (frame_toggle == 1)
		{
			if (frame_count == 0 && line >= 0 && line < 72) test = 1;
			else if (frame_count == 1 && line >= 72 && line < 144) test = 1;
		}
		else if (frame_toggle == 2)
		{
			if (frame_count == 0 && line >= 0 && line < 36) test = 1;
			else if (frame_count == 1 && line >= 36 && line < 72) test = 1;
			else if (frame_count == 2 && line >= 72 && line < 108) test = 1;
			else if (frame_count == 3 && line >= 108 && line < 144) test = 1;
		}
	}
	else
	{
		if (frame_count == 0) test = 1;
	}
	
	if (test == 1)
	{
		if (screen_toggle == 0)
		{
			for(unsigned int x = 0; x < LCD_WIDTH; x++)
			{
				screen_buffer[((line)+168) * SCREEN_X + x + 248] = 
						selected_palette[(pixels[(x)] & LCD_PALETTE_ALL) >> 4]
							[pixels[(x)] & 3];
			}
		}
		else if (screen_toggle == 1)
		{
			for(unsigned int x = 0; x < LCD_WIDTH*2; x++)
			{
				screen_buffer[((line<<1)+80) * SCREEN_X + x + 160] = 
						selected_palette[(pixels[(x>>1)] & LCD_PALETTE_ALL) >> 4]
							[pixels[(x>>1)] & 3];
				screen_buffer[((line<<1)+81) * SCREEN_X + x + 160] = 
						selected_palette[(pixels[(x>>1)] & LCD_PALETTE_ALL) >> 4]
							[pixels[(x>>1)] & 3];
			}
		}
		else if (screen_toggle == 2)
		{
			for(unsigned int x = 0; x < LCD_WIDTH; x++)
			{
				for (unsigned int p = 0; p < 3; p++)
				{
					screen_buffer[((line*3) + 16) * SCREEN_X + x * 3 + 80 + p] = 
							selected_palette[(pixels[(x)] & LCD_PALETTE_ALL) >> 4]
								[pixels[(x)] & 3];
					screen_buffer[((line*3) + 17) * SCREEN_X + x * 3 + 80 + p] = 
							selected_palette[(pixels[(x)] & LCD_PALETTE_ALL) >> 4]
								[pixels[(x)] & 3];
					screen_buffer[((line*3) + 18) * SCREEN_X + x * 3 + 80 + p] = 
							selected_palette[(pixels[(x)] & LCD_PALETTE_ALL) >> 4]
								[pixels[(x)] & 3];
				}
			}
		}
	}
}
#endif


int BurnROM()
{
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	FILINFO fno;

	// Wait for the disk to initialise
	while(disk_initialize(0));
	// Mount the disk
	f_mount(&fso, "", 0);
	// Change dir to the root directory
	f_chdir("/");
	// Open the directory
	f_opendir(&dir, ".");
	
	menu_x = 484;
	menu_y = 16;
	menu_pos = 0;
	menu_max = 51;
	
	int total = 0;
	char filename[50][13];
	char gamename[50][17];
	unsigned char choice = 0;
	
	char buffer[1];
	unsigned int bytes;
	unsigned int result;
	
	display_string(menu_x + 8, menu_y, "*** Select Game \\");
	menu_y += 8;
	
	while (1)
	{
		for (int i=0; i<12; i++)
		{
			fno.fname[i] = 0;
			filename[total][i] = 0;
		}
		
		f_readdir(&dir, &fno);
		
		if (fno.fname[0] == 0) break;
		else
		{
			for (int i=0; i<12; i++)
			{
				if (fno.fname[i] > 0x20 && fno.fname[i] < 0x80)
				{
					filename[total][i] = fno.fname[i];
				}
				else
				{
					filename[total][i] = ' ';
					fno.fname[i] = ' ';
				}
			}
			
			// look for a .gb here, then add to total, otherwise do not add to total
			
			result = 0;
			
			for (int i=0; i<12; i++)
			{
				if (filename[total][i] == '.')
				{
					if (filename[total][i+1] == 'G')
					{
						if (filename[total][i+2] == 'B')
						{
							result = 1;
							
							break;
						}
					}
				}
			}
			
			if (result == 1)
			{
				total++;
			}
			
			if (total >= 50) break;
		}
	}
	
	for (int i=0; i<total; i++)
	{
		result = f_open(&file, filename[i], FA_READ);
		if (result == 0)
		{	
			for (unsigned int j=0; j<0x0134; j++)
			{
				while (f_read(&file, &buffer, 1, &bytes) != 0) { }
			}
			
			for (unsigned int j=0; j<16; j++)
			{
				while (f_read(&file, &buffer, 1, &bytes) != 0) { }
				
				if (buffer[0] >= 0x20 && buffer[0] < 0x7F)
				{
					gamename[i][j] = buffer[0];
				}
				else
				{
					gamename[i][j] = ' ';
				}
			}
				
			while (f_sync(&file) != 0) { }
			while (f_close(&file) != 0) { }
			
			gamename[i][16] = '\\';
			
			display_string(menu_x + 8, menu_y, gamename[i]);
			menu_y += 8;
		}
		else
		{
			display_string(menu_x + 8, menu_y, "                \\");
			menu_y += 8;
		}
	}
	
	for (int i=total; i<50; i++)
	{	
		display_string(menu_x + 8, menu_y, "                \\");
		menu_y += 8;
	}
	
	f_rewinddir(&dir);
	
	menu_y = 16;
	
	for (int i=0; i<51; i++)
	{
		display_character(menu_x, menu_y + i * 8, ' ');
	}

	choice = 0;
	
	while (choice == 0)
	{
		choice = (unsigned char)Menu();

		if (choice >= 1)
		{
			choice -= 1;
			
			if (choice < total)
			{
				char rom_name[13];

				for (int i=0; i<13; i++) rom_name[i] = 0;

				rom_name[0] = '/';

				for (int i=0; i<12; i++)
				{
					if (filename[choice][i] > 0x20 && filename[choice][i] < 0x7F)
					{
						rom_name[i+1] = filename[choice][i];
					}
					else break;
				}

				// set background
				for (unsigned int y=0; y<SCREEN_Y; y++)
				{
					for (unsigned int x=0; x<SCREEN_X; x++)
					{
						screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
					}
				}

				NVMBurnROM(rom_name);

				break;
			}
			
			choice = 0; // back to loop
			menu_pos = 0;
		}
	}
	
	return 1;
}

int PeanutGB()
{
	// set background
	for (unsigned int y=0; y<SCREEN_Y; y++)
	{
		for (unsigned int x=0; x<SCREEN_X; x++)
		{
			screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
		}
	}
	
	struct gb_s gb;
	struct priv_t priv;

	enum gb_init_error_e gb_ret;
	int ret = EXIT_SUCCESS;
	
	char save_file_name[13];
	
	for (int i=0; i<13; i++) save_file_name[i] = 0;
	
	save_file_name[0] = '/';
	save_file_name[1] = 'G';
	save_file_name[2] = 'A';
	save_file_name[3] = 'M';
	save_file_name[4] = 'E';
	save_file_name[5] = '-';
	save_file_name[6] = 'A'; // this to change on 'cart_bank'
	save_file_name[7] = '.';
	save_file_name[8] = 'S';
	save_file_name[9] = 'A';
	save_file_name[10] = 'V';
	
	
	int pos = 0;
	
	for (int i=0; i<8; i++) game_name[i] = 0;
	
	for (int i=0; i<16; i++)
	{
		if (cart_rom[0x0134+i] > 0x40 && cart_rom[0x0134+i] < 0x5B) // only the first 8 capital letters
		{
			game_name[pos] = cart_rom[0x0134+i];
			pos++;
			
			if (pos >= 9) break;
		}
	}
	
	for (int i=pos; i<8; i++)
	{
		game_name[i] = ' ';
	}
	
	
	/* TODO: Sanity check input GB file. */

	/* Initialise emulator context. */
	gb_ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write,
			 &gb_error, &priv);

	switch(gb_ret)
	{
	case GB_INIT_NO_ERROR:
		break;

	case GB_INIT_CARTRIDGE_UNSUPPORTED:
		SendString("Unsupported cartridge.\r\n\\");
		ret = EXIT_FAILURE;
		while (1) { }

	case GB_INIT_INVALID_CHECKSUM:
		SendString("Invalid ROM: Checksum failure.\r\n\\");
		ret = EXIT_FAILURE;
		while (1) { }

	default:
		SendString("Unknown error \\");
		SendHex(gb_ret);
		SendString("\r\n\\");
		ret = EXIT_FAILURE;
		while (1) { }
	}
	
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	//FILINFO fno;

	// Wait for the disk to initialise
	while(disk_initialize(0));
	// Mount the disk
	f_mount(&fso, "", 0);
	// Change dir to the root directory
	f_chdir("/");
	// Open the directory
	f_opendir(&dir, ".");
	
	unsigned int bytes;
	unsigned char buffer[1];
	unsigned int result;
	
	unsigned char choice = 0;
	unsigned char ps2_found = 0;

	result = f_open(&file, "/DMG-BOOT.BIN", FA_READ);
	if (result == 0)
	{	
		for (unsigned int i=0; i<256; i++)
		{
			while (f_read(&file, &buffer, 1, &bytes) != 0) { }

			boot_rom[i] = buffer[0];
		}
		
		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }

		gb_set_boot_rom(&gb, gb_boot_rom_read);
		gb_reset(&gb);
	}
	else
	{
		SendString("Could not find DMG-BOOT.BIN file\n\r\\");
	}

	/* Set the RTC of the game cartridge. Only used by games that support it. */
	{
		time_t rawtime;
		time(&rawtime);
#ifdef _POSIX_C_SOURCE
		struct tm timeinfo;
		localtime_r(&rawtime, &timeinfo);
#else
		struct tm *timeinfo;
		timeinfo = localtime(&rawtime);
#endif

		/* You could potentially force the game to allow the player to
		 * reset the time by setting the RTC to invalid values.
		 *
		 * Using memset(&gb->cart_rtc, 0xFF, sizeof(gb->cart_rtc)) for
		 * example causes Pokemon Gold/Silver to say "TIME NOT SET",
		 * allowing the player to set the time without having some dumb
		 * password.
		 *
		 * The memset has to be done directly to gb->cart_rtc because
		 * gb_set_rtc() processes the input values, which may cause
		 * games to not detect invalid values.
		 */

		/* Set RTC. Only games that specify support for RTC will use
		 * these values. */
#ifdef _POSIX_C_SOURCE
		gb_set_rtc(&gb, &timeinfo);
#else
		gb_set_rtc(&gb, timeinfo);
#endif
	}

#if ENABLE_LCD
	gb_init_lcd(&gb, &lcd_draw_line);
#endif
	
	for (int i=0; i<3; i++)
	{
		custom_palette[i][0] = 0xFF;
		custom_palette[i][1] = 0x92;
		custom_palette[i][2] = 0x49;
		custom_palette[i][3] = 0x00;
	}

	auto_assign_palette(gb_colour_hash(&gb)); // default
	
	while (PORTJbits.RJ11 == 0) { }
	
	DelayMS(1000);
	
	// set background
	for (unsigned int y=0; y<SCREEN_Y; y++)
	{
		for (unsigned int x=0; x<SCREEN_X; x++)
		{
			screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
		}
	}
	
	for (unsigned int i=0; i<2; i++)
	{
		for (unsigned int j=0; j<8192; j++) audio_buffer[i][j] = 0x00;
	}
	
	while (1)
	{
		USBHostTasks();
		
		frame_position = 0; // clear frame position
		
		if (usb_mode == 0x00) // keyboard
		{
			while (usb_readpos != usb_writepos)
			{
				if (usb_state_array[usb_readpos] == 0x0B) // release
				{
					usb_readpos++;

					if (usb_state_array[usb_readpos] == 0x11) gb.direct.joypad |= JOYPAD_UP;
					else if (usb_state_array[usb_readpos] == 0x12) gb.direct.joypad |= JOYPAD_DOWN;
					else if (usb_state_array[usb_readpos] == 0x13) gb.direct.joypad |= JOYPAD_LEFT;
					else if (usb_state_array[usb_readpos] == 0x14) gb.direct.joypad |= JOYPAD_RIGHT;
					else if (usb_state_array[usb_readpos] == 0x0D) gb.direct.joypad |= JOYPAD_START;
					else if (usb_state_array[usb_readpos] == 0x20) gb.direct.joypad |= JOYPAD_SELECT;
					else if (usb_state_array[usb_readpos] == 0x58 ||
						usb_state_array[usb_readpos] == 0x78) gb.direct.joypad |= JOYPAD_A;
					else if (usb_state_array[usb_readpos] == 0x5A ||
						usb_state_array[usb_readpos] == 0x7A) gb.direct.joypad |= JOYPAD_B;

					usb_readpos++;
				}
				else
				{
					if (usb_state_array[usb_readpos] == 0x11) gb.direct.joypad &= ~JOYPAD_UP;
					else if (usb_state_array[usb_readpos] == 0x12) gb.direct.joypad &= ~JOYPAD_DOWN;
					else if (usb_state_array[usb_readpos] == 0x13) gb.direct.joypad &= ~JOYPAD_LEFT;
					else if (usb_state_array[usb_readpos] == 0x14) gb.direct.joypad &= ~JOYPAD_RIGHT;
					else if (usb_state_array[usb_readpos] == 0x0D) gb.direct.joypad &= ~JOYPAD_START;
					else if (usb_state_array[usb_readpos] == 0x20) gb.direct.joypad &= ~JOYPAD_SELECT;
					else if (usb_state_array[usb_readpos] == 0x58 ||
						usb_state_array[usb_readpos] == 0x78) gb.direct.joypad &= ~JOYPAD_A;
					else if (usb_state_array[usb_readpos] == 0x5A ||
						usb_state_array[usb_readpos] == 0x7A) gb.direct.joypad &= ~JOYPAD_B;

					usb_readpos++;
				}
			}
		}
		else if (usb_mode == 0x02) // xbox 360 controller
		{
			if (usb_readpos != usb_writepos)
			{
				gb.direct.joypad |= JOYPAD_UP;
				gb.direct.joypad |= JOYPAD_DOWN;
				gb.direct.joypad |= JOYPAD_LEFT;
				gb.direct.joypad |= JOYPAD_RIGHT;
				gb.direct.joypad |= JOYPAD_A;
				gb.direct.joypad |= JOYPAD_SELECT;
				gb.direct.joypad |= JOYPAD_B;
				gb.direct.joypad |= JOYPAD_START;

				while (usb_readpos != usb_writepos)
				{
					if ((usb_buttons[usb_readpos] & 0x0100) == 0x0100) gb.direct.joypad &= ~JOYPAD_UP;
					if ((usb_buttons[usb_readpos] & 0x0200) == 0x0200) gb.direct.joypad &= ~JOYPAD_DOWN;
					if ((usb_buttons[usb_readpos] & 0x0400) == 0x0400) gb.direct.joypad &= ~JOYPAD_LEFT;
					if ((usb_buttons[usb_readpos] & 0x0800) == 0x0800) gb.direct.joypad &= ~JOYPAD_RIGHT;
					if ((usb_buttons[usb_readpos] & 0x0010) == 0x0010) gb.direct.joypad &= ~JOYPAD_B;
					if ((usb_buttons[usb_readpos] & 0x0020) == 0x0020) gb.direct.joypad &= ~JOYPAD_A;
					if ((usb_buttons[usb_readpos] & 0x0040) == 0x0040) gb.direct.joypad &= ~JOYPAD_START;
					if ((usb_buttons[usb_readpos] & 0x0080) == 0x0080) gb.direct.joypad &= ~JOYPAD_SELECT;

					usb_readpos++;
				}
			}
		}
		else
		{	
			ps2_found = 0;

			for (unsigned char p=0; p<2; p++)
			{
				if (ps2_ready[p] == 0x01 && ps2_mode[p] == 0x00) // ready and keyboard
				{
					ps2_found = 1;
				}
			}

			if (ps2_found == 1) // ps2 keyboards
			{
				for (unsigned char p=0; p<2; p++)
				{
					if (ps2_ready[p] == 0x01 && ps2_mode[p] == 0x00) // ready and keyboard
					{
						while (ps2_readpos[p] != ps2_writepos[p])
						{
							if (ps2_state_array[p][ps2_readpos[p]] == 0x0B) // release
							{
								ps2_readpos[p]++;

								if (ps2_state_array[p][ps2_readpos[p]] == 0x11) gb.direct.joypad |= JOYPAD_UP;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x12) gb.direct.joypad |= JOYPAD_DOWN;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x13) gb.direct.joypad |= JOYPAD_LEFT;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x14) gb.direct.joypad |= JOYPAD_RIGHT;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x0D) gb.direct.joypad |= JOYPAD_START;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x20) gb.direct.joypad |= JOYPAD_SELECT;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x58 ||
									ps2_state_array[p][ps2_readpos[p]] == 0x78) gb.direct.joypad |= JOYPAD_A;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x5A ||
									ps2_state_array[p][ps2_readpos[p]] == 0x7A) gb.direct.joypad |= JOYPAD_B;

								ps2_readpos[p]++;
							}
							else
							{
								if (ps2_state_array[p][ps2_readpos[p]] == 0x11) gb.direct.joypad &= ~JOYPAD_UP;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x12) gb.direct.joypad &= ~JOYPAD_DOWN;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x13) gb.direct.joypad &= ~JOYPAD_LEFT;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x14) gb.direct.joypad &= ~JOYPAD_RIGHT;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x0D) gb.direct.joypad &= ~JOYPAD_START;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x20) gb.direct.joypad &= ~JOYPAD_SELECT;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x58 ||
									ps2_state_array[p][ps2_readpos[p]] == 0x78) gb.direct.joypad &= ~JOYPAD_A;
								else if (ps2_state_array[p][ps2_readpos[p]] == 0x5A ||
									ps2_state_array[p][ps2_readpos[p]] == 0x7A) gb.direct.joypad &= ~JOYPAD_B;

								ps2_readpos[p]++;
							}
						}
					}
				}
			}
			else // genesis joysticks
			{
				if (frame_count % 2 == 0)
				{
					gb.direct.joypad |= JOYPAD_UP;
					gb.direct.joypad |= JOYPAD_DOWN;
					gb.direct.joypad |= JOYPAD_LEFT;
					gb.direct.joypad |= JOYPAD_RIGHT;
					gb.direct.joypad |= JOYPAD_A;
					gb.direct.joypad |= JOYPAD_SELECT;

					if (PORTJbits.RJ0 == 0) gb.direct.joypad &= ~JOYPAD_UP;
					if (PORTJbits.RJ1 == 0) gb.direct.joypad &= ~JOYPAD_DOWN;
					if (PORTJbits.RJ2 == 0) gb.direct.joypad &= ~JOYPAD_LEFT;
					if (PORTJbits.RJ3 == 0) gb.direct.joypad &= ~JOYPAD_RIGHT;
					if (PORTJbits.RJ4 == 0) gb.direct.joypad &= ~JOYPAD_A;
					if (PORTJbits.RJ5 == 0) gb.direct.joypad &= ~JOYPAD_SELECT;

					if (PORTJbits.RJ6 == 0) gb.direct.joypad &= ~JOYPAD_UP;
					if (PORTJbits.RJ7 == 0) gb.direct.joypad &= ~JOYPAD_DOWN;
					if (PORTJbits.RJ10 == 0) gb.direct.joypad &= ~JOYPAD_LEFT;
					if (PORTJbits.RJ12 == 0) gb.direct.joypad &= ~JOYPAD_RIGHT;
					if (PORTJbits.RJ13 == 0) gb.direct.joypad &= ~JOYPAD_A;
					if (PORTJbits.RJ14 == 0) gb.direct.joypad &= ~JOYPAD_SELECT;

					PORTJbits.RJ15 = 0;
					TRISJbits.TRISJ15 = 0; // ground joy-select for next frame
				}
				else if (frame_count % 2 == 1)
				{
					gb.direct.joypad |= JOYPAD_B;
					gb.direct.joypad |= JOYPAD_START;

					if (PORTJbits.RJ4 == 0) gb.direct.joypad &= ~JOYPAD_B;
					if (PORTJbits.RJ5 == 0) gb.direct.joypad &= ~JOYPAD_START;

					if (PORTJbits.RJ13 == 0) gb.direct.joypad &= ~JOYPAD_B;
					if (PORTJbits.RJ14 == 0) gb.direct.joypad &= ~JOYPAD_START;

					TRISJbits.TRISJ15 = 1; // float joy-select (pulled high) for next frame
				}
			}
		}

		// special button on board saves the game cart ram
		if (PORTJbits.RJ11 == 0)
		{
			while (PORTJbits.RJ11 == 0) { }

			DelayMS(1000);

			for (unsigned int i=0; i<2; i++)
			{
				for (unsigned int j=0; j<8192; j++) audio_buffer[i][j] = 0x00;
			}
			
			for (unsigned int y=0; y<SCREEN_Y; y++)
			{
				for (unsigned int x=0; x<SCREEN_X; x++)
				{
					screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
				}
			}

			menu_x = 256;
			menu_y = 240;
			menu_pos = 0;
			menu_max = 8;

			display_string(menu_x, menu_y,		" Resume        \\");
			display_string(menu_x, menu_y+8,	" Palette Choice\\");
			if (sound_toggle == 0)			display_string(menu_x, menu_y+16, " Sound On      \\");
			else							display_string(menu_x, menu_y+16, " Sound Off     \\");
			if (screen_toggle == 0)			display_string(menu_x, menu_y+24, " Screen Large  \\");
			else if (screen_toggle == 1)	display_string(menu_x, menu_y+24, " Screen Small  \\");
			else							display_string(menu_x, menu_y+24, " Screen Medium \\");
			if (frame_toggle == 0)			display_string(menu_x, menu_y+32, " Frame Half    \\");
			else if (frame_toggle == 1)		display_string(menu_x, menu_y+32, " Frame Quarter \\");
			else							display_string(menu_x, menu_y+32, " Frame Single  \\");
			if (skip_toggle == 1)			display_string(menu_x, menu_y+40, " Skip On       \\");
			else							display_string(menu_x, menu_y+40, " Skip Off      \\");
			display_string(menu_x, menu_y+48,	" Load and Reset\\");
			display_string(menu_x, menu_y+56,	" Overwrite Save\\");

			choice = (unsigned char)Menu();

			if (choice == 0) { } // resume
			else if (choice == 1) // palette choice
			{
				for (unsigned int y=0; y<SCREEN_Y; y++)
				{
					for (unsigned int x=0; x<SCREEN_X; x++)
					{
						screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
					}
				}

				menu_x = 256;
				menu_y = 240;
				menu_pos = 0;
				menu_max = 19;

				display_string(menu_x, menu_y,		" *** Palette Choice?\\");
				display_string(menu_x, menu_y+8,	" Custom             \\");
				display_string(menu_x, menu_y+16,	" GBC Default        \\");
				display_string(menu_x, menu_y+24,	" GB Original        \\");
				display_string(menu_x, menu_y+32,	" GB Pocket          \\");
				display_string(menu_x, menu_y+40,	" GB Light           \\");
				display_string(menu_x, menu_y+48,	" Grayscale          \\");
				display_string(menu_x, menu_y+56,	" Preset 1           \\");
				display_string(menu_x, menu_y+64,	" Preset 2           \\");
				display_string(menu_x, menu_y+72,	" Preset 3           \\");
				display_string(menu_x, menu_y+80,	" Preset 4           \\");
				display_string(menu_x, menu_y+88,	" Preset 5           \\");
				display_string(menu_x, menu_y+96,	" Preset 6           \\");
				display_string(menu_x, menu_y+104,	" Preset 7           \\");
				display_string(menu_x, menu_y+112,	" Preset 8           \\");
				display_string(menu_x, menu_y+120,	" Preset 9           \\");
				display_string(menu_x, menu_y+128,	" Preset 10          \\");
				display_string(menu_x, menu_y+136,	" Preset 11          \\");
				display_string(menu_x, menu_y+144,	" Preset 12          \\");
				
				// display color preview
				for (int p=0; p<12; p++)
				{
					for (int x=0; x<4; x++)
					{
						for (int y=0; y<8; y++)
						{
							screen_buffer[(menu_y+16+y)*SCREEN_X + (menu_x+112 + p*4 + x)] = 
								gb_conversion[gb_lookup[gb_colour_hash(&gb)]][p]; // GBC default
							
							screen_buffer[(menu_y+24+y)*SCREEN_X + (menu_x+112 + p*4 + x)] = 
								gb_preset[0][p]; // GB original
							
							screen_buffer[(menu_y+32+y)*SCREEN_X + (menu_x+112 + p*4 + x)] = 
								gb_preset[1][p]; // GB pocket
							
							screen_buffer[(menu_y+40+y)*SCREEN_X + (menu_x+112 + p*4 + x)] = 
								gb_preset[2][p]; // GB light
							
							screen_buffer[(menu_y+48+y)*SCREEN_X + (menu_x+112 + p*4 + x)] = 
								gb_conversion[0x00][p]; // grayscale
							
							for (int i=0; i<12; i++)
							{
								screen_buffer[(menu_y+56 + i*8 + y)*SCREEN_X + (menu_x+112 + p*4 + x)] = 
									gb_preset[i+3][p]; // preset
							}
						}
					}
				}

				choice = 0;

				while (choice == 0)
				{
					choice = (unsigned char)Menu();
				}

				if (choice == 1) // custom palette
				{
					for (unsigned int y=0; y<SCREEN_Y; y++)
					{
						for (unsigned int x=0; x<SCREEN_X; x++)
						{
							screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
						}
					}
					
					unsigned char select = 0;
					unsigned char value = 0;

					menu_x = 256;
					menu_y = 240;
					menu_pos = 0;
					menu_max = 12;

					display_string(menu_x, menu_y,		" *** Custom Palette?\\");
					display_string(menu_x, menu_y+8,	"                    \\");
					display_string(menu_x, menu_y+16,	" ^                  \\");
					display_string(menu_x, menu_y+24,	" Next               \\");
					display_string(menu_x, menu_y+32,	" + Red              \\");
					display_string(menu_x, menu_y+40,	" - Red              \\");
					display_string(menu_x, menu_y+48,	" + Green            \\");
					display_string(menu_x, menu_y+56,	" - Green            \\");
					display_string(menu_x, menu_y+64,	" + Blue             \\");
					display_string(menu_x, menu_y+72,	" - Blue             \\");
					display_string(menu_x, menu_y+80,	" Reset              \\");
					display_string(menu_x, menu_y+88,	" Exit               \\");
					
					choice = 0;

					while (choice != 11)
					{
						// display color preview
						for (int p=0; p<12; p++)
						{
							for (int x=0; x<8; x++)
							{
								for (int y=0; y<8; y++)
								{
									screen_buffer[(menu_y+8+y)*SCREEN_X + (menu_x+8 + p*8 + x)] = 
										custom_palette[p/4][p%4];
								}
							}
						}
						
						value = custom_palette[select/4][select%4];
						
						// display color values
						display_character(menu_x+120, menu_y+16, (char)(((value & 0xE0) >> 5) + '0'));
						display_character(menu_x+128, menu_y+16, '-');
						display_character(menu_x+136, menu_y+16, (char)(((value & 0x1C) >> 2) + '0'));
						display_character(menu_x+144, menu_y+16, '-');
						display_character(menu_x+152, menu_y+16, (char)(((value & 0x03)) + '0'));
						
						choice = (unsigned char)Menu();
						
						//menu_pos = 0; // junky but works for now
						
						if (choice == 3)
						{
							display_character(menu_x+8+select*8, menu_y+16, ' ');
							
							select++;
							
							if (select >= 12) select = 0;
							
							display_character(menu_x+8+select*8, menu_y+16, '^');
						}
						
						value = custom_palette[select/4][select%4];
						
						if (choice == 4) // add red
						{
							if (((value & 0xE0) >> 5) != 0x07)
							{
								custom_palette[select/4][select%4] = (((((value & 0xE0) >> 5) + 1) & 0x07) << 5) + (value & 0x1F);
							}
						}
						else if (choice == 5) // minus red
						{
							if (((value & 0xE0) >> 5) != 0x00)
							{
								custom_palette[select/4][select%4] = (((((value & 0xE0) >> 5) - 1) & 0x07) << 5) + (value & 0x1F);
							}
						}
						else if (choice == 6) // add green
						{
							if (((value & 0x1C) >> 2) != 0x07)
							{
								custom_palette[select/4][select%4] = (((((value & 0x1C) >> 2) + 1) & 0x07) << 2) + (value & 0xE3);
							}
						}
						else if (choice == 7) // minus green
						{
							if (((value & 0x1C) >> 2) != 0x00)
							{
								custom_palette[select/4][select%4] = (((((value & 0x1C) >> 2) - 1) & 0x07) << 2) + (value & 0xE3);
							}
						}
						else if (choice == 8) // add blue
						{
							if (((value & 0x03)) != 0x03)
							{
								custom_palette[select/4][select%4] = (((value & 0x03) + 1) & 0x03) + (value & 0xFC);
							}
						}
						else if (choice == 9) // minus blue
						{
							if (((value & 0x03)) != 0x00)
							{
								custom_palette[select/4][select%4] = (((value & 0x03) - 1) & 0x03) + (value & 0xFC);
							}
						}
						else if (choice == 10) // reset (to selected palette)
						{
							for (int i=0; i<3; i++)
							{
								for (int j=0; j<4; j++)
								{
									custom_palette[i][j] = selected_palette[i][j];
								}
							}
						}
					}
					
					for (int i=0; i<3; i++)
					{
						for (int j=0; j<4; j++)
						{
							selected_palette[i][j] = custom_palette[i][j];
						}
					}
				}
				else if (choice == 2) auto_assign_palette(gb_colour_hash(&gb));
				else if (choice >= 3 && choice <= 6)
				{
					for (int i=0; i<3; i++)
					{
						for (int j=0; j<4; j++)
						{
							selected_palette[i][j] = gb_preset[choice-3][i*4+j];
						}
					}
				}
				else if (choice == 6) auto_assign_palette(0x00);
				else
				{
					for (int i=0; i<3; i++)
					{
						for (int j=0; j<4; j++)
						{
							selected_palette[i][j] = gb_preset[choice-4][i*4+j];
						}
					}
				}
			}
			else if (choice == 2) // toggle sound
			{
				sound_toggle = 1 - sound_toggle;
			}
			else if (choice == 3) // toggle screen
			{
				if (screen_toggle == 0) screen_toggle = 2;
				else screen_toggle--;
			}
			else if (choice == 4) // toggle frames
			{
				frame_toggle++;
				if (frame_toggle >= 3) frame_toggle = 0;
			}
			else if (choice == 5) // toggle skip
			{
				skip_toggle = 1 - skip_toggle;
			}
			else if (choice == 6) // load and reset
			{
				for (unsigned int y=0; y<SCREEN_Y; y++)
				{
					for (unsigned int x=0; x<SCREEN_X; x++)
					{
						screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
					}
				}

				menu_x = 256;
				menu_y = 240;
				menu_pos = 0;
				menu_max = 6;

				display_string(menu_x, menu_y,		" *** Load and Reset?\\");
				display_string(menu_x, menu_y+8,	" No                 \\");
				display_string(menu_x, menu_y+16,	" File A             \\");
				display_string(menu_x, menu_y+24,	" File B             \\");
				display_string(menu_x, menu_y+32,	" File C             \\");
				display_string(menu_x, menu_y+40,	" File D             \\");

				
				// Wait for the disk to initialise
				while(disk_initialize(0));
				// Mount the disk
				f_mount(&fso, "", 0);
				// Change dir to the root directory
				f_chdir("/");
				// Open the directory
				f_opendir(&dir, ".");
				
				// game names
				result = f_open(&file, "/GAME-A.NAM", FA_READ);
				if (result == 0)
				{	
					for (unsigned int i=0; i<8; i++)
					{
						while (f_read(&file, &buffer, 1, &bytes) != 0) { }

						display_character(menu_x+96+i*8, menu_y+16, buffer[0]);
					}

					while (f_sync(&file) != 0) { }
					while (f_close(&file) != 0) { }
				}
				
				// Wait for the disk to initialise
				while(disk_initialize(0));
				// Mount the disk
				f_mount(&fso, "", 0);
				// Change dir to the root directory
				f_chdir("/");
				// Open the directory
				f_opendir(&dir, ".");
				
				result = f_open(&file, "/GAME-B.NAM", FA_READ);
				if (result == 0)
				{	
					for (unsigned int i=0; i<8; i++)
					{
						while (f_read(&file, &buffer, 1, &bytes) != 0) { }

						display_character(menu_x+96+i*8, menu_y+24, buffer[0]);
					}

					while (f_sync(&file) != 0) { }
					while (f_close(&file) != 0) { }
				}
				
				// Wait for the disk to initialise
				while(disk_initialize(0));
				// Mount the disk
				f_mount(&fso, "", 0);
				// Change dir to the root directory
				f_chdir("/");
				// Open the directory
				f_opendir(&dir, ".");
				
				result = f_open(&file, "/GAME-C.NAM", FA_READ);
				if (result == 0)
				{	
					for (unsigned int i=0; i<8; i++)
					{
						while (f_read(&file, &buffer, 1, &bytes) != 0) { }

						display_character(menu_x+96+i*8, menu_y+32, buffer[0]);
					}

					while (f_sync(&file) != 0) { }
					while (f_close(&file) != 0) { }
				}
				
				// Wait for the disk to initialise
				while(disk_initialize(0));
				// Mount the disk
				f_mount(&fso, "", 0);
				// Change dir to the root directory
				f_chdir("/");
				// Open the directory
				f_opendir(&dir, ".");
				
				result = f_open(&file, "/GAME-D.NAM", FA_READ);
				if (result == 0)
				{	
					for (unsigned int i=0; i<8; i++)
					{
						while (f_read(&file, &buffer, 1, &bytes) != 0) { }

						display_character(menu_x+96+i*8, menu_y+40, buffer[0]);
					}

					while (f_sync(&file) != 0) { }
					while (f_close(&file) != 0) { }
				}
				
				choice = 0;

				while (choice == 0)
				{
					choice = (unsigned char)Menu();
				}

				if (choice == 1) { } // do nothing
				else if (choice >= 2)
				{
					cart_bank = choice - 2;

					save_file_name[6] = (char)('A' + cart_bank);

					/* Load Save File. */
					read_cart_ram_file(save_file_name, (uint8_t *)&cart_ram, gb_get_save_size(&gb));

					DelayMS(1000);

					// reset afterwards
					gb_reset(&gb);
				}
			}
			else if (choice == 7) // save
			{
				for (unsigned int y=0; y<SCREEN_Y; y++)
				{
					for (unsigned int x=0; x<SCREEN_X; x++)
					{
						screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
					}
				}

				menu_x = 256;
				menu_y = 240;
				menu_pos = 0;
				menu_max = 6;

				display_string(menu_x, menu_y,		" *** Overwrite Save?\\");
				display_string(menu_x, menu_y+8,	" No                 \\");
				display_string(menu_x, menu_y+16,	" File A             \\");
				display_string(menu_x, menu_y+24,	" File B             \\");
				display_string(menu_x, menu_y+32,	" File C             \\");
				display_string(menu_x, menu_y+40,	" File D             \\");

				// Wait for the disk to initialise
				while(disk_initialize(0));
				// Mount the disk
				f_mount(&fso, "", 0);
				// Change dir to the root directory
				f_chdir("/");
				// Open the directory
				f_opendir(&dir, ".");
				
				// game names
				result = f_open(&file, "/GAME-A.NAM", FA_READ);
				if (result == 0)
				{	
					for (unsigned int i=0; i<8; i++)
					{
						while (f_read(&file, &buffer, 1, &bytes) != 0) { }

						display_character(menu_x+96+i*8, menu_y+16, buffer[0]);
					}

					while (f_sync(&file) != 0) { }
					while (f_close(&file) != 0) { }
				}
				
				// Wait for the disk to initialise
				while(disk_initialize(0));
				// Mount the disk
				f_mount(&fso, "", 0);
				// Change dir to the root directory
				f_chdir("/");
				// Open the directory
				f_opendir(&dir, ".");
				
				result = f_open(&file, "/GAME-B.NAM", FA_READ);
				if (result == 0)
				{	
					for (unsigned int i=0; i<8; i++)
					{
						while (f_read(&file, &buffer, 1, &bytes) != 0) { }

						display_character(menu_x+96+i*8, menu_y+24, buffer[0]);
					}

					while (f_sync(&file) != 0) { }
					while (f_close(&file) != 0) { }
				}
				
				// Wait for the disk to initialise
				while(disk_initialize(0));
				// Mount the disk
				f_mount(&fso, "", 0);
				// Change dir to the root directory
				f_chdir("/");
				// Open the directory
				f_opendir(&dir, ".");
				
				result = f_open(&file, "/GAME-C.NAM", FA_READ);
				if (result == 0)
				{	
					for (unsigned int i=0; i<8; i++)
					{
						while (f_read(&file, &buffer, 1, &bytes) != 0) { }

						display_character(menu_x+96+i*8, menu_y+32, buffer[0]);
					}

					while (f_sync(&file) != 0) { }
					while (f_close(&file) != 0) { }
				}
				
				// Wait for the disk to initialise
				while(disk_initialize(0));
				// Mount the disk
				f_mount(&fso, "", 0);
				// Change dir to the root directory
				f_chdir("/");
				// Open the directory
				f_opendir(&dir, ".");
				
				result = f_open(&file, "/GAME-D.NAM", FA_READ);
				if (result == 0)
				{	
					for (unsigned int i=0; i<8; i++)
					{
						while (f_read(&file, &buffer, 1, &bytes) != 0) { }

						display_character(menu_x+96+i*8, menu_y+40, buffer[0]);
					}

					while (f_sync(&file) != 0) { }
					while (f_close(&file) != 0) { }
				}
				
				
				choice = 0;

				while (choice == 0)
				{
					choice = (unsigned char)Menu();
				}

				if (choice == 1) { } // do nothing
				else if (choice >= 2)
				{
					cart_bank = choice - 2;

					save_file_name[6] = (char)('A' + cart_bank);

					// Record save file.
					write_cart_ram_file(save_file_name, (uint8_t *)&cart_ram, gb_get_save_size(&gb));

					DelayMS(1000);
				}
			}

			// set background
			for (unsigned int y=0; y<SCREEN_Y; y++)
			{
				for (unsigned int x=0; x<SCREEN_X; x++)
				{
					screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
				}
			}
		}

		frame_count++;
		
		if (frame_toggle == 0 || frame_toggle == 1)
		{
			if (frame_count >= 2) frame_count = 0; // draws particular rows on the screen
		}
		else if (frame_toggle == 2)
		{
			if (frame_count >= 4) frame_count = 0; // draws particular rows on the screen
		}

		/* Execute CPU cycles until the screen has to be redrawn. */
		gb_run_frame(&gb);

#if ENABLE_SOUND
		if (sound_toggle > 0)
		{
			// playing audio
			audio_callback(&gb, (uint8_t *)&audio_buffer[1-audio_bank], AUDIO_NSAMPLES);
		}
#endif
			
		// speed limiter for when occasionally the Gameboy is too fast
		while (frame_position < 805) { } // wait for frame trigger interrupt 
		
#if ENABLE_SOUND
		if (sound_toggle > 0)
		{
			audio_bank = 1 - audio_bank;
			audio_position = 0; // start at beginning
			audio_length = 8192; // could be up to 8192 if full length
			audio_switch = 1; // only one run through buffer
		}
#else
		if (sound_toggle > 0) audio_position = 0;
#endif
		
		//PORTEbits.RE8 = 1; // TEMPORARY!!!
	}

	return ret;
}


				