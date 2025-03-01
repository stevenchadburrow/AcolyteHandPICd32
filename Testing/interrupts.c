


void __attribute__((vector(_OUTPUT_COMPARE_3_VECTOR), interrupt(ipl7srs))) oc3_handler()
{		
    IFS0bits.OC3IF = 0;  // clear interrupt flag
	
	PORTH = 0;

	if (audio_enable > 0)
	{
		audio_counter = audio_counter + 1;
		
		if (audio_counter >= 4)
		{
			audio_counter = 0;
			
			// 8-bit signed audio add 0x80, unsigned add 0x00
			PORTJ = (unsigned short)(((audio_buffer[AUDIO_LEN*audio_frame+audio_read]) + 0x00) + 
				(((audio_buffer[AUDIO_LEN*audio_frame+audio_read]) + 0x00) << 8));

			audio_read = audio_read + 1;

			if (audio_read >= AUDIO_LEN)
			{
				audio_read = 0;
			}
		}
	}
	
	screen_scanline = screen_scanline + 1; // increment scanline
	
	if (screen_scanline == 1066)
	{
		screen_scanline = 0;
	}
	
	if (screen_scanline == 0)
	{
		DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
		DCH1SSA = VirtToPhys(screen_line); // transfer source physical address
		DCH1SSIZ = SCREEN_X; // source size
		DCH1DSIZ = 1; // dst size 
		DCH1CSIZ = SCREEN_X; // X byte per event
		DCH1CONbits.CHEN = 1; // enable channel
	}
	else if (screen_scanline < SCREEN_Y*4)
	{	
		if (screen_interlace == 0 || (screen_interlace > 0 && (screen_scanline & 0x0001) == 0x0000))
		{
			DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
			DCH1SSA = VirtToPhys(screen_buffer + SCREEN_X*SCREEN_Y*screen_frame + SCREEN_X*((screen_scanline)>>2)); // transfer source physical address
			DCH1SSIZ = SCREEN_X; // source size
			DCH1DSIZ = 1; // dst size 
			DCH1CSIZ = SCREEN_X; // X byte per event
			DCH1CONbits.CHEN = 1; // enable channel
		}
		else
		{
			DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
			DCH1SSA = VirtToPhys(screen_zero); // transfer source physical address
			DCH1SSIZ = 1; // source size
			DCH1DSIZ = 1; // dst size 
			DCH1CSIZ = 1; // X byte per event
			DCH1CONbits.CHEN = 1; // enable channel
		}
	}
	else if (screen_scanline == SCREEN_Y*4)
	{
		DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
		DCH1SSA = VirtToPhys(screen_line); // transfer source physical address
		DCH1SSIZ = SCREEN_X; // source size
		DCH1DSIZ = 1; // dst size 
		DCH1CSIZ = SCREEN_X; // X byte per event
		DCH1CONbits.CHEN = 1; // enable channel
	}
	else if (screen_scanline == SCREEN_Y*4+1)
	{
		if (controller_enable > 0)
		{
			if (TRISKbits.TRISK6 > 0)
			{
				controller_status_1 = (controller_status_1 & 0x0C);

				controller_status_1 = controller_status_1 | 
					((!PORTKbits.RK0) << 4) | // up
					((!PORTKbits.RK1) << 5) | // down
					((!PORTKbits.RK2) << 6) | // left
					((!PORTKbits.RK3) << 7) | // right
					((!PORTKbits.RK4) << 1) | // B
					((!PORTKbits.RK5)); // A

				controller_status_2 = (controller_status_2 & 0x0C);

				controller_status_2 = controller_status_2 | 
					((!PORTFbits.RF0) << 4) | // up
					((!PORTFbits.RF1) << 5) | // down
					((!PORTFbits.RF2) << 6) | // left
					((!PORTFbits.RF4) << 7) | // right
					((!PORTFbits.RF5) << 1) | // B
					((!PORTFbits.RF8)); // A

				PORTKbits.RK6 = 0; // ground when not floating
				TRISKbits.TRISK6 = 0;
			}
			else
			{
				controller_status_1 = (controller_status_1 & 0xF3);

				controller_status_1 = controller_status_1 |
					((!PORTKbits.RK4) << 2) | // select
					((!PORTKbits.RK5) << 3); // start

				controller_status_2 = (controller_status_2 & 0xF3);

				controller_status_2 = controller_status_2 |
					((!PORTFbits.RF5) << 2) | // select
					((!PORTFbits.RF8) << 3); // start

				PORTKbits.RK6 = 0;
				TRISKbits.TRISK6 = 1; // high when floating
			}
		}
		else
		{
			PORTKbits.RK6 = 0;
			TRISKbits.TRISK6 = 1; // high when floating
		}
	}
	
	return;
}

void __attribute__((vector(_UART3_RX_VECTOR), interrupt(ipl3srs))) u3rx_handler() //, nomips16)) u3rx_handler()
{	
	IFS4bits.U3RXIF = 0;  // clear interrupt flag
	
	// check for errors
	if(U3STAbits.FERR == 1)
	{
		return;
	}
		
	// clear overrun error
	if(U3STAbits.OERR == 1)
	{
		U3STAbits.OERR = 0;
		return;
	}
	
	// check if there is a character ready to read
	if(U3STAbits.URXDA == 1)
	{
		unsigned char data = U3RXREG; // get character
		
		if (data == '*') // debug?
		{
			
		}
		else
		{
			U3TXREG = data; // echo character received
		}
	}
	
	return;
}

