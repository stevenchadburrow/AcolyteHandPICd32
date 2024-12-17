/*

How to extract data for Bad Apple?

First, I found the TouhouBadApple.flv online on archive.org.  Just get a clean version.

Then run:

ffmpeg -i TouhouBadApple.flv -s 240x192 -r 60 TouhouBadApple-Compressed.mp4

That brings the video into a compressed form.  

For the audio side of things:

ffmpeg -i TouhouBadApple.flv TouhouBadApple.wav
sox TouhouBadApple.wav -r 23040 -c 1 -b 32 -e signed-integer TouhouBadApple-Compressed.wav

Now you have everything you need to run the ExtractData.cpp program!

Finally to put on SD card:

sudo fdisk -l

Make sure to find the right drive!!!

Then:

sudo dd if=TouhouBadApple-Final.bin of=/dev/sdc bs=100M conv=fsync

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Video is at 60 FPS at 240x192 resolution, color (but will be mono).

unsigned long min_frames = 0; // should be 0
unsigned long max_frames = 16000; // should be 13138, change as need be for debug
unsigned long single = 0; // should be 0 unless testing

int main(const int argc, const char **argv)
{
	if (argc < 3)
	{
		printf("Arguments: <input.xxx> <output.bin>\n");
		return 0;
	}

	char command[512];

	for (int i=0; i<512; i++) command[i] = 0;
	sprintf(command, "ffmpeg -y -i %s -s 240x192 -r 60 Temp.mp4", argv[1]);
	system(command);

	for (int i=0; i<512; i++) command[i] = 0;
	sprintf(command, "ffmpeg -y -i %s Temp1.wav", argv[1]);
	system(command);

	for (int i=0; i<512; i++) command[i] = 0;
	sprintf(command, "sox Temp1.wav -r 11520 -c 1 -b 32 -e signed-integer Temp2.wav");
	system(command);

	int bytes = 1;
	unsigned char buffer = 0;

	FILE *output = NULL;

	output = fopen(argv[2], "wb");
	if (!output)
	{
		printf("Error!\n");
		
		return 0;
	}

	fclose(output);

	unsigned char picture[49152];

	unsigned char red, green, blue;

	unsigned char color;

	unsigned char shift;

	FILE *video = NULL, *audio = NULL;

	audio = fopen("Temp2.wav", "rb");
	if (!audio)
	{
		printf("Error!\n");

		return 0;
	}

	for (int i=0; i<44; i++) fscanf(audio, "%c", &buffer); // header

	for (unsigned long frame=(single==0?min_frames:max_frames-1); frame<max_frames; frame+=1000) // change to 13138 frames in the video
	{
		printf("Frame %lu/%lu\n", frame, max_frames);

		system("rm Temp2.bin");

		for (unsigned long block=0; block<(frame+1000>=max_frames?max_frames-frame:1000); block+=2)
		{
			sprintf(command, "ffmpeg -i Temp.mp4 -y -hide_banner -loglevel error -vf 'select=eq(n\\,%lu)' -vframes 1 Temp.bmp;", frame+block);
			system(command);

			if (block % 100 == 0) printf("Block %lu\n", block);

			video = NULL;

			video = fopen("Temp.bmp", "rb");
			if (!video)
			{
				printf("Error!\n");
			
				return 0;
			}

			for (int i=0; i<54; i++) fscanf(video, "%c", &buffer); // header

			bytes = 1;

			for (int i=0; i<192; i++)
			{
				for (int j=0; j<240; j++)
				{
					fscanf(video, "%c%c%c", &blue, &green, &red);

					color = (unsigned char)((red + green + blue) / 3);

					if (color >= 0xC0) picture[i * 240 + j] = 0xC0;
					else if (color >= 0x80) picture[i * 240 + j] = 0x80;
					else if (color >= 0x40) picture[i * 240 + j] = 0x40;
					else picture[i * 240 + j] = 0x00;

					/*
					if (red >= 0x80 || green >= 0x80 || blue >= 0x80)
					{
						picture[i * 240 + j] = 0x80;
					}
					else
					{
						picture[i * 240 + j] = 0x00;
					}
					*/
				}
			}

			fclose(video);

			output = NULL;

			output = fopen("Temp1.bin", "wb");
			if (!output)
			{
				printf("Error!\n");
			
				return 0;
			}

			shift = 0x00;

			for (int i=192-1; i>=0; i--) // invert y-values
			{
				for (int j=0; j<120; j+=4)
				{
					shift = 0x00;

					for (int l=0; l<4; l++)
					{
						shift = shift >> 2;

						shift = shift | picture[i * 240 + j + l];
					}

					fprintf(output, "%c", shift);
				}

				/*
				for (int j=0; j<120; j+=8)
				{
					shift = 0x00;

					for (int l=0; l<8; l++)
					{
						shift = shift >> 1;

						shift = shift | picture[i * 240 + j + l];
					}
					
					fprintf(output, "%c", shift);
				}
				*/

				fscanf(audio, "%c", &buffer);  // reducing 32-bit to 16-bit
				fscanf(audio, "%c", &buffer);
				fscanf(audio, "%c", &buffer);
				if (buffer == 0x00) buffer = 0x01; // when zero, marks end of video
				fprintf(output, "%c", buffer);
				fscanf(audio, "%c", &buffer);
				fprintf(output, "%c", buffer);
				

				for (int j=120; j<240; j+=4)
				{
					shift = 0x00;

					for (int l=0; l<4; l++)
					{
						shift = shift >> 2;

						shift = shift | picture[i * 240 + j + l];
					}

					fprintf(output, "%c", shift);
				}

				/*
				for (int j=0; j<120; j+=8)
				{
					shift = 0x00;

					for (int l=0; l<8; l++)
					{
						shift = shift >> 1;

						shift = shift | picture[i * 240 + j + l];
					}
					
					fprintf(output, "%c", shift);
				}
				*/

				fscanf(audio, "%c", &buffer);  // reducing 32-bit to 16-bit
				fscanf(audio, "%c", &buffer);
				fscanf(audio, "%c", &buffer);
				if (buffer == 0x00) buffer = 0x01; // when zero, marks end of video
				fprintf(output, "%c", buffer);
				fscanf(audio, "%c", &buffer);
				fprintf(output, "%c", buffer);
			}

			fclose(output);

			sprintf(command, "cat Temp1.bin >> Temp2.bin");
			system(command);
		}	

		sprintf(command, "cat Temp2.bin >> %s", argv[2]);
		system(command);
	}

	output = NULL;

	output = fopen("Temp2.bin", "wb");
	if (!output)
	{
		printf("Error!\n");
	
		return 0;
	}

	for (int i=0; i<512; i++)
	{
		fprintf(output, "%c", 0x00); // final block has all zeros, end of video
	}

	fclose(output);

	sprintf(command, "cat Temp2.bin >> %s", argv[2]);
	system(command);

	printf("Success!\n");

	return 1;
}
