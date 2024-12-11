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

sudo dd if=TouhouBadApple-Final.bin of=/dev/sdd bs=100M conv=fsync

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Video is at 60 FPS at 240x192 resolution, color (but will be mono).

unsigned long min_frames = 0; // should be 0
unsigned long max_frames = 13138; // should be 13138, change as need be for debug
unsigned long single = 0; // should be 0 unless testing

int main()
{
	int bytes = 1;
	unsigned char buffer = 0;


	FILE *output = NULL;

	output = fopen("TouhouBadApple-Final.bin", "wb");
	if (!output)
	{
		printf("Error!\n");
		
		return 0;
	}

	fclose(output);


	char command[512];

	unsigned char picture[49152];

	unsigned char red, green, blue;

	unsigned char shift;

	FILE *video = NULL, *audio = NULL;

	audio = fopen("TouhouBadApple-Compressed.wav", "rb");
	if (!audio)
	{
		printf("Error!\n");

		return 0;
	}

	for (int i=0; i<44; i++) fscanf(audio, "%c", &buffer); // header
	
	for (unsigned long frame=(single==0?min_frames:max_frames-1); frame<max_frames; frame+=1) // change to 13138 frames in the video
	{
		if (frame % 10 == 0) printf("Frame %lu/%lu\n", frame, max_frames);

		sprintf(command, "ffmpeg -i TouhouBadApple-Compressed.mp4 -y -hide_banner -loglevel error -vf 'select=eq(n\\,%lu)' -vframes 1 Frame.bmp;", frame);

		system(command);

		video = NULL;

		video = fopen("Frame.bmp", "rb");
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

				if (red >= 0x80 || green >= 0x80 || blue >= 0x80)
				{
					picture[i * 240 + j] = 0x80;
				}
				else
				{
					picture[i * 240 + j] = 0x00;
				}
			}
		}

		fclose(video);

		output = NULL;

		output = fopen("TouhouBadApple-Single.bin", "wb");
		if (!output)
		{
			printf("Error!\n");
		
			return 0;
		}

		shift = 0x00;

		for (int i=192-1; i>=0; i--) // invert y-values
		{
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

			fscanf(audio, "%c", &buffer);
			fscanf(audio, "%c", &buffer);
			fscanf(audio, "%c", &buffer);
			fscanf(audio, "%c", &buffer); // reducing 32-bit to 8-bit

			fprintf(output, "%c", buffer);

			for (int j=120; j<240; j+=8)
			{
				shift = 0x00;

				for (int l=0; l<8; l++)
				{
					shift = shift >> 1;

					shift = shift | picture[i * 240 + j + l];
				}
				
				fprintf(output, "%c", shift);
			}

			fscanf(audio, "%c", &buffer);
			fscanf(audio, "%c", &buffer);
			fscanf(audio, "%c", &buffer);
			fscanf(audio, "%c", &buffer); // reducing 32-bit to 8-bit

			fprintf(output, "%c", buffer);
		}

		fclose(output);

		sprintf(command, "cat TouhouBadApple-Single.bin >> TouhouBadApple-Final.bin");

		system(command);
	}

	
	printf("Success!\n");

	return 1;
}
