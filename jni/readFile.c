/*
 * readFile.c
 *
 *  Created on: May 14, 2013
 *      Author: Strahinja Petrovic
 */

#include "readFile.h"
#include <stdio.h>
#include "const.h"
#include <stdlib.h>
#include <string.h>

char coefFIRName[256];
FILE *coeffFIR = NULL;

void readCoefs(double *bufferFIR)
{
			int i;
			strcpy(coefFIRName,"/sdcard/coeffFIR.txt");
			coeffFIR = fopen (coefFIRName,"rb");

			if(coeffFIR == NULL)
			{
				printf("Cannot open file %s, readFile.c\n", coefFIRName);
				exit(1);
			}
				for(i =0;i<COEF_NUM;i++)
					{
						fscanf(coeffFIR,"%lf",&bufferFIR[i]);
					}
			fclose (coeffFIR);
}



