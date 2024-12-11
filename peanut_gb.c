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


uint8_t draw_frame = 0;

const char *cart_rom = (char *)0x9D100000;
volatile uint8_t __attribute__((coherent,address(0x80078000))) cart_ram[32768];
uint8_t boot_rom[256];
uint8_t selected_palette[3][4];

uint8_t cart_bank = 0; // up to 4 banks (for now)

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


/**
 * Returns a byte from the ROM file at the given address.
 */
uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr)
{
	return cart_rom[addr];
}

/**
 * Returns a byte from the cartridge RAM at the given address.
 */
uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr)
{
	return cart_ram[addr];
}

/**
 * Writes a given byte to the cartridge RAM at the given address.
 */
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
	/* If save file not required. */
	if(len == 0)
	{
		return;
	}
	
	
	for (int i=0; i<16; i++)
	{
		if (save_file_name[i] <= 0x20 || save_file_name[i] >= 0x7F)
		{
			save_file_name[i] = 0;
		}
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
 
	char buffer[1];
	unsigned int bytes;
	unsigned int result;

	result = f_open(&file, save_file_name, FA_READ);
	if (result == 0)
	{	
		display_string(8, 8, "Reading Cart RAM from File\\");
		
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
	
	/*
	display_string(8, 8, "Reading Cart RAM...\\");
	
	int test = 0;

	for (int i=0; i<5; i++)
	{
		test = sdcard_initialize();

		if (test == 1) break;
	}

	if (test == 1)
	{
		for (unsigned int i=0; i<64; i++)
		{
			test = sdcard_readblock(0x003F, (unsigned int)(0xFE00 + 0x0080 * cart_bank + i*2)); // last 128KB within the first 1GB
			
			DelayMS(1);
			
			if (test == 0)
			{
				SendString("Error occured while reading Cart RAM\r\n\\");
				
				return;
			}
			
			for (unsigned int j=0; j<512; j++)
			{
				cart_ram[i*512 + j] = sdcard_block[j];
			}
		}
		
		SendString("Success reading Cart RAM\r\n\\");
	}
	else
	{
		SendString("Error initializing when reading Cart RAM\r\n\\");
	}
	*/
	
	return;
}

void write_cart_ram_file(char save_file_name[16], uint8_t *dest,
			 const size_t len)
{
	
	for (int i=0; i<16; i++)
	{
		if (save_file_name[i] <= 0x20 || save_file_name[i] >= 0x7F)
		{
			save_file_name[i] = 0;
		}
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
 
	unsigned char buffer[1];
	unsigned int bytes;
	unsigned int result;
	
	result = f_open(&file, save_file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if (result == 0)
	{	
		display_string(8, 8, "Writing Cart RAM to File\\");
		
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
	
	/*
	display_string(8, 8, "Writing Cart RAM...\\");
	
	int test = 0;

	for (int i=0; i<5; i++)
	{
		test = sdcard_initialize();

		if (test == 1) break;
	}

	if (test == 1)
	{
		for (unsigned int i=0; i<64; i++)
		{
			for (unsigned int j=0; j<512; j++)
			{
				sdcard_block[j] = cart_ram[i*512 + j];
			}

			test = sdcard_writeblock(0x003F, (unsigned int)(0xFE00 + 0x0080 * cart_bank + i*2)); // last 128KB within the first 1GB
			
			if (test == 0)
			{
				SendString("Error occured while writing Cart RAM\r\n\\");
				
				return;
			}
			
			test = 1;
			
			test = sdcard_readblock(0x003F, (unsigned int)(0xFE00 + 0x0080 * cart_bank + i*2));
			
			if (test == 0)
			{
				SendString("Error occured while reading back Cart RAM\r\n\\");
				
				return;
			}
			
			test = 1;
			
			for (unsigned int j=0; j<512; j++)
			{
				if (sdcard_block[j] != cart_ram[i*512 + j])
				{
					test = 0;
					break;
				}
			}
			
			if (test == 0)
			{
				SendString("Error occured while comparing Cart RAM\r\n\\");
				
				return;
			}
		}
		
		SendString("Success writing Cart RAM\r\n\\");
	}
	else
	{
		SendString("Error initializing when writing Cart RAM\r\n\\");
	}
	*/
	
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
	size_t palette_bytes = 3 * 4 * sizeof(uint8_t);

	switch(game_checksum)
	{
	/* Balloon Kid and Tetris Blast */
	case 0x71:
	case 0xFF:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7FFF, 0x7E60, 0x7C00, 0x0000 }, /* OBJ0 */
			//{ 0x7FFF, 0x7E60, 0x7C00, 0x0000 }, /* OBJ1 */
			//{ 0x7FFF, 0x7E60, 0x7C00, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	/* Pokemon Yellow and Tetris */
	case 0x15:
	case 0xDB:
	case 0x95: /* Not officially */
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7FFF, 0x7FE0, 0x7C00, 0x0000 }, /* OBJ0 */
			//{ 0x7FFF, 0x7FE0, 0x7C00, 0x0000 }, /* OBJ1 */
			//{ 0x7FFF, 0x7FE0, 0x7C00, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	/* Donkey Kong */
	case 0x19:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ0 */
			//{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ1 */
			//{ 0x7FFF, 0x7E60, 0x7C00, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	/* Pokemon Blue */
	case 0x61:
	case 0x45:

	/* Pokemon Blue Star */
	case 0xD8:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ0 */
			//{ 0x7FFF, 0x329F, 0x001F, 0x0000 }, /* OBJ1 */
			//{ 0x7FFF, 0x329F, 0x001F, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	/* Pokemon Red */
	case 0x14:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7FFF, 0x3FE6, 0x0200, 0x0000 }, /* OBJ0 */
			//{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ1 */
			//{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	/* Pokemon Red Star */
	case 0x8B:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ0 */
			//{ 0x7FFF, 0x329F, 0x001F, 0x0000 }, /* OBJ1 */
			//{ 0x7FFF, 0x3FE6, 0x0200, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	/* Kirby */
	case 0x27:
	case 0x49:
	case 0x5C:
	case 0xB3:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7D8A, 0x6800, 0x3000, 0x0000 }, /* OBJ0 */
			//{ 0x001F, 0x7FFF, 0x7FEF, 0x021F }, /* OBJ1 */
			//{ 0x527F, 0x7FE0, 0x0180, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	/* Donkey Kong Land [1/2/III] */
	case 0x18:
	case 0x6A:
	case 0x4B:
	case 0x6B:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7F08, 0x7F40, 0x48E0, 0x2400 }, /* OBJ0 */
			//{ 0x7FFF, 0x2EFF, 0x7C00, 0x001F }, /* OBJ1 */
			//{ 0x7FFF, 0x463B, 0x2951, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	/* Link's Awakening */
	case 0x70:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7FFF, 0x03E0, 0x1A00, 0x0120 }, /* OBJ0 */
			//{ 0x7FFF, 0x329F, 0x001F, 0x001F }, /* OBJ1 */
			//{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	/* Mega Man [1/2/3] & others I don't care about. */
	case 0x01:
	case 0x10:
	case 0x29:
	case 0x52:
	case 0x5D:
	case 0x68:
	case 0x6D:
	case 0xF6:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7FFF, 0x329F, 0x001F, 0x0000 }, /* OBJ0 */
			//{ 0x7FFF, 0x3FE6, 0x0200, 0x0000 }, /* OBJ1 */
			//{ 0x7FFF, 0x7EAC, 0x40C0, 0x0000 }  /* BG */
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
		break;
	}

	default:
	{
		const uint8_t palette[3][4] =
		{
			//{ 0x7FFF, 0x5294, 0x294A, 0x0000 },
			//{ 0x7FFF, 0x5294, 0x294A, 0x0000 },
			//{ 0x7FFF, 0x5294, 0x294A, 0x0000 }
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 },
			{ 0xFF, 0x92, 0x49, 0x00 }, 
		};
		memcpy(selected_palette, palette, palette_bytes);
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
	if (draw_frame > 0)
	{
		for(unsigned int x = 0; x < LCD_WIDTH*2; x++)
		{
			screen_buffer[((line<<1)+32) * SCREEN_X + x + 96] = 
					selected_palette
					[(pixels[(x>>1)] & LCD_PALETTE_ALL) >> 4]
					[pixels[(x>>1)] & 3];
			screen_buffer[((line<<1)+33) * SCREEN_X + x + 96] = 
					selected_palette
					[(pixels[(x>>1)] & LCD_PALETTE_ALL) >> 4]
					[pixels[(x>>1)] & 3];
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
	
	menu_x = 344;
	menu_y = 16;
	menu_pos = 0;
	menu_max = 33;
	
	int total = 0;
	char filename[32][13];
	char gamename[32][17];
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
			
			if (total >= 32) break;
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
	
	for (int i=total; i<32; i++)
	{	
		display_string(menu_x + 8, menu_y, "                \\");
		menu_y += 8;
	}
	
	f_rewinddir(&dir);
	
	menu_y = 16;
	
	for (int i=0; i<33; i++)
	{
		display_character(menu_x, menu_y + i * 8, ' ');
	}

/*	
	unsigned int list_loop = 1;
	unsigned long list_delay = 0;
	unsigned char list_up = 0;
	unsigned char list_down = 0;
	
	while (list_loop > 0)
	{
		if (list_up == 1)
		{
			list_up = 2;
			
			display_character(list_x, list_y + choice * 8, ' ');

			if (choice > 0) choice--;

			display_character(list_x, list_y + choice * 8, '>');
		}
		
		if (list_down == 1)
		{
			list_down = 2;
			
			display_character(list_x, list_y + choice * 8, ' ');

			if (choice < 31) choice++;

			display_character(list_x, list_y + choice * 8, '>');
		}
		
		if (list_delay > 0)
		{
			list_delay--;
			
			continue;
		}
		
		list_delay = 0x0007FFFF;
		
		if (PORTJbits.RJ0 == 0)
		{
			if (list_up == 0)
			{
				list_up = 1;
			}
		}
		else list_up = 0;
		
		if (PORTJbits.RJ1 == 0)
		{
			if (list_down == 0)
			{
				list_down = 1;
			}
		}
		else list_down = 0;
		
		if (PORTJbits.RJ4 == 0 || PORTJbits.RJ5 == 0)
		{
			if (choice < total)
			{
				list_loop = 0;
			}
		}
	}
*/	
	
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
	
	// This is commented out because I want to make only
	// one single save file, overwrite it as need be.
	/*
	
	for (int i=0; i<13; i++) save_file_name[i] = 0;
	
	save_file_name[0] = '/';
	
	int pos = 1;
	
	for (int i=0; i<16; i++)
	{
		if (cart_rom[0x0134+i] > 0x40 && cart_rom[0x0134+i] < 0x5B) // only the first 8 capital letters
		{
			save_file_name[pos] = cart_rom[0x0134+i];
			pos++;
			
			if (pos >= 9) break;
		}
	}
	
	save_file_name[pos] = '.';
	save_file_name[pos+1] = 'S';
	save_file_name[pos+2] = 'A';
	save_file_name[pos+3] = 'V';
	*/
	
	
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
	unsigned char sound_toggle = 1;
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

	auto_assign_palette(gb_colour_hash(&gb));
	
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
	
	while (1)
	{
		frame_trigger = 0; // clear frame trigger
		
		TMR9 = 0x0000;
		T9CONbits.ON = 1; // turn on Timer9
		
		// two frames per frame trigger
		for (unsigned char loop=0; loop<2; loop++)
		{
			// if device connected...
			if (USBA_DEVICE_CONNECTED)
			{
				USBA_host_tasks();
				USBA_HID_tasks();
				if (USBA_EP1_RECEIVED > 0)
				{
					USBA_EP1_receive(USBA_EP1_buffer);
					USBA_HID_process_report();
					USBA_EP1_RECEIVED--;
				}

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
					
					if (usb_mode == 0x01) // keyboard
					{
						if (USBA_device_keys[0x52].pressed) gb.direct.joypad &= ~JOYPAD_UP;
						if (USBA_device_keys[0x51].pressed) gb.direct.joypad &= ~JOYPAD_DOWN;
						if (USBA_device_keys[0x50].pressed) gb.direct.joypad &= ~JOYPAD_LEFT;
						if (USBA_device_keys[0x4F].pressed) gb.direct.joypad &= ~JOYPAD_RIGHT;

						if (USBA_device_keys[0x1B].pressed) gb.direct.joypad &= ~JOYPAD_A;
						if (USBA_device_keys[0x1D].pressed) gb.direct.joypad &= ~JOYPAD_B;
						if (USBA_device_keys[0x2C].pressed) gb.direct.joypad &= ~JOYPAD_SELECT;
						if (USBA_device_keys[0x28].pressed) gb.direct.joypad &= ~JOYPAD_START;
					}
					else if (usb_mode == 0x03) // joystick
					{
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
					if (draw_frame > 0)
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
					else
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
				
				menu_x = 96;
				menu_y = 328;
				menu_pos = 0;
				menu_max = 5;
				
				display_string(menu_x, menu_y,		" Resume        \\");
				display_string(menu_x, menu_y+8,	" Sound On      \\");
				display_string(menu_x, menu_y+16,	" Sound Off     \\");
				display_string(menu_x, menu_y+24,	" Load and Reset\\");
				display_string(menu_x, menu_y+32,	" Overwrite Save\\");
	
				choice = (unsigned char)Menu();
				
				if (choice == 0) { } // resume
				else if (choice == 1) // sound on
				{
					sound_toggle = 1;
				}
				else if (choice == 2) // sound off
				{
					sound_toggle = 0;
				}
				else if (choice == 3) // load and reset
				{
					for (unsigned int y=320; y<SCREEN_Y; y++)
					{
						for (unsigned int x=0; x<SCREEN_X; x++)
						{
							screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
						}
					}
					
					menu_x = 96;
					menu_y = 328;
					menu_pos = 0;
					menu_max = 6;

					display_string(menu_x, menu_y,		" *** Load and Reset?\\");
					display_string(menu_x, menu_y+8,	" No                 \\");
					display_string(menu_x, menu_y+16,	" File A             \\");
					display_string(menu_x, menu_y+24,	" File B             \\");
					display_string(menu_x, menu_y+32,	" File C             \\");
					display_string(menu_x, menu_y+40,	" File D             \\");

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
				else if (choice == 4) // save
				{
					for (unsigned int y=320; y<SCREEN_Y; y++)
					{
						for (unsigned int x=0; x<SCREEN_X; x++)
						{
							screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
						}
					}
					
					menu_x = 96;
					menu_y = 328;
					menu_pos = 0;
					menu_max = 6;

					display_string(menu_x, menu_y,		" *** Overwrite Save?\\");
					display_string(menu_x, menu_y+8,	" No                 \\");
					display_string(menu_x, menu_y+16,	" File A             \\");
					display_string(menu_x, menu_y+24,	" File B             \\");
					display_string(menu_x, menu_y+32,	" File C             \\");
					display_string(menu_x, menu_y+40,	" File D             \\");

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
						
						/* Record save file. */
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
			
			draw_frame = 1 - draw_frame; // alternate drawing frames

			/* Execute CPU cycles until the screen has to be redrawn. */
			gb_run_frame(&gb);

#if ENABLE_SOUND
			if (sound_toggle > 0)
			{
				// playing audio
				audio_callback(&gb, (uint8_t *)&audio_buffer, AUDIO_NSAMPLES);
				audio_position = 0;
			}
#else
			if (sound_toggle > 0) audio_position = 0;
#endif
		}
		
		// speed limiter for when occasionally the Gameboy is too fast
		while (frame_trigger == 0) { } // wait for frame trigger interrupt
	}

	return ret;
}


				