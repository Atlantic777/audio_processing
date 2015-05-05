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
WAV_HEADER inWAVhdr, outputWAVhdr;

int main(int argc, char *argv[])
{
    FILE *wav_in  = NULL;
    FILE *wav_out = NULL;
    char WavInputName[256];
    char WavOutputName[256];
    int i, j, k;
    i = j = k = 0;

    circularBufferR = (double*) calloc(sizeof(double), COEF_NUM);
    circularBufferL = (double*) calloc(sizeof(double), COEF_NUM);
    yBufferR        = (double*) calloc(sizeof(double), COEF_NUM);
    yBufferL        = (double*) calloc(sizeof(double), COEF_NUM);
    sampleBuffer    = (double**)calloc(MAX_NUM_CHANNEL, sizeof(double*));
    sampleBufferOut = (double**)calloc(MAX_NUM_CHANNEL, sizeof(double*));

    for (i = 0; i < MAX_NUM_CHANNEL; ++i) {
        sampleBuffer[i]     = (double*) calloc(BLOCK_SIZE, sizeof(double));
        sampleBufferOut[i]  = (double*) calloc(BLOCK_SIZE, sizeof(double));
    }

    strcpy(WavInputName, SONG_NAME);
    wav_in = OpenWavFileForRead(WavInputName, "rb");

    strcpy(WavOutputName, DST_NAME);
    wav_out = OpenWavFileForRead(WavOutputName, "wb");

    ReadWavHeader(wav_in);

    outputWAVhdr = inWAVhdr;

    outputWAVhdr.fmt.NumChannels    = inWAVhdr.fmt.NumChannels;
    int oneChannelSubChunk2Size     = inWAVhdr.data.SubChunk2Size/inWAVhdr.fmt.NumChannels;
    int oneChannelByteRate          = inWAVhdr.fmt.ByteRate/inWAVhdr.fmt.NumChannels;
    int oneChannelBlockAlign        = inWAVhdr.fmt.BlockAlign/inWAVhdr.fmt.NumChannels;
    outputWAVhdr.data.SubChunk2Size = oneChannelSubChunk2Size*outputWAVhdr.fmt.NumChannels;
    outputWAVhdr.fmt.ByteRate       = oneChannelByteRate*outputWAVhdr.fmt.NumChannels;
    outputWAVhdr.fmt.BlockAlign     = oneChannelBlockAlign*outputWAVhdr.fmt.NumChannels;

    WriteWavHeader(wav_out);
    initCoef(0);


    // processing loop
    {
        int sample;
        int BytesPerSample          = inWAVhdr.fmt.BitsPerSample/8;
        const double SAMPLE_SCALE   = -(double)(1 << 31);
        int iNumSamples     = inWAVhdr.data.SubChunk2Size/(inWAVhdr.fmt.NumChannels*inWAVhdr.fmt.BitsPerSample/8);
        int iterNum         = 0;
        int diff            = 0;
        int BLOCK_SIZE_t    = 0;

        if(iNumSamples % BLOCK_SIZE == 0) {
            iterNum         = iNumSamples / BLOCK_SIZE;
            BLOCK_SIZE_t    = BLOCK_SIZE;
        } else {
            iterNum = iNumSamples / BLOCK_SIZE+1;
            diff    = iNumSamples % BLOCK_SIZE;
        }


        for (i = 0; i < iterNum; ++i) {
            if(i == iterNum-1 && diff != 0) {
                BLOCK_SIZE_t = diff;
            }

            for (j = 0; j < BLOCK_SIZE_t; ++j) {
                for (k = 0; k < inWAVhdr.fmt.NumChannels; ++k) {
                    sample = 0;
                    fread(&sample, BytesPerSample, 1, wav_in);
                    sample = sample << (32 - inWAVhdr.fmt.BitsPerSample);

                    sampleBuffer[k][j] = (double)sample / SAMPLE_SCALE;
                }
            }

            if(stop == 1) {
                stop = 0;
                return;
            }
            audio_processing();


            for (j = 0; j < BLOCK_SIZE_t; ++j) {
               for (k = 0; k < outputWAVhdr.fmt.NumChannels; k++) {
                   sample = sampleBufferOut[k][j] * SAMPLE_SCALE;
                   sample = sample >> (32 - inWAVhdr.fmt.BitsPerSample);
                   fwrite(&sample, inWAVhdr.fmt.BitsPerSample/8, 1, wav_out);
               }
            }
        }
    }

    fclose(wav_in);
    fclose(wav_out);
    free(circularBufferL);
    free(circularBufferR);


    for (i = 0; i < MAX_NUM_CHANNEL; ++i) {
        free(sampleBuffer[i]);
        free(sampleBufferOut[i]);
    }


    free(sampleBuffer);
    free(sampleBufferOut);

    return 0;
}
