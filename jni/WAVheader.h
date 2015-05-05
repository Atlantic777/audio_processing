/*
 * WAVheader.h
 *
 *  Created on: Apr 24, 2013
 *      Author: Strahinja Petrovic
 */

#ifndef WAVHEADER_H_
#define WAVHEADER_H_

#include <stdio.h>
#include "const.h"




typedef struct T_WAV_HEADER_RIFF
{
		char		ChunkID[4];			// "RIFF"
		DWORD		ChunkSize;			// = 36 + SubChunk2Size
		char		Format[4];			// "WAVE"
}WAV_HEADER_RIFF;

typedef struct T_WAV_HEADER_FMT
{
		char		SubChunk1ID[4];		// "fmt "
		DWORD		SubChunk1Size;		// 16 for PCM
		WORD	AudioFormat;		// PCM = 1 (i.e. Linear quantization)
		WORD	NumChannels;
		DWORD	SampleRate;
		DWORD	ByteRate;			// = SampleRate * NumChannels * BitsPerSample/8
		WORD	BlockAlign;			// = NumChannels * BitsPerSample/8
		WORD	BitsPerSample;

 } WAV_HEADER_FMT;

typedef struct T_WAV_HEADER_DATA
{
		char		SubChunk2ID[4];		// "data"
		DWORD		SubChunk2Size;		// = NumSamples * NumChannels * BitsPerSample/8
} WAV_HEADER_DATA;

typedef struct T_WAV_HEADER
{
	WAV_HEADER_RIFF		riff;
	WAV_HEADER_FMT		fmt;
	WAV_HEADER_DATA		data;
	long				HeaderSize;
} WAV_HEADER;

int ReadWavHeader (FILE *refFile);
int WriteWavHeader (FILE *refFile);
FILE* OpenWavFileForRead (char *FileName,char *stMode );

#endif
