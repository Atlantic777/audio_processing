/*
 * main.c
 *
 *  Created on: Apr 24, 2013
 *      Author: Strahinja Petrovic
 */

#include <stdlib.h>
#include <string.h>
#include "const.h"
#include "WAVheader.h"
#include "processing.h"

double **sampleBuffer;
double **sampleBufferOut;
static int stop = 0;
extern double *circularBufferL;
extern double *circularBufferR;
extern double *yBufferL;
extern double *yBufferR;
WAV_HEADER inWAVhdr,outputWAVhdr;

int main()
{
	FILE *wav_in = NULL;
	FILE *wav_out = NULL;
	char WavInputName[256];
	char WavOutputNameOut[256];
	int i = 0;
	int j = 0;
	int k = 0;

	circularBufferR = (double*) calloc(sizeof(double),COEF_NUM);
	circularBufferL = (double*) calloc(sizeof(double),COEF_NUM);
    yBufferR = (double*)calloc(sizeof(double), COEF_NUM);
    yBufferL = (double*)calloc(sizeof(double), COEF_NUM);
	sampleBuffer = (double**) calloc(MAX_NUM_CHANNEL,sizeof(double*));
	sampleBufferOut = (double**) calloc(MAX_NUM_CHANNEL,sizeof(double*));

	for(i=0;i<MAX_NUM_CHANNEL;i++) {
		sampleBuffer[i] = (double*) calloc(BLOCK_SIZE,sizeof(double));
		sampleBufferOut[i] = (double*) calloc(BLOCK_SIZE,sizeof(double));
	}
	// Open input and output wav files
	//-------------------------------------------------
	strcpy(WavInputName,"Freq_sweep.wav");			//write name of input signal
	wav_in = OpenWavFileForRead (WavInputName,"rb");
		strcpy(WavOutputNameOut,"outFir.wav");
		wav_out = OpenWavFileForRead (WavOutputNameOut,"wb");
	//-------------------------------------------------
	// Read input wav header
	//-------------------------------------------------
	ReadWavHeader(wav_in);
	//-------------------------------------------------
	// Write wav header to output file
	//-------------------------------------------------
	outputWAVhdr = inWAVhdr;
	// If number of output channle is changed
	//-------------------------------------------------
	outputWAVhdr.fmt.NumChannels = inWAVhdr.fmt.NumChannels; // change number of channels
	int oneChannelSubChunk2Size = inWAVhdr.data.SubChunk2Size/inWAVhdr.fmt.NumChannels;
	int oneChannelByteRate = inWAVhdr.fmt.ByteRate/inWAVhdr.fmt.NumChannels;
	int oneChannelBlockAlign = inWAVhdr.fmt.BlockAlign/inWAVhdr.fmt.NumChannels;
	outputWAVhdr.data.SubChunk2Size = oneChannelSubChunk2Size*outputWAVhdr.fmt.NumChannels;
	outputWAVhdr.fmt.ByteRate = oneChannelByteRate*outputWAVhdr.fmt.NumChannels;
	outputWAVhdr.fmt.BlockAlign = oneChannelBlockAlign*outputWAVhdr.fmt.NumChannels;

	WriteWavHeader(wav_out);
    initCoef(0);							// get coefficients for FIR or IIR filter

	// Processing loop
	//-------------------------------------------------
	{
		int sample;
		int BytesPerSample = inWAVhdr.fmt.BitsPerSample/8;
		const double SAMPLE_SCALE = -(double)(1 << 31);		//2^31
		int iNumSamples = inWAVhdr.data.SubChunk2Size/(inWAVhdr.fmt.NumChannels*inWAVhdr.fmt.BitsPerSample/8);
		int iterNum = 0;
		int diff=0;
		int BLOCK_SIZE_t = 0;

		if(iNumSamples % BLOCK_SIZE == 0) 		// Verify that the file length is a multiple of BLOCK_SIZE
		{
			iterNum = iNumSamples/BLOCK_SIZE;
			BLOCK_SIZE_t = BLOCK_SIZE;
		}
		else
		{
			iterNum = iNumSamples/BLOCK_SIZE+1;
			diff = iNumSamples % BLOCK_SIZE;
		}
		// exact file length should be handled correctly...
		for( i=0; i<iterNum; i++)
		{
			if(i == iterNum -1 && diff != 0)
			{
				BLOCK_SIZE_t = diff;
			}
			for( j=0; j<BLOCK_SIZE_t; j++)
			{
				for( k=0; k<inWAVhdr.fmt.NumChannels; k++)
				{
					sample = 0;
					fread(&sample, BytesPerSample, 1, wav_in);
					sample = sample << (32 - inWAVhdr.fmt.BitsPerSample);
					sampleBuffer[k][j] = (double)sample / SAMPLE_SCALE;				// scale sample to 1.0/-1.0 range
				}
			}

			if(stop == 1) {
				stop = 0;
				return;
			}

			audio_processing(); 												// call audio processing for BLOCK_SIZE samples !

			for( j=0; j<BLOCK_SIZE_t; j++)
			{
				for( k=0; k<outputWAVhdr.fmt.NumChannels; k++)
				{
						sample = sampleBufferOut[k][j] * SAMPLE_SCALE ;				// crude, non-rounding
						sample = sample >> (32 - inWAVhdr.fmt.BitsPerSample);
						fwrite(&sample, inWAVhdr.fmt.BitsPerSample/8, 1, wav_out);  //write in wav file FIR
				}
			}
		}
	}

	// Close files
	//-------------------------------------------------

		fclose(wav_in);
		fclose(wav_out);
		free(circularBufferL);
		free(circularBufferR);
		for(i = 0;i<MAX_NUM_CHANNEL;i++) {
			free(sampleBuffer[i]);
			free(sampleBufferOut[i]);
		}
		free(sampleBuffer);
		free(sampleBufferOut);
		return ;
}
