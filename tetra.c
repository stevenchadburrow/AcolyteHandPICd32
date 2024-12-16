
const unsigned char tetra_random[224] = 
{
	0, 4, 3, 6, 2, 5, 1,
	4, 1, 3, 5, 2, 0, 6,
	6, 5, 2, 0, 4, 1, 3,
	5, 4, 6, 0, 3, 1, 2,
	
	6, 0, 1, 5, 3, 4, 2,
	2, 4, 0, 5, 1, 6, 3,
	3, 1, 6, 2, 4, 0, 5,
	4, 5, 1, 6, 0, 2, 3,
	
	6, 5, 0, 1, 4, 3, 2,
	3, 5, 2, 6, 1, 0, 4,
	1, 3, 6, 5, 4, 2, 0,
	2, 4, 6, 0, 1, 3, 5,
	
	5, 2, 6, 0, 4, 1, 3,
	5, 0, 2, 4, 6, 1, 3,
	1, 4, 2, 6, 0, 3, 5,
	6, 0, 3, 2, 5, 4, 1,
	
	
	2, 6, 3, 5, 4, 0, 1,
	1, 3, 2, 6, 5, 0, 4,
	6, 4, 0, 3, 5, 2, 1,
	2, 5, 3, 4, 6, 0, 1,
	
	4, 5, 0, 6, 2, 1, 3,
	2, 1, 3, 0, 4, 5, 6,
	2, 0, 5, 4, 6, 1, 3,
	6, 2, 3, 0, 5, 4, 1,
	
	0, 6, 5, 3, 1, 4, 2,
	4, 5, 1, 2, 0, 6, 3,
	4, 1, 6, 3, 0, 2, 5,
	6, 0, 2, 5, 4, 3, 1,
	
	1, 3, 5, 2, 6, 0, 4,
	3, 1, 2, 6, 4, 0, 5,
	4, 2, 0, 1, 6, 3, 5,
	4, 3, 0, 5, 1, 6, 2,
};

const char tetra_map[448] =
{
    // I
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	
	' ', ' ', ' ', ' ',
	'*', '*', '*', '*',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	
	' ', ' ', ' ', ' ',
	'*', '*', '*', '*',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
  
	// J
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	' ', '*', ' ', ' ',
	' ', '*', '*', '*',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	' ', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', '*', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	'*', '*', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', ' ', ' ',
	 
	// L
	' ', '*', ' ', ' ',
	' ', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	' ', '*', '*', '*',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', '*', ' ',
	
	' ', ' ', ' ', ' ',
	' ', ' ', '*', ' ',
	'*', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	
	// O
	' ', '*', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', '*', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	// S
	' ', ' ', '*', '*',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', '*', '*',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', '*', ' ',
	' ', ' ', ' ', ' ',
	
	// T
	' ', '*', ' ', ' ',
	'*', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ',
	'*', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', '*', ' ', ' ',
	'*', '*', ' ', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	// Z
	'*', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', '*', ' ',
	' ', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	'*', '*', ' ', ' ',
	' ', '*', '*', ' ',
	' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ',
	
	' ', ' ', '*', ' ',
	' ', '*', '*', ' ',
	' ', '*', ' ', ' ',
	' ', ' ', ' ', ' ',
};

// solid is one single value
void tetra_solid(unsigned int x, unsigned int y, unsigned char value)
{	
	for (unsigned int i=0; i<16; i++)
	{
		for (unsigned int j=0; j<16; j++)
		{
			screen_buffer[(y*16+i)*SCREEN_X+x*16+j] = value;
		}
	}		
};

// block is fancy looking
void tetra_block(unsigned int x, unsigned int y, unsigned char value)
{
	for (unsigned int i=0; i<16; i++)
	{
		for (unsigned int j=0; j<16; j++)
		{
			if (i == 0) screen_buffer[(y*16+i)*SCREEN_X+x*16+j] = 0xFF;
			else if (j == 0) screen_buffer[(y*16+i)*SCREEN_X+x*16+j] = 0xFF;
			else screen_buffer[(y*16+i)*SCREEN_X+x*16+j] = value;
		}
	}	
};

const unsigned int tetra_size_x = 10;
const unsigned int tetra_size_y = 20;
volatile unsigned char tetra_board[2*10*20];

struct tetra_struct_vars
{
	volatile unsigned int pos_x[2]; // start at 4
	volatile unsigned int pos_y[2]; // start at 4
	volatile unsigned int rot[2];
	volatile unsigned int new_pos_x[2];
	volatile unsigned int new_rot[2];
	volatile unsigned int piece[2];
	volatile unsigned int new_piece[2];

	volatile unsigned int seed;

	volatile unsigned int bag[14];
	volatile unsigned int bag_pos[2];

	volatile unsigned int speed[2];
	
	volatile unsigned int game_over[2];

	volatile unsigned int lines[2];
	volatile unsigned int timer[2];
	
	/*
	volatile int background;
	volatile int background_counter;
	volatile int background_trans;
	volatile int background_delay;
	*/
	
