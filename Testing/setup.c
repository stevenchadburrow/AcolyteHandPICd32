
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
	TRISK = 0x00BF; // JOY-A and BUTTON
	TRISF = 0x01FF; // JOY-B (and USBID)
	CNPUD = 0xC000; // pull-ups for UART

	// set oscillator and timers
	SYSKEY = 0x0; // reset
	SYSKEY = 0xAA996655; // unlock key #1
	SYSKEY = 0x556699AA; // unlock key #2
	
	CFGCONbits.DMAPRI = 1; // DMA does have highest priority (?)
	CFGCONbits.CPUPRI = 0; // CPU does not have highest priority (?)
	CFGCONbits.OCACLK = 1; // use alternate OC/TMR table
	
	PB1DIV = 0x00008001; // divide by 2
	PB2DIV = 0x00008004; // change PB2 clock to 216 / 5 = 43.2 MHz for SPI and UART
	PB3DIV = 0x00008000; // set OC and TMR clock division by 1
	PB4DIV = 0x00008001; // divide by 2
	PB5DIV = 0x00008001; // divide by 2
	//PB6DIV = 0x00008001; // divide by 2
	PB7DIV = 0x00008000; // CPU clock divide by 1
	SPLLCON = 0x01110001; // use PLL to bring external 24 MHz into 216 MHz
	
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
	//RPD0R = 0xC; // OC1 on pin RPD0 (pixel clock)
	RPD5R = 0xB; // OC2 on pin RPD5 (visible clock)
	RPD2R = 0xB; // OC3 on pin RPD2 (h-sync)
	RPD3R = 0xC; // OC7 on pin RPD3 (v-sync)
	SDI1R = 0xE; // SDI1 on pin RPD6 (miso)
	RPD7R = 0x5; // SDO1 on pin RPD7 (mosi)
	// SCK1 on RD1 (sclk)
	// CS1 on RD4 (cs)
	RPD14R = 0x1; // U3TX on pin RPD14 (uart tx)
	U3RXR = 0xB; // U3RX on pin RPD15 (uart rx)
	CFGCONbits.IOLOCK = 1; // PPS is locked
	SYSKEY = 0x0; // re-lock
	 
	// set OC2 and OC3 and TMR5, horizontal visible and sync clocks
	OC2CON = 0x0; // reset OC2
	OC2CON = 0x0000000D; // toggle, use Timer5
	OC2R = 0x02D0; // h-visible rise
	OC2RS = 0x0CD0; // h-blank fall
	OC3CON = 0x0; // reset OC3
	OC3CON = 0x0000000D; // toggle, use Timer5
	OC3R = 0x0000; // h-sync rise
	OC3RS = 0x00E0; // h-sync fall
	T5CON = 0x0000; // reset Timer5, prescale of 1:1
	TMR5 = 0x0000; // zero out counter (offset some cycles)
	PR5 = 0x0D2F; // h-reset (minus one)
	
	// set OC7 and TMR6, vertical sync clock
	OC7CON = 0x0; // reset OC7
	OC7CON = 0x0025; // toggle, use Timer6, 32-bit
	OC7R = 0x00000000; // v-sync rise
	OC7RS = 0x00002790; // v-sync fall
	T6CON = 0x0008; // prescale of 1:1, 32-bit
	TMR6 = 0x00000000; // zero out counter (offset some cycles)
	PR6 = 0x0036E9DF; // v-reset (minus one)
	
	// TMR3, start of scanline
	T3CON = 0x0000; // reset Timer3, prescale of 1:1
	TMR3 = 0x0A60; // zero out counter (offset some cycles)
	PR3 = 0x0D2F; // h-reset (minus one)
	
	// TMR4, end of scanline
	T4CON = 0x0000; // rest Timer4, prescale of 1:1
	TMR4 = 0x0260; // zero out counter
	PR4 = 0x0D2F; // h-reset (minus one)
	
	IPC4bits.OC3IP = 0x7; // interrupt priority 7
	IPC4bits.OC3IS = 0x0; // interrupt sub-priority 0
	IFS0bits.OC3IF = 0; // OC3 clear flag
	IEC0bits.OC3IE = 1; // OC3 interrupt on (set priority here?)
	
	OC2CONbits.ON = 1; // turn OC2 on
	OC3CONbits.ON = 1; // turn OC3 on
	OC7CONbits.ON = 1; // turn OC7 on

	// DMA setup
	IEC4bits.DMA0IE = 0; // disable interrupts
	IFS4bits.DMA0IF = 0; // clear flags
	IEC4bits.DMA1IE = 0; // disable interrupts
	IFS4bits.DMA1IF = 0; // clear flags
	DMACONbits.ON = 1; // enable the DMA controller
	
	DCH0CONbits.CHEN = 0; // disable channel
	DCH0ECONbits.CHSIRQ = 12; // start on Output Compare 2 interrupt
	DCH0ECONbits.SIRQEN = 1; // enable start interrupt
	DCH0INT = 0x0000; // clear all interrupts
	DCH0INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH0INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH0CONbits.CHCHN = 0; // disallow chaining
	DCH0CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH0CONbits.CHPRI = 0x3; // highest priority
	DCH0SSA = VirtToPhys(screen_zero); // transfer source physical address
	DCH0DSA = VirtToPhys(((unsigned char*)&PORTH + 1)); // transfer destination physical address
	//DCH0DSA = VirtToPhys(&PORTH); // transfer destination physical address
	DCH0SSIZ = 1; // source size
	DCH0DSIZ = 1; // dst size 
	DCH0CSIZ = 1; // 1 byte per event

	DCH1CONbits.CHEN = 0; // disable channel
	DCH1ECONbits.CHSIRQ = 14; // start on Timer 3 interrupt
	DCH1ECONbits.SIRQEN = 1; // enable start interrupt
	DCH1INT = 0x0000; // clear all interrupts
	DCH1INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH1INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH1CONbits.CHCHN = 1; // allow chaining
	DCH1CONbits.CHCHNS = 0; // chain from higher channel
	DCH1CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH1CONbits.CHPRI = 0x3; // highest priority
	DCH1DSA = VirtToPhys(((unsigned char*)&PORTH + 1)); // transfer destination physical address
	//DCH1DSA = VirtToPhys(&PORTH); // transfer destination physical address
	DCH1SSIZ = SCREEN_X; // source size
	DCH1DSIZ = 1; // dst size 
	DCH1CSIZ = SCREEN_X; // X byte per event
	
	DCH2CONbits.CHEN = 0; // disable channel
	DCH2ECONbits.CHSIRQ = 19; // start on Timer 4 interrupt
	DCH2ECONbits.SIRQEN = 1; // enable start interrupt
	DCH2INT = 0x0000; // clear all interrupts
	DCH2INTbits.CHBCIF = 0; // clear transfer complete flag
	DCH2INTbits.CHBCIE = 1; // enable transfer complete interrupt
	DCH2CONbits.CHCHN = 1; // allow chaining
	DCH2CONbits.CHCHNS = 0; // chain from higher channel
	DCH2CONbits.CHAED = 1; // get next DMA ready for quick transition???
	DCH2CONbits.CHPRI = 0x3; // highest priority
	DCH2SSA = VirtToPhys(screen_zero); // transfer source physical address
	DCH2DSA = VirtToPhys(((unsigned char*)&PORTH + 1)); // transfer destination physical address
	//DCH2DSA = VirtToPhys(&PORTH); // transfer destination physical address
	DCH2SSIZ = 1; // source size
	DCH2DSIZ = 1; // dst size 
	DCH2CSIZ = 1; // 1 byte per event

 
	// set up PS/2 Keyboard and Mouse on PORTD (RD9-RD10,RD12-RD13)
	CNCONDbits.ON = 1; // turn on interrupt-on-change
	CNCONDbits.EDGEDETECT = 1; // edge detect, not mismatch
	CNNED = 0x1200; // negative edge on RD9 and RD12
	CNFD = 0x0000; // clear flags
	
	IPC30bits.CNDIP = 0x4; // interrupt priority 4
	IPC30bits.CNDIS = 0x0; // interrupt sub-priority 0
	IFS3bits.CNDIF = 0;  // clear interrupt flag
	IEC3bits.CNDIE = 1; // enable interrupts

	// set up UART here
	U3BRG = 0x0118; // 43.2 MHz to 9600 baud = 43200000/(16*9600)-1 = 280.25 = 0x0118
	
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
	
	IPC39bits.U3RXIP = 0x3; // U3RX interrupt priority level 3
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
	
	
	// set shadow register priorities???
	PRISS = 0x76543210;
	
	// enable multi-vector interrupts???
	INTCONSET = _INTCON_MVEC_MASK;
	
	// IDK, just turn on interrupts globally???
	__builtin_enable_interrupts();
	

	// default to black screen
	PORTH = 0;
	
	// turn LED off by default
	PORTDbits.RD11 = 1;

	
	for (unsigned short i=0; i<SCREEN_X*2; i++) screen_line[i] = 0x00;


/*	
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
*/
/*	
	for (unsigned int i=0; i<2; i++)
	{
		for (unsigned int j=0; j<8192; j++) audio_buffer[i][j] = 0x00;
	}
	
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
*/
	
	// turn on video timers
	T5CONbits.ON = 1; // turn on TMR5 horizontal sync (cycle offset pre-calculated above)
	T3CONbits.ON = 1; // turn on TMR3 scanline start (cycle offset pre-calculated above)
	T4CONbits.ON = 1; // turn on TMR4 scanline end (independent of others)
	T6CONbits.ON = 1; // turn on TMR6 vertical sync (cycle offset pre-calculated above)
	
	
	return;
}
