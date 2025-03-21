
// nes.c

// An NES emulator designed for the PIC32MZ
// Yet easy to implement on other platforms
// Written by: Professor Steven Chad Burrow



// if using for other platforms, adjust variable types here
// 'volatile' seems to keep it from getting general exception errors
// but it also slows down the whole system!
volatile unsigned char *cart_rom = (volatile unsigned char *)0x9D100000;

volatile unsigned char __attribute__((address(0x8004E000))) cpu_ram[2048]; // only cpu ram from 0x0000 to 0x07FF
volatile unsigned char __attribute__((address(0x80050000))) ppu_ram[2048]; // ppu ram from 0x2000 to 0x2FFF (halved, mirrored)
volatile unsigned char __attribute__((address(0x80052000))) prg_ram[8192]; // cpu ram from 0x6000 to 0x7FFF (if used)
volatile unsigned char __attribute__((address(0x80054000))) chr_ram[8192]; // ppu ram from 0x0000 to 0x1FFF (if used)
volatile unsigned char __attribute__((address(0x8004F700))) oam_ram[256]; // special sprite ram inside of ppu
volatile unsigned char __attribute__((address(0x8004F600))) pal_ram[32]; // special palette ram inside of ppu

unsigned long prg_offset = 0x00000000; // offsets in cart_rom
unsigned long chr_offset = 0x00000000;
unsigned long end_offset = 0x00000000;

unsigned char nes_init_flag = 0;
unsigned char nes_reset_flag = 0;
unsigned char nes_audio_flag = 1;
unsigned long nes_pixel_location = 0;

volatile unsigned long nes_interrupt_count = 0;

volatile unsigned char nes_hack_top_hud = 0; // Duck Tales
volatile unsigned char nes_hack_bottom_hud = 0; // Mario 3 and Kirby
volatile unsigned char nes_hack_sprite_priority = 0; // Mario 3

unsigned long cpu_current_cycles = 0, cpu_dma_cycles = 0;
unsigned long ppu_scanline_cycles = 0;
unsigned long ppu_frame_cycles = 0;
unsigned long ppu_frame_count = 0;
volatile signed long ppu_scanline_count = 0; // needs to be signed
volatile signed long ppu_scanline_interrupt = 0;
volatile signed long ppu_scanline_sprite_0 = 0;
unsigned long apu_sample_cycles = 0;

unsigned short map_number = 0x0000;

unsigned long map_unrom_bank = 0x0000;
unsigned long map_cnrom_bank = 0x0000;
unsigned long map_anrom_bank = 0x0000;

unsigned long map_mmc1_shift = 0x0000;
unsigned long map_mmc1_count = 0x0000;
unsigned long map_mmc1_prg_mode = 0x0003; // should be 0x0003
unsigned long map_mmc1_chr_mode = 0x0000;
unsigned long map_mmc1_chr_bank_0 = 0x0010; // leave at 0x0010 for large PRG_ROM
unsigned long map_mmc1_chr_bank_1 = 0x0000;
unsigned long map_mmc1_prg_bank = 0x0000;
unsigned long map_mmc1_ram = 0x0000;

unsigned long map_mmc3_bank_next = 0x0000;
unsigned long map_mmc3_prg_mode = 0x0000;
unsigned long map_mmc3_chr_mode = 0x0000;
unsigned long map_mmc3_bank_r0 = 0x0000;
unsigned long map_mmc3_bank_r1 = 0x0000;
unsigned long map_mmc3_bank_r2 = 0x0000;
unsigned long map_mmc3_bank_r3 = 0x0000;
unsigned long map_mmc3_bank_r4 = 0x0000;
unsigned long map_mmc3_bank_r5 = 0x0000;
unsigned long map_mmc3_bank_r6 = 0x0000;
unsigned long map_mmc3_bank_r7 = 0x0000;
unsigned long map_mmc3_ram = 0x0000;
unsigned long map_mmc3_irq_latch = 0x007F;
unsigned long map_mmc3_irq_counter = 0x007F; // start at something above 0??
unsigned long map_mmc3_irq_enable = 0x0000;
unsigned long map_mmc3_irq_previous = 0x0000;
unsigned long map_mmc3_irq_interrupt = 0x0000;

unsigned long cpu_reg_a = 0x0000, cpu_reg_x = 0x0000, cpu_reg_y = 0x0000, cpu_reg_s = 0x00FD;
unsigned long cpu_flag_c = 0x0000, cpu_flag_z = 0x0000, cpu_flag_v = 0x0000, cpu_flag_n = 0x0000;
unsigned long cpu_flag_d = 0x0000, cpu_flag_i = 0x0001;
unsigned long cpu_reg_pc = 0xFFFC;

unsigned long cpu_temp_opcode = 0x0000, cpu_temp_memory = 0x0000, cpu_temp_address = 0x0000; 
unsigned long cpu_temp_result = 0x0000, cpu_temp_cycles = 0x0000;

unsigned long cpu_status_r = 0x0000;

unsigned long ppu_reg_v = 0x0000, ppu_reg_t = 0x0000, ppu_reg_w = 0x0000;
unsigned long ppu_reg_a = 0x0000, ppu_reg_b = 0x0000;
unsigned long ppu_reg_x = 0x0000;
unsigned long ppu_reg_r = 0x0000;

unsigned long ppu_flag_e = 0x0000, ppu_flag_p = 0x0000, ppu_flag_h = 0x0000;
unsigned long ppu_flag_b = 0x0000, ppu_flag_s = 0x0000, ppu_flag_i = 0x0000;
unsigned long ppu_flag_v = 0x0000, ppu_flag_0 = 0x0000, ppu_flag_o = 0x0000;

unsigned long ppu_flag_g = 0x0000, ppu_flag_lb = 0x0000, ppu_flag_ls = 0x0000;
unsigned long ppu_flag_eb = 0x0000, ppu_flag_es = 0x0000;

unsigned long ppu_status_0 = 0x0000;
signed long ppu_status_s = 0x0000; // needs to be signed
signed long ppu_status_d = 0x0000;
unsigned long ppu_status_m = 0x0000;

unsigned long ctl_flag_s = 0x0000;
unsigned long ctl_value_1 = 0x0000, ctl_value_2 = 0x0000;
unsigned long ctl_latch_1 = 0x0000, ctl_latch_2 = 0x0000;

unsigned short apu_pulse_1_d = 0x0000, apu_pulse_2_d = 0x0000;
unsigned short apu_pulse_1_u = 0x0000, apu_pulse_2_u = 0x0000;
unsigned short apu_pulse_1_i = 0x0000, apu_pulse_2_i = 0x0000;
unsigned short apu_pulse_1_c = 0x0000, apu_pulse_2_c = 0x0000;
unsigned short apu_pulse_1_v = 0x0000, apu_pulse_2_v = 0x0000;
unsigned short apu_pulse_1_m = 0x0000, apu_pulse_2_m = 0x0000;
unsigned short apu_pulse_1_r = 0x0000, apu_pulse_2_r = 0x0000;
unsigned short apu_pulse_1_s = 0x0000, apu_pulse_2_s = 0x0000;
unsigned short apu_pulse_1_a = 0x0000, apu_pulse_2_a = 0x0000;
unsigned short apu_pulse_1_n = 0x0000, apu_pulse_2_n = 0x0000;
unsigned short apu_pulse_1_p = 0x0000, apu_pulse_2_p = 0x0000;
unsigned short apu_pulse_1_w = 0x0000, apu_pulse_2_w = 0x0000;
unsigned short apu_pulse_1_e = 0x0000, apu_pulse_2_e = 0x0000;
unsigned short apu_pulse_1_t = 0x0000, apu_pulse_2_t = 0x0000;
unsigned short apu_pulse_1_k = 0x0000, apu_pulse_2_k = 0x0000;
unsigned short apu_pulse_1_l = 0x0000, apu_pulse_2_l = 0x0000;
unsigned short apu_pulse_1_o = 0x0000, apu_pulse_2_o = 0x0000;

unsigned short apu_triangle_c = 0x0000;
unsigned short apu_triangle_r = 0x0000;
unsigned short apu_triangle_v = 0x0000;
unsigned short apu_triangle_f = 0x0000;
unsigned short apu_triangle_h = 0x0000;
unsigned short apu_triangle_q = 0x0000;
unsigned short apu_triangle_t = 0x0000;
unsigned short apu_triangle_k = 0x0000;
unsigned short apu_triangle_l = 0x0000;
unsigned short apu_triangle_p = 0x0000;
unsigned short apu_triangle_d = 0x0000;
unsigned short apu_triangle_o = 0x0000;

unsigned short apu_noise_i = 0x0000;
unsigned short apu_noise_c = 0x0000;
unsigned short apu_noise_v = 0x0000;
unsigned short apu_noise_m = 0x0000;
unsigned short apu_noise_r = 0x0000;
unsigned short apu_noise_s = 0x0001; // must be set to 0x0001
unsigned short apu_noise_x = 0x0000;
unsigned short apu_noise_d = 0x0000;
unsigned short apu_noise_t = 0x0000;
unsigned short apu_noise_k = 0x0000;
unsigned short apu_noise_l = 0x0000;
unsigned short apu_noise_o = 0x0000;

unsigned short apu_dmc_i = 0x0000;
unsigned short apu_dmc_l = 0x0000;
unsigned short apu_dmc_r = 0x0000;
unsigned short apu_dmc_k = 0x0000;
unsigned short apu_dmc_d = 0x0000;
unsigned short apu_dmc_a = 0x0000;
unsigned short apu_dmc_s = 0x0000;
unsigned short apu_dmc_b = 0x0000;
unsigned short apu_dmc_t = 0x0000;
unsigned short apu_dmc_o = 0x0000;

unsigned short apu_flag_i = 0x0000;
unsigned short apu_flag_f = 0x0000;
unsigned short apu_flag_d = 0x0000;
unsigned short apu_flag_n = 0x0000;
unsigned short apu_flag_t = 0x0000;
unsigned short apu_flag_2 = 0x0000;
unsigned short apu_flag_1 = 0x0000;
unsigned short apu_flag_m = 0x0000;
unsigned short apu_flag_b = 0x0001;

unsigned short apu_counter_q = 0x0000;
unsigned short apu_counter_s = 0x0000;
unsigned short apu_mixer_output = 0x0000;

volatile unsigned char ppu_palette[64] = {
	0x6D,0x03,0x02,0x46,0x82,0xA0,0xA0,0x80,0x44,0x0C,0x0C,0x08,0x09,0x00,0x00,0x00,
	0xB6,0x0F,0x0B,0x6B,0xC3,0xE1,0xE4,0xE8,0xAC,0x14,0x14,0x15,0x12,0x00,0x00,0x00,
	0xFF,0x37,0x73,0x8F,0xEF,0xEA,0xED,0xF5,0xF4,0xBC,0x59,0x5E,0x1F,0x6D,0x00,0x00,
	0xFF,0xBF,0xB7,0xD7,0xF7,0xF7,0xFA,0xFE,0xF9,0xDD,0xBE,0xBF,0x1F,0xFB,0x00,0x00
};

volatile unsigned char apu_length[32] = {
	 10, 254,  20,   2,  40,   4,  80,   6, 160,   8,  60,  10,  14,  12,  26,  14,
	 12,  16,  24,  18,  48,  20,  96,  22, 192,  24,  72,  26,  16,  28,  32,  30
};

volatile unsigned char apu_duty[32] = {
	0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

volatile unsigned short apu_rate[16] = {
	428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
};

volatile unsigned short apu_period[16] = {
	4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
};

volatile void __attribute__((vector(_TIMER_8_VECTOR), interrupt(ipl2srs))) t8_handler()
{		
	IFS1bits.T8IF = 0;  // clear interrupt flag
	
	if (audio_enable > 0)
	{
		// 8-bit signed audio add 0x80, unsigned add 0x00
		PORTJ = (unsigned short)(((audio_buffer[(audio_read)%AUDIO_LEN]) + 0x00) + 
			(((audio_buffer[(audio_read)%AUDIO_LEN]) + 0x00) << 8));

		audio_read = audio_read + 1;

		if (audio_read >= AUDIO_LEN)
		{
			audio_read = 0;
		}
	}
}

volatile void __attribute__((vector(_TIMER_9_VECTOR), interrupt(ipl3srs))) t9_handler()
{		
	IFS1bits.T9IF = 0;  // clear interrupt flag
	
	nes_interrupt_count = nes_interrupt_count + 1;
	
	if (nes_interrupt_count > 60*262) nes_interrupt_count = 60*262;
}

// change for platform
void nes_error(unsigned char code)
{			
	SendChar('\n');
	SendChar('\r');
	
	SendString("Error \\");
	SendHex(code);
	SendString("\n\r\\");
	
	DelayMS(1000);
	
	if (code == 0x00)
	{
		SendString("File Issue \\");
		SendString("\n\r\\");
		
		screen_frame = 0;
		
		for (unsigned int i=0; i<28; i++)
		{
			display_string(0, 0x0008*i, "File Issue\\");
		}
	}
	else if (code == 0x01)
	{
		SendString("Verify Mismatch \\");
		SendString("\n\r\\");
		
		screen_frame = 0;
		
		for (unsigned int i=0; i<28; i++)
		{
			display_string(0, 0x0008*i, "Verify Mismatch\\");
		}
	}
	else if (code == 0x02)
	{
		SendString("Unknown Opcode \\");
		SendLongHex(cpu_reg_pc);
		SendChar(':');
		SendHex(cpu_temp_opcode);
		SendString("\n\r\\");
		
		screen_frame = 0;
		
		for (unsigned int i=0; i<28; i++)
		{
			display_string(0, 0x0008*i, "Unknown Opcode\\");
		}
	}
	
	while (PORTKbits.RK4 == 1 && PORTKbits.RK5 == 1) { }
	
	DelayMS(1000);
	
	// soft reset system
	SYSKEY = 0x0; // reset
	SYSKEY = 0xAA996655; // unlock key #1
	SYSKEY = 0x556699AA; // unlock key #2
	RSWRST = 1; // set bit to reset of system
	SYSKEY = 0x0; // re-lock
	RSWRST; // read from register to reset
	while (1) { } // wait until reset occurs
}

unsigned char nes_save(char *filename)
{
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	
	//SendString("Initializing disk\n\r\\");
	
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
	unsigned char flag;
	
	result = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if (result == 0)
	{		
		for (unsigned int i=0; i<8192; i++)
		{
			buffer[0] = prg_ram[i];
			
			while (f_write(&file, buffer, 1, &bytes) != 0) { }
		}
		
		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }
		
		//SendString("Wrote cart ram to file\n\r\\");
		
		flag = 1;
	}
	else
	{
		//SendString("Could not write cart ram to file\n\r\\");
		
		flag = 0;
		
		nes_error(0x00);
	}	
	
	return flag;
}

unsigned char nes_load(char *filename)
{
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	
	//SendString("Initializing disk\n\r\\");
	
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
	unsigned char flag;
	
	result = f_open(&file, filename, FA_READ);
	if (result == 0)
	{		
		for (unsigned int i=0; i<8192; i++)
		{
			while (f_read(&file, &buffer[0], 1, &bytes) != 0) { } // MUST READ ONE BYTE AT A TIME!!!
			
			prg_ram[i] = buffer[0];
		}
		
		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }
		
		//SendString("Read cart ram from file\n\r\\");
		
		flag = 1;
	}
	else
	{		
		//SendString("Could not read cart ram from file\n\r\\");
		
		flag = 0;
		
		nes_error(0x00);
	}	
	
	return flag;
}

unsigned char nes_verify(char *filename)
{
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	
	//SendString("Initializing disk\n\r\\");
	
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
	unsigned char flag;
	
	unsigned long offset = 16 + (unsigned char)cart_rom[4]*16384 + (unsigned char)cart_rom[5]*8192;
	
	result = f_open(&file, filename, FA_READ);
	if (result == 0)
	{		
		flag = 1;
		
		for (unsigned int i=0; i<offset; i++)
		{
			while (f_read(&file, &buffer[0], 1, &bytes) != 0) { } // MUST READ ONE BYTE AT A TIME!!!
			
			if ((unsigned char)cart_rom[i] != buffer[0])
			{
				nes_error(0x01);
				
				flag = 0;
				
				break;
			}
		}
		
		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }
		
		//SendString("Read cart ram from file\n\r\\");
	}
	else
	{		
		//SendString("Could not read cart ram from file\n\r\\");
		
		flag = 0;
		
		nes_error(0x00);
	}	
	
	return flag;
}

// change for platform
unsigned char nes_burn(char *filename)
{
	for (unsigned long i=0x1D100000; i<0x1D200000; i+=0x00001000) // pages are 0x1000
	{
		NVMErasePage(i);
	}
	
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	
	unsigned char buffer[4][1];
	unsigned long word[4];
	unsigned int bytes;
	unsigned int result;
	unsigned char flag;
	unsigned long addr;
	
	//SendString("Initializing disk\n\r\\");
	
	// Wait for the disk to initialise
	while(disk_initialize(0));
	// Mount the disk
	f_mount(&fso, "", 0);
	// Change dir to the root directory
	f_chdir("/");
	// Open the directory
	f_opendir(&dir, ".");
	
	//SendString("Attempting to read\n\r\\");

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

		//SendString("Read successful\n\r\\");
		
		flag = nes_verify(filename);
	}
	else
	{
		//SendString("Read failure\n\r\\");
		
		flag = 0;
		
		nes_error(0x00);
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
	
	return flag; // never gets returned
}

// change for platform
void nes_pixel(unsigned short pos_x, unsigned short pos_y, unsigned char color)
{
	nes_pixel_location = ((pos_y))*SCREEN_X+((pos_x<<1))+(1-screen_frame)*SCREEN_XY;
	screen_buffer[(nes_pixel_location)] = color;
	screen_buffer[(nes_pixel_location+1)] = color;
}

// change for platform
void nes_frame()
{
	screen_frame = 1 - screen_frame;
}

