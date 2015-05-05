/*
 * processing.c
 *
 *  Created on: Apr 24, 2013
 *      Author: Strahinja Petrovic
 */

#include "processing.h"
#include "const.h"
#include "readFile.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

extern double **sampleBuffer;
extern double **sampleBufferOut;
double coeff[COEF_NUM];
double *circularBufferL;
double *circularBufferR;
double *yBufferL;
double *yBufferR;
int readL = 0;
int readR = 0;

#define HP_ALPHA -0.7
#define HP_K     +1

#define LP_ALPHA  0.7
#define LP_K      -1

#define EQ_ALPHA  0.88
#define EQ_BETA  -0.81
#define EQ_K      1

double *hp_coefs;
double *lp_coefs;
double *ekv_coefs;

int num_coefs = 2;

void ikhp(double, int);
void iklp(double, int);
void ikekv(double, double, int);

// TODO: dodati biranje opcije, po 2 opcije za svaki, ukupno 6 komada

void initCoef()
{
	//readCoefs(coeff);  // read coefficients for filter from file
	hp_coefs = malloc(4*sizeof(double));
	lp_coefs = malloc(4*sizeof(double));
	ekv_coefs = malloc(6*sizeof(double));

	ikhp(HP_ALPHA, HP_K);
	iklp(LP_ALPHA, LP_K);
	ikekv(EQ_ALPHA, EQ_BETA, EQ_K);

    int i;
    for (i = 0; i < COEF_NUM*2; ++i) {
        printf("%.2lf ", ekv_coefs[i]);
    }
    puts("");
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

		sampleBufferOut[0][i] = shelving_filter(left_sample, circularBufferL, yBufferL, &readL,hp_coefs);
		sampleBufferOut[1][i] = shelving_filter(right_sample,circularBufferR, yBufferR, &readR,hp_coefs);
	}
}

/*
 * int sample - one sample from input signal
 * double *circularBuffer - in this buffer you must put samples for filtering
 * int *read - possition of current sample in BLOCK of samples
 */

double shelving_filter(double sample,double *circularBuffer, double *yBuffer, int *read, double *coefs)
{
    int i, target;
    double fir_sum = 0;
    double iir_sum = 0;
	double res ;

    circularBuffer[*read] = sample;
    /* printf("Storing new sample (%.4lf) at %d\n", sample, *read); */
    /* printf("x:[%.4lf %.4lf %.4lf]\n", circularBuffer[0], circularBuffer[1], circularBuffer[6]); */
    /* printf("y:[%.4lf %.4lf %.4lf]\n", yBuffer[0], yBuffer[1], yBuffer[6]); */

    /* printf("Sample: %.2lf\n", sample); */

    for (i = 0; i < COEF_NUM; ++i) {
        target = (*read-i+COEF_NUM)%COEF_NUM;
        fir_sum += coefs[i]*circularBuffer[target];
        /* printf("FIR target %d, %.4lf * %.4lf\n", target, coefs[i], circularBuffer[target]); */
    }

    for (i = 1; i < COEF_NUM; ++i) {
        target = (*read-i+COEF_NUM)%COEF_NUM;
        iir_sum += coefs[COEF_NUM+i]*yBuffer[target];
        /* printf("IIR target %d, %.4lf * %.4lf\n", target, coefs[COEF_NUM+i], yBuffer[target]); */
    }

    /* printf("fir sum: %.4lf iir_sum %.4lf\n", fir_sum, iir_sum); */
    res = (1/coefs[COEF_NUM])*(fir_sum - iir_sum);
    yBuffer[*read]        = res;

    *read = (*read+1)%COEF_NUM;

    /* printf("Result is: %.4lf\n", res); */
    /* puts("--------"); */
    return res;

}

void ikhp(double alpha, int k) {
	int K = pow(2, k);
	hp_coefs[0] = 1+K+K*alpha-alpha;  //P0
	hp_coefs[1] = -alpha-alpha*K-K+1; //P1

	hp_coefs[2] = 2;                  //Q0
	hp_coefs[3] = -2*alpha;           //Q1
}

void iklp(double alpha, int k) {
	int K = pow(2, k);

	lp_coefs[0] = 1+K+alpha-K*alpha;  //P0
	lp_coefs[1] = -alpha-alpha*K-1+K; //P1

	lp_coefs[2] = 2;                  //Q0
	lp_coefs[3] = -2*alpha;           //Q1
}

void ikekv(double alpha, double beta, int k) {
	int K = pow(2, k);

	ekv_coefs[0] = K*(1-alpha)+1+alpha;   //P0
	ekv_coefs[1] = -2*beta*(1+alpha);     //P1
	ekv_coefs[2] = K*(alpha-1)+alpha+1;   //P2

	ekv_coefs[3] = 2;                     //Q0
	ekv_coefs[4] = 2*(-beta*(1+alpha));       //Q1
	ekv_coefs[5] = 2*alpha;                 //
}