	volatile unsigned char joy_curr[2];
	volatile unsigned char joy_prev[2];
	volatile unsigned int joy_delay[2];
	volatile unsigned int joy_button_delay[2];
	
};

struct tetra_struct_vars tetra_vars;

void Tetra()
{	
	TRISJbits.TRISJ15 = 1; // float joy-select (pulled high)
	
	display_foreground_color = 0xFF; // change color of text!
	display_background_color = 0x00; 
	
	unsigned int press_delay = 0x0000;
	unsigned int press_speed = 0x001F;
	unsigned int heights_delay = 0x0000;
	unsigned int heights_speed = 0x001F;
	
	unsigned char directions[4] = { 0, 0, 0, 0 };
	unsigned char buttons[4] = { 2, 2, 2, 2 };
	unsigned int cursors[2] = { SCREEN_X/2, SCREEN_Y/2 };
	unsigned char clicks[2] = { 2, 2 };
	unsigned char heights = 1;
	
	unsigned char key_active = 0;
	unsigned char mouse_active = 0;
	unsigned char mouse_move = 0;
	
	unsigned char pause[2] = { 0, 0 }; // not sure if this is needed any more, it was glitching at some point
	
	unsigned int overall_delay = 0x0000;
	
	for (volatile unsigned int z=0; z<2; z++)
	{	
		tetra_vars.pos_x[z] = 7; // start at 4
		tetra_vars.pos_y[z] = 4; // start at 4
		tetra_vars.rot[z] = 0;
		tetra_vars.new_pos_x[z] = 7;
		tetra_vars.new_rot[z] = 0;
		tetra_vars.piece[z] = 0;
		tetra_vars.new_piece[z] = 1;
		
		for (volatile unsigned int i=0; i<7; i++)
		{
			tetra_vars.bag[i + z * 7] = i;
		}
		
		tetra_vars.bag_pos[z] = 1;
		
		tetra_vars.speed[z] = 0; // max of 13

		tetra_vars.seed = 0;
		
		tetra_vars.game_over[z] = 0x01;
		
		tetra_vars.lines[z] = 0;
		tetra_vars.timer[z] = 0;

		tetra_vars.joy_curr[z] = 0x00;
		tetra_vars.joy_prev[z] = 0x00;
		tetra_vars.joy_delay[z] = 0;
		tetra_vars.joy_button_delay[z] = 0;

		for (volatile unsigned int i=0; i<tetra_size_y; i++)
		{
			for (volatile unsigned int j=0; j<tetra_size_x; j++)
			{
				if (i != tetra_size_y - 1)
				{
					tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = ' '; // empty
				}
				else
				{
					tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = '_'; // permanent bottom
				}
			}
		}
	}
	
	/*
	tetra_vars.background = 1;
	tetra_vars.background_counter = 0;
	tetra_vars.background_trans = -1;
	tetra_vars.background_delay = 0;
	*/
	
	// set background
	for (unsigned int y=0; y<SCREEN_Y; y++)
	{
		for (unsigned int x=0; x<SCREEN_X; x++)
		{
#ifdef SPLASH
			screen_buffer[y*SCREEN_X+x] = splash_default[y * SCREEN_X + x];
#else
			screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
#endif
		}
	}
	
	volatile unsigned int test = 0;
	volatile unsigned int count = 0;
	
	while (1)
	{		
		if (overall_delay > 0x0000)
		{
			overall_delay--;
			continue;
		}
		
		overall_delay = 0x1FFF;
		
		/*
		if (tetra_vars.background_trans >= 0)
		{
			if (tetra_vars.background_delay > 0)
			{
			 	tetra_vars.background_delay--;
			}
			else
			{
				// set background
				for (unsigned int x=0; x<SCREEN_X; x++)
				{
#ifdef SPLASH
					if (tetra_vars.background == 1) screen_buffer[tetra_vars.background_trans*SCREEN_X+x] = splash_brighting[tetra_vars.background_trans * SCREEN_X + x];
					else if (tetra_vars.background == 2) screen_buffer[tetra_vars.background_trans*SCREEN_X+x] = splash_floating[tetra_vars.background_trans * SCREEN_X + x];
					else if (tetra_vars.background == 3) screen_buffer[tetra_vars.background_trans*SCREEN_X+x] = splash_calling[tetra_vars.background_trans * SCREEN_X + x];
					else if (tetra_vars.background == 4) screen_buffer[tetra_vars.background_trans*SCREEN_X+x] = splash_blushing[tetra_vars.background_trans * SCREEN_X + x];
#else
					screen_buffer[tetra_vars.background_trans*SCREEN_X+x] = 0x25; // blue-grey
#endif
				}
				
				tetra_vars.background_trans--;
				
				tetra_vars.background_delay = 5;
			}
		}
		*/
		
		key_active = 0;
		mouse_active = 0;
		mouse_move = 0;
	
		// PS/2 keyboards and mice
		for (unsigned char p=0; p<2; p++)
		{
			if (ps2_ready[p] == 0x01 && ps2_mode[p] == 0x00) // ready and keyboard
			{
				key_active = 1;
				
				if (ps2_readpos[p] != ps2_writepos[p])
				{
					if (ps2_state_array[p][ps2_readpos[p]] == 0x0B) // release
					{
						ps2_readpos[p]++;

						if (ps2_state_array[p][ps2_readpos[p]] == 0x11)
						{
							directions[0] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x12)
						{
							directions[1] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x13)
						{
							directions[2] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x14)
						{
							directions[3] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x20)
						{
							buttons[0] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x30)
						{
							buttons[1] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x7A ||
							ps2_state_array[p][ps2_readpos[p]] == 0x5A)
						{
							buttons[2] = 0;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x78 ||
							ps2_state_array[p][ps2_readpos[p]] == 0x58)
						{
							buttons[3] = 0;
						}

						ps2_readpos[p]++;
					}
					else
					{
						if (ps2_state_array[p][ps2_readpos[p]] == 0x11)
						{
							directions[0] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x12)
						{
							directions[1] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x13)
						{
							directions[2] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x14)
						{
							directions[3] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x20)
						{
							buttons[0] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x30)
						{
							buttons[1] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x78 ||
							ps2_state_array[p][ps2_readpos[p]] == 0x58)
						{
							buttons[2] = 1;
						}
						else if (ps2_state_array[p][ps2_readpos[p]] == 0x7A ||
							ps2_state_array[p][ps2_readpos[p]] == 0x5A)
						{
							buttons[3] = 1;
						}

						ps2_readpos[p]++;
					}

					break;
				}
			}
			else if (ps2_ready[p] == 0x01 && ps2_mode[p] == 0x01) // ready and mouse
			{
				mouse_active = 1;
				
				if (ps2_readpos[p] != ps2_writepos[p])
				{	
					mouse_move = 1;
					
					if ((ps2_state_array[p][ps2_readpos[p]] & 0x01) == 0x01) // left click
					{
						if (clicks[1] == 0) clicks[1] = 1;
					}
					else clicks[1] = 0;
					
					if ((ps2_state_array[p][ps2_readpos[p]] & 0x02) == 0x02) // right click
					{
						if (clicks[0] == 0) clicks[0] = 1;
						else clicks[0] = 2;
					}
					else clicks[0] = 0;
					
					cursors[0] = ps2_cursor_x[p][ps2_readpos[p]];
					cursors[1] = ps2_cursor_y[p][ps2_readpos[p]];
					
					ps2_readpos[p]++;	
				}
			}
		}
		
		USBHostTasks();
			
		if (key_active == 0)
		{
			if (usb_mode == 0x00) // keyboard
			{
				key_active = 1;

				if (usb_readpos != usb_writepos)
				{
					if (usb_state_array[usb_readpos] == 0x0B) // release
					{
						usb_readpos++;

						if (usb_state_array[usb_readpos] == 0x11)
						{
							directions[0] = 0;
						}
						else if (usb_state_array[usb_readpos] == 0x12)
						{
							directions[1] = 0;
						}
						else if (usb_state_array[usb_readpos] == 0x13)
						{
							directions[2] = 0;
						}
						else if (usb_state_array[usb_readpos] == 0x14)
						{
							directions[3] = 0;
						}
						else if (usb_state_array[usb_readpos] == 0x20)
						{
							buttons[0] = 0;
						}
						else if (usb_state_array[usb_readpos] == 0x30)
						{
							buttons[1] = 0;
						}
						else if (usb_state_array[usb_readpos] == 0x7A ||
							usb_state_array[usb_readpos] == 0x5A)
						{
							buttons[2] = 0;
						}
						else if (usb_state_array[usb_readpos] == 0x78 ||
							usb_state_array[usb_readpos] == 0x58)
						{
							buttons[3] = 0;
						}

						usb_readpos++;
					}
					else
					{
						if (usb_state_array[usb_readpos] == 0x11)
						{
							directions[0] = 1;
						}
						else if (usb_state_array[usb_readpos] == 0x12)
						{
							directions[1] = 1;
						}
						else if (usb_state_array[usb_readpos] == 0x13)
						{
							directions[2] = 1;
						}
						else if (usb_state_array[usb_readpos] == 0x14)
						{
							directions[3] = 1;
						}
						else if (usb_state_array[usb_readpos] == 0x20)
						{
							buttons[0] = 1;
						}
						else if (usb_state_array[usb_readpos] == 0x30)
						{
							buttons[1] = 1;
						}
						else if (usb_state_array[usb_readpos] == 0x78 ||
							usb_state_array[usb_readpos] == 0x58)
						{
							buttons[2] = 1;
						}
						else if (usb_state_array[usb_readpos] == 0x7A ||
							usb_state_array[usb_readpos] == 0x5A)
						{
							buttons[3] = 1;
						}

						usb_readpos++;
					}
				}
			}
		}

		if (mouse_active == 0)
		{
			if (usb_mode == 0x01) // mouse
			{	
				mouse_active = 1;

				if (usb_readpos != usb_writepos)
				{	
					mouse_move = 1;

					if ((usb_state_array[usb_readpos]) == 0x01) // left click
					{
						if (clicks[1] == 0) clicks[1] = 1;
					}
					else clicks[1] = 0;

					if ((usb_state_array[usb_readpos]) == 0x02) // right click
					{
						if (clicks[0] == 0) clicks[0] = 1;
						else clicks[0] = 2;
					}
					else clicks[0] = 0;

					cursors[0] = (unsigned int)(usb_cursor_x[usb_readpos]);
					cursors[1] = (unsigned int)(SCREEN_Y - usb_cursor_y[usb_readpos]);

					usb_readpos++;	
				}
			}
		}
		
		if (key_active > 0)
		{
			if (press_delay > 0x0000) press_delay--;
		}
		
		if (mouse_active > 0)
		{
			if (heights_delay > 0x0000) heights_delay--;
		}
		
		tetra_vars.joy_prev[0] = tetra_vars.joy_curr[0];
		tetra_vars.joy_curr[0] = 0xFF; 
		
		if (pause[0] == 0)
		{
			if (PORTJbits.RJ0 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0x7F);
			if (PORTJbits.RJ1 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xBF);
			if (PORTJbits.RJ2 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xDF);
			if (PORTJbits.RJ3 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xEF);
			if (PORTJbits.RJ4 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xF7);
			if (PORTJbits.RJ5 == 0) tetra_vars.joy_curr[0] = (tetra_vars.joy_curr[0] & 0xFB);
			
			// change joystick select pin here if you want
		}
		else pause[0]--;
		
		if (usb_mode != 0x02) // xbox-type controller
		{
			tetra_vars.joy_prev[1] = tetra_vars.joy_curr[1];
			tetra_vars.joy_curr[1] = 0xFF; 

			if (pause[1] == 0)
			{
				if (PORTJbits.RJ6 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0x7F);
				if (PORTJbits.RJ7 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xBF);
				if (PORTJbits.RJ10 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xDF);
				if (PORTJbits.RJ12 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xEF);
				if (PORTJbits.RJ13 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xF7);
				if (PORTJbits.RJ14 == 0) tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xFB);
				
				// change joystick select pin here if you want
			}
			else pause[1]--;
		}
		else
		{	
			tetra_vars.joy_prev[1] = tetra_vars.joy_curr[1];
			
			if (pause[1] == 0)
			{	
				while (usb_readpos != usb_writepos)
				{	
					tetra_vars.joy_curr[1] = 0xFF;

					if ((usb_buttons[usb_readpos] & 0x0100) == 0x0100) // up
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0x7F);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x80);
					}

					if ((usb_buttons[usb_readpos] & 0x0200) == 0x0200) // down
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xBF);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x40);
					}

					if ((usb_buttons[usb_readpos] & 0x0400) == 0x0400) // left
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xDF);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x20);
					}

					if ((usb_buttons[usb_readpos] & 0x0800) == 0x0800) // right
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xEF);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x10);
					}

					if ((usb_buttons[usb_readpos] & 0x0010) == 0x0010) // A
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xF7);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x08);
					}

					if ((usb_buttons[usb_readpos] & 0x0020) == 0x0020) // B
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] & 0xFB);
					}
					else
					{
						tetra_vars.joy_curr[1] = (tetra_vars.joy_curr[1] | 0x04);
					}

					usb_readpos++;
				}
			}
			else pause[1]--;
		}
		
		tetra_vars.seed++; // random 
		
		// DELAY HERE!!!
		
		for (volatile unsigned int z=0; z<2; z++)
		{
			if (tetra_vars.game_over[z] == 0)
			{
				for (volatile unsigned int i=0; i<tetra_size_y; i++)
				{
					for (volatile unsigned int j=0; j<tetra_size_x; j++)
					{
						if (tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] == '*') 
						{
							tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = ' ';
						}
					}
				}
			}

			tetra_vars.new_pos_x[z] = tetra_vars.pos_x[z];
			tetra_vars.new_rot[z] = tetra_vars.rot[z];

			tetra_vars.timer[z]++;

			if (tetra_vars.timer[z] > 128 - 8 * tetra_vars.speed[z]) // determines how fast it falls
			{
				tetra_vars.timer[z] = 0;
			}

			tetra_vars.joy_delay[z]++;

			if (tetra_vars.joy_delay[z] > 20) // determines button turbo speed
			{
				tetra_vars.joy_delay[z] = 0;
				tetra_vars.joy_prev[z] = tetra_vars.joy_prev[z] | 0xF0;
			}
			
			if (tetra_vars.joy_button_delay[z] > 0) tetra_vars.joy_button_delay[z]--;
			
			// mice
			if (z == 1)
			{
				if (mouse_active == 1 && mouse_move == 1)
				{
					tetra_vars.new_pos_x[z] = (unsigned int)(cursors[0] / (unsigned int)(SCREEN_X / (tetra_size_x+2)));

					if (cursors[1] < 10) heights = 0;
					else if (cursors[1] > SCREEN_Y-10) heights = 2;
					else heights = 1;
				}
			}

			if ((((tetra_vars.joy_curr[z] & 0x80) == 0x00) && ((tetra_vars.joy_prev[z] & 0x80) == 0x80)) ||
				(press_delay == 0x0000 && directions[0] == 1 && z == 0) ||
				(heights_delay == 0x0000 && heights == 2 && z == 1)) // up
			{
				tetra_vars.timer[z] = 1; // not zero
				tetra_vars.joy_delay[z] = 0;
				
				if (z == 0) press_delay = press_speed;
				if (z == 1) heights_delay = heights_speed;
			}
			else if ((((tetra_vars.joy_curr[z] & 0x40) == 0x00) && ((tetra_vars.joy_prev[z] & 0x40) == 0x40)) ||
				(press_delay == 0x0000 && directions[1] == 1 && z == 0) ||
				(heights_delay == 0x0000 && heights == 0 && z == 1)) // down
			{
				tetra_vars.timer[z] = 0;
				tetra_vars.joy_delay[z] = 0;
				
				if (z == 0) press_delay = press_speed;
				if (z == 1) heights_delay = heights_speed;
			}
			else if ((((tetra_vars.joy_curr[z] & 0x20) == 0x00) && ((tetra_vars.joy_prev[z] & 0x20) == 0x20)) ||
				(press_delay == 0x0000 && directions[2] == 1 && z == 0)) // left
			{
				tetra_vars.new_pos_x[z]--;
				tetra_vars.joy_delay[z] = 0;
				
				if (z == 0) press_delay = press_speed;
			}
			else if ((((tetra_vars.joy_curr[z] & 0x10) == 0x00) && ((tetra_vars.joy_prev[z] & 0x10) == 0x10)) ||
				(press_delay == 0x0000 && directions[3] == 1 && z == 0)) // right
			{
				tetra_vars.new_pos_x[z]++;
				tetra_vars.joy_delay[z] = 0;
				
				if (z == 0) press_delay = press_speed;
			}
			else if ((((tetra_vars.joy_curr[z] & 0x08) == 0x00) && ((tetra_vars.joy_prev[z] & 0x08) == 0x08) && tetra_vars.joy_button_delay[z] == 0) ||
				(press_delay == 0x0000 && (buttons[0] == 1 || buttons[2] == 1) && z == 0) ||
				(clicks[0] == 1 && z == 1)) // button 1
			{
				tetra_vars.joy_button_delay[z] = 0x003F;
				
				if (tetra_vars.game_over[z] != 0)
				{
					tetra_vars.game_over[z] = 0; // play again
					
					tetra_vars.speed[z] = 0;
					
					tetra_vars.lines[z] = 0;
					
					for (volatile unsigned int i=0; i<tetra_size_y; i++)
					{
						for (volatile unsigned int j=0; j<tetra_size_x; j++)
						{
							if (i != tetra_size_y - 1)
							{
								tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = ' '; // empty
							}
							else
							{
								tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = '_'; // permanent bottom
							}
						}
					}
				}
				else
				{  
					tetra_vars.new_rot[z]++;
					if (tetra_vars.new_rot[z] == 4) tetra_vars.new_rot[z] = 0;
					tetra_vars.joy_delay[z] = 0;
				}
				
				if (z == 0)
				{
					press_delay = press_speed;
					if (buttons[0] == 1) buttons[0] = 2;
					if (buttons[2] == 1) buttons[2] = 2;
				}
				if (z == 1)
				{
					if (clicks[0] == 1) clicks[0] = 2;
				}
			}
			else if ((((tetra_vars.joy_curr[z] & 0x04) == 0x00) && ((tetra_vars.joy_prev[z] & 0x04) == 0x04) && tetra_vars.joy_button_delay[z] == 0) ||
				(press_delay == 0x0000 && (buttons[1] == 1 || buttons[3] == 1) && z == 0) ||
				(clicks[1] == 1 && z == 1)) // button 2
			{
				tetra_vars.joy_button_delay[z] = 0x003F;
				
				if (tetra_vars.game_over[z] != 0)
				{
					tetra_vars.game_over[z] = 0; // play again
					
					tetra_vars.speed[z] = 0;
					
					tetra_vars.lines[z] = 0;
					
					for (volatile unsigned int i=0; i<tetra_size_y; i++)
					{
						for (volatile unsigned int j=0; j<tetra_size_x; j++)
						{
							if (i != tetra_size_y - 1)
							{
								tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = ' '; // empty
							}
							else
							{
								tetra_board[j + i * tetra_size_x + z * tetra_size_x * tetra_size_y] = '_'; // permanent bottom
							}
						}
					}
				}
				else
				{
					if (tetra_vars.new_rot[z] == 0) tetra_vars.new_rot[z] = 3;
					else tetra_vars.new_rot[z]--;
					tetra_vars.joy_delay[z] = 0;
				}
				
				if (z == 0)
				{
					press_delay = press_speed;
					if (buttons[1] == 1) buttons[1] = 2;
					if (buttons[3] == 1) buttons[3] = 2;
				}
				if (z == 1)
				{
					if (clicks[1] == 1) clicks[1] = 2;
				}
			}
			
			if (tetra_vars.game_over[z] != 0) continue;

			for (volatile unsigned int k=0; k<4; k++)
			{
				for (volatile unsigned int i=0; i<4; i++)
				{
					for (volatile unsigned int j=0; j<4; j++)
					{
						if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.new_rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] != ' ')
						{
							if (tetra_vars.new_pos_x[z] + j >= 0x80 || tetra_vars.new_pos_x[z] + j <= 0x03)
							{	
								tetra_vars.new_pos_x[z]++;
								i = 5;
								j = 5;
							}
							else if (tetra_vars.new_pos_x[z] + j >= tetra_size_x + 4)
							{
								tetra_vars.new_pos_x[z]--;
								i = 5;
								j = 5;
							}
						}
					}
				}
			}
			
			test = 0;

			for (volatile unsigned int i=0; i<4; i++)
			{
				for (volatile unsigned int j=0; j<4; j++)
				{
					if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.new_rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] == '*')
					{
						if (tetra_board[(tetra_vars.new_pos_x[z] + j - 4) + (tetra_vars.pos_y[z] + i - 4) * tetra_size_x + z * tetra_size_x * tetra_size_y] != ' ')
						{
							test = 1;
							i = 5;
							j = 5;
						}
					}
				}
			}

			if (test == 0)
			{
				tetra_vars.pos_x[z] = tetra_vars.new_pos_x[z];
				tetra_vars.rot[z] = tetra_vars.new_rot[z];
			}

			if (tetra_vars.timer[z] == 0)
			{
				tetra_vars.pos_y[z]++;

				test = 0;

				for (volatile unsigned int i=0; i<4; i++)
				{
					for (volatile unsigned int j=0; j<4; j++)
					{
						if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] == '*')
						{
							if (tetra_board[(tetra_vars.pos_x[z] + j - 4) + (tetra_vars.pos_y[z] + i - 4) * tetra_size_x + z * tetra_size_x * tetra_size_y] != ' ')
							{
								test = 1;
								i = 5;
								j = 5;
							}
						}
					}
				}

				if (test == 1)
				{	
					tetra_vars.pos_y[z]--;

					for (volatile unsigned int i=0; i<4; i++)
					{
						for (volatile unsigned int j=0; j<4; j++)
						{
							if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] == '*')
							{
								tetra_board[(tetra_vars.pos_x[z] + j - 4) + (tetra_vars.pos_y[z] + i - 4) * tetra_size_x + z * tetra_size_x * tetra_size_y] = '#';
							}
						}
					}
					
					// SOUND HERE
					music_note(523, 250, 0);

					if (tetra_vars.pos_y[z] == 4)
					{
						tetra_vars.game_over[z] = 1;
						
						// SOUND HERE
						music_note(262, 250, 0);
					}
					else
					{
						count = 0;

						for (volatile unsigned int i=tetra_size_y-1; i>=1; i--)
						{
							test = 0;

							for (volatile unsigned int j=0; j<tetra_size_x; j++)
							{
								if (tetra_board[j + (i-1) * tetra_size_x + z * tetra_size_x * tetra_size_y] == ' ') test = 1;
							}

							if (test == 0)
							{
								for (volatile unsigned int j=i; j>=2; j--)
								{
									for (volatile unsigned int k=0; k<tetra_size_x; k++)
									{
										tetra_board[k + (j-1) * tetra_size_x + z * tetra_size_x * tetra_size_y] = 
											tetra_board[k + (j-2) * tetra_size_x + z * tetra_size_x * tetra_size_y];
									}
								}

								i++; // test that row again

								tetra_vars.lines[z]++;
								count++;
								
								/*
								tetra_vars.background_counter++;
								
								if (tetra_vars.background_counter >= 4 && tetra_vars.background_trans < 0)
								{
									tetra_vars.background_counter = 0;
									
									tetra_vars.background++;
									
									if (tetra_vars.background >= 5) tetra_vars.background = 1;
									
									tetra_vars.background_trans = SCREEN_Y;
								}
								*/
								
								pause[0] = 0x3F;
								pause[1] = 0x3F;
								
								// SOUND HERE
								music_note(1047, 250, 0);
							}
						}

						if (tetra_vars.game_over[1-z] != 0) count = 0;

						if (count >= 1) count--;

						for (volatile unsigned int i=0; i<count; i++)
						{
							for (volatile unsigned int j=0; j<tetra_size_y-1; j++)
							{
								for (volatile unsigned int k=0; k<tetra_size_x; k++)
								{
									if (j < tetra_size_y-2)
									{
										tetra_board[k + j * tetra_size_x + (1-z) * tetra_size_x * tetra_size_y] = 
											tetra_board[k + (j+1) * tetra_size_x + (1-z) * tetra_size_x * tetra_size_y];
									}
									else
									{
										if (k == tetra_vars.seed % 10) tetra_board[k + j * tetra_size_x + (1-z) * tetra_size_x * tetra_size_y] = ' ';
										else tetra_board[k + j * tetra_size_x + (1-z) * tetra_size_x * tetra_size_y] = '#';
									}
								}
							}
						}
						
						tetra_vars.pos_y[1-z] -= count;
						if (tetra_vars.pos_y[1-z] < 4) tetra_vars.pos_y[1-z] = 4;
					
						tetra_vars.speed[z] = (unsigned int)(tetra_vars.lines[z] / 10);
						if (tetra_vars.speed[z] > 9) tetra_vars.speed[z] = 13; // max

						tetra_vars.pos_x[z] = 7; // start at 4
						tetra_vars.pos_y[z] = 4; // start at 4
						tetra_vars.rot[z] = 0;
						tetra_vars.piece[z] = tetra_vars.new_piece[z];

						tetra_vars.bag_pos[z]++;

						if (tetra_vars.bag_pos[z] == 7)
						{
							tetra_vars.bag_pos[z] = 0;

							for (volatile unsigned int i=0; i<7; i++)
							{
								tetra_vars.bag[(i + ((tetra_vars.seed & 0xF0) >> 4)) % 7 + z * 7] = tetra_random[i + (tetra_vars.seed & 0x1F) * 7];
							}
						}

						tetra_vars.new_piece[z] = tetra_vars.bag[tetra_vars.bag_pos[z] + z * 7];
					}
				}
			}

			for (volatile unsigned int i=0; i<4; i++)
			{
				for (volatile unsigned int j=0; j<4; j++)
				{
					if (tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64] == '*')
					{
						tetra_board[(tetra_vars.pos_x[z] + j - 4) + (tetra_vars.pos_y[z] + i - 4) * tetra_size_x + z * tetra_size_x * tetra_size_y] = 
							tetra_map[(unsigned int)j + (unsigned int)i*4 + (unsigned int)tetra_vars.rot[z]*16 + (unsigned int)tetra_vars.piece[z]*64];
					}
				}
			}
		}
		
		unsigned int vert = 0x02;
		unsigned int horz = 0x02;
		
		for (unsigned int i=0; i<(tetra_size_y-1)*16; i++)
		{
			for (unsigned int j=0; j<16; j++)
			{
				screen_buffer[(i+(vert+2)*16)*SCREEN_X+j+horz*16-16] = 0x92; // light grey
				screen_buffer[(i+(vert+2)*16)*SCREEN_X+j+horz*16+tetra_size_x*16] = 0x92; // light grey
			}
		}
		
		for (unsigned int i=0; i<16; i++)
		{
			for (unsigned int j=0; j<(tetra_size_x+2)*16; j++)
			{
				screen_buffer[(i+(vert+2)*16-16)*SCREEN_X+j+horz*16-16] = 0x92; // light grey
				screen_buffer[(i+(vert+2)*16+(tetra_size_y-1)*16)*SCREEN_X+j+horz*16-16] = 0x92; // light grey
			}
		}

		for (volatile unsigned int i=0; i<tetra_size_y-1; i++)
		{
			for (volatile unsigned int j=0; j<tetra_size_x; j++)
			{
				switch (tetra_board[j + i * tetra_size_x])
				{
					case ' ':
					{
						tetra_solid(j + horz, i + 0x02 + vert, 0x00); // black
						break;
					}
					case '*':
					{
						if (tetra_vars.piece[0] == 0) tetra_block(j + horz, i + 0x02 + vert, 0x1F); // cyan
						else if (tetra_vars.piece[0] == 1) tetra_block(j + horz, i + 0x02 + vert, 0x03); // blue
						else if (tetra_vars.piece[0] == 2) tetra_block(j + horz, i + 0x02 + vert, 0xF0); // orange
						else if (tetra_vars.piece[0] == 3) tetra_block(j + horz, i + 0x02 + vert, 0xFC); // yellow
						else if (tetra_vars.piece[0] == 4) tetra_block(j + horz, i + 0x02 + vert, 0x1C); // green
						else if (tetra_vars.piece[0] == 5) tetra_block(j + horz, i + 0x02 + vert, 0xE3); // magenta
						else if (tetra_vars.piece[0] == 6) tetra_block(j + horz, i + 0x02 + vert, 0xE0); // red
						break;
					}
					case '#':
					{
						tetra_block(j + horz, i + 0x02 + vert, 0x49); // dark grey
						break;
					}
				}
			}
		}

		display_decimal((unsigned int)((horz + 0x07) * 16 + 8), vert * 16, tetra_vars.lines[0]);

		if (tetra_vars.new_piece[0] == 0) display_character(horz * 16, vert * 16, 'I');
		else if (tetra_vars.new_piece[0] == 1) display_character(horz * 16, vert * 16, 'J');
		else if (tetra_vars.new_piece[0] == 2) display_character(horz * 16, vert * 16, 'L');
		else if (tetra_vars.new_piece[0] == 3) display_character(horz * 16, vert * 16, 'O');
		else if (tetra_vars.new_piece[0] == 4) display_character(horz * 16, vert * 16, 'S');
		else if (tetra_vars.new_piece[0] == 5) display_character(horz * 16, vert * 16, 'T');
		else if (tetra_vars.new_piece[0] == 6) display_character(horz * 16, vert * 16, 'Z');

		if (tetra_vars.game_over[0] != 0x00)
		{
			display_string((unsigned int)((horz + 0x04) * 16 - 8), (unsigned int)(0x0170 + vert * 16), "Press \\");
			display_string((unsigned int)((horz + 0x04) * 16 - 8), (unsigned int)(0x0178 + vert * 16), "Button\\");
		}
		else
		{
			display_string((unsigned int)((horz + 0x04) * 16 - 8), (unsigned int)(0x0170 + vert * 16), "      \\");
			display_string((unsigned int)((horz + 0x04) * 16 - 8), (unsigned int)(0x0178 + vert * 16), "      \\");
		}
		
		vert = 0x02;
		horz = 0x44;
		
		for (unsigned int i=0; i<(tetra_size_y-1)*16; i++)
		{
			for (unsigned int j=0; j<16; j++)
			{
				screen_buffer[(i+(vert+2)*16)*SCREEN_X+j+horz*16-16] = 0x92; // light grey
				screen_buffer[(i+(vert+2)*16)*SCREEN_X+j+horz*16+tetra_size_x*16] = 0x92; // light grey
			}
		}
		
		for (unsigned int i=0; i<16; i++)
		{
			for (unsigned int j=0; j<(tetra_size_x+2)*16; j++)
			{
				screen_buffer[(i+(vert+2)*16-16)*SCREEN_X+j+horz*16-16] = 0x92; // light grey
				screen_buffer[(i+(vert+2)*16+(tetra_size_y-1)*16)*SCREEN_X+j+horz*16-16] = 0x92; // light grey
			}
		}

		for (volatile unsigned int i=0; i<tetra_size_y-1; i++)
		{
			for (volatile unsigned int j=0; j<tetra_size_x; j++)
			{
				switch (tetra_board[j + i * tetra_size_x + tetra_size_x * tetra_size_y])
				{
					case ' ':
					{
						tetra_solid(j + horz, i + 0x02 + vert, 0x00); // black
						break;
					}
					case '*':
					{
						if (tetra_vars.piece[1] == 0) tetra_block(j + horz, i + 0x02 + vert, 0x1F); // cyan
						else if (tetra_vars.piece[1] == 1) tetra_block(j + horz, i + 0x02 + vert, 0x03); // blue
						else if (tetra_vars.piece[1] == 2) tetra_block(j + horz, i + 0x02 + vert, 0xF0); // orange
						else if (tetra_vars.piece[1] == 3) tetra_block(j + horz, i + 0x02 + vert, 0xFC); // yellow
						else if (tetra_vars.piece[1] == 4) tetra_block(j + horz, i + 0x02 + vert, 0x1C); // green
						else if (tetra_vars.piece[1] == 5) tetra_block(j + horz, i + 0x02 + vert, 0xE3); // magenta
						else if (tetra_vars.piece[1] == 6) tetra_block(j + horz, i + 0x02 + vert, 0xE0); // red
						break;
					}
					case '#':
					{
						tetra_block(j + horz, i + 0x02 + vert, 0x49); // mid grey
						break;
					}
				}
			}
		}

		display_decimal((unsigned int)((horz + 0x07) * 16 + 8), vert * 16, tetra_vars.lines[1]);

		if (tetra_vars.new_piece[1] == 0) display_character(horz * 16, vert * 16, 'I');
		else if (tetra_vars.new_piece[1] == 1) display_character(horz * 16, vert * 16, 'J');
		else if (tetra_vars.new_piece[1] == 2) display_character(horz * 16, vert * 16, 'L');
		else if (tetra_vars.new_piece[1] == 3) display_character(horz * 16, vert * 16, 'O');
		else if (tetra_vars.new_piece[1] == 4) display_character(horz * 16, vert * 16, 'S');
		else if (tetra_vars.new_piece[1] == 5) display_character(horz * 16, vert * 16, 'T');
		else if (tetra_vars.new_piece[1] == 6) display_character(horz * 16, vert * 16, 'Z');

		if (tetra_vars.game_over[0] != 0x00)
		{
			display_string((unsigned int)((horz + 0x04) * 16 - 8), (unsigned int)(0x0170 + vert * 16), "Press \\");
			display_string((unsigned int)((horz + 0x04) * 16 - 8), (unsigned int)(0x0178 + vert * 16), "Button\\");
		}
		else
		{
			display_string((unsigned int)((horz + 0x04) * 16 - 8), (unsigned int)(0x0170 + vert * 16), "      \\");
			display_string((unsigned int)((horz + 0x04) * 16 - 8), (unsigned int)(0x0178 + vert * 16), "      \\");
		}
	}
};
