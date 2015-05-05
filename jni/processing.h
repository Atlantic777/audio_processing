#ifndef PROCESSING_H
#define PROCESSING_H
#include "const.h"

void audio_processing();
void initCoef();

void iklp(double, int);
void ikhp(double, int);
void ikekv(double,double,int);

double shelving_filter(double sample,
                       double *circularBuffer,
                       double *yBuffer,
                       int *read,
                       double *coefs
                       );

#endif
