
// nes.c

// An NES emulator designed for the PIC32MZ
// Yet easy to implement on other platforms
// Written by: Professor Steven Chad Burrow

/*
// basic example of main()

#include "nes.c"

int main()
{
	if (nes_load("MARIO.NES")) // change to NES rom filename
	{
		while (1)
		{ 
			nes_loop(89348, 114, 29780); // change values to skip and go faster
		}
	}
	
	return 0;
}
*/

// By the way, to convert .mp4 to .gif:
// ffmpeg -i input.mp4 -vf scale=320:-1 -r 10 -f image2pipe -vcodec ppm - | convert -delay 10 -loop 0 - output.gif



// if using for other platforms, adjust variable types here
volatile unsigned char __attribute__((address(0x80050000))) prg_rom[65536]; // eventually will be burned to flash rom
volatile unsigned char __attribute__((address(0x80060000))) ppu_ram[16384]; // includes chr_rom from 0x0000 to 0x1FFF
volatile unsigned char __attribute__((address(0x80064000))) cpu_ram[2048]; // only internal ram from 0x0000 to 0x0800
volatile unsigned char __attribute__((address(0x80064800))) oam_ram[256]; // special sprite ram inside of ppu

volatile unsigned char cart_header[16];

volatile unsigned char nes_init_flag = 0;
volatile unsigned long nes_pixel_location = 0;

volatile unsigned long cpu_current_cycles = 0, cpu_total_cycles = 0;
volatile unsigned long ppu_enable_cycles = 0, apu_enable_cycles = 0;
volatile unsigned long btn_enable_cycles = 0;

volatile unsigned short cpu_reg_a = 0x0000, cpu_reg_x = 0x0000, cpu_reg_y = 0x0000, cpu_reg_s = 0x00FD;
volatile unsigned short cpu_flag_c = 0x0000, cpu_flag_z = 0x0000, cpu_flag_v = 0x0000, cpu_flag_n = 0x0000;
volatile unsigned short cpu_flag_d = 0x0000, cpu_flag_i = 0x0000;
volatile unsigned short cpu_reg_pc = 0xFFFC;

volatile unsigned short cpu_temp_opcode = 0x0000, cpu_temp_memory = 0x0000, cpu_temp_address = 0x0000; 
volatile unsigned short cpu_temp_result = 0x0000, cpu_temp_cycles = 0x0000;

volatile unsigned short ppu_reg_v = 0x0000, ppu_reg_t = 0x0000, ppu_reg_x = 0x0000, ppu_reg_w = 0x0000;
volatile unsigned short ppu_reg_a = 0x0000, ppu_reg_b = 0x0000;
volatile unsigned short ppu_flag_e = 0x0001, ppu_flag_p = 0x0000, ppu_flag_h = 0x0000, ppu_flag_b = 0x0000;
volatile unsigned short ppu_flag_s = 0x0000, ppu_flag_i = 0x0000, ppu_flag_n = 0x0000;
volatile unsigned short ppu_flag_v = 0x0001, ppu_flag_0 = 0x0000, ppu_flag_o = 0x0000;

volatile unsigned short ppu_status_0 = 0x0000;

volatile unsigned short ctl_flag_s = 0x0000;
volatile unsigned short ctl_value_1 = 0x0000, ctl_value_2 = 0x0000;
volatile unsigned short ctl_latch_1 = 0x0000, ctl_latch_2 = 0x0000;

volatile unsigned short apu_pulse_1_d = 0x0000, apu_pulse_2_d = 0x0000;
volatile unsigned short apu_pulse_1_u = 0x0000, apu_pulse_2_u = 0x0000;
volatile unsigned short apu_pulse_1_i = 0x0000, apu_pulse_2_i = 0x0000;
volatile unsigned short apu_pulse_1_c = 0x0000, apu_pulse_2_c = 0x0000;
volatile unsigned short apu_pulse_1_v = 0x0000, apu_pulse_2_v = 0x0000;
volatile unsigned short apu_pulse_1_m = 0x0000, apu_pulse_2_m = 0x0000;
volatile unsigned short apu_pulse_1_r = 0x0000, apu_pulse_2_r = 0x0000;
volatile unsigned short apu_pulse_1_s = 0x0000, apu_pulse_2_s = 0x0000;
volatile unsigned short apu_pulse_1_n = 0x0000, apu_pulse_2_n = 0x0000;
volatile unsigned short apu_pulse_1_p = 0x0000, apu_pulse_2_p = 0x0000;
volatile unsigned short apu_pulse_1_w = 0x0000, apu_pulse_2_w = 0x0000;
volatile unsigned short apu_pulse_1_e = 0x0000, apu_pulse_2_e = 0x0000;
volatile unsigned short apu_pulse_1_t = 0x0000, apu_pulse_2_t = 0x0000;
volatile unsigned short apu_pulse_1_k = 0x0000, apu_pulse_2_k = 0x0000;
volatile unsigned short apu_pulse_1_l = 0x0000, apu_pulse_2_l = 0x0000;
volatile unsigned short apu_pulse_1_o = 0x0000, apu_pulse_2_o = 0x0000;

volatile unsigned short apu_triangle_c = 0x0000;
volatile unsigned short apu_triangle_r = 0x0000;
volatile unsigned short apu_triangle_v = 0x0000;
volatile unsigned short apu_triangle_f = 0x0000;
volatile unsigned short apu_triangle_h = 0x0000;
volatile unsigned short apu_triangle_q = 0x0000;
volatile unsigned short apu_triangle_t = 0x0000;
volatile unsigned short apu_triangle_k = 0x0000;
volatile unsigned short apu_triangle_l = 0x0000;
volatile unsigned short apu_triangle_p = 0x0000;
volatile unsigned short apu_triangle_d = 0x0000;
volatile unsigned short apu_triangle_o = 0x0000;

volatile unsigned short apu_noise_i = 0x0000;
volatile unsigned short apu_noise_c = 0x0000;
volatile unsigned short apu_noise_v = 0x0000;
volatile unsigned short apu_noise_m = 0x0000;
volatile unsigned short apu_noise_r = 0x0000;
volatile unsigned short apu_noise_s = 0xABCD; // random?
volatile unsigned short apu_noise_d = 0x0000;
volatile unsigned short apu_noise_t = 0x0000;
volatile unsigned short apu_noise_k = 0x0000;
volatile unsigned short apu_noise_l = 0x0000;
volatile unsigned short apu_noise_o = 0x0000;