// change for platform
void nes_sound(unsigned char sample)
{
	audio_buffer[(audio_write)%AUDIO_LEN] = sample;
	
	audio_write = audio_write + 1;
	
	if (audio_write >= AUDIO_LEN)
	{
		audio_write = 0;
	}
}
	
// change for platform
void nes_buttons()
{
	ctl_value_1 = 0xFF080000 | (controller_status_3 << 8) | controller_status_1;
	ctl_value_2 = 0xFF040000 | (controller_status_4 << 8) | controller_status_2;
}

unsigned char nes_read_cpu_ram(unsigned long addr)
{
	return cpu_ram[(addr&2047)];
	//return cpu_ram[addr];
}

void nes_write_cpu_ram(unsigned long addr, unsigned char val)
{
	cpu_ram[(addr&2047)] = val;
	//cpu_ram[addr] = val;
}

unsigned char nes_read_ppu_ram(unsigned long addr)
{
	return ppu_ram[(addr&2047)];
	//return ppu_ram[addr];
}

void nes_write_ppu_ram(unsigned long addr, unsigned char val)
{
	ppu_ram[(addr&2047)] = val;
	//ppu_ram[addr] = val;
}

unsigned char nes_read_prg_ram(unsigned long addr)
{
	return prg_ram[(addr&8191)];
	//return prg_ram[addr];
}

void nes_write_prg_ram(unsigned long addr, unsigned char val)
{
	prg_ram[(addr&8191)] = val;
	//prg_ram[addr] = val;
}

unsigned char nes_read_chr_ram(unsigned long addr)
{
	return chr_ram[(addr&8191)];
	//return chr_ram[addr];
}

void nes_write_chr_ram(unsigned long addr, unsigned char val)
{
	chr_ram[(addr&8191)] = val;
	//chr_ram[addr] = val;
}

unsigned char nes_read_oam_ram(unsigned long addr)
{
	return oam_ram[(addr&255)];
	//return oam_ram[addr];
}

void nes_write_oam_ram(unsigned long addr, unsigned char val)
{
	oam_ram[(addr&255)] = val;
	//oam_ram[addr] = val;
}

unsigned char nes_read_pal_ram(unsigned long addr)
{
	return pal_ram[(addr&31)];
	//return pal_ram[addr];
}

void nes_write_pal_ram(unsigned long addr, unsigned char val)
{
	pal_ram[(addr&31)] = val;
	//pal_ram[addr] = val;
}

unsigned char nes_read_cart_rom(unsigned long addr)
{
	//debug_location = 3;
	//debug_value = addr;
	
	return (unsigned char)(unsigned char)cart_rom[addr];
}

void nes_irq_decrement()
{
	if (map_mmc3_irq_counter == 0)
	{
		map_mmc3_irq_counter = map_mmc3_irq_latch;
		
		if (map_mmc3_irq_latch == 0)
		{
			map_mmc3_irq_interrupt = 1;
		}
	}
	else
	{
		if (ppu_flag_s != ppu_flag_b) // only if using different nametables
		{
			map_mmc3_irq_counter = map_mmc3_irq_counter - 1;

			if (map_mmc3_irq_counter == 0 && map_mmc3_irq_previous == 1) map_mmc3_irq_interrupt = 1;
		}
	}

	map_mmc3_irq_previous = map_mmc3_irq_counter;
}

unsigned char cpu_read(unsigned long addr)
{	
	debug_location = 1;
	debug_value = addr;
	
	if (addr < 0x00002000)
	{
		return nes_read_cpu_ram((addr&0x000007FF)); // internal ram (and mirrors)
	}
	else if (addr < 0x00004000) // ppu (and mirrors)
	{
		switch ((addr&0x00000007))
		{
			case 0x00: // ppuctrl
			{
				return 0x00;
				
				break;
			}
			case 0x01: // ppumask
			{
				return 0x00;
				
				break;
			}
			case 0x02: // ppustatus
			{				
				unsigned char val = ((ppu_flag_v << 7) | (ppu_flag_0 << 6) | (ppu_flag_o << 5));
				
				ppu_flag_v = 0x0000;
				
				ppu_reg_w = 0x0000;
				
				return val;
				
				break;
			}
			case 0x03: // oamaddr
			{
				return 0x00;
				
				break;
			}
			case 0x04: // oamdata
			{
				return nes_read_oam_ram(ppu_reg_a);
				
				break;
			}
			case 0x05: // ppuscroll
			{
				return 0x00;
				
				break;
			}
			case 0x06: // ppuaddr
			{
				return 0x00;
				
				break;
			}
			case 0x07: // ppudata
			{	
				unsigned char val = ppu_reg_b;
				
				if (ppu_reg_v < 0x2000)
				{
					if ((unsigned char)cart_rom[5] > 0) // chr_rom
					{
						if (map_number == 1) // mmc1
						{
							if (map_mmc1_chr_mode == 0) // 8KB
							{
								if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
								{
									ppu_reg_b = nes_read_cart_rom(((chr_offset+0x00001000*(map_mmc1_chr_bank_0&0x1E)+ppu_reg_v)));
								}
								else
								{
									
								}
							}
							else if (map_mmc1_chr_mode == 1) // 4KB banked
							{
								if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
								{
									if (ppu_reg_v < 0x1000)
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x00001000*(map_mmc1_chr_bank_0)+ppu_reg_v)));
									}
									else
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x00001000*(map_mmc1_chr_bank_1)+ppu_reg_v-0x1000)));
									}
								}
								else
								{
									
								}
							}
						}
						else if (map_number == 2) // unrom
						{
							ppu_reg_b = nes_read_cart_rom(((chr_offset+ppu_reg_v)));
						}
						else if (map_number == 3) // cnrom
						{
							ppu_reg_b = nes_read_cart_rom(((chr_offset+0x2000*map_cnrom_bank+ppu_reg_v)));
						}
						else if (map_number == 4) // mmc3
						{							
							if (map_mmc3_chr_mode == 0x0000)
							{
								switch ((ppu_reg_v&0xFC00))
								{
									case 0x0000:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0800*map_mmc3_bank_r0+ppu_reg_v)));
										break;
									}
									case 0x0400:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0800*map_mmc3_bank_r0+ppu_reg_v)));
										break;
									}
									case 0x0800:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0800*map_mmc3_bank_r1+ppu_reg_v-0x0800)));
										break;
									}
									case 0x0C00:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0800*map_mmc3_bank_r1+ppu_reg_v-0x0800)));
										break;
									}
									case 0x1000:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0400*map_mmc3_bank_r2+ppu_reg_v-0x1000)));
										break;
									}
									case 0x1400:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0400*map_mmc3_bank_r3+ppu_reg_v-0x1400)));
										break;
									}
									case 0x1800:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0400*map_mmc3_bank_r4+ppu_reg_v-0x1800)));
										break;
									}
									case 0x1C00:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0400*map_mmc3_bank_r5+ppu_reg_v-0x1C00)));
										break;
									}
								}
							}
							else
							{
								switch ((ppu_reg_v&0xFC00))
								{
									case 0x0000:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0400*map_mmc3_bank_r2+ppu_reg_v)));
										break;
									}
									case 0x0400:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0400*map_mmc3_bank_r3+ppu_reg_v-0x0400)));
										break;
									}
									case 0x0800:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0400*map_mmc3_bank_r4+ppu_reg_v-0x0800)));
										break;
									}
									case 0x0C00:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0400*map_mmc3_bank_r5+ppu_reg_v-0x0C00)));
										break;
									}
									case 0x1000:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0800*map_mmc3_bank_r0+ppu_reg_v-0x1000)));
										break;
									}
									case 0x1400:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0800*map_mmc3_bank_r0+ppu_reg_v-0x1000)));
										break;
									}
									case 0x1800:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0800*map_mmc3_bank_r1+ppu_reg_v-0x1800)));
										break;
									}
									case 0x1C00:
									{
										ppu_reg_b = nes_read_cart_rom(((chr_offset+0x0800*map_mmc3_bank_r1+ppu_reg_v-0x1800)));
										break;
									}
								}
							}
						}
						else // nrom
						{
							ppu_reg_b = nes_read_cart_rom(((chr_offset+ppu_reg_v)));
						}
					}
					else // chr_ram
					{
						ppu_reg_b = nes_read_chr_ram(ppu_reg_v);
					}
				}
				else if (ppu_reg_v >= 0x2000 && ppu_reg_v < 0x3000)
				{
					if (ppu_reg_v < 0x2400)
					{
						if (ppu_status_m == 0x0000 || ppu_status_m == 0x0001 || ppu_status_m == 0x0002)
						{
							ppu_reg_b = nes_read_ppu_ram(ppu_reg_v-0x2000);
						}
						else if (ppu_status_m == 0x0003)
						{
							ppu_reg_b = nes_read_ppu_ram(ppu_reg_v-0x2000+0x0400);
						}
					}
					else if (ppu_reg_v < 0x2800)
					{
						if (ppu_status_m == 0x0000 || ppu_status_m == 0x0002) // vertical scrolling
						{
							ppu_reg_b = nes_read_ppu_ram(ppu_reg_v-0x2400);
						}
						else if (ppu_status_m == 0x0001 || ppu_status_m == 0x0003) // horizontal scrolling
						{
							ppu_reg_b = nes_read_ppu_ram(ppu_reg_v-0x2000);
						}
					}
					else if (ppu_reg_v < 0x2C00)
					{
						if (ppu_status_m == 0x0000 || ppu_status_m == 0x0003) // vertical scrolling
						{
							ppu_reg_b = nes_read_ppu_ram(ppu_reg_v-0x2400);
						}
						else if (ppu_status_m == 0x0001 || ppu_status_m == 0x0002) // horizontal scrolling
						{
							ppu_reg_b = nes_read_ppu_ram(ppu_reg_v-0x2800);
						}
					}
					else
					{
						if (ppu_status_m == 0x0000 || ppu_status_m == 0x0001 || ppu_status_m == 0x0003)
						{
							ppu_reg_b = nes_read_ppu_ram(ppu_reg_v-0x2800);
						}
						else if (ppu_status_m == 0x0002)
						{
							ppu_reg_b = nes_read_ppu_ram(ppu_reg_v-0x2C00);
						}
					}
				}
				else if (ppu_reg_v >= 0x3F00 && ppu_reg_v < 0x4000)
				{
					ppu_reg_b = nes_read_pal_ram((ppu_reg_v&0x001F));
				}
				 
				if (ppu_flag_i == 0x0000)
				{
					ppu_reg_v += 0x0001;
				}
				else
				{
					ppu_reg_v += 0x0020;
				}
				
				//ppu_reg_r = ppu_reg_v;
				
				return val;
				
				break;
			}
		}
	}
	else if (addr < 0x00004018) // apu and i/o
	{
		switch ((addr&0x000000FF))
		{
			case 0x15: // apu status
			{
				unsigned char val = ((apu_flag_i << 7) |
					(apu_flag_f << 6) |
					(apu_flag_d << 4) |
					(apu_flag_n << 3) |
					(apu_flag_t << 2) |
					(apu_flag_2 << 1) |
					(apu_flag_1));

				apu_flag_f = 0x0000;
				
				return val;
				
				break;
			}
			case 0x16: // controller 1
			{
				if (ctl_flag_s == 0x0000)
				{
					unsigned char val = ((ctl_latch_1 & 0x01) | 0x40);
					
					ctl_latch_1 = (ctl_latch_1 >> 1);
					
					return val;
				}
				else
				{
					return (ctl_value_1 & 0x01);
				}
				
				break;
			}
			case 0x17: // controller 2
			{
				if (ctl_flag_s == 0x0000)
				{
					unsigned char val = ((ctl_latch_2 & 0x01) | 0x40);
					
					ctl_latch_2 = (ctl_latch_2 >> 1);
					
					return val;
				}
				else
				{
					return (ctl_value_2 & 0x01);
				}
				
				break;
			}
			default:
			{
				return 0xFF;
				break;
			}
		}
	}
	else if (addr < 0x00004020) // disabled apu and i/o
	{
		return 0xFF; 
	}
	else if (addr < 0x00006000) // unmapped
	{
		return 0xFF; 
	}
	else if (addr < 0x00008000) // cart ram
	{	
		//if (cpu_status_r > 0)
		//{
			if (map_number == 0x0001)
			{
				if (map_mmc1_ram == 0)
				{
					return nes_read_prg_ram(addr-0x6000);
				}
				else return 0xFF;
			}
			else if (map_number == 0x0004)
			{
				if (map_mmc3_ram == 1)
				{
					return nes_read_prg_ram(addr-0x6000);
				}
				else return 0xFF;
			}
			else
			{
				return nes_read_prg_ram(addr-0x6000);
			}
		//}
		//else return 0xFF;
	}
	else if (addr < 0x0000C000) // cart rom (lower half)
	{
		if (map_number == 0x0001) // mmc1
		{
			if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
			{
				if (map_mmc1_prg_mode == 0 || map_mmc1_prg_mode == 1)
				{
					return nes_read_cart_rom((prg_offset+0x8000*((map_mmc1_prg_bank&0x0E)>>1)+addr-0x8000));
				}
				else if (map_mmc1_prg_mode == 2)
				{
					return nes_read_cart_rom((prg_offset+addr-0x8000));
				}
				else
				{
					return nes_read_cart_rom((prg_offset+0x4000*map_mmc1_prg_bank+addr-0x8000));
				}
			}
			else // up to 512KB
			{
				if (map_mmc1_prg_mode == 0 || map_mmc1_prg_mode == 1)
				{
					return nes_read_cart_rom((prg_offset+0x00040000*((map_mmc1_chr_bank_0&0x10)>>4)+0x8000*((map_mmc1_prg_bank&0x0E)>>1)+addr-0x8000));
				}
				else if (map_mmc1_prg_mode == 2)
				{
					return nes_read_cart_rom((prg_offset+0x00040000*((map_mmc1_chr_bank_0&0x10)>>4)+addr-0x8000));
				}
				else
				{
					return nes_read_cart_rom((prg_offset+0x00040000*((map_mmc1_chr_bank_0&0x10)>>4)+0x4000*map_mmc1_prg_bank+addr-0x8000));
				}
			}
		}
		else if (map_number == 0x0002) // unrom
		{
			return nes_read_cart_rom((prg_offset+0x4000*map_unrom_bank+addr-0x8000));
		}
		else if (map_number == 0x0003) // cnrom
		{
			return nes_read_cart_rom((prg_offset+addr-0x8000));
		}
		else if (map_number == 0x0004) // mmc3
		{
			if (addr < 0x0000A000)
			{
				if (map_mmc3_prg_mode == 0x0000)
				{
					return nes_read_cart_rom((prg_offset+0x2000*map_mmc3_bank_r6+addr-0x8000));
				}
				else
				{	
					return nes_read_cart_rom((prg_offset+0x2000*(((unsigned char)cart_rom[4]<<1)-2)+addr-0x8000));
				}
			}
			else
			{
				return nes_read_cart_rom((prg_offset+0x2000*map_mmc3_bank_r7+addr-0xA000));
			}
		}
		else if (map_number == 0x0007) // anrom
		{
			return nes_read_cart_rom((prg_offset+0x8000*map_anrom_bank+addr-0x8000));
		}
		else // nrom
		{
			return nes_read_cart_rom((prg_offset+addr-0x8000));
		}
	}
	else if (addr < 0x00010000) // cart rom (upper half)
	{
		if (map_number == 0x0001) // mmc1
		{
			if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
			{
				if (map_mmc1_prg_mode == 0 || map_mmc1_prg_mode == 1)
				{
					return nes_read_cart_rom((prg_offset+0x8000*((map_mmc1_prg_bank&0x0E)>>1)+addr-0x8000));
				}
				else if (map_mmc1_prg_mode == 2)
				{
					return nes_read_cart_rom((prg_offset+0x4000*map_mmc1_prg_bank+addr-0xC000));
				}
				else
				{
					return nes_read_cart_rom((prg_offset+0x4000*((unsigned char)cart_rom[4]-1)+addr-0xC000));
				}
			}
			else // up to 512KB
			{
				if (map_mmc1_prg_mode == 0 || map_mmc1_prg_mode == 1)
				{
					return nes_read_cart_rom((prg_offset+0x00040000*((map_mmc1_chr_bank_0&0x10)>>4)+0x8000*((map_mmc1_prg_bank&0x0E)>>1)+addr-0x8000));
				}
				else if (map_mmc1_prg_mode == 2)
				{
					return nes_read_cart_rom((prg_offset+0x00040000*((map_mmc1_chr_bank_0&0x10)>>4)+0x4000*map_mmc1_prg_bank+addr-0xC000));
				}
				else
				{
					return nes_read_cart_rom((prg_offset+0x00040000*((map_mmc1_chr_bank_0&0x10)>>4)+0x4000*(((unsigned char)cart_rom[4]-0x10)-1)+addr-0xC000));
				}
			}
		}
		else if (map_number == 0x0002) // unrom
		{
			return nes_read_cart_rom((prg_offset+0x4000*((unsigned char)cart_rom[4]-1)+addr-0xC000));
		}
		else if (map_number == 0x0003) // cnrom
		{
			if ((unsigned char)cart_rom[4] == 0x01)
			{
				return nes_read_cart_rom((prg_offset+addr-0xC000));
			}
			else
			{
				return nes_read_cart_rom((prg_offset+addr-0x8000));
			}
		}
		else if (map_number == 0x0004) // mmc3
		{
			if (addr < 0x0000E000)
			{
				if (map_mmc3_prg_mode == 0x0000)
				{
					return nes_read_cart_rom((prg_offset+0x2000*(((unsigned char)cart_rom[4]<<1)-2)+addr-0xC000));
				}
				else
				{	
					return nes_read_cart_rom((prg_offset+0x2000*map_mmc3_bank_r6+addr-0xC000));
				}
			}
			else
			{
				return nes_read_cart_rom((prg_offset+0x2000*(((unsigned char)cart_rom[4]<<1)-1)+addr-0xE000));
			}
		}
		else if (map_number == 0x0007) // anrom
		{
			return nes_read_cart_rom((prg_offset+0x8000*map_anrom_bank+addr-0x8000));
		}
		else // nrom
		{
			if ((unsigned char)cart_rom[4] == 0x01)
			{
				return nes_read_cart_rom((prg_offset+addr-0xC000));
			}
			else
			{
				return nes_read_cart_rom((prg_offset+addr-0x8000));
			}
		}
	}
	else
	{
		return cpu_read((addr & 0x0000FFFF));
	}
	
	return 0xFF;
}

