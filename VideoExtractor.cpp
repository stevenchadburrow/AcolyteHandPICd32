/*

This works for any .flv, .mp4, etc, whatever 'ffmpeg' will handle.
Must also have 'sox' installed for audio purposes.

Run with command like this (for 3 minutes and 21 seconds of video):

./VideoExtractor.o Input.mp4 Output.bin 3 21

When done, check for your drive with this command:

sudo fdisk -l

We are assuming you want /dev/sdc here, so use put it on that drive with a command like this:

sudo dd if=Output.bin of=/dev/sdc bs=100M conv=fsync oflag=seek_bytes seek=0
sudo dd if=Output.bin of=/dev/sdc bs=100M conv=fsync oflag=seek_bytes seek=536870912
sudo dd if=Output.bin of=/dev/sdc bs=100M conv=fsync oflag=seek_bytes seek=1073741824
sudo dd if=Output.bin of=/dev/sdc bs=100M conv=fsync oflag=seek_bytes seek=1610612736

The 'oflag' and 'seek' portions put it at a specific address.  Remove if you want it starting at address 0.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Video is at 60 FPS at 240x192 resolution, color (but will be mono).

unsigned long min_frames = 0; // should be 0
unsigned long max_frames = 0; // means changed by arguments, should be 13138 for Bad Apple, change as need be for debug
unsigned long single = 0; // should be 0 unless testing

int main(const int argc, const char **argv)
{
	if (argc < 5)
	{
		printf("Arguments: <input.xxx> <output.bin> <minutes> <seconds>\n");
		return 0;
	}

	if (max_frames == 0) // ready for a new value
	{
		max_frames = 60 * (atoi(argv[3]) * 60 + atoi(argv[4]));
	}

	char command[512];

	for (int i=0; i<512; i++) command[i] = 0;
	sprintf(command, "ffmpeg -y -i %s -s 240x192 -r 60 Temp1.mp4", argv[1]);
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
		for (int i=0; i<512; i++) command[i] = 0;
		sprintf(command, "ffmpeg -y -i Temp1.mp4 -hide_banner -loglevel error -vf select='between(n\\,%lu\\,%lu),setpts=PTS-STARTPTS' Temp2.mp4", frame, frame+999);
		system(command);

		printf("Frame %lu/%lu\n", frame, max_frames);

		system("rm Temp2.bin");

		for (unsigned long block=0; block<(frame+1000>=max_frames?max_frames-frame:1000); block+=4)
		{
			sprintf(command, "ffmpeg -i Temp2.mp4 -y -hide_banner -loglevel error -vf 'select=eq(n\\,%lu)' -vframes 1 Temp.bmp;", block);
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

					color = (red & 0xE0) + ((green & 0xE0) >> 3) + ((blue & 0xC0) >> 6);

					picture[i * 240 + j] = color;
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
				for (int j=0; j<4; j++)
				{
					for (int k=j*60; k<(j+1)*60; k++)
					{
						shift = 0x00;

						shift = picture[i * 240 + k];

						fprintf(output, "%c", shift);
					}

					fscanf(audio, "%c", &buffer);
					if (buffer == 0x00) buffer = 0x01; // when zero, marks end of video
					fprintf(output, "%c", buffer);
					fscanf(audio, "%c", &buffer);
					fprintf(output, "%c", buffer);
					fscanf(audio, "%c", &buffer);
					fprintf(output, "%c", buffer);
					fscanf(audio, "%c", &buffer);
					fprintf(output, "%c", buffer);
				}
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
