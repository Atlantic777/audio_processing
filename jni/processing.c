/*
 * processing.c
 *
 *  Created on: Apr 24, 2013
 *      Author: Strahinja Petrovic
 */

#include "processing.h"
#include "const.h"
#include "readFile.h"

extern double **sampleBuffer;
extern double **sampleBufferOut;
double coeff[COEF_NUM];
double *circularBufferL;
double *circularBufferR;
int readL = 0;
int readR = 0;

void initCoef()
{
	readCoefs(coeff);  // read coefficients for filter from file
}

void audio_processing()
{
	double left_sample=0;
	double right_sample=0;
	int i=0;

	for(i=0;i<BLOCK_SIZE;i++)
	{
		left_sample = sampleBuffer[0][i]; 			// 0. row - left channel
		right_sample = sampleBuffer[1][i]; 			// 1. row - right channel

		sampleBufferOut[0][i] = shelving_filter(left_sample,circularBufferL,&readL,coeff);
		sampleBufferOut[1][i] = shelving_filter(right_sample,circularBufferR,&readR,coeff);
	}
}

/*
 * int sample - one sample from input signal
 * double *circularBuffer - in this buffer you must put samples for filtering
 * int *read - possition of current sample in BLOCK of samples
 */

double shelving_filter(double sample,double *circularBuffer,int *read, double *coefs)
{
	/* TO DO
	 *
	 */
}