void cpu_write(unsigned long addr, unsigned char val)
{	
	debug_location = 2;
	debug_value = addr;
	
	if (addr < 0x00002000)
	{
		nes_write_cpu_ram((addr&0x000007FF), val); // internal ram (and mirrors)
	}
	else if (addr < 0x00004000) // ppu (and mirrors)
	{
		switch ((addr&0x00000007))
		{
			case 0x00: // ppuctrl
			{	
				ppu_flag_e = (val>>7);
				ppu_flag_p = ((val>>6)&0x01);
				ppu_flag_h = ((val>>5)&0x01);
				ppu_flag_b = ((val>>4)&0x01);
				ppu_flag_s = ((val>>3)&0x01);
				ppu_flag_i = ((val>>2)&0x01);
				
				ppu_reg_t = ((ppu_reg_t & 0x73FF) | (((unsigned long)val & 0x0003) << 10));
				
				break;
			}
			case 0x01: // ppumask
			{	
				ppu_flag_g = (val & 0x01);
				ppu_flag_lb = ((val & 0x02) >> 1);
				ppu_flag_ls = ((val & 0x04) >> 2);
				ppu_flag_eb = ((val & 0x08) >> 3);
				ppu_flag_es = ((val & 0x10) >> 4);
				
				break;
			}
			case 0x02: // ppustatus
			{
				ppu_reg_w = 0x0000;
				
				break;
			}
			case 0x03: // oamaddr
			{
				ppu_reg_a = val;
				
				break;
			}
			case 0x04: // oamdata
			{
				nes_write_oam_ram(ppu_reg_a, val);
				ppu_reg_a++;
				
				break;
			}
			case 0x05: // ppuscroll
			{
				if (ppu_reg_w == 0x0000)
				{
					ppu_reg_t = ((ppu_reg_t & 0x7FE0) | (((unsigned long)val & 0x00F8) >> 3));
					ppu_reg_x = ((unsigned long)val & 0x0007);
					ppu_reg_w = 0x0001;
				}
				else
				{
					ppu_reg_t = ((ppu_reg_t & 0x0C1F) | 
						(((unsigned long)val & 0x0007) << 12) |
						(((unsigned long)val & 0x00F8) << 2));
					ppu_reg_w = 0x0000;
				}
				
				break;
			}
			case 0x06: // ppuaddr
			{
				if (ppu_reg_w == 0x0000)
				{
					ppu_reg_t = ((ppu_reg_t & 0x00FF) | (((unsigned long)val & 0x003F) << 8));
					ppu_reg_w = 0x0001;
				}
				else
				{	
					ppu_reg_t = ((ppu_reg_t & 0x7F00) | (unsigned long)val);
					ppu_reg_v = ppu_reg_t;
					ppu_reg_w = 0x0000;
					
					ppu_reg_r = ppu_reg_t;
				}
				
				//if (map_number == 0x0004) // mmc3
				//{
				//	nes_irq_decrement(); // is this right???
				//}
				
				break;
			}
			case 0x07: // ppudata
			{
				if (ppu_reg_v < 0x2000)
				{
					nes_write_chr_ram(ppu_reg_v, val);
				}
				else if (ppu_reg_v >= 0x2000 && ppu_reg_v < 0x3000)
				{	
					if (ppu_reg_v < 0x2400)
					{
						if (ppu_status_m == 0x0000 || ppu_status_m == 0x0001 || ppu_status_m == 0x0002)
						{
							nes_write_ppu_ram(ppu_reg_v-0x2000, val);
						}
						else if (ppu_status_m == 0x0003)
						{
							nes_write_ppu_ram(ppu_reg_v-0x2000+0x0400, val);
						}
					}
					else if (ppu_reg_v < 0x2800)
					{
						if (ppu_status_m == 0x0000 || ppu_status_m == 0x0002) // vertical scrolling
						{
							nes_write_ppu_ram(ppu_reg_v-0x2400, val);
						}
						else if (ppu_status_m == 0x0001 || ppu_status_m == 0x0003) // horizontal scrolling
						{
							nes_write_ppu_ram(ppu_reg_v-0x2000, val);
						}
					}
					else if (ppu_reg_v < 0x2C00)
					{
						if (ppu_status_m == 0x0000 || ppu_status_m == 0x0003) // vertical scrolling
						{
							nes_write_ppu_ram(ppu_reg_v-0x2400, val);
						}
						else if (ppu_status_m == 0x0001 || ppu_status_m == 0x0002) // horizontal scrolling
						{
							nes_write_ppu_ram(ppu_reg_v-0x2800, val);
						}
					}
					else
					{
						if (ppu_status_m == 0x0000 || ppu_status_m == 0x0001 || ppu_status_m == 0x0003)
						{
							nes_write_ppu_ram(ppu_reg_v-0x2800, val);
						}
						else if (ppu_status_m == 0x0002)
						{
							nes_write_ppu_ram(ppu_reg_v-0x2C00, val);
						}
					}
				}
				else if (ppu_reg_v == 0x3F00 || ppu_reg_v == 0x3F10)
				{
					nes_write_pal_ram(0x00, val);
					nes_write_pal_ram(0x10, val);
				}
				else if (ppu_reg_v > 0x3F00 && ppu_reg_v < 0x4000)
				{
					nes_write_pal_ram((ppu_reg_v&0x001F), val);
				}
					
				if (ppu_flag_i == 0x0000)
				{
					ppu_reg_v += 0x0001;
				}
				else
				{
					ppu_reg_v += 0x0020;
				}
				
				//ppu_reg_r = ppu_reg_v;
				
				break;
			}
		}
	}
	else if (addr < 0x00004018) // apu and i/o
	{
		switch ((addr&0x000000FF))
		{
			case 0x00: // pulse 1 volume
			{
				apu_pulse_1_c = ((val & 0x10) >> 4);
				
				if (apu_pulse_1_c > 0)
				{
					apu_pulse_1_v = (val & 0x0F);
				}
				else
				{
					apu_pulse_1_v = 0x0F;
				}
				
				apu_pulse_1_m = (val & 0x0F);
				
				apu_pulse_1_r = apu_pulse_1_m + 1;
				
				apu_pulse_1_i = ((val & 0x20) >> 5);
				
				apu_pulse_1_d = ((val & 0xC0) >> 6);
				
				break;
			}
			case 0x01: // pulse 1 sweep
			{
				apu_pulse_1_s = (val & 0x07);
				
				apu_pulse_1_n = ((val & 0x08) >> 3);
				
				apu_pulse_1_p = ((val & 0x70) >> 4);
				
				apu_pulse_1_w = apu_pulse_1_p + 1;
				
				apu_pulse_1_e = ((val & 0x80) >> 7);
				
				break;
			}
			case 0x02: // pulse 1 timer
			{
				apu_pulse_1_t = ((apu_pulse_1_t & 0x0700) | val);
				
				apu_pulse_1_k = apu_pulse_1_t;
				
				break;
			}
			case 0x03: // pulse 1 length
			{
				apu_pulse_1_t = ((apu_pulse_1_t & 0x00FF) | ((val & 0x07) << 8));
				
				apu_pulse_1_l = apu_length[(val>>3)];
				
				apu_pulse_1_k = apu_pulse_1_t;
				
				break;
			}
			case 0x04: // pulse 2 volume
			{
				apu_pulse_2_c = ((val & 0x10) >> 4);
				
				if (apu_pulse_2_c > 0)
				{
					apu_pulse_2_v = (val & 0x0F);
				}
				else
				{
					apu_pulse_2_v = 0x0F;
				}
				
				apu_pulse_2_m = (val & 0x0F);
				
				apu_pulse_2_r = apu_pulse_2_m + 1;
				
				apu_pulse_2_i = ((val & 0x20) >> 5);
				
				apu_pulse_2_d = ((val & 0xC0) >> 6);
				
				break;
			}
			case 0x05: // pulse 2 sweep
			{
				apu_pulse_2_s = (val & 0x07);
				
				apu_pulse_2_n = ((val & 0x08) >> 3);
				
				apu_pulse_2_p = ((val & 0x70) >> 4);
				
				apu_pulse_2_w = apu_pulse_2_p + 1;
				
				apu_pulse_2_e = ((val & 0x80) >> 7);

				break;
			}
			case 0x06: // pulse 2 timer
			{
				apu_pulse_2_t = ((apu_pulse_2_t & 0x0700) | val);
				
				apu_pulse_2_k = apu_pulse_2_t;
				
				break;
			}
			case 0x07: // pulse 2 length
			{
				apu_pulse_2_t = ((apu_pulse_2_t & 0x00FF) | ((val & 0x07) << 8));
				
				apu_pulse_2_l = apu_length[(val>>3)];
				
				apu_pulse_2_k = apu_pulse_2_t;
				
				break;
			}
			case 0x08: // triangle linear
			{
				apu_triangle_r = (val & 0x7F);
				
				apu_triangle_v = apu_triangle_r;
				
				apu_triangle_c = ((val & 0x80) >> 7);
				
				break;
			}
			case 0x09: // triangle unused
			{
				break;
			}
			case 0x0A: // triangle timer
			{
				apu_triangle_t = ((apu_triangle_t & 0x0700) | val);
				
				apu_triangle_k = apu_triangle_t;
				
				break;
			}
			case 0x0B: // triangle length
			{
				apu_triangle_t = ((apu_triangle_t & 0x00FF) | ((val & 0x07) << 8));
				
				apu_triangle_l = apu_length[(val>>3)] + 1;
				
				apu_triangle_k = apu_triangle_t;
				
				apu_triangle_f = 0x0001;
				
				break;
			}
			case 0x0C: // noise volume
			{
				apu_noise_c = ((val & 0x10) >> 4);
				
				if (apu_noise_c > 0)
				{
					apu_noise_v = (val & 0x0F);
				}
				else
				{
					apu_noise_v = 0x0F;
				}
				
				apu_noise_m = (val & 0x0F);
				
				apu_noise_r = apu_noise_m + 1;
				
				apu_noise_i = ((val & 0x20) >> 5);
				
				break;
			}
			case 0x0D: // noise unused
			{
				
				break;
			}
			case 0x0E: // noise period
			{
				apu_noise_d = ((val & 0x80) >> 7);
				
				apu_noise_t = apu_period[(val & 0x0F)];
				
				apu_noise_k = apu_noise_t;
				
				break;
			}
			case 0x0F: // noise length
			{
				apu_noise_l = apu_length[(val>>3)];
				
				break;
			}
			case 0x10: // DMC frequency
			{
				apu_dmc_i = (val >> 7);
				apu_dmc_l = ((val >> 6) & 0x01);
				apu_dmc_r = apu_rate[(val & 0x0F)];

				apu_dmc_k = apu_dmc_r;
				
				if (apu_dmc_i == 0) apu_flag_i = 0;
				
				break;
			}
			case 0x11: // DMC load
			{	
				apu_dmc_d = (val & 0x7F);
				
				break;
			}
			case 0x12: // DMC address
			{
				apu_dmc_a = 0xC000 + ((unsigned short)val << 6); 
				
				break;
			}
			case 0x13: // DMC length
			{
				apu_dmc_s = (val << 4) + 1;
				
				break;
			}
			case 0x14: // oam dma
			{
				if ((cpu_current_cycles&0x00000002) == 0x00000000) cpu_dma_cycles = 513; // 513 cycles on even
				else cpu_dma_cycles = 514; // 514 cycles on odd
				
				for (unsigned long loop=0; loop<256; loop++)
				{
					nes_write_oam_ram(loop, cpu_read(loop+((unsigned long)val<<8))); // perhaps read from internal ram directly to make faster?
				}
				
				break;
			}
			case 0x15: // apu status
			{	
				apu_flag_d = ((val>>4)&0x01);
				apu_flag_n = ((val>>3)&0x01);
				apu_flag_t = ((val>>2)&0x01);
				apu_flag_2 = ((val>>1)&0x01);
				apu_flag_1 = (val&0x01);
				
				if (apu_flag_d == 0) apu_dmc_s = 0;
				if (apu_flag_n == 0) apu_noise_l = 0;
				if (apu_flag_t == 0) apu_triangle_l = 0;
				if (apu_flag_2 == 0) apu_pulse_2_l = 0;
				if (apu_flag_1 == 0) apu_pulse_1_l = 0;
				
				apu_flag_i = 0;
				
				break;
			}
			case 0x16: // controller 1
			{
				if (ctl_flag_s > 0 && val == 0x00)
				{
					ctl_latch_1 = ctl_value_1;
					ctl_latch_2 = ctl_value_2;
				}
					
				ctl_flag_s = (val&0x01);
				
				break;
			}
			case 0x17: // apu frame counter
			{
				apu_flag_m = ((val>>7)&0x01);
				apu_flag_b = ((val>>6)&0x01);
				
				if (apu_flag_b > 0) apu_flag_f = 0x0000;
				
				break;
			}
			default:
			{
				break;
			}
		}
	}
	else if (addr < 0x00004020) // disabled apu and i/o
	{
	} 
	else if (addr < 0x00006000) // unmapped
	{
	}
	else if (addr < 0x00008000) // cart ram
	{
		//if (cpu_status_r > 0)
		//{
			if (map_number == 0x0001)
			{
				if (map_mmc1_ram == 0)
				{
					nes_write_prg_ram(addr-0x6000, val);
				}
			}
			else if (map_number == 0x0004)
			{
				if (map_mmc3_ram == 1)
				{
					nes_write_prg_ram(addr-0x6000, val);
				}
			}
			else
			{
				nes_write_prg_ram(addr-0x6000, val);
			}
		//}
	}
	else if (addr < 0x00010000) // cart rom and mapper
	{
		if (map_number == 0x0001) // mmc1
		{
			if ((val & 0x80) != 0x00)
			{
				map_mmc1_shift = 0x0000;
				map_mmc1_count = 0x0000;
				map_mmc1_prg_mode = 0x0003;
				map_mmc1_chr_mode = 0x0000;
				map_mmc1_chr_bank_0 = 0x0010;
				map_mmc1_chr_bank_1 = 0x0000;
				map_mmc1_prg_bank = 0x0000;
				map_mmc1_ram = 0x0000;
			}
			else
			{
				map_mmc1_shift = ((map_mmc1_shift >> 1) | (((unsigned long)val & 0x01) << 4));
				map_mmc1_count = map_mmc1_count + 1;
				
				if (map_mmc1_count >= 5)
				{
					map_mmc1_count = 0;
				
					if (addr < 0x0000A000) // control
					{
						switch ((map_mmc1_shift & 0x03)) // nametable
						{
							case 0x00:
							{
								ppu_status_m = 0x0002; // single, first bank
								
								break;
							}
							case 0x01:
							{
								ppu_status_m = 0x0003; // single, second bank
								
								break;
							}
							case 0x02:
							{
								ppu_status_m = 0x0001; // horizontal scrolling
								
								break;
							}
							case 0x03:
							{	
								ppu_status_m = 0x0000; // vertical scrolling
								
								break;
							}
						}
						
						map_mmc1_prg_mode = ((map_mmc1_shift & 0x0C) >> 2);
						map_mmc1_chr_mode = ((map_mmc1_shift & 0x10) >> 4);
					}
					else if (addr < 0x0000C000) // chr bank 0
					{
						map_mmc1_chr_bank_0 = (map_mmc1_shift & 0x1F);
					}
					else if (addr < 0x0000E000) // chr bank 1
					{
						map_mmc1_chr_bank_1 = (map_mmc1_shift & 0x1F);
					}
					else // prg bank
					{	
						map_mmc1_prg_bank = (map_mmc1_shift & 0x0F);
						map_mmc1_ram = ((map_mmc1_shift & 0x10) >> 4);
					}
					
					map_mmc1_shift = 0x0000;
				}
			}
		}
		else if (map_number == 0x0002) // unrom
		{
			map_unrom_bank = ((unsigned long)val & 0x0F);
		}
		else if (map_number == 0x0003) // cnrom
		{
			map_cnrom_bank = ((unsigned long)val & 0x03);
		}
		else if (map_number == 0x0004) // mmc3
		{
			if (addr < 0x0000A000) // banks
			{
				if ((addr & 0x00000001) == 0x00000000) // even
				{
					map_mmc3_bank_next = ((unsigned long)val & 0x07);
					map_mmc3_prg_mode = (((unsigned long)val & 0x40) >> 6);
					map_mmc3_chr_mode = (((unsigned long)val & 0x80) >> 7);
				}
				else // odd
				{
					switch (map_mmc3_bank_next)
					{
						case 0x00:
						{
							map_mmc3_bank_r0 = (((unsigned long)val & 0xFE) >> 1);
							break;
						}
						case 0x01:
						{
							map_mmc3_bank_r1 = (((unsigned long)val & 0xFE) >> 1);
							break;
						}
						case 0x02:
						{
							map_mmc3_bank_r2 = (unsigned long)val;
							break;
						}
						case 0x03:
						{
							map_mmc3_bank_r3 = (unsigned long)val;
							break;
						}
						case 0x04:
						{
							map_mmc3_bank_r4 = (unsigned long)val;
							break;
						}
						case 0x05:
						{
							map_mmc3_bank_r5 = (unsigned long)val;
							break;
						}
						case 0x06:
						{
							map_mmc3_bank_r6 = ((unsigned long)val & 0x3F);
							break;
						}
						case 0x07:
						{
							map_mmc3_bank_r7 = ((unsigned long)val & 0x3F);
							break;
						}
					}
				}
			}
			else if (addr < 0x0000C000) // mirroring and ram enable
			{
				if ((addr & 0x00000001) == 0x00000000) // even
				{
					if (((unsigned long)val & 0x01) == 0x00)
					{
						ppu_status_m = 0x0001; // horizontal scrolling
					}
					else
					{
						ppu_status_m = 0x0000; // vertical scrolling
					}
				}
				else // odd
				{
					map_mmc3_ram = (((unsigned long)val & 0x80) >> 7);
				}
			}
			else if (addr < 0x0000E000) // irq values
			{
				if ((addr & 0x00000001) == 0x00000000) // even
				{
					map_mmc3_irq_latch = (unsigned long)val;
				}
				else // odd
				{
					map_mmc3_irq_counter = 0; // will reload later
				}
			}
			else // irq enable
			{
				if ((addr & 0x00000001) == 0x00000000) // even
				{
					map_mmc3_irq_enable = 0x0000; // disable
				}
				else // odd
				{
					map_mmc3_irq_enable = 0x0001; // enable
				}
			}
		}
		else if (map_number == 0x0007) // anrom
		{
			map_anrom_bank = ((unsigned long)val & 0x07);
			
			if (((unsigned long)val & 0x10) == 0x00)
			{
				ppu_status_m = 0x0002;
			}
			else
			{
				ppu_status_m = 0x0003;
			}
		}
	}
	else
	{
		cpu_write((addr & 0x0000FFFF), val);
	}
}

