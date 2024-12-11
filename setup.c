
void Setup()
{
	// turn off analog
	ANSELA = 0x0;
	ANSELB = 0x0;
	ANSELC = 0x0;
	ANSELD = 0x0;
	ANSELE = 0x0;
	ANSELF = 0x0;
	ANSELG = 0x0;
	ANSELH = 0x0;
	//ANSELI = 0x0;
	ANSELJ = 0x0;
	//ANSELK = 0x0;
	
	// make everything output
	TRISA = 0x0;
	TRISB = 0x0;
	TRISC = 0x0;
	TRISD = 0x0;
	TRISE = 0x0;
	TRISF = 0x0;
	TRISG = 0x0;
	TRISH = 0x0;
	//TRISI = 0x0;
	TRISJ = 0x0;
	TRISK = 0x0;
	
	// set all ports to ground
	PORTA = 0x0;
	PORTB = 0x0;
	PORTC = 0x0;
	PORTD = 0x0;
	PORTE = 0x0;
	PORTF = 0x0;
	PORTG = 0x0;
	PORTH = 0x0;
	//PORTI = 0x0;
	PORTJ = 0x0;
	PORTK = 0x0;
	
	// disable all interrupts
	IEC0 = 0x0;
	IEC1 = 0x0;
	IEC2 = 0x0;
	IEC3 = 0x0;
	IEC4 = 0x0;
	IEC5 = 0x0;
	IEC6 = 0x0;
	//IEC7 = 0x0;
	
	// clear all flags
	IFS0 = 0x0;
	IFS1 = 0x0;
	IFS2 = 0x0;
	IFS3 = 0x0;
	IFS4 = 0x0;
	IFS5 = 0x0;
	IFS6 = 0x0;
	//IFS7 = 0x0;
	

	// Set up caching
	unsigned int cp0 = _mfc0(16, 0);
	cp0 &= ~0x07;
	cp0 |= 0b011; // K0 = Cacheable, non-coherent, write-back, write allocate
	//cp0 &= ~0x03;
	//cp0 |= 0x02; // K0 = Uncachable
	_mtc0(16, 0, cp0);  
	
	 
	// change tri-state on pins
	TRISD = 0xB640; // MISO, KEY, LED, MOUSE, and UART
	TRISJ = 0xFCFF; // JOY-A, JOY-B, and BUTTON
	CNPUD = 0xC000; // pull-ups for UART

	// set oscillator and timers
	SYSKEY = 0x0; // reset
	SYSKEY = 0xAA996655; // unlock key #1
	SYSKEY = 0x556699AA; // unlock key #2
	
	CFGCONbits.DMAPRI = 1; // DMA does have highest priority (?)
	CFGCONbits.CPUPRI = 0; // CPU does not have highest priority (?)
	CFGCONbits.OCACLK = 1; // use alternate OC/TMR table
	
	PB1DIV = 0x00008001; // divide by 2
	PB2DIV = 0x00008007; //0x00008003; // change PB2 clock to 150 / 8 = 18.75 MHz for SPI and UART
	PB3DIV = 0x00008000; // set OC and TMR clock division by 1
	PB4DIV = 0x00008001; // divide by 2
	PB5DIV = 0x00008001; // divide by 2
	//PB6DIV = 0x00008001; // divide by 2
	PB7DIV = 0x00008000; // CPU clock divide by 1
	SPLLCON = 0x01310301; //0x02270203; // use PLL to bring external 24 MHz into 150 MHz
	
	// PRECON - Set up prefetch
	PRECONbits.PFMSECEN = 0; // Flash SEC Interrupt Enable (Do not generate an interrupt when the PFMSEC bit is set)
	PRECONbits.PREFEN = 0b11; // Predictive Prefetch Enable (Enable predictive prefetch for any address)
	PRECONbits.PFMWS = 0b010; // PFM Access Time Defined in Terms of SYSCLK Wait States (Two wait states)
    
	CFGCONbits.USBSSEN = 1; // USB?

	
	OSCCONbits.SLPEN = 0; // WAIT instruction puts CPU into idle mode
	OSCCONbits.NOSC = 0x1; // switch to SPLL
	OSCCONbits.OSWEN = 1; // enable the switch
	SYSKEY = 0x0; // re-lock
	
	while (OSCCONbits.OSWEN != 0) { } // wait for clock switch to complete

	// set PPS
	SYSKEY = 0x0; // reset
	SYSKEY = 0xAA996655; // unlock key #1
	SYSKEY = 0x556699AA; // unlock key #2
	CFGCONbits.IOLOCK = 0; // PPS is unlocked
	RPD0R = 0xC; // OC1 on pin RPD0 (pixel clock)
	RPD5R = 0xB; // OC2 on pin RPD5 (visible clock)
	RPD2R = 0xB; // OC3 on pin RPD2 (h-sync)
	RPD3R = 0xB; // OC4 on pin RPD3 (v-sync)
	SDI1R = 0xE; // SDI1 on pin RPD6 (miso)
	RPD7R = 0x5; // SDO1 on pin RPD7 (mosi)
	// SCK1 on RD1 (sclk)
	// CS1 on RD4 (cs)
	RPE9R = 0xC; // OC8 on pin RPE9 (audio 1)
	RPE8R = 0xD; // OC9 on pin RPE8 (audio 2)
	RPD14R = 0x1; // U3TX on pin RPD14 (uart tx)
	U3RXR = 0xB; // U3RX on pin RPD15 (uart rx)
	CFGCONbits.IOLOCK = 1; // PPS is locked
	SYSKEY = 0x0; // re-lock
	
	// set OC1 and TMR4, pixel clock
	// I've had to adjust these values many times...
	OC1CON = 0x0; // reset OC1
	OC1CON = 0x00000003; // toggle, use Timer4
	OC1R = 0x0001; //0x0000; // pixel-sync rise (adjust)
	OC1RS = 0x0001; //0x0000; // pixel-sync fall (adjust)
	T4CON = 0x0; // rest Timer4, prescale of 1:1
	TMR4 = 0x0; // zero out counter
	PR4 = 0x01; //0x01; // pixel-reset (minus one)
	
	// set OC2 and OC3 and TMR5, horizontal visible and sync clocks
	OC2CON = 0x0; // reset OC2
	OC2CON = 0x0000000D; // toggle, use Timer5
	OC2R = 0x0230; // h-visible rise
	OC2RS = 0x0A30; //0x0640; // h-blank fall
	OC3CON = 0x0; // reset OC3
	OC3CON = 0x0000000D; // toggle, use Timer5
	OC3R = 0x0110; //0x0690; // h-sync rise
	OC3RS = 0x0A5F; //0x0789; // h-sync fall
	T5CON = 0x0000; // reset Timer5, prescale of 1:1
	TMR5 = 0x0000; // zero out counter (offset some cycles)
	PR5 = 0x0A5F; //0x083F; // h-reset (minus one)
	
	// set OC4 and TMR2, vertical sync clock
	OC4CON = 0x0; // reset OC4
	OC4CON = 0x00000005; // toggle, use Timer2
	OC4R = 0x00F9; // v-sync rise
	OC4RS = 0x82A8; //0x0042; // v-sync fall
	T2CON = 0x0060; //0x0050; // prescale of 1:64
	TMR2 = 0x0000; // zero out counter (offset some cycles)
	PR2 = 0x82A8; //0xA1E7; // v-reset (minus one)
	
	// TMR3, start of scanline
	T3CON = 0x0000; // reset Timer3, prescale of 1:1
	TMR3 = 0x0830; // zero out counter (offset some cycles)
	PR3 = 0x0A5F; //0xA1E7; // v-reset (minus one)
	
	IPC4bits.OC3IP = 0x7; // interrupt priority 7
	IPC4bits.OC3IS = 0x0; // interrupt sub-priority 0
	IFS0bits.OC3IF = 0; // OC3 clear flag
	IEC0bits.OC3IE = 1; // OC3 interrupt on (set priority here?)
	
	OC1CONbits.ON = 1; // turn OC1 on
	OC2CONbits.ON = 1; // turn OC2 on
	OC3CONbits.ON = 1; // turn OC3 on
	OC4CONbits.ON = 1; // turn OC4 on
	OC5CONbits.ON = 1; // turn OC5 on

	// DMA setup
	IEC4bits.DMA0IE = 0; // disable interrupts
	IFS4bits.DMA0IF = 0; // clear flags
	IEC4bits.DMA1IE = 0; // disable interrupts
	IFS4bits.DMA1IF = 0; // clear flags
	DMACONbits.ON = 1; // enable the DMA controller
	
	DCH0CONbits.CHEN = 0; // disable channel
	DCH0ECONbits.CHSIRQ = 14; // start on Timer 3 interrupt
	DCH0ECONbits.SIRQEN = 1; // enable start interrupt
	DCH0INT = 0x0000; // clear all interrupts
	DCH0INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH0INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH0CONbits.CHCHN = 0; // disallow chaining
	DCH0CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH0CONbits.CHPRI = 0x3; // highest priority
	DCH0DSA = VirtToPhys(&PORTE); // transfer destination physical address
	DCH0SSIZ = SCREEN_X; //400; // source size
	DCH0DSIZ = 1; // dst size 
	DCH0CSIZ = SCREEN_X; //400; // 1 byte per event

	DCH1CONbits.CHEN = 0; // disable channel
	DCH1ECONbits.CHSIRQ = 19; // start on Timer 4 interrupt
	DCH1ECONbits.SIRQEN = 1; // enable start interrupt
	DCH1INT = 0x0000; // clear all interrupts
	DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH1INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH1CONbits.CHCHN = 1; // allow chaining
	DCH1CONbits.CHCHNS = 0; // chain from higher channel
	DCH1CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH1CONbits.CHPRI = 0x3; // highest priority
	DCH1SSA = VirtToPhys(screen_zero); // transfer source physical address
	DCH1DSA = VirtToPhys(&PORTE); // transfer destination physical address
	DCH1SSIZ = 1; // source size
	DCH1DSIZ = 1; // dst size 
	DCH1CSIZ = 1; // 1 byte per event
	
	
	// new audio timer
	T8CON = 0x0000; // reset Timer8, prescale of 1:1
	TMR8 = 0x0000; // zero out counter
	PR8 = 0x11E2; // determines audio sample rate, around 32.768 Hz for Gameboy
	IPC9bits.T8IP = 0x3; // audio interrupt priority 3
	IPC9bits.T8IS = 0x0; // sub-priority 1
	IFS1bits.T8IF = 0; // clear flags
	IEC1bits.T8IE = 1; // enable interrupts
	//IEC1bits.T8IE = 0; // disable interrupts
	T8CONbits.ON = 1; // turn on TMR8 
	
	// new frame timer
	T9CON = 0x0070; // reset Timer9, prescale of 1:256
	TMR9 = 0x0000; // zero out counter
	PR9 = 0x4C90; // determines length of two frames of 59.73 Hz on Gameboy
	IPC10bits.T9IP = 0x2; // frame interrupt priority 2
	IPC10bits.T9IS = 0x0; // sub-priority 1
	IFS1bits.T9IF = 0; // clear flags
	IEC1bits.T9IE = 1; // enable interrupts
	//IEC1bits.T9IE = 0; // disable interrupts
	T9CONbits.ON = 0; // turn off TMR9 
	
	// set up PS/2 Keyboard and Mouse on PORTD (RD9-RD10,RD12-RD13)
	CNCONDbits.ON = 1; // turn on interrupt-on-change
	CNCONDbits.EDGEDETECT = 1; // edge detect, not mismatch
	CNNED = 0x1200; // negative edge on RD9 and RD12
	CNFD = 0x0000; // clear flags
	
	IPC30bits.CNDIP = 0x5; // interrupt priority 5
	IPC30bits.CNDIS = 0x0; // interrupt sub-priority 0
	IFS3bits.CNDIF = 0;  // clear interrupt flag
	IEC3bits.CNDIE = 1; // enable interrupts

	// set up UART here
	U3BRG = 0x0079; // 18.75 MHz to 9600 baud = 18750000/(16*9600)-1 = 121.07 = 0x0079
	
	U3MODEbits.STSEL = 0; // 1-Stop bit
	U3MODEbits.PDSEL = 0; // No Parity, 8-Data bits
	U3MODEbits.BRGH = 0; // Standard-Speed mode
	U3MODEbits.ABAUD = 0; // Auto-Baud disabled
	
	//U3MODEbits.URXINV = 1; // reverse polarity
	//U3STAbits.UTXINV = 1; // reverse polarity
	
	U3MODEbits.UEN = 0x0; // just use TX and RX
	
	U3STAbits.UTXISEL = 0x0; // Interrupt after one TX character is transmitted
	U3STAbits.URXISEL = 0x0; // interrupt after one RX character is received
	
	U3STAbits.URXEN = 1; // enable RX
	U3STAbits.UTXEN = 1; // enable TX
	
	IPC39bits.U3RXIP = 0x4; // U3RX interrupt priority level 4
	IPC39bits.U3RXIS = 0x0; // U3RX interrupt sub-priority level 0
	//IPC39bits.U3EIP = 0x5; // U3E interrupt priority level 5
	//IPC39bits.U3EIS = 0x3; // U3E interrupt sub-priority level 3
	IFS4bits.U3RXIF = 0;  // clear interrupt flag
	IEC4bits.U3RXIE = 1; // U3RX interrupt on (set priority here?)
	
	U3MODEbits.ON = 1; // turn UART on
	
	// set up SPI here
	PORTDbits.RD4 = 1; // CS is high
	PORTDbits.RD7 = 1; // MOSI is high
	PORTDbits.RD1 = 0; // CLK is low	
	
	SPI1CONbits.ON = 0; // turn SPI module off
	SPI1CON2 = 0; // disable audio, etc.
	SPI1CONbits.DISSDI = 0; // SDI controlled by module
	SPI1CONbits.DISSDO = 0; // SDO controlled by module
	SPI1CONbits.MODE16 = 0x0; // 8-bit mode
	SPI1CONbits.MSTEN = 1; // host mode
	SPI1CONbits.MSSEN = 0; // client SS disabled
	SPI1CONbits.SSEN = 0; // client SS disabled
	SPI1CONbits.SMP = 0; // input sampled in middle of output time
	SPI1CONbits.CKE = 1; // output changes on idle-to-active clock
	SPI1CONbits.CKP = 0; // clock is idle low, active high
	SPI1CONbits.ON = 1; // turn SPI module on
	
	SPI1BUF = 0xFFFF; // dummy write
	while (SPI1STATbits.SPIRBF == 0) { } // wait
	sdcard_block[0] = SPI1BUF; // dummy read
	
	/*
	// Timer for 'USBA_device_millis' used with USB
	T1CON = 0x0; // disable timer 1
	TMR1 = 0; // clear timer 1
	IEC0bits.T1IE = 1; // Enable interrupt for timer 1
	IFS0bits.T1IF = 0; // Clear interrupt flag for timer 1
	IPC1bits.T1IP = 2; // Interrupt priority 2
	IPC1bits.T1IS = 0; // Sub-priority 0
	PR1 = (unsigned int)(SYS_FREQ / 1000 / 24); // Set up the period
	T1CONbits.TCKPS = 0b10; // pre-scale of 4
	T1CONbits.TON = 1; // Turn on the timer 1
	*/
	
	// Init USBA_device_millis
	// now using H-SYNC interrupt for millis timing
	USBA_device_millis = 0;
	
	// set shadow register priorities???
	PRISS = 0x76543210;
	
	// enable multi-vector interrupts???
	INTCONSET = _INTCON_MVEC_MASK;
	
	// IDK, just turn on interrupts globally???
	__builtin_enable_interrupts();
	

	// default to black screen
	PORTE = 0;
	
	// turn LED off by default
	PORTDbits.RD11 = 1;

	
	// set display buffer
	for (unsigned int y=0; y<SCREEN_Y; y++)
	{
		for (unsigned int x=0; x<SCREEN_X; x++)
		{
#ifdef SPLASH
			screen_buffer[y*SCREEN_X+x] = splash_default[y * SCREEN_X + x];
#else
			screen_buffer[y*SCREEN_X+x] = 0x25; // blue-grey
			//screen_buffer[y*SCREEN_X+x] = (unsigned char)((x + y) % 256); // test pattern
			//if (x % 2 == 0) screen_buffer[y*SCREEN_X+x] = 0xFF; // white
			//else screen_buffer[y*SCREEN_X+x] = 0x1F; // cyan
#endif
		}
	}
	
	// set black scanline
	for (unsigned int x=0; x<SCREEN_X; x++) screen_blank[x] = 0x00;
	
	for (unsigned int i=0; i<8192; i++) audio_buffer[i] = 0x00;
	
	// clear ps2 buffers
	for (unsigned int i=0; i<256; i++)
	{
		ps2_state_array[0][i] = 0x00;
		ps2_state_array[1][i] = 0x00;
		ps2_cursor_x[0][i] = 0x0000;
		ps2_cursor_x[1][i] = 0x0000;
		ps2_cursor_y[0][i] = 0x0000;
		ps2_cursor_y[1][i] = 0x0000;
		
		usb_state_array[i] = 0x00;
		usb_cursor_x[i] = 0x0000;
		usb_cursor_y[i] = 0x0000;
	}	
	
	// turn on video timers
	T4CONbits.ON = 1; // turn on TMR4 (independent of others)
	T3CONbits.ON = 1; // turn on TMR3 (cycle offset pre-calculated above)
	T5CONbits.ON = 1; // turn on TMR5 (cycle offset pre-calculated above)
	T2CONbits.ON = 1; // turn on TMR2 (cycle offset pre-calculated above)
	
	
	return;
}