volatile unsigned short apu_dmc_i = 0x0000;
volatile unsigned short apu_dmc_l = 0x0000;
volatile unsigned short apu_dmc_r = 0x0000;
volatile unsigned short apu_dmc_k = 0x0000;
volatile unsigned short apu_dmc_d = 0x0000;
volatile unsigned short apu_dmc_a = 0x0000;
volatile unsigned short apu_dmc_s = 0x0000;
volatile unsigned short apu_dmc_b = 0x0000;
volatile unsigned short apu_dmc_t = 0x0000;
volatile unsigned short apu_dmc_o = 0x0000;

volatile unsigned short apu_flag_i = 0x0000;
volatile unsigned short apu_flag_f = 0x0000;
volatile unsigned short apu_flag_d = 0x0000;
volatile unsigned short apu_flag_n = 0x0000;
volatile unsigned short apu_flag_t = 0x0000;
volatile unsigned short apu_flag_2 = 0x0000;
volatile unsigned short apu_flag_1 = 0x0000;
volatile unsigned short apu_flag_m = 0x0000;
volatile unsigned short apu_flag_b = 0x0001;

volatile unsigned short apu_counter_q = 0x0000;
volatile unsigned short apu_counter_s = 0x0000;
volatile unsigned short apu_mixer_output = 0x0000;

volatile unsigned char ppu_palette[64] = {
	0x6D,0x03,0x02,0x46,0x82,0xA0,0xA0,0x80,0x44,0x0C,0x0C,0x08,0x09,0x00,0x00,0x00,
	0xB6,0x0F,0x0B,0x6B,0xC3,0xE1,0xE4,0xE8,0xAC,0x14,0x14,0x15,0x12,0x00,0x00,0x00,
	0xFF,0x37,0x73,0x8F,0xEF,0xEA,0xED,0xF5,0xF4,0xBC,0x59,0x5E,0x1F,0x6D,0x00,0x00,
	0xFF,0xBF,0xB7,0xD7,0xF7,0xF7,0xFA,0xFE,0xF9,0xDD,0xBE,0xBF,0x1F,0xFB,0x00,0x00
};

volatile unsigned char apu_length[32] = {
	  0, 254,  20,   2,  40,   4,  80,   6, 160,   8,  60,  10,  14,  12,  26,  14,
	 12,  16,  24,  18,  48,  20,  96,  22, 192,  24,  72,  26,  16,  28,  32,  30
};

volatile unsigned short apu_duty[32] = {
	0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

volatile unsigned short apu_rate[16] = {
	428, 380, 340, 320, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
};

volatile unsigned short apu_period[16] = {
	4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
};

// change for platform
unsigned char nes_load(char *filename)
{
	// Global variables
	FIL file; // File handle for the file we open
	DIR dir; // Directory information for the current directory
	FATFS fso; // File System Object for the file system we are reading from
	
	char buffer[1];
	unsigned int bytes;
	unsigned int result;
	
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
		for (unsigned int i=0; i<16; i++)
		{
			while (f_read(&file, &buffer, 1, &bytes) != 0) { }

			cart_header[i] = buffer[0];
		}
		
		for (unsigned int i=0; i<cart_header[4]*16384; i++)
		{
			while (f_read(&file, &buffer, 1, &bytes) != 0) { }
			
			if (cart_header[4] == 0x01)
			{
				prg_rom[i+0x4000] = buffer[0];
			}
			else
			{
				prg_rom[i] = buffer[0];
			}
		}
		
		for (unsigned int i=0; i<cart_header[5]*8192; i++)
		{
			while (f_read(&file, &buffer, 1, &bytes) != 0) { }

			ppu_ram[i] = buffer[0]; // load chr_rom into ppu_ram
		}

		while (f_sync(&file) != 0) { }
		while (f_close(&file) != 0) { }

		//SendString("Read successful\n\r\\");
		
		return 1;
	}
	else
	{
		//SendString("Read failure\n\r\\");
		
		return 0;
	}
}

// change for platform
void nes_pixel(unsigned short pos_x, unsigned short pos_y, unsigned char color)
{
	nes_pixel_location = ((pos_y))*SCREEN_X+((pos_x<<1))+(1-screen_frame)*SCREEN_XY; // optimize!
	screen_buffer[nes_pixel_location] = color;
	screen_buffer[nes_pixel_location+1] = color;
}

// change for platform
void nes_frame()
{
	screen_frame = 1 - screen_frame;
}

// change for platform
void nes_sound(unsigned char sample)
{
	PORTJ = (sample & 0x00FF);
}
	
// change for platform
void nes_buttons()
{
	ctl_value_1 = controller_status_1;
	ctl_value_2 = controller_status_2;
}

// change for platform
void nes_error(unsigned char code)
{		
	SendChar('\n');
	SendChar('\r');
	
	SendString("Error ");
	SendHex(code);
	SendString("\n\r\\");
	
	if (code == 0x00)
	{
		SendString("Unknown Opcode \\");
		SendHex(cpu_temp_opcode);
		SendString("\n\r\\");
	}
}