// cpu addressing modes
#define CPU_IMM { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_reg_pc++); }

#define CPU_ZPR { \
	cpu_temp_memory = (unsigned long)(cpu_ram[(unsigned long)cpu_read(cpu_reg_pc++)]&0x00FF); }

#define CPU_ZPW { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); }

#define CPU_ZPM { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); }

#define CPU_ZPXR { \
	cpu_temp_memory = (unsigned long)(cpu_ram[(((unsigned long)cpu_read(cpu_reg_pc++)+cpu_reg_x)&0x00FF)]&0x00FF); }

#define CPU_ZPXW { \
	cpu_temp_address = (((unsigned long)cpu_read(cpu_reg_pc++)+cpu_reg_x)&0x00FF); }

#define CPU_ZPXM { \
	cpu_temp_address = (((unsigned long)cpu_read(cpu_reg_pc++)+cpu_reg_x)&0x00FF); }

#define CPU_ZPYR { \
	cpu_temp_memory = (unsigned long)(cpu_ram[(((unsigned long)cpu_read(cpu_reg_pc++)+cpu_reg_y)&0x00FF)]&0x00FF); }

#define CPU_ZPYW { \
	cpu_temp_address = (((unsigned long)cpu_read(cpu_reg_pc++)+cpu_reg_y)&0x00FF); }

#define CPU_ABSR { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); }

#define CPU_ABSW { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8); }
	
#define CPU_ABSM { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8); }

#define CPU_ABXR { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_cycles += ((cpu_temp_address+cpu_reg_x)>>8); \
	cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_x; \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); }

#define CPU_ABXW { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_cycles += ((cpu_temp_address+cpu_reg_x)>>8); \
	cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_x; }
	
#define CPU_ABXM { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_x; }
	
#define CPU_ABYR { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_cycles += ((cpu_temp_address+cpu_reg_y)>>8); \
	cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_y; \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); }

#define CPU_ABYW { \
	cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_cycles += ((cpu_temp_address+cpu_reg_y)>>8); \
	cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_y; }
	
#define CPU_INDXR { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_address = (unsigned long)cpu_ram[((cpu_temp_memory+cpu_reg_x)&0x00FF)]+((unsigned long)cpu_ram[((cpu_temp_memory+cpu_reg_x+1)&0x00FF)]<<8); \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); }

#define CPU_INDXW { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_address = (unsigned long)cpu_ram[((cpu_temp_memory+cpu_reg_x)&0x00FF)]+((unsigned long)cpu_ram[((cpu_temp_memory+cpu_reg_x+1)&0x00FF)]<<8); }
	
#define CPU_INDYR { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_address = (unsigned long)cpu_ram[cpu_temp_memory]+((unsigned long)cpu_ram[((cpu_temp_memory+1)&0x00FF)]<<8); \
	cpu_temp_cycles += (((cpu_temp_address&0x00FF)+cpu_reg_y)>>8); \
	cpu_temp_address += cpu_reg_y; \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); }

#define CPU_INDYW { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_reg_pc++); \
	cpu_temp_address = (unsigned long)cpu_ram[cpu_temp_memory]+((unsigned long)cpu_ram[((cpu_temp_memory+1)&0x00FF)]<<8); \
	cpu_temp_address += cpu_reg_y; }
	
// instructions
#define CPU_ADC { \
	cpu_temp_result = cpu_reg_a+cpu_temp_memory+cpu_flag_c; \
	cpu_flag_c = (cpu_temp_result>0x00FF); \
	cpu_temp_result = (cpu_temp_result&0x00FF); \
	cpu_flag_z = (cpu_temp_result==0x0000); \
	cpu_flag_v = !(((cpu_reg_a^cpu_temp_result)&0x80)&&((cpu_reg_a^cpu_temp_memory)&0x80)); \
	cpu_flag_n = (cpu_temp_result>>7); \
	cpu_reg_a = cpu_temp_result; }
	
#define CPU_AND { \
	cpu_reg_a = (cpu_reg_a&cpu_temp_memory); \
	cpu_flag_z = (cpu_reg_a==0x0000); \
	cpu_flag_n = (cpu_reg_a>>7); }
	
#define CPU_ASL { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); \
	cpu_flag_c = (cpu_temp_memory>>7); \
	cpu_temp_memory = ((cpu_temp_memory<<1)&0x00FF); \
	cpu_write(cpu_temp_address,(unsigned char)(cpu_temp_memory & 0x00FF)); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }
	
#define CPU_BIT { \
	cpu_flag_v = ((cpu_temp_memory>>6)&0x01); \
	cpu_flag_n = (cpu_temp_memory>>7); \
	cpu_temp_memory = (cpu_reg_a&cpu_temp_memory); \
	cpu_flag_z = (cpu_temp_memory == 0x0000); }
	
#define CPU_CMP { \
	cpu_flag_c = (cpu_reg_a>=cpu_temp_memory); \
	cpu_flag_z = (cpu_reg_a==cpu_temp_memory); \
	cpu_temp_memory = ((cpu_reg_a-cpu_temp_memory)&0x00FF); \
	cpu_flag_n = (cpu_temp_memory>>7); }
	
#define CPU_CPX { \
	cpu_flag_c = (cpu_reg_x>=cpu_temp_memory); \
	cpu_flag_z = (cpu_reg_x==cpu_temp_memory); \
	cpu_temp_memory = ((cpu_reg_x-cpu_temp_memory)&0x00FF); \
	cpu_flag_n = (cpu_temp_memory>>7); }
	
#define CPU_CPY { \
	cpu_flag_c = (cpu_reg_y>=cpu_temp_memory); \
	cpu_flag_z = (cpu_reg_y==cpu_temp_memory); \
	cpu_temp_memory = ((cpu_reg_y-cpu_temp_memory)&0x00FF); \
	cpu_flag_n = (cpu_temp_memory>>7); }
	
#define CPU_DEC { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); \
	cpu_temp_memory = ((cpu_temp_memory-1)&0x00FF); \
	cpu_write(cpu_temp_address,(unsigned char)(cpu_temp_memory&0x00FF)); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }

#define CPU_EOR { \
	cpu_reg_a = (cpu_reg_a^cpu_temp_memory); \
	cpu_flag_z = (cpu_reg_a==0x0000); \
	cpu_flag_n = (cpu_reg_a>>7); }

#define CPU_INC { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); \
	cpu_temp_memory = ((cpu_temp_memory+1)&0x00FF); \
	cpu_write(cpu_temp_address,(unsigned char)(cpu_temp_memory&0x00FF)); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }

#define CPU_LDA { \
	cpu_reg_a = cpu_temp_memory; \
	cpu_flag_z = (cpu_reg_a==0x0000); \
	cpu_flag_n = (cpu_reg_a>>7); }

#define CPU_LDX { \
	cpu_reg_x = cpu_temp_memory; \
	cpu_flag_z = (cpu_reg_x==0x0000); \
	cpu_flag_n = (cpu_reg_x>>7); }

#define CPU_LDY { \
	cpu_reg_y = cpu_temp_memory; \
	cpu_flag_z = (cpu_reg_y==0x0000); \
	cpu_flag_n = (cpu_reg_y>>7); }

#define CPU_LSR { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); \
	cpu_flag_c = (cpu_temp_memory&0x01); \
	cpu_temp_memory = (cpu_temp_memory>>1); \
	cpu_write(cpu_temp_address,(unsigned char)(cpu_temp_memory&0x00FF)); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }

#define CPU_ORA { \
	cpu_reg_a = (cpu_reg_a|cpu_temp_memory); \
	cpu_flag_z = (cpu_reg_a==0x0000); \
	cpu_flag_n = (cpu_reg_a>>7); }

#define CPU_ROL { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); \
	cpu_temp_memory = (((cpu_temp_memory<<1)&0x01FF)|cpu_flag_c); \
	cpu_flag_c = (cpu_temp_memory>>8); \
	cpu_temp_memory = (cpu_temp_memory&0x00FF); \
	cpu_write(cpu_temp_address,(unsigned char)(cpu_temp_memory&0x00FF)); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }

#define CPU_ROR { \
	cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address); \
	cpu_temp_memory = (cpu_temp_memory|(cpu_flag_c<<8)); \
	cpu_flag_c = (cpu_temp_memory&0x01); \
	cpu_temp_memory = (cpu_temp_memory>>1); \
	cpu_write(cpu_temp_address,(unsigned char)(cpu_temp_memory&0x00FF)); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }

#define CPU_SBC { \
	cpu_temp_result = cpu_reg_a-cpu_temp_memory-(0x00001-cpu_flag_c); \
	cpu_flag_c = (0x0001-((cpu_temp_result&0x8000)>>15)); \
	cpu_temp_result = (cpu_temp_result&0x00FF); \
	cpu_flag_z = (cpu_temp_result==0x0000); \
	cpu_flag_v = (((cpu_reg_a^cpu_temp_result)&0x80)&&((cpu_reg_a^cpu_temp_memory)&0x80)); \
	cpu_flag_n = (cpu_temp_result>>7); \
	cpu_reg_a = cpu_temp_result; }

#define CPU_STA { \
	cpu_write(cpu_temp_address, (unsigned char)(cpu_reg_a&0x00FF)); }

#define CPU_STX { \
	cpu_write(cpu_temp_address, (unsigned char)(cpu_reg_x&0x00FF)); }

#define CPU_STY { \
	cpu_write(cpu_temp_address, (unsigned char)(cpu_reg_y&0x00FF)); }

// internal functions
#define CPU_BRA { \
	cpu_temp_address = cpu_reg_pc; \
	if (cpu_temp_memory > 127) cpu_reg_pc = (unsigned long)((cpu_reg_pc + cpu_temp_memory - 256) & 0x0000FFFF); \
	else cpu_reg_pc = (unsigned long)((cpu_reg_pc + cpu_temp_memory) & 0x0000FFFF); \
	cpu_temp_cycles += ((cpu_temp_address&0xFF00)!=(cpu_reg_pc&0xFF00)); }

#define CPU_PUSH { \
	cpu_ram[0x0100+(cpu_reg_s&0x00FF)]=cpu_temp_memory; \
	cpu_reg_s=((cpu_reg_s-1)&0x00FF); }

#define CPU_PULL { \
	cpu_reg_s=((cpu_reg_s+1)&0x00FF); \
	cpu_temp_memory=cpu_ram[(0x0100+(cpu_reg_s&0x00FF))]; }


