/*
 * processing.h
 *
 *  Created on: Apr 24, 2013
 *      Author: Strahinja Petrovic
 */

#ifndef PROCESSING_H_
#define PROCESSING_H_
#include "const.h"

void audio_processing();
void initCoef();
double shelving_filter(double sample,double *circularBuffer,int *read, double *coefs);

#endif /* PROCESSING_H_ */