unsigned char cpu_read(unsigned short addr)
{	
	if (addr < 0x2000) return cpu_ram[(addr&0x07FF)]; // internal ram (and mirrors)
	else if (addr < 0x4000) // ppu (and mirrors)
	{
		switch ((addr&0x0007))
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
				return oam_ram[ppu_reg_a];
				
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
				
				ppu_reg_b = ppu_ram[ppu_reg_t];
				 
				if (ppu_flag_i == 0x0000)
				{
					ppu_reg_t += 0x0001;
				}
				else
				{
					ppu_reg_t += 0x0020;
				}
				
				return val;
				
				break;
			}
		}
	}
	else if (addr < 0x4018) // apu and i/o
	{
		switch ((addr&0x00FF))
		{
			case 0x15: // apu status
			{
				unsigned char val = ((apu_flag_i >> 7) |
					((apu_flag_f >> 6) & 0x01) |
					((apu_flag_d >> 4) & 0x01) |
					((apu_flag_n >> 3) & 0x01) |
					((apu_flag_t >> 2) & 0x01) |
					((apu_flag_2 >> 1) & 0x01) |
					(apu_flag_1 & 0x01));
				
				apu_flag_f = 0x0000;
				
				return val;
				
				break;
			}
			case 0x16: // controller 1
			{
				if (ctl_flag_s == 0x0000)
				{
					unsigned char val = (ctl_latch_1 & 0x01);
					
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
					unsigned char val = (ctl_latch_2 & 0x01);
					
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
				return 0x00;
				break;
			}
		}
	}
	else if (addr < 0x4020) // disabled apu and i/o
	{
		return 0x00; 
	}
	else if (addr < 0x6000) // unmapped
	{
		return 0x00; 
	}
	else if (addr < 0x8000) // cart ram
	{	
		return 0x00;
	}
	else return prg_rom[addr-0x8000]; // cart rom and mapper
}

void cpu_write(unsigned short addr, unsigned char val)
{		
	if (addr < 0x2000) cpu_ram[(addr&0x07FF)] = val; // internal ram (and mirrors)
	else if (addr < 0x4000) // ppu (and mirrors)
	{
		switch ((addr&0x0007))
		{
			case 0x00: // ppuctrl
			{				
				ppu_flag_e = (val>>7);
				ppu_flag_p = ((val>>6)&0x01);
				ppu_flag_h = ((val>>5)&0x01);
				ppu_flag_b = ((val>>4)&0x01);
				ppu_flag_s = ((val>>3)&0x01);
				ppu_flag_i = ((val>>2)&0x01);
				ppu_flag_n = (val&0x03);
				
				break;
			}
			case 0x01: // ppumask
			{	
				break;
			}
			case 0x02: // ppustatus
			{
				break;
			}
			case 0x03: // oamaddr
			{
				ppu_reg_a = val;
				
				break;
			}
			case 0x04: // oamdata
			{
				oam_ram[ppu_reg_a] = val;
				ppu_reg_a++;
				
				break;
			}
			case 0x05: // ppuscroll
			{
				break;
			}
			case 0x06: // ppuaddr
			{
				if (ppu_reg_w == 0x0000)
				{
					ppu_reg_t = ((ppu_reg_t & 0x00FF) | (val << 8));
					ppu_reg_w = 0x0001;
				}
				else
				{
					ppu_reg_t = ((ppu_reg_t & 0xFF00) | val);
					ppu_reg_w = 0x0000;
				}
				
				break;
			}
			case 0x07: // ppudata
			{
				ppu_ram[ppu_reg_t] = val;
				
				if (ppu_flag_i == 0x0000)
				{
					ppu_reg_t += 0x0001;
				}
				else
				{
					ppu_reg_t += 0x0020;
				}
				
				break;
			}
		}
	}
	else if (addr < 0x4018) // apu and i/o
	{
		switch ((addr&0x00FF))
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
				
				apu_triangle_l = apu_length[(val>>3)];
				
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
				apu_dmc_a = 0xC000 + (val << 6); 
				
				break;
			}
			case 0x13: // DMC length
			{
				apu_dmc_s = (val << 4) + 1;
				
				break;
			}
			case 0x14: // oam dma
			{
				if ((cpu_current_cycles&0x00000002) == 0x00000000) cpu_current_cycles += 513; // 513 cycles on even
				else cpu_current_cycles += 514; // 514 cycles on odd
				
				for (unsigned short loop=0; loop<256; loop++)
				{
					oam_ram[loop] = cpu_read(loop+(val<<8)); // perhaps read from internal ram directly to make faster?
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
	else if (addr < 0x4020) // disabled apu and i/o
	{
	} 
	else if (addr < 0x6000) // unmapped
	{
	}
	else if (addr < 0x8000) // cart ram
	{
	}
	else // cart rom and mapper
	{
	}
}

// cpu addressing modes
#define CPU_IMM { \
	cpu_temp_cycles = 0x0002; \
	cpu_temp_memory = cpu_read(cpu_reg_pc++); }

#define CPU_ZPR { \
	cpu_temp_cycles = 0x0003; \
	cpu_temp_memory = (unsigned short)(cpu_ram[cpu_read(cpu_reg_pc++)]&0x00FF); }

#define CPU_ZPW { \
	cpu_temp_cycles = 0x0003; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); }

#define CPU_ZPM { \
	cpu_temp_cycles = 0x0005; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); }

#define CPU_ZPXR { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_memory = (unsigned short)(cpu_ram[((cpu_read(cpu_reg_pc++)+cpu_reg_x)&0x00FF)]&0x00FF); }

#define CPU_ZPXW { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_address = ((cpu_read(cpu_reg_pc++)+cpu_reg_x)&0x00FF); }

#define CPU_ZPXM { \
	cpu_temp_cycles = 0x0006; \
	cpu_temp_address = ((cpu_read(cpu_reg_pc++)+cpu_reg_x)&0x00FF); }

#define CPU_ZPYR { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_memory = (unsigned short)(cpu_ram[((cpu_read(cpu_reg_pc++)+cpu_reg_y)&0x00FF)]&0x00FF); }

#define CPU_ZPYW { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_address = ((cpu_read(cpu_reg_pc++)+cpu_reg_y)&0x00FF); }

#define CPU_ABSR { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); \
	cpu_temp_address += (cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_memory = cpu_read(cpu_temp_address); }

#define CPU_ABSW { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); \
	cpu_temp_address += (cpu_read(cpu_reg_pc++)<<8); }
	
#define CPU_ABSM { \
	cpu_temp_cycles = 0x0006; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); \
	cpu_temp_address += (cpu_read(cpu_reg_pc++)<<8); }

#define CPU_ABXR { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); \
	cpu_temp_cycles += ((cpu_temp_address+cpu_reg_x)>>8); \
	cpu_temp_address += (cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_x; \
	cpu_temp_memory = cpu_read(cpu_temp_address); }