unsigned long cpu_run()
{	
	cpu_temp_opcode = (unsigned long)cpu_read(cpu_reg_pc++);
	
	//debug_opcode = cpu_temp_opcode;
	//debug_operand_1 = cpu_read(cpu_reg_pc);
	//debug_operand_2 = cpu_read(cpu_reg_pc+1);
	
	switch (cpu_temp_opcode)
	{
		// ADC
		case 0x69: { cpu_temp_cycles = 2; CPU_IMM; CPU_ADC; break; }
		case 0x65: { cpu_temp_cycles = 3; CPU_ZPR; CPU_ADC; break; }
		case 0x75: { cpu_temp_cycles = 4; CPU_ZPXR; CPU_ADC; break; }
		case 0x6D: { cpu_temp_cycles = 4; CPU_ABSR; CPU_ADC; break; }
		case 0x7D: { cpu_temp_cycles = 4; CPU_ABXR; CPU_ADC; break; }
		case 0x79: { cpu_temp_cycles = 4; CPU_ABYR; CPU_ADC; break; }
		case 0x61: { cpu_temp_cycles = 6; CPU_INDXR; CPU_ADC; break; }
		case 0x71: { cpu_temp_cycles = 5; CPU_INDYR; CPU_ADC; break; }
		
		// AND
		case 0x29: { cpu_temp_cycles = 2; CPU_IMM; CPU_AND; break; }
		case 0x25: { cpu_temp_cycles = 3; CPU_ZPR; CPU_AND; break; }
		case 0x35: { cpu_temp_cycles = 4; CPU_ZPXR; CPU_AND; break; }
		case 0x2D: { cpu_temp_cycles = 4; CPU_ABSR; CPU_AND; break; }
		case 0x3D: { cpu_temp_cycles = 4; CPU_ABXR; CPU_AND; break; }
		case 0x39: { cpu_temp_cycles = 4; CPU_ABYR; CPU_AND; break; }
		case 0x21: { cpu_temp_cycles = 6; CPU_INDXR; CPU_AND; break; }
		case 0x31: { cpu_temp_cycles = 5; CPU_INDYR; CPU_AND; break; }
		
		// ASL
		case 0x0A:
		{
			cpu_temp_cycles = 0x0002;
			cpu_flag_c = (cpu_reg_a>>7);
			cpu_reg_a = ((cpu_reg_a<<1)&0x00FF);
			cpu_flag_z = (cpu_reg_a==0x0000);
			cpu_flag_n = (cpu_reg_a>>7);
			break;
		}
		case 0x06: { cpu_temp_cycles = 5; CPU_ZPM; CPU_ASL; break; }
		case 0x16: { cpu_temp_cycles = 6; CPU_ZPXM; CPU_ASL; break; }
		case 0x0E: { cpu_temp_cycles = 6; CPU_ABSM; CPU_ASL; break; }
		case 0x1E: { cpu_temp_cycles = 7; CPU_ABXM; CPU_ASL; break; }
		
		// BCC
		case 0x90: { cpu_temp_cycles = 2; CPU_IMM; 
			if (cpu_flag_c == 0x0000) { cpu_temp_cycles = 3; CPU_BRA; } break; }
		// BCS
		case 0xB0: { cpu_temp_cycles = 2; CPU_IMM; 
			if (cpu_flag_c != 0x0000) { cpu_temp_cycles = 3; CPU_BRA; } break; }
		// BEQ
		case 0xF0: { cpu_temp_cycles = 2; CPU_IMM; 
			if (cpu_flag_z != 0x0000) { cpu_temp_cycles = 3; CPU_BRA; } break; }
		
		// BIT
		case 0x24: { cpu_temp_cycles = 3; CPU_ZPR; CPU_BIT; break; }
		case 0x2C: { cpu_temp_cycles = 4; CPU_ABSR; CPU_BIT; break; }
	
		// BMI
		case 0x30: { cpu_temp_cycles = 2; CPU_IMM; 
			if (cpu_flag_n != 0x0000) { cpu_temp_cycles = 3; CPU_BRA; } break; }
		// BNE
		case 0xD0: { cpu_temp_cycles = 2; CPU_IMM; 
			if (cpu_flag_z == 0x0000) { cpu_temp_cycles = 3; CPU_BRA; } break; }
		// BPL
		case 0x10: { cpu_temp_cycles = 2; CPU_IMM; 
			if (cpu_flag_n == 0x0000) { cpu_temp_cycles = 3; CPU_BRA; } break; }
		
		// BRK
		case 0x00:
		{
			//SendLongHex(cpu_reg_pc);
			//SendString("BRK\n\r\\");
			
			cpu_temp_cycles = 0x0007;
			cpu_temp_memory = ((cpu_reg_pc+1)>>8); // add one to PC
			CPU_PUSH;
			cpu_temp_memory = ((cpu_reg_pc+1)&0x00FF); // add one to PC
			CPU_PUSH;
			cpu_temp_memory = ((cpu_flag_n<<7)|(cpu_flag_v<<6)|
				(cpu_flag_d<<3)|(cpu_flag_i<<2)|(cpu_flag_z<<1)|cpu_flag_c); //|0x30);
			CPU_PUSH;
			cpu_reg_pc = (unsigned long)cpu_read(0xFFFE);
			cpu_reg_pc += ((unsigned long)cpu_read(0xFFFF)<<8);
			
			cpu_current_cycles += 7;
			
			cpu_flag_i = 1;
			
			break;
		}
		
		// BVC
		case 0x50:
		{
			cpu_temp_cycles = 0x0002;
			cpu_temp_memory = (unsigned long)cpu_read(cpu_reg_pc++);
			if (cpu_flag_v == 0x0000) { cpu_temp_cycles = 3; CPU_BRA; }
			break;
		}
		
		// BVS
		case 0x70:
		{
			cpu_temp_cycles = 0x0002;
			cpu_temp_memory = (unsigned long)cpu_read(cpu_reg_pc++);
			if (cpu_flag_v != 0x0000) { cpu_temp_cycles = 3; CPU_BRA; }
			break;
		}
		
		// CLC
		case 0x18:
		{
			cpu_temp_cycles = 0x0002;
			cpu_flag_c = 0x0000;
			break;
		}
		
		// CLD
		case 0xD8:
		{
			cpu_temp_cycles = 0x0002;
			cpu_flag_d = 0x0000;
			break;
		}
		
		// CLI
		case 0x58:
		{
			cpu_temp_cycles = 0x0002;
			cpu_flag_i = 0x0000;
			break;
		}
		
		// CLV
		case 0xB8:
		{
			cpu_temp_cycles = 0x0002;
			cpu_flag_v = 0x0000;
			break;
		}
		
		// CMP
		case 0xC9: { cpu_temp_cycles = 2; CPU_IMM; CPU_CMP; break; }
		case 0xC5: { cpu_temp_cycles = 3; CPU_ZPR; CPU_CMP; break; }
		case 0xD5: { cpu_temp_cycles = 4; CPU_ZPXR; CPU_CMP; break; }
		case 0xCD: { cpu_temp_cycles = 4; CPU_ABSR; CPU_CMP; break; }
		case 0xDD: { cpu_temp_cycles = 4; CPU_ABXR; CPU_CMP; break; }
		case 0xD9: { cpu_temp_cycles = 4; CPU_ABYR; CPU_CMP; break; }
		case 0xC1: { cpu_temp_cycles = 6; CPU_INDXR; CPU_CMP; break; }
		case 0xD1: { cpu_temp_cycles = 5; CPU_INDYR; CPU_CMP; break; }
		
		// CPX
		case 0xE0: { cpu_temp_cycles = 2; CPU_IMM; CPU_CPX; break; }
		case 0xE4: { cpu_temp_cycles = 3; CPU_ZPR; CPU_CPX; break; }
		case 0xEC: { cpu_temp_cycles = 4; CPU_ABSR; CPU_CPX; break; }
		
		// CPY
		case 0xC0: { cpu_temp_cycles = 2; CPU_IMM; CPU_CPY; break; }
		case 0xC4: { cpu_temp_cycles = 3; CPU_ZPR; CPU_CPY; break; }
		case 0xCC: { cpu_temp_cycles = 4; CPU_ABSR; CPU_CPY; break; }
		
		// DEC
		case 0xC6: { cpu_temp_cycles = 5; CPU_ZPM; CPU_DEC; break; }
		case 0xD6: { cpu_temp_cycles = 6; CPU_ZPXM; CPU_DEC; break; }
		case 0xCE: { cpu_temp_cycles = 6; CPU_ABSM; CPU_DEC; break; }
		case 0xDE: { cpu_temp_cycles = 7; CPU_ABXM; CPU_DEC; break; }
		
		// DEX
		case 0xCA:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_x = ((cpu_reg_x-1) & 0x00FF);
			cpu_flag_z = (cpu_reg_x == 0x0000);
			cpu_flag_n = (cpu_reg_x >> 7);
			break;
		}
		
		// DEY
		case 0x88:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_y = ((cpu_reg_y-1) & 0x00FF);
			cpu_flag_z = (cpu_reg_y == 0x0000);
			cpu_flag_n = (cpu_reg_y >> 7);
			break;
		}
		
		// EOR
		case 0x49: { cpu_temp_cycles = 2; CPU_IMM; CPU_EOR; break; }
		case 0x45: { cpu_temp_cycles = 3; CPU_ZPR; CPU_EOR; break; }
		case 0x55: { cpu_temp_cycles = 4; CPU_ZPXR; CPU_EOR; break; }
		case 0x4D: { cpu_temp_cycles = 4; CPU_ABSR; CPU_EOR; break; }
		case 0x5D: { cpu_temp_cycles = 4; CPU_ABXR; CPU_EOR; break; }
		case 0x59: { cpu_temp_cycles = 4; CPU_ABYR; CPU_EOR; break; }
		case 0x41: { cpu_temp_cycles = 6; CPU_INDXR; CPU_EOR; break; }
		case 0x51: { cpu_temp_cycles = 5; CPU_INDYR; CPU_EOR; break; }
		
		// INC
		case 0xE6: { cpu_temp_cycles = 5; CPU_ZPM; CPU_INC; break; }
		case 0xF6: { cpu_temp_cycles = 6; CPU_ZPXM; CPU_INC; break; }
		case 0xEE: { cpu_temp_cycles = 6; CPU_ABSM; CPU_INC; break; }
		case 0xFE: { cpu_temp_cycles = 7; CPU_ABXM; CPU_INC; break; }
		
		// INX
		case 0xE8:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_x = ((cpu_reg_x+1) & 0x00FF);
			cpu_flag_z = (cpu_reg_x == 0x0000);
			cpu_flag_n = (cpu_reg_x >> 7);
			break;
		}
		
		// INY
		case 0xC8:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_y = ((cpu_reg_y+1) & 0x00FF);
			cpu_flag_z = (cpu_reg_y == 0x0000);
			cpu_flag_n = (cpu_reg_y >> 7);
			break;
		}
		
		// JMP
		case 0x4C:
		{
			cpu_temp_cycles = 0x0003;
			cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++);
			cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8);
			cpu_reg_pc = cpu_temp_address;
			break;
		}
		case 0x6C:
		{
			cpu_temp_cycles = 0x0005;
			cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++);
			cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc++)<<8);
			cpu_temp_memory = (unsigned long)cpu_read(cpu_temp_address);
			cpu_temp_memory += (unsigned long)(cpu_read((cpu_temp_address&0xFF00)|(((cpu_temp_address&0x00FF)+1)&0x00FF))<<8);
			cpu_reg_pc = cpu_temp_memory;
			break;
		}
		
		// JSR
		case 0x20:
		{
			cpu_temp_cycles = 0x0006;
			cpu_temp_memory = ((cpu_reg_pc+1)>>8);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_reg_pc+1)&0x00FF);
			CPU_PUSH;
			cpu_temp_address = (unsigned long)cpu_read(cpu_reg_pc++);
			cpu_temp_address += ((unsigned long)cpu_read(cpu_reg_pc)<<8);
			cpu_reg_pc = cpu_temp_address;
			break;
		}
		
		// LDA
		case 0xA9: { cpu_temp_cycles = 2; CPU_IMM; CPU_LDA; break; }
		case 0xA5: { cpu_temp_cycles = 3; CPU_ZPR; CPU_LDA; break; }
		case 0xB5: { cpu_temp_cycles = 4; CPU_ZPXR; CPU_LDA; break; }
		case 0xAD: { cpu_temp_cycles = 4; CPU_ABSR; CPU_LDA; break; }
		case 0xBD: { cpu_temp_cycles = 4; CPU_ABXR; CPU_LDA; break; }
		case 0xB9: { cpu_temp_cycles = 4; CPU_ABYR; CPU_LDA; break; }
		case 0xA1: { cpu_temp_cycles = 6; CPU_INDXR; CPU_LDA; break; }
		case 0xB1: { cpu_temp_cycles = 5; CPU_INDYR; CPU_LDA; break; }
		
		// LDX
		case 0xA2: { cpu_temp_cycles = 2; CPU_IMM; CPU_LDX; break; }
		case 0xA6: { cpu_temp_cycles = 3; CPU_ZPR; CPU_LDX; break; }
		case 0xB6: { cpu_temp_cycles = 4; CPU_ZPYR; CPU_LDX; break; }
		case 0xAE: { cpu_temp_cycles = 4; CPU_ABSR; CPU_LDX; break; }
		case 0xBE: { cpu_temp_cycles = 4; CPU_ABYR; CPU_LDX; break; }
		
		// LDY
		case 0xA0: { cpu_temp_cycles = 2; CPU_IMM; CPU_LDY; break; }
		case 0xA4: { cpu_temp_cycles = 3; CPU_ZPR; CPU_LDY; break; }
		case 0xB4: { cpu_temp_cycles = 4; CPU_ZPXR; CPU_LDY; break; }
		case 0xAC: { cpu_temp_cycles = 4; CPU_ABSR; CPU_LDY; break; }
		case 0xBC: { cpu_temp_cycles = 4; CPU_ABXR; CPU_LDY; break; }
		
		// LSR
		case 0x4A:
		{
			cpu_temp_cycles = 0x0002;
			cpu_flag_c = (cpu_reg_a&0x01);
			cpu_reg_a = (cpu_reg_a>>1);
			cpu_flag_z = (cpu_reg_a==0x0000);
			cpu_flag_n = (cpu_reg_a>>7);
			break;
		}
		case 0x46: { cpu_temp_cycles = 5; CPU_ZPM; CPU_LSR; break; }
		case 0x56: { cpu_temp_cycles = 6; CPU_ZPXM; CPU_LSR; break; }
		case 0x4E: { cpu_temp_cycles = 6; CPU_ABSM; CPU_LSR; break; }
		case 0x5E: { cpu_temp_cycles = 7; CPU_ABXM; CPU_LSR; break; }
		
		// NOP
		case 0xEA:
		{
			cpu_temp_cycles = 0x0002;
			break;
		}
		
		// ORA
		case 0x09: { cpu_temp_cycles = 2; CPU_IMM; CPU_ORA; break; }
		case 0x05: { cpu_temp_cycles = 3; CPU_ZPR; CPU_ORA; break; }
		case 0x15: { cpu_temp_cycles = 4; CPU_ZPXR; CPU_ORA; break; }
		case 0x0D: { cpu_temp_cycles = 4; CPU_ABSR; CPU_ORA; break; }
		case 0x1D: { cpu_temp_cycles = 4; CPU_ABXR; CPU_ORA; break; }
		case 0x19: { cpu_temp_cycles = 4; CPU_ABYR; CPU_ORA; break; }
		case 0x01: { cpu_temp_cycles = 6; CPU_INDXR; CPU_ORA; break; }
		case 0x11: { cpu_temp_cycles = 5; CPU_INDYR; CPU_ORA; break; }
		
		// PHA
		case 0x48:
		{
			cpu_temp_cycles = 0x0003;
			cpu_temp_memory = cpu_reg_a;
			CPU_PUSH;
			break;
		}
		
		// PHP
		case 0x08:
		{
			cpu_temp_cycles = 0x0003;
			cpu_temp_memory = ((cpu_flag_n<<7)|(cpu_flag_v<<6)|
				(cpu_flag_d<<3)|(cpu_flag_i<<2)|(cpu_flag_z<<1)|cpu_flag_c); //|0x30);
			CPU_PUSH;
			break;
		}
		
		// PLA
		case 0x68:
		{
			cpu_temp_cycles = 0x0004;
			CPU_PULL;
			cpu_reg_a = cpu_temp_memory;
			cpu_flag_z = (cpu_reg_a==0);
			cpu_flag_n = (cpu_reg_a>>7);
			break;
		}
		
		// PLP
		case 0x28:
		{
			cpu_temp_cycles = 0x0004;
			CPU_PULL;
			cpu_flag_n = (cpu_temp_memory>>7);
			cpu_flag_v = ((cpu_temp_memory>>6)&0x01);
			cpu_flag_d = ((cpu_temp_memory>>3)&0x01);
			cpu_flag_i = ((cpu_temp_memory>>2)&0x01);
			cpu_flag_z = ((cpu_temp_memory>>1)&0x01);
			cpu_flag_c = (cpu_temp_memory&0x01);
			break;
		}
		
		// ROL
		case 0x2A:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_a = (((cpu_reg_a<<1)&0x01FF)|cpu_flag_c);
			cpu_flag_c = (cpu_reg_a>>8);
			cpu_reg_a = (cpu_reg_a&0x00FF);
			cpu_flag_z = (cpu_reg_a==0x0000);
			cpu_flag_n = (cpu_reg_a>>7);
			break;
		}
		case 0x26: { cpu_temp_cycles = 5; CPU_ZPM; CPU_ROL; break; }
		case 0x36: { cpu_temp_cycles = 6; CPU_ZPXM; CPU_ROL; break; }
		case 0x2E: { cpu_temp_cycles = 6; CPU_ABSM; CPU_ROL; break; }
		case 0x3E: { cpu_temp_cycles = 7; CPU_ABXM; CPU_ROL; break; }

		// ROR
		case 0x6A:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_a = (cpu_reg_a|(cpu_flag_c<<8));
			cpu_flag_c = (cpu_reg_a&0x01);
			cpu_reg_a = (cpu_reg_a>>1);
			cpu_flag_z = (cpu_reg_a==0x0000);
			cpu_flag_n = (cpu_reg_a>>7);
			break;
		}
		case 0x66: { cpu_temp_cycles = 5; CPU_ZPM; CPU_ROR; break; }
		case 0x76: { cpu_temp_cycles = 6; CPU_ZPXM; CPU_ROR; break; }
		case 0x6E: { cpu_temp_cycles = 6; CPU_ABSM; CPU_ROR; break; }
		case 0x7E: { cpu_temp_cycles = 7; CPU_ABXM; CPU_ROR; break; }	
		
		// RTI
		case 0x40:
		{
			cpu_temp_cycles = 0x0006;
			CPU_PULL;
			cpu_flag_n = ((cpu_temp_memory>>7)&0x01);
			cpu_flag_v = ((cpu_temp_memory>>6)&0x01);
			cpu_flag_d = ((cpu_temp_memory>>3)&0x01);
			cpu_flag_i = ((cpu_temp_memory>>2)&0x01);
			cpu_flag_z = ((cpu_temp_memory>>1)&0x01);
			cpu_flag_c = (cpu_temp_memory&0x01);
			CPU_PULL;
			cpu_reg_pc = cpu_temp_memory;
			CPU_PULL;
			cpu_reg_pc += (cpu_temp_memory<<8);	
			
			//SendLongHex(cpu_reg_pc);
			//SendString("RTI\n\r\\");
			
			break;
		}
		
		// RTS
		case 0x60:
		{
			cpu_temp_cycles = 0x0006;
			CPU_PULL;
			cpu_reg_pc = cpu_temp_memory;
			CPU_PULL;
			cpu_reg_pc += (cpu_temp_memory<<8)+1;
			break;
		}
		
		// SBC
		case 0xE9: { cpu_temp_cycles = 2; CPU_IMM; CPU_SBC; break; }
		case 0xE5: { cpu_temp_cycles = 3; CPU_ZPR; CPU_SBC; break; }
		case 0xF5: { cpu_temp_cycles = 4; CPU_ZPXR; CPU_SBC; break; }
		case 0xED: { cpu_temp_cycles = 4; CPU_ABSR; CPU_SBC; break; }
		case 0xFD: { cpu_temp_cycles = 4; CPU_ABXR; CPU_SBC; break; }
		case 0xF9: { cpu_temp_cycles = 4; CPU_ABYR; CPU_SBC; break; }
		case 0xE1: { cpu_temp_cycles = 6; CPU_INDXR; CPU_SBC; break; }
		case 0xF1: { cpu_temp_cycles = 5; CPU_INDYR; CPU_SBC; break; }
		
		// SEC
		case 0x38:
		{
			cpu_temp_cycles = 0x0002;
			cpu_flag_c = 0x0001;
			break;
		}
		
		// SED
		case 0xF8:
		{
			cpu_temp_cycles = 0x0002;
			cpu_flag_d = 0x0001;
			break;
		}
		
		// SEI
		case 0x78:
		{
			cpu_temp_cycles = 0x0002;
			cpu_flag_i = 0x0001;
			break;
		}
		
		// STA
		case 0x85: { CPU_ZPW; CPU_STA; cpu_temp_cycles = 3; break; }
		case 0x95: { CPU_ZPXW; CPU_STA; cpu_temp_cycles = 4; break; }
		case 0x8D: { CPU_ABSW; CPU_STA; cpu_temp_cycles = 4; break; }
		case 0x9D: { CPU_ABXW; CPU_STA; cpu_temp_cycles = 5; break; }
		case 0x99: { CPU_ABYW; CPU_STA; cpu_temp_cycles = 5; break; }
		case 0x81: { CPU_INDXW; CPU_STA; cpu_temp_cycles = 6; break; }
		case 0x91: { CPU_INDYW; CPU_STA; cpu_temp_cycles = 6; break; }
		
		// STX
		case 0x86: { CPU_ZPW; CPU_STX; cpu_temp_cycles = 3; break; }
		case 0x96: { CPU_ZPYW; CPU_STX; cpu_temp_cycles = 4; break; }
		case 0x8E: { CPU_ABSW; CPU_STX; cpu_temp_cycles = 4; break; }
		
		// STY
		case 0x84: { CPU_ZPW; CPU_STY; cpu_temp_cycles = 3; break; }
		case 0x94: { CPU_ZPXW; CPU_STY; cpu_temp_cycles = 4; break; }
		case 0x8C: { CPU_ABSW; CPU_STY; cpu_temp_cycles = 4; break; }
		
		// TAX
		case 0xAA:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_x = cpu_reg_a;
			cpu_flag_z = (cpu_reg_x==0);
			cpu_flag_n = (cpu_reg_x>>7);
			break;
		}
		
		// TAY
		case 0xA8:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_y = cpu_reg_a;
			cpu_flag_z = (cpu_reg_y==0);
			cpu_flag_n = (cpu_reg_y>>7);
			break;
		}
		
		// TSX
		case 0xBA:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_x = cpu_reg_s;
			cpu_flag_z = (cpu_reg_x==0);
			cpu_flag_n = (cpu_reg_x>>7);
			break;
		}
		
		// TXA
		case 0x8A:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_a = cpu_reg_x;
			cpu_flag_z = (cpu_reg_a==0);
			cpu_flag_n = (cpu_reg_a>>7);
			break;
		}
		
		// TXS
		case 0x9A:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_s = cpu_reg_x;
			cpu_flag_z = (cpu_reg_s==0);
			cpu_flag_n = (cpu_reg_s>>7);
			break;
		}
		
		// TYA
		case 0x98:
		{
			cpu_temp_cycles = 0x0002;
			cpu_reg_a = cpu_reg_y;
			cpu_flag_z = (cpu_reg_a==0);
			cpu_flag_n = (cpu_reg_a>>7);
			break;
		}
		
		default:
		{
			cpu_temp_cycles = 0x0000;
		}
	}
	
	return cpu_temp_cycles;
}

