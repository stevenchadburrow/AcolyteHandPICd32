// Converts a 400x300 .bmp file into raw hex data

#include <stdio.h>
#include <stdlib.h>

int main(const int argc, const char **argv)
{
	if (argc < 3)
	{
		printf("Arguments: <input.bmp> <output.hex>\n");

		return 0;
	}

	FILE *input, *output;

	input = fopen(argv[1], "rb");
	if (!input)
	{
		printf("Error reading .bmp\n");

		return 0;
	}

	output = fopen(argv[2], "wt");
	if (!output)
	{
		printf("Error writing .hex\n");

		return 0;
	}

	unsigned char buffer;

	unsigned char value;

	unsigned char red, green, blue;

	unsigned char full[400*300];

	for (unsigned long i=0; i<400*300; i++)
	{
		full[i] = 0;
	}

	unsigned long total = 0;

	for (int i=0; i<54; i++) fscanf(input, "%c", &buffer); // header

	for (int i=0; i<300; i++)
	{
		for (int j=0; j<400; j++)
		{
			value = 0x00;

			fscanf(input, "%c%c%c", &blue, &green, &red);

			value = ((red / 32) << 5) + ((green / 32) << 2) + (blue / 64); 
			
			full[i * 400 + j] = value;

			total++;
		}
		
		
	}

	for (int i=300-1; i>=0; i--) // inverting the y-values, because... we need to?
	{
		fprintf(output, "\t");

		for (int j=0; j<400; j++)
		{
			fprintf(output, "0x%02X,", full[i * 400 + j]);

			total++;
		}

		fprintf(output, "\n");
	}

	fclose(input);

	fclose(output);
	
	return 1;
}