#define CPU_ABXW { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); \
	cpu_temp_cycles += ((cpu_temp_address+cpu_reg_x)>>8); \
	cpu_temp_address += (cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_x; }
	
#define CPU_ABXM { \
	cpu_temp_cycles = 0x0007; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); \
	cpu_temp_address += (cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_x; }
	
#define CPU_ABYR { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); \
	cpu_temp_cycles += ((cpu_temp_address+cpu_reg_y)>>8); \
	cpu_temp_address += (cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_y; \
	cpu_temp_memory = cpu_read(cpu_temp_address); }

#define CPU_ABYW { \
	cpu_temp_cycles = 0x0004; \
	cpu_temp_address = cpu_read(cpu_reg_pc++); \
	cpu_temp_cycles += ((cpu_temp_address+cpu_reg_y)>>8); \
	cpu_temp_address += (cpu_read(cpu_reg_pc++)<<8); \
	cpu_temp_address += cpu_reg_y; }
	
#define CPU_INDXR { \
	cpu_temp_cycles = 0x0006; \
	cpu_temp_memory = cpu_read(cpu_reg_pc++); \
	cpu_temp_address = cpu_ram[((cpu_temp_memory+cpu_reg_x)&0x00FF)]+(cpu_ram[((cpu_temp_memory+cpu_reg_x+1)&0x00FF)]<<8); \
	cpu_temp_memory = cpu_read(cpu_temp_address); }

#define CPU_INDXW { \
	cpu_temp_cycles = 0x0006; \
	cpu_temp_memory = cpu_read(cpu_reg_pc++); \
	cpu_temp_address = cpu_ram[((cpu_temp_memory+cpu_reg_x)&0x00FF)]+(cpu_ram[((cpu_temp_memory+cpu_reg_x+1)&0x00FF)]<<8); }
	
#define CPU_INDYR { \
	cpu_temp_cycles = 0x0005; \
	cpu_temp_memory = cpu_read(cpu_reg_pc++); \
	cpu_temp_address = cpu_ram[cpu_temp_memory]+(cpu_ram[((cpu_temp_memory+1)&0x00FF)]<<8); \
	cpu_temp_cycles += (((cpu_temp_address&0x00FF)+cpu_reg_y)>>8); \
	cpu_temp_address += cpu_reg_y; \
	cpu_temp_memory = cpu_read(cpu_temp_address); }

#define CPU_INDYW { \
	cpu_temp_cycles = 0x0006; \
	cpu_temp_memory = cpu_read(cpu_reg_pc++); \
	cpu_temp_address = cpu_ram[cpu_temp_memory]+(cpu_ram[((cpu_temp_memory+1)&0x00FF)]<<8); \
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
	cpu_temp_memory = cpu_read(cpu_temp_address); \
	cpu_flag_c = (cpu_temp_memory>>7); \
	cpu_temp_memory = ((cpu_temp_memory<<1)&0x00FF); \
	cpu_write(cpu_temp_address,(unsigned char)cpu_temp_memory); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }
	
#define CPU_BIT { \
	cpu_flag_v = ((cpu_temp_memory>>6)&0x01); \
	cpu_flag_n = (cpu_temp_memory>>7); \
	cpu_temp_memory = (cpu_reg_a&cpu_temp_memory); \
	cpu_flag_z = (cpu_temp_memory == 0x0000); }
	
#define CPU_BRA { \
	cpu_temp_address = cpu_reg_pc; \
	cpu_reg_pc = (unsigned short)(cpu_reg_pc+(signed char)cpu_temp_memory); \
	cpu_temp_cycles = 0x0003; \
	cpu_temp_cycles += ((cpu_temp_address&0xFF00)!=(cpu_reg_pc&0xFF00)); }
	
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
	cpu_temp_memory = cpu_read(cpu_temp_address); \
	cpu_temp_memory = ((cpu_temp_memory-1)&0x00FF); \
	cpu_write(cpu_temp_address,(unsigned char)cpu_temp_memory); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }

#define CPU_EOR { \
	cpu_reg_a = (cpu_reg_a^cpu_temp_memory); \
	cpu_flag_z = (cpu_reg_a==0x0000); \
	cpu_flag_n = (cpu_reg_a>>7); }

#define CPU_INC { \
	cpu_temp_memory = cpu_read(cpu_temp_address); \
	cpu_temp_memory = ((cpu_temp_memory+1)&0x00FF); \
	cpu_write(cpu_temp_address,(unsigned char)cpu_temp_memory); \
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
	cpu_temp_memory = cpu_read(cpu_temp_address); \
	cpu_flag_c = (cpu_temp_memory&0x01); \
	cpu_temp_memory = (cpu_temp_memory>>1); \
	cpu_write(cpu_temp_address,(unsigned char)cpu_temp_memory); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }

#define CPU_ORA { \
	cpu_reg_a = (cpu_reg_a|cpu_temp_memory); \
	cpu_flag_z = (cpu_reg_a==0x0000); \
	cpu_flag_n = (cpu_reg_a>>7); }

#define CPU_ROL { \
	cpu_temp_memory = cpu_read(cpu_temp_address); \
	cpu_temp_memory = (((cpu_temp_memory<<1)&0x01FF)|cpu_flag_c); \
	cpu_flag_c = (cpu_temp_memory>>8); \
	cpu_temp_memory = (cpu_temp_memory&0x00FF); \
	cpu_write(cpu_temp_address,(unsigned char)cpu_temp_memory); \
	cpu_flag_z = (cpu_temp_memory==0x0000); \
	cpu_flag_n = (cpu_temp_memory>>7); }

#define CPU_ROR { \
	cpu_temp_memory = cpu_read(cpu_temp_address); \
	cpu_temp_memory = (cpu_temp_memory|(cpu_flag_c<<8)); \
	cpu_flag_c = (cpu_temp_memory&0x01); \
	cpu_temp_memory = (cpu_temp_memory>>1); \
	cpu_write(cpu_temp_address,(unsigned char)cpu_temp_memory); \
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
	cpu_write(cpu_temp_address, (unsigned char)cpu_reg_a); }

#define CPU_STX { \
	cpu_write(cpu_temp_address, (unsigned char)cpu_reg_x); }

