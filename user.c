

#ifdef USER_SPACE

// this is where the user can put their own programs
// below is just some demo code
// only variables at specific locations outside of the USER_SPACE can be used
// such as 'screen_buffer[]'

void USER_ROM testing_hex(unsigned char value)
{
	while (U3STAbits.UTXBF == 1) { }
	if (value/16 >= 10) U3TXREG = (char)(value/16 + 'A' - 10);
	else U3TXREG = (char)(value/16 + '0');
	while (U3STAbits.UTXBF == 1) { }
	if (value%16 >= 10) U3TXREG = (char)(value%16 + 'A' - 10);
	else U3TXREG = (char)(value%16 + '0');
};

unsigned int USER_RAM testing_value;

// user code starts here at UserMain()
void USER_ROM UserMain()
{
	testing_value = 0x0000;
	
	for (unsigned int i=0; i<256; i++) testing_value += i;
	
	testing_hex((unsigned char)(testing_value / 256));
	
	testing_hex((unsigned char)(testing_value % 256));
	
	while (1) { }
}

#endif