void nes_border()
{
	unsigned char pixel_color = 0;

	pixel_color = ppu_palette[pal_ram[0]];
	
	for (unsigned short y=8; y<232; y++) // remove overscan
	{
		for (unsigned short x=0; x<256; x++)
		{
			nes_pixel(x, y, pixel_color); // background color
		}
	}
}

void nes_background(signed short line)
{
	debug_location = 5;
	debug_value = line;
	
	unsigned long scroll_x = 0, scroll_y = 0;
	unsigned long scroll_n = 0;
	unsigned long scroll_t = 0, scroll_l = 0;
	
	unsigned long add_t = 0, add_l = 0;
	
	unsigned long pixel_lookup = 0, pixel_table = 0;
	unsigned long pixel_x = 0, pixel_y = 0;
	unsigned char pixel_high = 0, pixel_low = 0;
	unsigned char pixel_color = 0;
	
	if (line >= 0 && line < 8)
	{
		ppu_reg_r = ppu_reg_t;
	}
	else
	{
		ppu_reg_r = ((ppu_reg_r & 0x7BE0) | (ppu_reg_t & 0x041F));
	}
	
	if (nes_hack_bottom_hud > 0)
	{
		if ((map_number == 4 && ppu_scanline_interrupt > 0) || (map_number != 4 && ppu_scanline_sprite_0 > 0))
		{
			ppu_reg_r = ((ppu_reg_r & 0x041F) | 0x0800); // hack for Mario 3, Kirby, and Double Dragon
		}
	}
	
	if (ppu_flag_eb > 0)
	{
		pixel_y = line - ((ppu_reg_r & 0x7000) >> 12);
		
		if (pixel_y >= 8 && pixel_y < 232) // remove overscan above and below
		{
			//for (unsigned char w=0; w<33; w++) // go one more than 32
			for (unsigned char w=1; w<32; w++) // or remove far edges for speed
			{					
				scroll_x = (ppu_reg_r & 0x001F) + w;
				
				scroll_y = ((ppu_reg_r & 0x03E0) >> 5) + (line>>3);
				
				scroll_n = ((ppu_reg_r & 0x0C00) >> 10);

				switch (scroll_n)
				{
					case (0x00):
					{
						if (scroll_x >= 32)
						{
							scroll_n += 1;
							scroll_x -= 32;
						}
						
						if (scroll_y >= 30)
						{
							scroll_n += 2;
							scroll_y -= 30;
						}

						break;
					}
					case (0x01):
					{
						if (scroll_x >= 32)
						{
							scroll_n -= 1;
							scroll_x -= 32;
						}

						if (scroll_y >= 30)
						{
							scroll_n += 2;
							scroll_y -= 30;
						}

						break;
					}
					case (0x02):
					{
						if (scroll_x >= 32)
						{
							scroll_n += 1;
							scroll_x -= 32;
						}

						if (scroll_y >= 30)
						{
							scroll_n -= 2;
							scroll_y -= 30;
						}

						break;
					}
					case (0x03):
					{
						if (scroll_x >= 32)
						{
							scroll_n -= 1;
							scroll_x -= 32;
						}

						if (scroll_y >= 30)
						{
							scroll_n -= 2;
							scroll_y -= 30;
						}	

						break;
					}
				}
				
				scroll_t = (scroll_n<<10)+0x03C0+((scroll_y&0xFC)<<1)+((scroll_x&0xFC)>>2);
				
				scroll_l = (scroll_n<<10)+(scroll_y<<5)+(scroll_x);
				
				add_t = (((scroll_y&0x02)|((scroll_x>>1)&0x01))<<1);
				
				add_l = 0x1000*ppu_flag_b+(line&0x07);	
				
				if (ppu_status_m == 0x0001) // horizontal scrolling
				{
					if (scroll_t < 0x0800)
					{
						pixel_table = (ppu_ram[(scroll_t)]>>add_t);
					}
					else
					{
						pixel_table = (ppu_ram[(scroll_t-0x0800)]>>add_t);
					}
				}
				else if (ppu_status_m == 0x0000) // vertical scrolling
				{
					if (scroll_t < 0x0400)
					{
						pixel_table = (ppu_ram[(scroll_t)]>>add_t);
					}
					else if (scroll_t < 0x0800)
					{
						pixel_table = (ppu_ram[(scroll_t-0x0400)]>>add_t);
					}
					else if (scroll_t < 0x0C00)
					{
						pixel_table = (ppu_ram[(scroll_t-0x0400)]>>add_t);
					}
					else
					{
						pixel_table = (ppu_ram[(scroll_t-0x0800)]>>add_t);
					}
				}
				else if (ppu_status_m == 0x0002) // single, first bank
				{
					pixel_table = (ppu_ram[((scroll_t&0x03FF))]>>add_t);
				}
				else if (ppu_status_m == 0x0003) // single, second bank
				{
					pixel_table = (ppu_ram[((scroll_t&0x03FF)+0x0400)]>>add_t);
				}
				
				if (ppu_status_m == 0x0001) // horizontal scrolling
				{
					if (scroll_l < 0x0800)
					{
						pixel_lookup = (ppu_ram[(scroll_l)]<<4)+add_l;
					}
					else
					{
						pixel_lookup = (ppu_ram[(scroll_l-0x0800)]<<4)+add_l;
					}
				}
				else if (ppu_status_m == 0x0000) // vertical scrolling
				{
					if (scroll_l < 0x0400)
					{
						pixel_lookup = (ppu_ram[(scroll_l)]<<4)+add_l;
					}
					else if (scroll_l < 0x0800)
					{
						pixel_lookup = (ppu_ram[(scroll_l-0x0400)]<<4)+add_l;
					}
					else if (scroll_l < 0x0C00)
					{
						pixel_lookup = (ppu_ram[(scroll_l-0x0400)]<<4)+add_l;
					}
					else
					{
						pixel_lookup = (ppu_ram[(scroll_l-0x0800)]<<4)+add_l;
					}
				}
				else if (ppu_status_m == 0x0002) // single, first bank
				{
					pixel_lookup = (ppu_ram[((scroll_l&0x03FF))]<<4)+add_l;
				}
				else if (ppu_status_m == 0x0003) // single, second bank
				{
					pixel_lookup = (ppu_ram[((scroll_l&0x03FF)+0x0400)]<<4)+add_l;
				}
				
				if ((unsigned char)cart_rom[5] > 0)
				{
					if (map_number == 1) // mmc1
					{
						if (map_mmc1_chr_mode == 0) // 8KB
						{
							if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
							{
								pixel_lookup += 0x00001000*(map_mmc1_chr_bank_0&0x1E);
							}
							else
							{
								
							}
						}
						else if (map_mmc1_chr_mode == 1) // 4KB banked
						{
							if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
							{
								if (pixel_lookup < 0x1000)
								{
									pixel_lookup += 0x00001000*(map_mmc1_chr_bank_0);
								}
								else
								{
									pixel_lookup += 0x00001000*(map_mmc1_chr_bank_1);
									pixel_lookup -= 0x00001000;
								}
							}
							else
							{

							}
						}
					}
					else if (map_number == 3) // cnrom
					{
						pixel_lookup += 0x2000*map_cnrom_bank;
					}
					else if (map_number == 4) // mmc3
					{
						if (map_mmc3_chr_mode == 0x0000)
						{
							switch ((pixel_lookup&0xFC00))
							{
								case 0x0000:
								{
									pixel_lookup += 0x00000800*map_mmc3_bank_r0;
									break;
								}
								case 0x0400:
								{
									pixel_lookup += 0x00000800*map_mmc3_bank_r0;
									break;
								}
								case 0x0800:
								{
									pixel_lookup += 0x00000800*map_mmc3_bank_r1;
									pixel_lookup -= 0x0800;
									break;
								}
								case 0x0C00:
								{
									pixel_lookup += 0x00000800*map_mmc3_bank_r1;
									pixel_lookup -= 0x0800;
									break;
								}
								case 0x1000:
								{
									pixel_lookup += 0x00000400*map_mmc3_bank_r2;
									pixel_lookup -= 0x1000;
									break;
								}
								case 0x1400:
								{
									pixel_lookup += 0x00000400*map_mmc3_bank_r3;
									pixel_lookup -= 0x1400;
									break;
								}
								case 0x1800:
								{
									pixel_lookup += 0x00000400*map_mmc3_bank_r4;
									pixel_lookup -= 0x1800;
									break;
								}
								case 0x1C00:
								{
									pixel_lookup += 0x00000400*map_mmc3_bank_r5;
									pixel_lookup -= 0x1C00;
									break;
								}
							}
						}
						else
						{
							switch ((pixel_lookup&0xFC00))
							{
								case 0x0000:
								{
									pixel_lookup += 0x00000400*map_mmc3_bank_r2;
									break;
								}
								case 0x0400:
								{
									pixel_lookup += 0x00000400*map_mmc3_bank_r3;
									pixel_lookup -= 0x0400;
									break;
								}
								case 0x0800:
								{
									pixel_lookup += 0x00000400*map_mmc3_bank_r4;
									pixel_lookup -= 0x0800;
									break;
								}
								case 0x0C00:
								{
									pixel_lookup += 0x00000400*map_mmc3_bank_r5;
									pixel_lookup -= 0x0C00;
									break;
								}
								case 0x1000:
								{
									pixel_lookup += 0x00000800*map_mmc3_bank_r0;
									pixel_lookup -= 0x1000;
									break;
								}
								case 0x1400:
								{
									pixel_lookup += 0x00000800*map_mmc3_bank_r0;
									pixel_lookup -= 0x1000;
									break;
								}
								case 0x1800:
								{
									pixel_lookup += 0x00000800*map_mmc3_bank_r1;
									pixel_lookup -= 0x1800;
									break;
								}
								case 0x1C00:
								{
									pixel_lookup += 0x00000800*map_mmc3_bank_r1;
									pixel_lookup -= 0x1800;
									break;
								}
							}
						}
					}
					
					pixel_lookup += chr_offset;

					pixel_low = (unsigned char)cart_rom[(pixel_lookup)];
					pixel_high = (unsigned char)cart_rom[(pixel_lookup+8)];
				}
				else
				{
					pixel_low = chr_ram[(pixel_lookup)];
					pixel_high = chr_ram[(pixel_lookup+8)];
				}

				for (unsigned char i=0; i<8; i++)
				{
					pixel_color = ((pixel_high>>6)&0x02)|(pixel_low>>7);

					if (pixel_color != 0x00)
					{
						pixel_x = w * 8 + i - (ppu_reg_x & 0x07);

						if (pixel_x >= 0 && pixel_x < 256)
						{
							if (ppu_flag_lb > 0 || w*8+i >= 8) 
							{
								if (ppu_flag_g > 0)
								{
									nes_pixel(pixel_x, pixel_y, ppu_palette[(pal_ram[(((pixel_table&0x03)<<2)+pixel_color)]&0x30)]);
								}
								else
								{
									nes_pixel(pixel_x, pixel_y, ppu_palette[pal_ram[(((pixel_table&0x03)<<2)+pixel_color)]]);
								}
							}
						}
					}

					pixel_high = ((pixel_high << 1) & 0xFF);
					pixel_low = ((pixel_low << 1) & 0xFF);
				}
			}
		}
	}
}
	
void nes_sprites(unsigned char ground, unsigned long min_y, unsigned long max_y)
{
	debug_location = 6;
	debug_value = ground;
	
	unsigned long sprite_x = 0, sprite_y = 0;
	unsigned long sprite_attr = 0, sprite_tile = 0;
	unsigned char sprite_flip_horz = 0, sprite_flip_vert = 0;
	
	unsigned long pixel_x = 0, pixel_y = 0;
	unsigned long pixel_lookup = 0;
	unsigned char pixel_high = 0, pixel_low = 0;
	unsigned char pixel_color = 0;
	
	if (ppu_flag_es > 0)
	{
		for (signed char s=63; s>=0; s--) // must be signed!
		{
			sprite_x = oam_ram[(((s<<2)+3)&0x00FF)];
			sprite_y = oam_ram[(((s<<2)+0)&0x00FF)];

			if (sprite_x < 0xF9 && sprite_y < 0xEF && sprite_y >= min_y && sprite_y < max_y)
			{
				sprite_attr = oam_ram[(((s<<2)+2)&0x00FF)];

				if (((sprite_attr&0x20)>>5) == ground || ground > 1) // foreground/background
				{
					sprite_flip_horz = ((sprite_attr>>6)&0x01);
					sprite_flip_vert = (sprite_attr>>7);

					if (ppu_flag_h == 0) // 8x8 sprites
					{
						sprite_tile = oam_ram[(s<<2)+1];

						for (unsigned char j=0; j<8; j++)
						{
							if ((unsigned char)cart_rom[5] > 0)
							{
								pixel_lookup = sprite_tile*16+0x1000*ppu_flag_s+(sprite_flip_vert==0x00?j:7-j);
								
								if (map_number == 1) // mmc1
								{
									if (map_mmc1_chr_mode == 0) // 8KB
									{
										if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
										{
											pixel_lookup += 0x00001000*(map_mmc1_chr_bank_0&0x1E);
										}
										else
										{

										}
									}
									else if (map_mmc1_chr_mode == 1) // 4KB banked
									{
										if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
										{
											if (pixel_lookup < 0x1000)
											{
												pixel_lookup += 0x00001000*(map_mmc1_chr_bank_0);
											}
											else
											{
												pixel_lookup += 0x00001000*(map_mmc1_chr_bank_1);
												pixel_lookup -= 0x00001000;
											}
										}
										else
										{

										}
									}
								}
								else if (map_number == 3) // cnrom
								{
									pixel_lookup += 0x2000*map_cnrom_bank;
								}
								else if (map_number == 4) // mmc3
								{
									if (map_mmc3_chr_mode == 0x0000)
									{
										switch ((pixel_lookup&0xFC00))
										{
											case 0x0000:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r0;
												break;
											}
											case 0x0400:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r0;
												break;
											}
											case 0x0800:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r1;
												pixel_lookup -= 0x0800;
												break;
											}
											case 0x0C00:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r1;
												pixel_lookup -= 0x0800;
												break;
											}
											case 0x1000:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r2;
												pixel_lookup -= 0x1000;
												break;
											}
											case 0x1400:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r3;
												pixel_lookup -= 0x1400;
												break;
											}
											case 0x1800:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r4;
												pixel_lookup -= 0x1800;
												break;
											}
											case 0x1C00:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r5;
												pixel_lookup -= 0x1C00;
												break;
											}
										}
									}
									else
									{
										switch ((pixel_lookup&0xFC00))
										{
											case 0x0000:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r2;
												break;
											}
											case 0x0400:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r3;
												pixel_lookup -= 0x0400;
												break;
											}
											case 0x0800:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r4;
												pixel_lookup -= 0x0800;
												break;
											}
											case 0x0C00:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r5;
												pixel_lookup -= 0x0C00;
												break;
											}
											case 0x1000:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r0;
												pixel_lookup -= 0x1000;
												break;
											}
											case 0x1400:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r0;
												pixel_lookup -= 0x1000;
												break;
											}
											case 0x1800:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r1;
												pixel_lookup -= 0x1800;
												break;
											}
											case 0x1C00:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r1;
												pixel_lookup -= 0x1800;
												break;
											}
										}
									}
								}
								
								pixel_lookup += chr_offset;

								pixel_low = (unsigned char)cart_rom[(pixel_lookup)];
								pixel_high = (unsigned char)cart_rom[(pixel_lookup+8)];
							}
							else
							{
								pixel_lookup = sprite_tile*16+0x1000*ppu_flag_s+(sprite_flip_vert==0x00?j:7-j);

								pixel_low = chr_ram[(pixel_lookup)];
								pixel_high = chr_ram[(pixel_lookup+8)];
							}

							for (unsigned char i=0; i<8; i++)
							{
								if (sprite_flip_horz == 0x00) pixel_color = ((pixel_high>>6)&0x02)|(pixel_low>>7);
								else pixel_color = ((pixel_high<<1)&0x02)|(pixel_low&0x01);
								
								if (pixel_color != 0x00)
								{
									pixel_x = sprite_x+i;
									pixel_y = sprite_y+j+1;
									
									if (ppu_flag_ls > 0 || pixel_x >= 8) 
									{
										if (pixel_x >= 0 && pixel_x < 256 && pixel_y >= 8 && pixel_y < 232)
										{
											if (ppu_flag_g > 0)
											{
												nes_pixel(pixel_x, pixel_y, ppu_palette[(pal_ram[(0x0010+((sprite_attr&0x03)<<2)+pixel_color)]&0x30)]);
											}
											else
											{
												nes_pixel(pixel_x, pixel_y, ppu_palette[pal_ram[(0x0010+((sprite_attr&0x03)<<2)+pixel_color)]]);
											}
										}
									}
								}

								if (sprite_flip_horz == 0x00)
								{
									pixel_high = pixel_high << 1;
									pixel_low = pixel_low << 1;
								}
								else
								{
									pixel_high = pixel_high >> 1;
									pixel_low = pixel_low >> 1;
								}
							}
						}
					}
					else // 8x16 sprites
					{
						sprite_tile = (oam_ram[(s<<2)+1] & 0xFE);

						for (unsigned char j=0; j<16; j++)
						{
							if ((unsigned char)cart_rom[5] > 0)
							{
								pixel_lookup = sprite_tile*16+0x1000*(oam_ram[(((s<<2)+1)&0x00FF)]&0x01)+(sprite_flip_vert==0x00?j:15-j);

								if (map_number == 1) // mmc1
								{
									if (map_mmc1_chr_mode == 0) // 8KB
									{
										if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
										{
											pixel_lookup += 0x00001000*(map_mmc1_chr_bank_0&0x1E);
										}
										else
										{

										}
									}
									else if (map_mmc1_chr_mode == 1) // 4KB banked
									{
										if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
										{
											if (pixel_lookup < 0x1000)
											{
												pixel_lookup += 0x00001000*(map_mmc1_chr_bank_0);
											}
											else
											{
												pixel_lookup += 0x00001000*(map_mmc1_chr_bank_1);
												pixel_lookup -= 0x00001000;
											}
										}
										else
										{

										}
									}
								}
								else if (map_number == 3) // cnrom
								{
									pixel_lookup += 0x2000*map_cnrom_bank;
								}
								else if (map_number == 4) // mmc3
								{
									if (map_mmc3_chr_mode == 0x0000)
									{
										switch ((pixel_lookup&0xFC00))
										{
											case 0x0000:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r0;
												break;
											}
											case 0x0400:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r0;
												break;
											}
											case 0x0800:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r1;
												pixel_lookup -= 0x0800;
												break;
											}
											case 0x0C00:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r1;
												pixel_lookup -= 0x0800;
												break;
											}
											case 0x1000:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r2;
												pixel_lookup -= 0x1000;
												break;
											}
											case 0x1400:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r3;
												pixel_lookup -= 0x1400;
												break;
											}
											case 0x1800:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r4;
												pixel_lookup -= 0x1800;
												break;
											}
											case 0x1C00:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r5;
												pixel_lookup -= 0x1C00;
												break;
											}
										}
									}
									else
									{
										switch ((pixel_lookup&0xFC00))
										{
											case 0x0000:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r2;
												break;
											}
											case 0x0400:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r3;
												pixel_lookup -= 0x0400;
												break;
											}
											case 0x0800:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r4;
												pixel_lookup -= 0x0800;
												break;
											}
											case 0x0C00:
											{
												pixel_lookup += 0x00000400*map_mmc3_bank_r5;
												pixel_lookup -= 0x0C00;
												break;
											}
											case 0x1000:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r0;
												pixel_lookup -= 0x1000;
												break;
											}
											case 0x1400:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r0;
												pixel_lookup -= 0x1000;
												break;
											}
											case 0x1800:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r1;
												pixel_lookup -= 0x1800;
												break;
											}
											case 0x1C00:
											{
												pixel_lookup += 0x00000800*map_mmc3_bank_r1;
												pixel_lookup -= 0x1800;
												break;
											}
										}
									}
								}
								
								pixel_lookup += chr_offset;
								
								if ((sprite_flip_vert==0x00?j:15-j) < 8)
								{
									pixel_low = (unsigned char)cart_rom[(pixel_lookup)];
									pixel_high = (unsigned char)cart_rom[(pixel_lookup+8)];
								}
								else
								{
									pixel_low = (unsigned char)cart_rom[(pixel_lookup+8)];
									pixel_high = (unsigned char)cart_rom[(pixel_lookup+16)];
								}
							}
							else
							{
								pixel_lookup = sprite_tile*16+0x1000*(oam_ram[(((s<<2)+1)&0x00FF)]&0x01)+(sprite_flip_vert==0x00?j:15-j);

								if ((sprite_flip_vert==0x00?j:15-j) < 8)
								{
									pixel_low = chr_ram[(pixel_lookup)];
									pixel_high = chr_ram[(pixel_lookup+8)];
								}
								else
								{
									pixel_low = chr_ram[(pixel_lookup+8)];
									pixel_high = chr_ram[(pixel_lookup+16)];
								}
							}

							for (unsigned char i=0; i<8; i++)
							{
								if (sprite_flip_horz == 0x00) pixel_color = ((pixel_high>>6)&0x02)|(pixel_low>>7);
								else pixel_color = ((pixel_high<<1)&0x02)|(pixel_low&0x01);

								if (pixel_color != 0x00)
								{
									pixel_x = sprite_x+i;
									pixel_y = sprite_y+j+1;
									
									if (ppu_flag_ls > 0 || pixel_x >= 8) 
									{
										if (pixel_x >= 0 && pixel_x < 256 && pixel_y >= 8 && pixel_y < 232)
										{
											if (ppu_flag_g > 0)
											{
												nes_pixel(pixel_x, pixel_y, ppu_palette[(pal_ram[(0x0010+((sprite_attr&0x03)<<2)+pixel_color)]&0x30)]);
											}
											else
											{
												nes_pixel(pixel_x, pixel_y, ppu_palette[pal_ram[(0x0010+((sprite_attr&0x03)<<2)+pixel_color)]]);
											}
										}
									}
								}

								if (sprite_flip_horz == 0x00)
								{
									pixel_high = ((pixel_high << 1) & 0xFF);
									pixel_low = ((pixel_low << 1) & 0xFF);
								}
								else
								{
									pixel_high = ((pixel_high >> 1) & 0xFF);
									pixel_low = ((pixel_low >> 1) & 0xFF);
								}
							}
						}
					}
				}
			}
		}
	}
}

