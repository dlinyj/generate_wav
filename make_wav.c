/* make_wav.c
 * Creates a WAV file from an array of ints.
 * Output is monophonic, signed 16-bit samples
 * copyright
 * Fri Jun 18 16:36:23 PDT 2010 Kevin Karplus
 * Creative Commons license Attribution-NonCommercial
 *  http://creativecommons.org/licenses/by-nc/3.0/
 */
 
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "make_wav.h"
 
void write_little_endian(unsigned int word, int num_bytes, FILE *wav_file)
{
	unsigned buf;
	while(num_bytes>0)
	{   buf = word & 0xff;
		fwrite(&buf, 1,1, wav_file);
		num_bytes--;
	word >>= 8;
	}
}
 
/* information about the WAV file format from
 
http://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 
 */
 
void write_wav(char * filename, unsigned long num_samples, short int * data, int s_rate)
{
	FILE* wav_file;
	unsigned int sample_rate;
	unsigned int num_channels;
	unsigned int bytes_per_sample;
	unsigned int byte_rate;
	unsigned long i;    /* counter for samples */
 
	num_channels = 1;   /* monoaural */
	bytes_per_sample = 2;
 
	if (s_rate<=0) sample_rate = 44100;
	else sample_rate = (unsigned int) s_rate;
 
	byte_rate = sample_rate*num_channels*bytes_per_sample;
 
	wav_file = fopen(filename, "w");
	assert(wav_file);   /* make sure it opened */
 
	/* write RIFF header */
	fwrite("RIFF", 1, 4, wav_file);
	write_little_endian(36 + bytes_per_sample* num_samples*num_channels, 4, wav_file);
	fwrite("WAVE", 1, 4, wav_file);
 
	/* write fmt  subchunk */
	fwrite("fmt ", 1, 4, wav_file);
	write_little_endian(16, 4, wav_file);   /* SubChunk1Size is 16 */
	write_little_endian(1, 2, wav_file);    /* PCM is format 1 */
	write_little_endian(num_channels, 2, wav_file);
	write_little_endian(sample_rate, 4, wav_file);
	write_little_endian(byte_rate, 4, wav_file);
	write_little_endian(num_channels*bytes_per_sample, 2, wav_file);  /* block align */
	write_little_endian(8*bytes_per_sample, 2, wav_file);  /* bits/sample */
 
	/* write data subchunk */
	fwrite("data", 1, 4, wav_file);
	write_little_endian(bytes_per_sample* num_samples*num_channels, 4, wav_file);
	
	for (i=0; i< num_samples; i++)
	{   write_little_endian((unsigned int)(data[i]),bytes_per_sample, wav_file);
	}
 
	fclose(wav_file);
}


#define S_RATE  (44100)
#define BUF_SIZE (S_RATE*2) /* 2 second buffer */
 
//int buffer[BUF_SIZE];
short int buffer[BUF_SIZE];

double d_random(double min, double max)
{
    return min + (max - min) / RAND_MAX * rand();
}
 
int main(int argc, char * argv)
{
	int i;
	//float t;
	//float amplitude = 32000;
	float amplitude = 16000;
	//float freq_Hz = 440;
	float freq_Hz = 100;
	//float phase=0;
	
	srand((unsigned int)time(0));
		
	//float freq_radians_per_sample = freq_Hz*2*M_PI/S_RATE;

	/* fill buffer with a sine wave */
	for (i=0; i<BUF_SIZE; i++)
	{
		//phase += freq_radians_per_sample;
		//buffer[i] = (int)(amplitude * sin(phase));
		
		buffer[i] = (int)(amplitude/10 * sin((float)(2*M_PI*i*freq_Hz/S_RATE)));
		buffer[i] +=(int)(amplitude/20 * sin((float)(2*M_PI*i*10*freq_Hz/S_RATE)));
		buffer[i] +=(int)(amplitude * sin((float)(2*M_PI*i*100*freq_Hz/S_RATE)));
		
		buffer[i] +=(int)amplitude*d_random(-1.0, 1.0); //nois
	}
	write_wav("test.wav", BUF_SIZE, buffer, S_RATE);
 
	return 0;
}
