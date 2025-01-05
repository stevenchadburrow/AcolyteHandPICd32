// Converts a 640x480 .bmp file into raw hex data (hi color makes resolution 640x240 instead)

#include <stdio.h>
#include <stdlib.h>

int main(const int argc, const char **argv)
{
	if (argc < 3)
	{
		printf("Arguments: <input.bmp> <name>\n");

		return 0;
	}

	FILE *input, *output;

	input = fopen(argv[1], "rb");
	if (!input)
	{
		printf("Error reading .bmp\n");

		return 0;
	}

	char filename[256];

	sprintf(filename, "%s.c", argv[2]);

	output = fopen(filename, "wt");
	if (!output)
	{
		printf("Error writing .hex\n");

		return 0;
	}

	fprintf(output, "const unsigned short %s[640*400] = {\n", argv[2]);

	unsigned char buffer;

	unsigned int value;

	unsigned char red, green, blue;

	unsigned int full[640*480];

	for (unsigned long i=0; i<640*480; i++)
	{
		full[i] = 0;
	}

	unsigned long total = 0;

	for (int i=0; i<54; i++) fscanf(input, "%c", &buffer); // header

	for (int i=0; i<480; i++)
	{
		for (int j=0; j<640; j++)
		{
			value = 0x00;

			fscanf(input, "%c%c%c", &blue, &green, &red);

			value = ((red >> 5) << 13) + ((green >> 5) << 10) + ((blue >> 6) << 8) + 
				(((red >> 3) & 0x03) << 6) + (((green >> 2) & 0x07) << 3) + (((blue >> 3) & 0x07)); 
			
			full[i * 640 + j] = value;

			total++;
		}
		
		
	}

	for (int i=440-1; i>=40; i--) // inverting the y-values, because... we need to?
	{
		fprintf(output, "\t");

		for (int j=0; j<640; j++)
		{
			fprintf(output, "0x%04X,", full[(i) * 640 + j]);

			total++;
		}

		fprintf(output, "\n");
	}

	fprintf(output, "};\n");

	fclose(input);

	fclose(output);
	
	return 1;
}