void nes_audio(unsigned long cycles)
{
	debug_location = 7;
	debug_value = cycles;
	
	apu_counter_q += (cycles);
	
	while (apu_counter_q >= 7446) //7456) // quarter of a frame
	{
		apu_counter_q -= 7446; //7456;
		
		apu_counter_s++;
		
		if (apu_counter_s == 1 || apu_counter_s == 2 || apu_counter_s == 3 ||
			((apu_counter_s == 4 && apu_flag_m == 0) || (apu_counter_s == 5 && apu_flag_m == 1)))
		{
			if (apu_pulse_1_c == 0)
			{
				if (apu_pulse_1_r > 0)
				{
					apu_pulse_1_r--;
				}
				else
				{
					apu_pulse_1_r = apu_pulse_1_m + 1;

					if (apu_pulse_1_v > 0)
					{
						apu_pulse_1_v--;
					}
					else
					{
						if (apu_pulse_1_i > 0) apu_pulse_1_v = 0x0F;
					}
				}
			}

			if (apu_pulse_2_c == 0)
			{
				if (apu_pulse_2_r > 0)
				{
					apu_pulse_2_r--;
				}
				else
				{
					apu_pulse_2_r = apu_pulse_2_m + 1;

					if (apu_pulse_2_v > 0)
					{
						apu_pulse_2_v--;
					}
					else
					{
						if (apu_pulse_2_i > 0) apu_pulse_2_v = 0x0F;
					}
				}
			}

			if (apu_triangle_f == 0)
			{
				if (apu_triangle_v > 0)
				{
					apu_triangle_v--;
				}
			}
			else if (apu_triangle_f == 1)
			{
				apu_triangle_v = apu_triangle_r;
			}
			
			if (apu_noise_c == 0)
			{
				if (apu_noise_r > 0)
				{
					apu_noise_r--;
				}
				else
				{
					apu_noise_r = apu_noise_m + 1;

					if (apu_noise_v > 0)
					{
						apu_noise_v--;
					}
					else
					{
						if (apu_noise_i > 0) apu_noise_v = 0x0F;
					}
				}
			}
		}

		if (apu_counter_s == 2 || 
			((apu_counter_s == 4 && apu_flag_m == 0) || (apu_counter_s == 5 && apu_flag_m == 1)))
		{
			if (apu_pulse_1_i == 0x0000)
			{
				if (apu_pulse_1_l > 0)
				{
					apu_pulse_1_l--;
				}
			}
			
			if (apu_pulse_1_e > 0x0000)
			{
				if (apu_pulse_1_w > 0)
				{
					apu_pulse_1_w--;
				}
				else
				{
					apu_pulse_1_w = apu_pulse_1_p + 1;
					
					apu_pulse_1_a = ((apu_pulse_1_t >> apu_pulse_1_s) & 0x07FF);
					
					if (apu_pulse_1_n > 0)
					{						
						if (apu_pulse_1_t < apu_pulse_1_a)
						{
							apu_pulse_1_t = 0;
						}
						else
						{
							apu_pulse_1_t = ((apu_pulse_1_t - apu_pulse_1_a) & 0x07FF);

							if (apu_pulse_1_t > 0) apu_pulse_1_t--; // only for pulse 1
						}
					}
					else
					{
						apu_pulse_1_t = (apu_pulse_1_t + apu_pulse_1_a);

						if (apu_pulse_1_t > 0x07FF) apu_pulse_1_t = 0;
					}
				}
			}

			if (apu_pulse_2_i == 0x0000)
			{
				if (apu_pulse_2_l > 0)
				{
					apu_pulse_2_l--;
				}
			}
			
			if (apu_pulse_2_e > 0x0000)
			{
				if (apu_pulse_2_w > 0)
				{
					apu_pulse_2_w--;
				}
				else
				{
					apu_pulse_2_w = apu_pulse_2_p + 1;

					apu_pulse_2_a = ((apu_pulse_2_t >> apu_pulse_2_s) & 0x07FF);
					
					if (apu_pulse_2_n > 0)
					{
						if (apu_pulse_2_t < apu_pulse_2_a)
						{
							apu_pulse_2_t = 0;
						}
						else
						{
							apu_pulse_2_t = ((apu_pulse_2_t - apu_pulse_2_a) & 0x07FF);
						}
					}
					else
					{
						apu_pulse_2_t = ((apu_pulse_2_t + apu_pulse_2_a) & 0x07FF);
						
						if (apu_pulse_2_t > 0x07FF) apu_pulse_2_t = 0;
					}
				}
			}

			if (apu_triangle_c == 0x0000)
			{	
				if (apu_triangle_l > 0)
				{
					apu_triangle_l--;
				}
				
				apu_triangle_f = 0;
			}
			
			if (apu_noise_i == 0x0000)
			{
				if (apu_noise_l > 0)
				{
					apu_noise_l--;
				}
			}
		}
		
		if (apu_counter_s == 4 && apu_flag_m == 0 && apu_flag_b == 0) // irq
		{
			apu_flag_f = 1;
		}
		
		if ((apu_counter_s == 4 && apu_flag_m == 0) ||
			(apu_counter_s == 5 && apu_flag_m == 1))
		{
			apu_counter_s = 0;
		}
	}
	
	if (apu_pulse_1_l > 0 && apu_pulse_1_t >= 8 && apu_flag_1 > 0)
	{
		apu_pulse_1_k += (cycles);

		while (apu_pulse_1_k >= (apu_pulse_1_t<<1))
		{
			apu_pulse_1_k -= (apu_pulse_1_t<<1);
			
			apu_pulse_1_o = (unsigned short)(apu_duty[(apu_pulse_1_d<<3)+apu_pulse_1_u] & (apu_pulse_1_v << 4));
			
			apu_pulse_1_u++;
			
			if (apu_pulse_1_u >= 8) apu_pulse_1_u = 0;
		}
	}
	else apu_pulse_1_o = 0x0000;
	
	if (apu_pulse_2_l > 0 && apu_pulse_2_t >= 8 && apu_flag_2 > 0)
	{
		apu_pulse_2_k += (cycles);

		while (apu_pulse_2_k >= (apu_pulse_2_t<<1))
		{
			apu_pulse_2_k -= (apu_pulse_2_t<<1);
			
			apu_pulse_2_o = (unsigned short)(apu_duty[(apu_pulse_2_d<<3)+apu_pulse_2_u] & (apu_pulse_2_v << 4));
			
			apu_pulse_2_u++;
			
			if (apu_pulse_2_u >= 8) apu_pulse_2_u = 0;
		}
	}
	else apu_pulse_2_o = 0x0000;
	
	if (apu_triangle_l > 0 && apu_triangle_r > 0 && apu_triangle_v > 0 && apu_triangle_t > 0 && apu_flag_t > 0)
	{
		apu_triangle_k += (cycles);

		while (apu_triangle_k >= (apu_triangle_t))
		{
			apu_triangle_k -= (apu_triangle_t);
			
			if (apu_triangle_d == 0)
			{
				if (apu_triangle_p > 0x00)
				{
					apu_triangle_p -= 0x11;
				}
				else
				{
					apu_triangle_d = 1;
					
					apu_triangle_p = 0x11;
				}
			}
			else
			{
				if (apu_triangle_p < 0xFF)
				{
					apu_triangle_p += 0x11;
				}
				else
				{
					apu_triangle_d = 0;
					
					apu_triangle_p = 0xEE;
				}
			}
			
			apu_triangle_o = (apu_triangle_p & 0x00FF);
		}
	}
	else apu_triangle_o = 0x0000;
	
	if (apu_noise_l > 0 && apu_flag_n > 0 && apu_noise_t > 0)
	{
		apu_noise_k += (cycles);

		while (apu_noise_k >= (apu_noise_t))
		{
			apu_noise_k -= (apu_noise_t);
			
			if (apu_noise_d == 0)
			{
				apu_noise_x = ((apu_noise_s & 0x0002) >> 1);
			}
			else
			{
				apu_noise_x = ((apu_noise_s & 0x0040) >> 6);
			}
			
			apu_noise_x = ((apu_noise_s & 0x0001) ^ apu_noise_x);
			
			if (apu_noise_x > 0) apu_noise_o = 0x0000;
			else apu_noise_o = (apu_noise_v << 4);
			
			apu_noise_s = (apu_noise_s >> 1);
			
			apu_noise_s = ((apu_noise_s | (apu_noise_x << 14)) & 0x7FFF);
		}
	}
	else apu_noise_o = 0x0000;
	
	if (apu_flag_d > 0 && apu_dmc_r > 0)
	{	
		apu_dmc_k += (cycles);

		while (apu_dmc_k >= (apu_dmc_r))
		{	
			apu_dmc_k -= (apu_dmc_r);
			
			if (apu_dmc_s > 0)
			{
				if (apu_dmc_b == 8)
				{
					apu_dmc_b = 0;
					
					apu_dmc_s--;
					
					if (apu_dmc_s == 0 && apu_dmc_i > 0) // irq
					{
						apu_flag_i = 1;
					}
					
					apu_dmc_t = (unsigned short)cpu_read(apu_dmc_a);
					
					apu_dmc_a++;
				}
			
				apu_dmc_b++;

				if ((apu_dmc_t & 0x01) == 0x00) apu_dmc_d = ((apu_dmc_d + 2) & 0x7F);
				else apu_dmc_d = ((apu_dmc_d - 2) & 0x7F);

				apu_dmc_t = (apu_dmc_t >> 1);
			}
			
			apu_dmc_o = (apu_dmc_d<<1);
		}
	}
	else apu_dmc_o = (apu_dmc_d<<1);
}

void nes_mixer()
{
	apu_mixer_output = 0x0000;
	
	apu_mixer_output += apu_pulse_1_o;	
	apu_mixer_output += apu_pulse_2_o;
	apu_mixer_output += (apu_triangle_o>>1);
	apu_mixer_output += (apu_noise_o>>1);
	apu_mixer_output += (apu_dmc_o);
	apu_mixer_output = (apu_mixer_output>>2); // divide by 4
			
	nes_sound(apu_mixer_output);
}