#define CPU_STY { \
	cpu_write(cpu_temp_address, (unsigned char)cpu_reg_y); }

// internal functions
#define CPU_PUSH { \
	cpu_ram[0x0100+cpu_reg_s]=cpu_temp_memory; \
	cpu_reg_s=((cpu_reg_s-1)&0x00FF); }

#define CPU_PULL { \
	cpu_reg_s=((cpu_reg_s+1)&0x00FF); \
	cpu_temp_memory=cpu_ram[0x0100+cpu_reg_s]; }


unsigned short cpu_run()
{	
	cpu_temp_opcode = cpu_read(cpu_reg_pc++);
	
	switch (cpu_temp_opcode)
	{
		// ADC
		case 0x69: { CPU_IMM; CPU_ADC; break; }
		case 0x65: { CPU_ZPR; CPU_ADC; break; }
		case 0x75: { CPU_ZPXR; CPU_ADC; break; }
		case 0x6D: { CPU_ABSR; CPU_ADC; break; }
		case 0x7D: { CPU_ABXR; CPU_ADC; break; }
		case 0x79: { CPU_ABYR; CPU_ADC; break; }
		case 0x61: { CPU_INDXR; CPU_ADC; break; }
		case 0x71: { CPU_INDYR; CPU_ADC; break; }
		
		// AND
		case 0x29: { CPU_IMM; CPU_AND; break; }
		case 0x25: { CPU_ZPR; CPU_AND; break; }
		case 0x35: { CPU_ZPXR; CPU_AND; break; }
		case 0x2D: { CPU_ABSR; CPU_AND; break; }
		case 0x3D: { CPU_ABXR; CPU_AND; break; }
		case 0x39: { CPU_ABYR; CPU_AND; break; }
		case 0x21: { CPU_INDXR; CPU_AND; break; }
		case 0x31: { CPU_INDYR; CPU_AND; break; }
		
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
		case 0x06: { CPU_ZPM; CPU_ASL; break; }
		case 0x16: { CPU_ZPXM; CPU_ASL; break; }
		case 0x0E: { CPU_ABSM; CPU_ASL; break; }
		case 0x1E: { CPU_ABXM; CPU_ASL; break; }
		
		// BCC
		case 0x90: { CPU_IMM; if (cpu_flag_c == 0x0000) CPU_BRA; break; }
		// BCS
		case 0xB0: { CPU_IMM; if (cpu_flag_c != 0x0000) CPU_BRA; break; }
		// BEQ
		case 0xF0: { CPU_IMM; if (cpu_flag_z != 0x0000) CPU_BRA; break; }
		
		// BIT
		case 0x24: { CPU_ZPR; CPU_BIT; break; }
		case 0x2C: { CPU_ABSR; CPU_BIT; break; }
	
		// BMI
		case 0x30: { CPU_IMM; if (cpu_flag_n != 0x0000) CPU_BRA; break; }
		// BNE
		case 0xD0: { CPU_IMM; if (cpu_flag_z == 0x0000) CPU_BRA; break; }
		// BPL
		case 0x10: { CPU_IMM; if (cpu_flag_n == 0x0000) CPU_BRA; break; }
		
		// BRK
		case 0x00:
		{
			cpu_temp_cycles = 0x0007;
			cpu_temp_memory = ((cpu_reg_pc+1)>>8);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_reg_pc+1)&0x00FF);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_flag_n<<7)|(cpu_flag_v<<6)|
				(cpu_flag_d<<3)|(cpu_flag_i<<2)|(cpu_flag_z<<1)|cpu_flag_c|0x30);
			CPU_PUSH;
			cpu_reg_pc = cpu_read(0xFFFE);
			cpu_reg_pc += (cpu_read(0xFFFF)<<8);
			break;
		}
		
		// BVC
		case 0x50:
		{
			cpu_temp_cycles = 0x0002;
			cpu_temp_memory = cpu_read(cpu_reg_pc++);
			if (cpu_flag_v == 0x0000) CPU_BRA;
			break;
		}
		
		// BVS
		case 0x70:
		{
			cpu_temp_cycles = 0x0002;
			cpu_temp_memory = cpu_read(cpu_reg_pc++);
			if (cpu_flag_v != 0x0000) CPU_BRA;
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
		case 0xC9: { CPU_IMM; CPU_CMP; break; }
		case 0xC5: { CPU_ZPR; CPU_CMP; break; }
		case 0xD5: { CPU_ZPXR; CPU_CMP; break; }
		case 0xCD: { CPU_ABSR; CPU_CMP; break; }
		case 0xDD: { CPU_ABXR; CPU_CMP; break; }
		case 0xD9: { CPU_ABYR; CPU_CMP; break; }
		case 0xC1: { CPU_INDXR; CPU_CMP; break; }
		case 0xD1: { CPU_INDYR; CPU_CMP; break; }
		
		// CPX
		case 0xE0: { CPU_IMM; CPU_CPX; break; }
		case 0xE4: { CPU_ZPR; CPU_CPX; break; }
		case 0xEC: { CPU_ABSR; CPU_CPX; break; }
		
		// CPY
		case 0xC0: { CPU_IMM; CPU_CPY; break; }
		case 0xC4: { CPU_ZPR; CPU_CPY; break; }
		case 0xCC: { CPU_ABSR; CPU_CPY; break; }
		
		// DEC
		case 0xC6: { CPU_ZPM; CPU_DEC; break; }
		case 0xD6: { CPU_ZPXM; CPU_DEC; break; }
		case 0xCE: { CPU_ABSM; CPU_DEC; break; }
		case 0xDE: { CPU_ABXM; CPU_DEC; break; }
		
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
		case 0x49: { CPU_IMM; CPU_EOR; break; }
		case 0x45: { CPU_ZPR; CPU_EOR; break; }
		case 0x55: { CPU_ZPXR; CPU_EOR; break; }
		case 0x4D: { CPU_ABSR; CPU_EOR; break; }
		case 0x5D: { CPU_ABXR; CPU_EOR; break; }
		case 0x59: { CPU_ABYR; CPU_EOR; break; }
		case 0x41: { CPU_INDXR; CPU_EOR; break; }
		case 0x51: { CPU_INDYR; CPU_EOR; break; }
		
		// INC
		case 0xE6: { CPU_ZPM; CPU_INC; break; }
		case 0xF6: { CPU_ZPXM; CPU_INC; break; }
		case 0xEE: { CPU_ABSM; CPU_INC; break; }
		case 0xFE: { CPU_ABXM; CPU_INC; break; }
		
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
			cpu_temp_address = (unsigned short)cpu_read(cpu_reg_pc++);
			cpu_temp_address += ((unsigned short)cpu_read(cpu_reg_pc++)<<8);
			cpu_reg_pc = cpu_temp_address;
			break;
		}
		case 0x6C:
		{
			cpu_temp_cycles = 0x0005;
			cpu_temp_address = (unsigned short)cpu_read(cpu_reg_pc++);
			cpu_temp_address += ((unsigned short)cpu_read(cpu_reg_pc++)<<8);
			cpu_temp_memory = (unsigned short)cpu_read(cpu_temp_address);
			cpu_temp_memory += (unsigned short)(cpu_read((cpu_temp_address&0xFF00)|(((cpu_temp_address&0x00FF)+1)&0x00FF))<<8);
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
			cpu_temp_address = cpu_read(cpu_reg_pc++);
			cpu_temp_address += (cpu_read(cpu_reg_pc)<<8);
			cpu_reg_pc = cpu_temp_address;
			break;
		}
		
		// LDA
		case 0xA9: { CPU_IMM; CPU_LDA; break; }
		case 0xA5: { CPU_ZPR; CPU_LDA; break; }
		case 0xB5: { CPU_ZPXR; CPU_LDA; break; }
		case 0xAD: { CPU_ABSR; CPU_LDA; break; }
		case 0xBD: { CPU_ABXR; CPU_LDA; break; }
		case 0xB9: { CPU_ABYR; CPU_LDA; break; }
		case 0xA1: { CPU_INDXR; CPU_LDA; break; }
		case 0xB1: { CPU_INDYR; CPU_LDA; break; }
		
		// LDX
		case 0xA2: { CPU_IMM; CPU_LDX; break; }
		case 0xA6: { CPU_ZPR; CPU_LDX; break; }
		case 0xB6: { CPU_ZPYR; CPU_LDX; break; }
		case 0xAE: { CPU_ABSR; CPU_LDX; break; }
		case 0xBE: { CPU_ABYR; CPU_LDX; break; }
		
		// LDY
		case 0xA0: { CPU_IMM; CPU_LDY; break; }
		case 0xA4: { CPU_ZPR; CPU_LDY; break; }
		case 0xB4: { CPU_ZPXR; CPU_LDY; break; }
		case 0xAC: { CPU_ABSR; CPU_LDY; break; }
		case 0xBC: { CPU_ABXR; CPU_LDY; break; }
		
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
		case 0x46: { CPU_ZPM; CPU_LSR; break; }
		case 0x56: { CPU_ZPXM; CPU_LSR; break; }
		case 0x4E: { CPU_ABSM; CPU_LSR; break; }
		case 0x5E: { CPU_ABXM; CPU_LSR; break; }
		
		// NOP
		case 0xEA:
		{
			cpu_temp_cycles = 0x0002;
			break;
		}
		
		// ORA
		case 0x09: { CPU_IMM; CPU_ORA; break; }
		case 0x05: { CPU_ZPR; CPU_ORA; break; }
		case 0x15: { CPU_ZPXR; CPU_ORA; break; }
		case 0x0D: { CPU_ABSR; CPU_ORA; break; }
		case 0x1D: { CPU_ABXR; CPU_ORA; break; }
		case 0x19: { CPU_ABYR; CPU_ORA; break; }
		case 0x01: { CPU_INDXR; CPU_ORA; break; }
		case 0x11: { CPU_INDYR; CPU_ORA; break; }
		
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
				(cpu_flag_d<<3)|(cpu_flag_i<<2)|(cpu_flag_z<<1)|cpu_flag_c|0x30);
			CPU_PUSH;
			break;
		}
		
		// PLA
		case 0x68:
		{
			cpu_temp_cycles = 0x0004;
			CPU_PULL;
			cpu_reg_a = cpu_temp_memory;
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
		case 0x26: { CPU_ZPM; CPU_ROL; break; }
		case 0x36: { CPU_ZPXM; CPU_ROL; break; }
		case 0x2E: { CPU_ABSM; CPU_ROL; break; }
		case 0x3E: { CPU_ABXM; CPU_ROL; break; }

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
		case 0x66: { CPU_ZPM; CPU_ROR; break; }
		case 0x76: { CPU_ZPXM; CPU_ROR; break; }
		case 0x6E: { CPU_ABSM; CPU_ROR; break; }
		case 0x7E: { CPU_ABXM; CPU_ROR; break; }	
		
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
		case 0xE9: { CPU_IMM; CPU_SBC; break; }
		case 0xE5: { CPU_ZPR; CPU_SBC; break; }
		case 0xF5: { CPU_ZPXR; CPU_SBC; break; }
		case 0xED: { CPU_ABSR; CPU_SBC; break; }
		case 0xFD: { CPU_ABXR; CPU_SBC; break; }
		case 0xF9: { CPU_ABYR; CPU_SBC; break; }
		case 0xE1: { CPU_INDXR; CPU_SBC; break; }
		case 0xF1: { CPU_INDYR; CPU_SBC; break; }
		
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
		case 0x85: { CPU_ZPW; CPU_STA; break; }
		case 0x95: { CPU_ZPXW; CPU_STA; break; }
		case 0x8D: { CPU_ABSW; CPU_STA; break; }
		case 0x9D: { CPU_ABXW; CPU_STA; cpu_temp_cycles = 0x0005; break; }
		case 0x99: { CPU_ABYW; CPU_STA; cpu_temp_cycles = 0x0005; break; }
		case 0x81: { CPU_INDXW; CPU_STA; break; }
		case 0x91: { CPU_INDYW; CPU_STA; break; }
		
		// STX
		case 0x86: { CPU_ZPW; CPU_STX; break; }
		case 0x96: { CPU_ZPYW; CPU_STX; break; }
		case 0x8E: { CPU_ABSW; CPU_STX; break; }
		
		// STY
		case 0x84: { CPU_ZPW; CPU_STY; break; }
		case 0x94: { CPU_ZPXW; CPU_STY; break; }
		case 0x8C: { CPU_ABSW; CPU_STY; break; }
		
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

void nes_video()
{
	unsigned short pixel_pos = 0, pixel_lookup = 0;
	unsigned char pixel_high = 0, pixel_low = 0, pixel_table = 0, pixel_color = 0;
	
	unsigned char sprite_x = 0, sprite_y = 0, sprite_attr = 0, sprite_tile = 0;
	unsigned char sprite_flip_horz = 0, sprite_flip_vert = 0;
		
	pixel_pos = 0;

	for (unsigned short y=0; y<240; y++)
	{
		for (unsigned short x=0; x<256; x++)
		{
			nes_pixel(x, y, ppu_palette[ppu_ram[0x3F00]]); // background color
		}
	}	

	for (signed char s=63; s>=0; s--) // must be signed!
	{
		sprite_attr = oam_ram[(s<<2)+2];

		if ((sprite_attr&0x20) == 0x20) // background
		{
			sprite_x = oam_ram[(s<<2)+3];
			sprite_y = oam_ram[(s<<2)+0];
			
			if (sprite_x < 0xF9 && sprite_y < 0xEF)
			{
				sprite_flip_horz = ((sprite_attr>>6)&0x01);
				sprite_flip_vert = (sprite_attr>>7);

				sprite_tile = oam_ram[(s<<2)+1];

				for (unsigned char j=0; j<8; j++)
				{
					pixel_lookup = sprite_tile*16+0x1000*ppu_flag_s+(sprite_flip_vert==0x00?j:7-j);

					pixel_low = ppu_ram[pixel_lookup];
					pixel_high = ppu_ram[pixel_lookup+8];

					for (unsigned char i=0; i<8; i++)
					{
						if (sprite_flip_horz == 0x00) pixel_color = ((pixel_high>>6)&0x02)|(pixel_low>>7);
						else pixel_color = ((pixel_high<<1)&0x02)|(pixel_low&0x01);

						if (pixel_color != 0x00) nes_pixel(sprite_x+i, sprite_y+j+1, ppu_palette[ppu_ram[0x3F10+((sprite_attr&0x03)<<2)+pixel_color]]);

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
		}
	}

	for (unsigned char h=0; h<30; h++)
	{
		for (unsigned char w=0; w<32; w++)
		{	
			if ((w&0x01) == 0x00) pixel_table = (ppu_ram[0x23C0+0x0400*ppu_flag_n+(h>>2)*8+(w>>2)]>>(((h&0x02)|((w>>1)&0x01))<<1));

			for (unsigned char j=0; j<8; j++)
			{
				pixel_lookup = ppu_ram[0x2000+0x0400*ppu_flag_n+pixel_pos]*16+0x1000*ppu_flag_b+j;

				pixel_low = ppu_ram[pixel_lookup];
				pixel_high = ppu_ram[pixel_lookup+8];

				for (unsigned char i=0; i<8; i++)
				{
					pixel_color = ((pixel_high>>6)&0x02)|(pixel_low>>7);

					if (pixel_color != 0x00) nes_pixel(w*8+i, h*8+j, ppu_palette[ppu_ram[0x3F00+((pixel_table&0x03)<<2)+pixel_color]]);

					pixel_high = pixel_high << 1;
					pixel_low = pixel_low << 1;
				}
			}

			pixel_pos++;
		}
	}

	for (signed char s=63; s>=0; s--) // must be signed!
	{
		sprite_attr = oam_ram[(s<<2)+2];

		if ((sprite_attr&0x20) == 0x00) // foreground
		{
			sprite_x = oam_ram[(s<<2)+3];
			sprite_y = oam_ram[(s<<2)+0];
			
			if (sprite_x < 0xF9 && sprite_y < 0xEF)
			{
				sprite_flip_horz = ((sprite_attr>>6)&0x01);
				sprite_flip_vert = (sprite_attr>>7);

				sprite_tile = oam_ram[(s<<2)+1];

				for (unsigned char j=0; j<8; j++)
				{
					pixel_lookup = sprite_tile*16+0x1000*ppu_flag_s+(sprite_flip_vert==0x00?j:7-j);

					pixel_low = ppu_ram[pixel_lookup];
					pixel_high = ppu_ram[pixel_lookup+8];

					for (unsigned char i=0; i<8; i++)
					{
						if (sprite_flip_horz == 0x00) pixel_color = ((pixel_high>>6)&0x02)|(pixel_low>>7);
						else pixel_color = ((pixel_high<<1)&0x02)|(pixel_low&0x01);

						if (pixel_color != 0x00) nes_pixel(sprite_x+i, sprite_y+j+1, ppu_palette[ppu_ram[0x3F10+((sprite_attr&0x03)<<2)+pixel_color]]);

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
		}
	}
}

void nes_audio(unsigned long cycles)
{
	apu_counter_q += (cycles);
	
	while (apu_counter_q >= 7456) // quarter of a frame
	{
		apu_counter_q -= 7456;
		
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

					if (apu_pulse_1_n > 0x0000)
					{
						if (apu_pulse_1_t >= apu_pulse_1_s)
						{
							apu_pulse_1_t = (apu_pulse_1_t - apu_pulse_1_s);
						}
						else
						{
							apu_pulse_1_t = 0;
						}
					}
					else apu_pulse_1_t = ((apu_pulse_1_t + apu_pulse_1_s) & 0x07FF);
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

					if (apu_pulse_2_n > 0x0000)
					{
						if (apu_pulse_2_t >= apu_pulse_2_s)
						{
							apu_pulse_2_t = (apu_pulse_2_t - apu_pulse_2_s);
						}
						else
						{
							apu_pulse_2_t = 0;
						}
					}
					else apu_pulse_2_t = ((apu_pulse_2_t << apu_pulse_2_s) & 0x07FF);
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
			
			cpu_temp_memory = ((cpu_reg_pc+1)>>8);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_reg_pc+1)&0x00FF);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_flag_n<<7)|(cpu_flag_v<<6)|
				(cpu_flag_d<<3)|(cpu_flag_i<<2)|(cpu_flag_z<<1)|cpu_flag_c|0x30);
			CPU_PUSH;
			cpu_reg_pc = cpu_read(0xFFFE);
			cpu_reg_pc += (cpu_read(0xFFFF)<<8);
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
			
			apu_pulse_1_o = (apu_duty[(apu_pulse_1_d<<3)+apu_pulse_1_u] & (apu_pulse_1_v << 4));
			
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
			
			apu_pulse_2_o = (apu_duty[(apu_pulse_2_d<<3)+apu_pulse_2_u] & (apu_pulse_2_v << 4));
			
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
			
			apu_triangle_o = apu_triangle_p;
		}
	}
	else apu_triangle_o = 0x0000;
	
	if (apu_noise_l > 0 && apu_flag_n > 0)
	{
		apu_noise_k += (cycles);

		while (apu_noise_k >= (apu_noise_t<<1))
		{
			apu_noise_k -= (apu_noise_t<<1);
			
			if (apu_noise_d == 0)
			{
				apu_noise_s = (apu_noise_s & 0x7FFF) ^ (apu_noise_s & 0x0003);
			}
			else
			{
				apu_noise_s = (apu_noise_s & 0x7FFF) ^ (apu_noise_s & 0x0041);
			}
			
			apu_noise_s = (apu_noise_s & 0x7FFF) | ((apu_noise_s & 0x0001) << 15);
				
			apu_noise_s = (apu_noise_s >> 1);
			
			if ((apu_noise_s & 0x0001) == 0x0001) apu_noise_o = 0x0000;
			else apu_noise_o = (apu_noise_v << 4);
		}
	}
	else apu_noise_o = 0x0000;
	
	if (apu_flag_d > 0)
	{	
		apu_dmc_k += (cycles);

		while (apu_dmc_k >= apu_dmc_r)
		{
			apu_dmc_k -= apu_dmc_r;
			
			if (apu_dmc_s > 0)
			{
				if (apu_dmc_b == 8)
				{
					apu_dmc_b = 0;
					
					apu_dmc_s--;
					
					if (apu_dmc_s == 0 && apu_dmc_i > 0)
					{
						apu_flag_i = 1;
						
						cpu_temp_memory = ((cpu_reg_pc+1)>>8);
						CPU_PUSH;
						cpu_temp_memory = ((cpu_reg_pc+1)&0x00FF);
						CPU_PUSH;
						cpu_temp_memory = ((cpu_flag_n<<7)|(cpu_flag_v<<6)|
							(cpu_flag_d<<3)|(cpu_flag_i<<2)|(cpu_flag_z<<1)|cpu_flag_c|0x30);
						CPU_PUSH;
						cpu_reg_pc = cpu_read(0xFFFE);
						cpu_reg_pc += (cpu_read(0xFFFF)<<8);
					}
					
					apu_dmc_t = cpu_read(apu_dmc_a);
					
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
	else apu_dmc_o = 0x0000;
	
	apu_mixer_output = 0x0000;
	
	apu_mixer_output += apu_pulse_1_o;	
	apu_mixer_output += apu_pulse_2_o;
	apu_mixer_output += (apu_triangle_o<<1);
	apu_mixer_output += (apu_noise_o<<1);
	apu_mixer_output += (apu_dmc_o<<1);
	apu_mixer_output = (apu_mixer_output>>3); // divide by 8
	
	nes_sound(apu_mixer_output);
}

void nes_loop(unsigned long video_loop_count, unsigned long audio_loop_count, unsigned long button_loop_count)
{	  
	if (nes_init_flag == 0)
	{
		nes_init_flag = 1;
		
		// reset
		cpu_reg_pc = prg_rom[0x7FFC] + (prg_rom[0x7FFD] << 8);
	}
	
	cpu_current_cycles = 0;

	cpu_current_cycles += cpu_run();

	if (cpu_current_cycles == 0)
	{
		nes_error(0x00);

		while (1) { }
	}
	
	cpu_total_cycles += (cpu_current_cycles<<1);
	
	if (cpu_total_cycles >= 59565) // 29780.5 cycles per frame
	{
		cpu_total_cycles -= 59565;

		// v-sync
		ppu_flag_v = 0x0001;
		
		// nmi
		if (ppu_flag_e != 0x0000)
		{
			cpu_temp_memory = ((cpu_reg_pc)>>8);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_reg_pc)&0x00FF);
			CPU_PUSH;
			cpu_temp_memory = ((cpu_flag_n<<7)|(cpu_flag_v<<6)|
				(cpu_flag_d<<3)|(cpu_flag_i<<2)|(cpu_flag_z<<1)|cpu_flag_c|0x30);
			CPU_PUSH;
			cpu_reg_pc = cpu_read(0xFFFA);
			cpu_reg_pc += (cpu_read(0xFFFB)<<8);
		}
	}
	else if (cpu_total_cycles >= 4546+(oam_ram[0]*227+oam_ram[3])) // very rough math
	{
		if (ppu_status_0 == 0) ppu_flag_0 = 1;
		
		ppu_status_0 = 1;
	}
	else if (cpu_total_cycles >= 4546) // 2273 cycles in v-blank
	{
		ppu_flag_0 = 0;
		
		ppu_status_0 = 0;
	}
	else
	{
		
	}

	ppu_enable_cycles += (cpu_current_cycles<<1);
	
	if (ppu_enable_cycles > (video_loop_count<<1))
	{			
		nes_video();

		nes_frame();
		
		ppu_enable_cycles -= (video_loop_count<<1);
	}
	
	apu_enable_cycles += (cpu_current_cycles);
	
	if (apu_enable_cycles >= audio_loop_count)
	{	
		nes_audio(apu_enable_cycles);
		
		apu_enable_cycles -= audio_loop_count;
	}
	
	btn_enable_cycles += cpu_current_cycles;
	
	if (btn_enable_cycles >= button_loop_count)
	{
		nes_buttons();
		
		btn_enable_cycles -= button_loop_count;
	}
}