void nes_sprite_0_calc()
{	
	// sprite 0
	if (ppu_flag_h == 0) // 8x8 sprites
	{
		ppu_status_s = 8;
		
		for (unsigned long i=0; i<8; i++)
		{
			if (map_number == 1) // mmc1
			{
				if (map_mmc1_chr_mode == 0) // 8KB
				{
					if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
					{
						if ((unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0&0x1E)+oam_ram[1]*16+0x1000*ppu_flag_s+i] != 0x00 ||
							(unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0&0x1E)+oam_ram[1]*16+0x1000*ppu_flag_s+i+8] != 0x00)
						{
							ppu_status_s = i+1;

							break;
						}
					}
					else
					{

					}
				}
				else if (map_mmc1_chr_mode == 1) // 4KB banked
				{
					if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
					{
						if (ppu_flag_s == 0)
						{
							if ((unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0)+oam_ram[1]*16+0x1000*ppu_flag_s+i] != 0x00 ||
								(unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0)+oam_ram[1]*16+0x1000*ppu_flag_s+i+8] != 0x00)
							{
								ppu_status_s = i+1;

								break;
							}
						}
						else
						{
							if ((unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_1)+oam_ram[1]*16+0x1000*ppu_flag_s+i] != 0x00 ||
								(unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_1)+oam_ram[1]*16+0x1000*ppu_flag_s+i+8] != 0x00)
							{
								ppu_status_s = i+1;

								break;
							}
						}
					}
					else
					{

					}
				}
			}
			else if (map_number == 2) // unrom
			{
				if ((unsigned char)cart_rom[chr_offset+oam_ram[1]*16+0x1000*ppu_flag_s+i] != 0x00 ||
					(unsigned char)cart_rom[chr_offset+oam_ram[1]*16+0x1000*ppu_flag_s+i+8] != 0x00)
				{
					ppu_status_s = i+1;

					break;
				}
			}
			else if (map_number == 3) // cnrom
			{
				if ((unsigned char)cart_rom[chr_offset+0x2000*map_cnrom_bank+oam_ram[1]*16+0x1000*ppu_flag_s+i] != 0x00 ||
					(unsigned char)cart_rom[chr_offset+0x2000*map_cnrom_bank+oam_ram[1]*16+0x1000*ppu_flag_s+i+8] != 0x00)
				{
					ppu_status_s = i+1;

					break;
				}				
			}
			else if (map_number == 4) // mmc3
			{
				ppu_status_s = i+1; // THIS NEEDS WORK!

				break;
			}
			else // nrom
			{
				if ((unsigned char)cart_rom[chr_offset+oam_ram[1]*16+0x1000*ppu_flag_s+i] != 0x00 ||
					(unsigned char)cart_rom[chr_offset+oam_ram[1]*16+0x1000*ppu_flag_s+i+8] != 0x00)
				{
					ppu_status_s = i+1;

					break;
				}
			}
		}
	}
	else // 8x16 sprites
	{
		ppu_status_s = 16;
		
		for (unsigned long i=0; i<16; i++)
		{
			if (i < 8)
			{
				if (map_number == 1) // mmc1
				{
					if (map_mmc1_chr_mode == 0) // 8KB
					{
						if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
						{
							if ((unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0&0x1E)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i] != 0x00 ||
								(unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0&0x1E)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00)
							{
								ppu_status_s = i+9;

								break;
							}
						}
						else
						{

						}
					}
					else if (map_mmc1_chr_mode == 1) // 4KB banked
					{
						if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
						{
							if (ppu_flag_s == 0)
							{
								if ((unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i] != 0x00 ||
									(unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00)
								{
									ppu_status_s = i+9;

									break;
								}
							}
							else
							{
								if ((unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_1)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i] != 0x00 ||
									(unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_1)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00)
								{
									ppu_status_s = i+9;

									break;
								}
							}
						}
						else
						{

						}
					}
				}
				else if (map_number == 2) // unrom
				{
					if ((unsigned char)cart_rom[chr_offset+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i] != 0x00 ||
						(unsigned char)cart_rom[chr_offset+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00)
					{
						ppu_status_s = i+9;

						break;
					}
				}
				else if (map_number == 3) // cnrom
				{
					if ((unsigned char)cart_rom[chr_offset+0x2000*map_cnrom_bank+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i] != 0x00 ||
						(unsigned char)cart_rom[chr_offset+0x2000*map_cnrom_bank+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00)
					{
						ppu_status_s = i+9;

						break;
					}
				}
				else if (map_number == 4) // mmc3
				{
					ppu_status_s = i+9; // THIS NEEDS WORK!

					break;
				}
				else // nrom
				{
					if ((unsigned char)cart_rom[chr_offset+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i] != 0x00 ||
						(unsigned char)cart_rom[chr_offset+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00)
					{
						ppu_status_s = i+9;

						break;
					}
				}
			}
			else
			{
				if (map_number == 1) // mmc1
				{
					if (map_mmc1_chr_mode == 0) // 8KB
					{
						if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
						{
							if ((unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0&0x1E)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00 ||
								(unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0&0x1E)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+16] != 0x00)
							{
								ppu_status_s = i+1;

								break;
							}
						}
						else
						{

						}
					}
					else if (map_mmc1_chr_mode == 1) // 4KB banked
					{
						if ((unsigned char)cart_rom[4] <= 0x10) // 256KB or less
						{
							if (ppu_flag_s == 0)
							{
								if ((unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00 ||
									(unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_0)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+16] != 0x00)
								{
									ppu_status_s = i+1;

									break;
								}
							}
							else
							{
								if ((unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_1)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00 ||
									(unsigned char)cart_rom[chr_offset+0x00001000*(map_mmc1_chr_bank_1)+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+16] != 0x00)
								{
									ppu_status_s = i+1;

									break;
								}
							}
						}
						else
						{

						}
					}
				}
				else if (map_number == 2) // unrom
				{
					if ((unsigned char)cart_rom[chr_offset+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00 ||
						(unsigned char)cart_rom[chr_offset+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+16] != 0x00)
					{
						ppu_status_s = i+1;

						break;
					}
				}
				else if (map_number == 3) // cnrom
				{
					if ((unsigned char)cart_rom[chr_offset+0x2000*map_cnrom_bank+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00 ||
						(unsigned char)cart_rom[chr_offset+0x2000*map_cnrom_bank+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+16] != 0x00)
					{
						ppu_status_s = i+1;

						break;
					}
				}
				else if (map_number == 4) // mmc3
				{
					ppu_status_s = i+1; // THIS NEEDS WORK!

					break;
				}
				else // nrom
				{
					if ((unsigned char)cart_rom[chr_offset+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+8] != 0x00 ||
						(unsigned char)cart_rom[chr_offset+(oam_ram[1]&0xFE)*16+0x1000*(oam_ram[1]&0x01)+i+16] != 0x00)
					{
						ppu_status_s = i+1;

						break;
					}
				}
			}
		}
	}
	
	ppu_status_s += oam_ram[0]; // add y-coordinate
	ppu_status_d = oam_ram[3]; // x-coordinate
}

// needs to be unoptimized else it will be deleted
void nes_wait(unsigned long loop_count)
{
	// wait for interrupts to catch up
	while (nes_interrupt_count < (loop_count)) { }
	
	nes_interrupt_count -= loop_count;
}


void nes_loop(unsigned long loop_count)
{	 
	if (nes_init_flag == 0)
	{
		nes_init_flag = 1;
		
		T8CON = 0x0000; // reset
		T8CON = 0x0000; // prescale of 1:1, 16-bit
		TMR8 = 0x0000; // zero out counter
		PR8 = 0xA0C5; // approx three scanlines (minus one)

		IPC9bits.T8IP = 0x2; // interrupt priority 2
		IPC9bits.T8IS = 0x0; // interrupt sub-priority 0
		IFS1bits.T8IF = 0; // T8 clear flag
		IEC1bits.T8IE = 1; // T8 interrupt on

		T9CON = 0x0000; // reset
		T9CON = 0x0060; // prescale of 1:64, 16-bit
		TMR9 = 0x0000; // zero out counter
		PR9 = 0xDB5E;  // one whole frame (minus one)

		IPC10bits.T9IP = 0x3; // interrupt priority 3
		IPC10bits.T9IS = 0x0; // interrupt sub-priority 0
		IFS1bits.T9IF = 0; // T9 clear flag
		IEC1bits.T9IE = 1; // T9 interrupt on

		// turn on timers
		T8CONbits.ON = 1;
		T9CONbits.ON = 1;
		
		nes_interrupt_count = 0;
		
		// offsets
		prg_offset = 16; // length of header
		chr_offset = 16 + (unsigned char)cart_rom[4]*16384; // length of header + prg_rom
		end_offset = 16 + (unsigned char)cart_rom[4]*16384 + (unsigned char)cart_rom[5]*8192; // end of cart rom (not used?)
		
		// mapper
		map_number = (((unsigned char)cart_rom[6] & 0xF0) >> 4) + ((unsigned char)cart_rom[7] & 0xF0);
		
		// scrolling mask
		if (((unsigned char)cart_rom[6] & 0x01) == 0x00)
		{
			ppu_status_m = 0x0000; // vertical scrolling
		}
		else
		{
			ppu_status_m = 0x0001; // horizontal scrolling
		}
		
		// prg ram
		cpu_status_r = (((unsigned char)cart_rom[6] & 0x02) >> 1); // (not used?)
		
		// reset
		cpu_reg_pc = (unsigned char)cart_rom[prg_offset+0x4000*((unsigned char)cart_rom[4]-1)+0x3FFC] + ((unsigned char)cart_rom[prg_offset+0x4000*((unsigned char)cart_rom[4]-1)+0x3FFD] << 8);
		
		// hacks
		nes_hack_top_hud = 0;
		nes_hack_bottom_hud = 0;
		nes_hack_sprite_priority = 0;
		
		unsigned long loc = prg_offset+0x4000*((unsigned char)cart_rom[4]-1)+0x3FE0;
		
		if (map_number == 1) // mmc1
		{
			if ((unsigned char)cart_rom[loc+0] == 0xFB && // Double Dragon
				(unsigned char)cart_rom[loc+1] == 0x4C &&
				(unsigned char)cart_rom[loc+2] == 0x6A &&
				(unsigned char)cart_rom[loc+3] == 0xFC &&
				(unsigned char)cart_rom[loc+4] == 0x4C &&
				(unsigned char)cart_rom[loc+5] == 0x9E &&
				(unsigned char)cart_rom[loc+6] == 0xFA &&
				(unsigned char)cart_rom[loc+7] == 0x00 &&
				(unsigned char)cart_rom[loc+8] == 0xEE &&
				(unsigned char)cart_rom[loc+9] == 0xF1 &&
				(unsigned char)cart_rom[loc+10] == 0xFF &&
				(unsigned char)cart_rom[loc+11] == 0xEA &&
				(unsigned char)cart_rom[loc+12] == 0xEA &&
				(unsigned char)cart_rom[loc+13] == 0xEA &&
				(unsigned char)cart_rom[loc+14] == 0x4C &&
				(unsigned char)cart_rom[loc+15] == 0x3D)
			{
				nes_hack_bottom_hud = 1;
			}
		}
		
		if (map_number == 2) // unrom
		{
			if ((unsigned char)cart_rom[loc+0] == 0xAA && // Duck Tales
				(unsigned char)cart_rom[loc+1] == 0x9D &&
				(unsigned char)cart_rom[loc+2] == 0xE5 &&
				(unsigned char)cart_rom[loc+3] == 0xFF &&
				(unsigned char)cart_rom[loc+4] == 0x60 &&
				(unsigned char)cart_rom[loc+5] == 0x00 &&
				(unsigned char)cart_rom[loc+6] == 0x01 &&
				(unsigned char)cart_rom[loc+7] == 0x02 &&
				(unsigned char)cart_rom[loc+8] == 0x03 &&
				(unsigned char)cart_rom[loc+9] == 0x04 &&
				(unsigned char)cart_rom[loc+10] == 0x05 &&
				(unsigned char)cart_rom[loc+11] == 0x06 &&
				(unsigned char)cart_rom[loc+12] == 0x00 &&
				(unsigned char)cart_rom[loc+13] == 0x00 &&
				(unsigned char)cart_rom[loc+14] == 0x00 &&
				(unsigned char)cart_rom[loc+15] == 0x00)
			{
				nes_hack_top_hud = 1;
			}
		}
		
		if (map_number == 4) // mmc3
		{
			if ((unsigned char)cart_rom[loc+0] == 0xFF && // Mario 3
				(unsigned char)cart_rom[loc+1] == 0xFF &&
				(unsigned char)cart_rom[loc+2] == 0xFF &&
				(unsigned char)cart_rom[loc+3] == 'S' &&
				(unsigned char)cart_rom[loc+4] == 'U' &&
				(unsigned char)cart_rom[loc+5] == 'P' &&
				(unsigned char)cart_rom[loc+6] == 'E' &&
				(unsigned char)cart_rom[loc+7] == 'R' &&
				(unsigned char)cart_rom[loc+8] == ' ' &&
				(unsigned char)cart_rom[loc+9] == 'M' &&
				(unsigned char)cart_rom[loc+10] == 'A' &&
				(unsigned char)cart_rom[loc+11] == 'R' &&
				(unsigned char)cart_rom[loc+12] == 'I' &&
				(unsigned char)cart_rom[loc+13] == 'O' &&
				(unsigned char)cart_rom[loc+14] == ' ' &&
				(unsigned char)cart_rom[loc+15] == '3')
			{
				nes_hack_bottom_hud = 1;
				nes_hack_sprite_priority = 1;
			}
			else if ((unsigned char)cart_rom[loc+0] == 0x00 && // Kirby
				(unsigned char)cart_rom[loc+1] == 0x52 &&
				(unsigned char)cart_rom[loc+2] == 0x47 &&
				(unsigned char)cart_rom[loc+3] == 0x4B &&
				(unsigned char)cart_rom[loc+4] == 0x4B &&
				(unsigned char)cart_rom[loc+5] == 0x5F &&
				(unsigned char)cart_rom[loc+6] == 'S' &&
				(unsigned char)cart_rom[loc+7] == 'e' &&
				(unsigned char)cart_rom[loc+8] == 't' &&
				(unsigned char)cart_rom[loc+9] == 'S' &&
				(unsigned char)cart_rom[loc+10] == 't' &&
				(unsigned char)cart_rom[loc+11] == 'i' &&
				(unsigned char)cart_rom[loc+12] == 'l' &&
				(unsigned char)cart_rom[loc+13] == 'l' &&
				(unsigned char)cart_rom[loc+14] == 'X' &&
				(unsigned char)cart_rom[loc+15] == 0x00)
			{
				nes_hack_bottom_hud = 1;
			}
		}
		
		//SendLongHex(cpu_reg_pc);
		//SendString("Initialized\n\r\\");
	}
	
	if (nes_reset_flag == 0)
	{
		nes_reset_flag = 1;
		
		// reset
		cpu_reg_pc = (unsigned char)cart_rom[prg_offset+0x4000*((unsigned char)cart_rom[4]-1)+0x3FFC] + ((unsigned char)cart_rom[prg_offset+0x4000*((unsigned char)cart_rom[4]-1)+0x3FFD] << 8);
		
		//SendLongHex(cpu_reg_pc);
		//SendString("Reset\n\r\\");
	}
	
	cpu_current_cycles = 0;

	debug_location = 0;
	
	cpu_current_cycles += cpu_run();
	
	//cpu_ram[0x0026] = 0x00; // hack for Turbo Racer
	
	debug_location = 4;
	
	cpu_current_cycles += cpu_dma_cycles;
	
	cpu_dma_cycles = 0;

	if (cpu_current_cycles == 0)
	{
		nes_error(0x02);
	}
	
	ppu_scanline_cycles += ((cpu_current_cycles<<1)+cpu_current_cycles);
	
	if (ppu_scanline_cycles >= 341) // 113.667 cycles per scanline
	{		
		ppu_scanline_cycles -= 341;

		if (ppu_frame_count == loop_count)
		{	
			nes_background(ppu_scanline_count);
		}
		
		if (map_number == 4) // mmc3
		{
			if (ppu_scanline_count > 0)
			{	
				nes_irq_decrement();
			}
		}
		
		ppu_scanline_count++;
	}
	
	apu_sample_cycles += (cpu_current_cycles);
	
	if (apu_sample_cycles >= 341) // three scanlines
	{
		apu_sample_cycles -= 341;
		
		if (nes_audio_flag > 0)
		{
			nes_audio(341); // three scanlines
			
			nes_mixer(); // move this somewhere else?
		}
	}
	
	// irq
	if (cpu_flag_i == 0 && !(cpu_temp_opcode == 0x58 || cpu_temp_opcode == 0x78 || cpu_temp_opcode == 0x28))
	{
		if (apu_flag_i == 1 || apu_flag_f == 1 || 
			(map_number == 4 && map_mmc3_irq_enable > 0 && map_mmc3_irq_interrupt > 0))
		{	
			//SendLongHex(cpu_reg_pc);
			//SendString("IRQ\n\r\\");
			
			cpu_temp_memory = ((cpu_reg_pc)>>8);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_reg_pc)&0x00FF);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_flag_n<<7)|(cpu_flag_v<<6)|
				(cpu_flag_d<<3)|(cpu_flag_i<<2)|(cpu_flag_z<<1)|cpu_flag_c); //|0x30);
			CPU_PUSH;
			cpu_reg_pc = (unsigned long)cpu_read(0xFFFE);
			cpu_reg_pc += ((unsigned long)cpu_read(0xFFFF)<<8);
			
			cpu_current_cycles += 7;
			
			cpu_flag_i = 1;
			
			ppu_scanline_interrupt = ppu_scanline_count;
			
			if (nes_hack_sprite_priority > 0)
			{
				nes_sprites(2, 0, ppu_scanline_count); // hack for Mario 3
			}
			else
			{
				nes_sprites(0, 0, ppu_scanline_count);
			}
		}
	}
	
	if (map_number == 4 && map_mmc3_irq_interrupt > 0) map_mmc3_irq_interrupt = 0;
	
	ppu_frame_cycles += (cpu_current_cycles<<1);
	
	if (ppu_frame_cycles < 4546) // 2273 cycles in v-blank
	{
		// v-sync
		ppu_flag_v = 0x0001;
		
		ppu_flag_0 = 0;
		
		ppu_status_0 = 0;
	}
	else if (ppu_frame_cycles < 59565) // 29780.5 cycles per frame
	{	
		if (ppu_flag_v == 0x0001)
		{	
			nes_sprite_0_calc();
			
			if (ppu_frame_count == loop_count)
			{
				nes_sprites(1, 0, 255);
			}
		}
		
		// v-sync
		ppu_flag_v = 0x0000;
		
		if (ppu_scanline_count >= (ppu_status_s) && ppu_scanline_cycles >= (ppu_status_d) && ppu_status_0 == 0) // very rough math
		{	
			ppu_status_0 = 1;
			
			ppu_flag_0 = 1;
			
			if (nes_hack_top_hud > 0 || nes_hack_bottom_hud > 0)
			{
				ppu_scanline_sprite_0 = ppu_scanline_count;
				
				nes_sprites(0, 0, ppu_scanline_count); // hack for Duck Tales and Double Dragon
			}
		}
	}
	else
	{		
		ppu_frame_cycles -= 59565;
		
		// v-sync
		ppu_flag_v = 0x0001;
		
		ppu_reg_a = 0;	
		
		// nmi
		if (ppu_flag_e != 0x0000)
		{	
			//SendLongHex(cpu_reg_pc);
			//SendString("NMI\n\r\\");
			
			cpu_temp_memory = ((cpu_reg_pc)>>8);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_reg_pc)&0x00FF);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_flag_n<<7)|(cpu_flag_v<<6)|
				(cpu_flag_d<<3)|(cpu_flag_i<<2)|(cpu_flag_z<<1)|cpu_flag_c); //|0x30);
			CPU_PUSH;
			cpu_reg_pc = (unsigned long)cpu_read(0xFFFA);
			cpu_reg_pc += ((unsigned long)cpu_read(0xFFFB)<<8);
			
			ppu_frame_cycles += 7;
		}

		// finish frame drawing
		if (ppu_frame_count >= loop_count)
		{
			ppu_frame_count = 0;
			
			if (ppu_scanline_sprite_0 > 0)
			{
				nes_sprites(0, ppu_scanline_sprite_0, 255);
			}
			else
			{
				nes_sprites(0, ppu_scanline_interrupt, 255);
			}

			nes_frame();
			
			nes_wait(loop_count);
		}
		
		ppu_scanline_interrupt = 0;
		ppu_scanline_sprite_0 = 0;
		
		ppu_scanline_count = -21;
		//ppu_scanline_count = 0;
		
		ppu_frame_count++;
		
		// start frame drawing
		if (ppu_frame_count >= loop_count)
		{
			nes_border();
		}
		
		nes_buttons();
	}
}

