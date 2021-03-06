
/**
  * NAME: Shayne Mujuru
  * UCID: 30029552
  * PROJ: AIV Code Optimization
  * Convolution of audio files using Smith Textbook Input Side Algorithm
  * Base code taken from Dr Leonard Menzara

   Based on the research and teachings of Dr Steven W. Smith
   http://www.dspguide.com/ch8.htm
   https://dsp.stackexchange.com/questions/13230/discrete-fourier-transform
   https://dsp.stackexchange.com/questions/55019/sampling-frequency-fast-fourier-transform-and-some-strange-results-in-python
  */

#include "WAV.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

//Function declarations
void convolve(double x[], int N, double h[], int M, double y[], int P);
void outputWAVFile(char* fileName);
void writeWaveFileHeader(int channels, int numberSamples, int bitsPerSample, double outputRate, FILE *outputFile);
size_t fwriteIntLSB(int data, FILE *stream);
size_t fwriteShortLSB(short int data, FILE *stream);
void signalToDouble(Wav* wav,double signalDouble[]);
void scaleSignal(double* outputSignal, Wav* inputFile, int outputSize);

//Initializations of input files 
Wav *inputFile = new Wav();
Wav *IRFile = new Wav();

int main (int argc, char* argv[]) {

//Check command args
    if(argc <= 3){
        printf("Usage: ./convolve <inputfile.wav> <IRfile.wav> <outputfile.wav>\n");
        return 0;
    }
    
    //reading input wav and IR files
    inputFile->readWavFile(argv[1]);
    IRFile->readWavFile(argv[2]);

    cout << "Input Wav File signal Size" << inputFile->signalSize << endl;
    cout << "IR file signal Size" << IRFile->signalSize << endl;

    //create the result of convolution and output as wavFile
    outputWAVFile(argv[3]);
}

/*****************************************************************************
**** Passes data needed to convolve data to the respective convolve function

Parameters : just the output file name 
******************************************************************************/

void outputWAVFile(char* file) {

    int dataSize = (IRFile->signalSize) - 1 + (inputFile->signalSize) ; //P = (M - 1) + N
    short* outputSignal = new short[dataSize];
    double* wholeWave = new double[dataSize];
   
    //represent signals as doubles for ease of computation
    double* inputWave = new double[inputFile->signalSize];
    signalToDouble(inputFile, inputWave);

    double* irWave = new double[IRFile->signalSize];
    signalToDouble(IRFile, irWave);

    //convolve signal
    convolve(inputWave, inputFile->signalSize, irWave, IRFile->signalSize, wholeWave, dataSize);

    scaleSignal(inputWave, inputFile, dataSize);

    for (int i = 0; i < dataSize; i++) 
        outputSignal[i] = (short) inputWave[i]; 

    //Write for file stream
    FILE* outputFileStream = fopen(file, "wb");
    if (outputFileStream == NULL) {
        fprintf(stderr, "File %s cannot be opened for writing\n", file);
        return;
    }

    writeWaveFileHeader(inputFile->numChannels, dataSize, inputFile->bitsPerSample, inputFile->sampleRate, outputFileStream);

    cout << "Writing convolved signal to file: " << file << endl;

    for(int i = 0; i < dataSize; i++){
        fwriteShortLSB(outputSignal[i], outputFileStream);
    }

    cout << "Writing successful" << endl;

    fclose(outputFileStream);
}

/****************************************************************************
Removes clipping in audio
*****************************************************************************/

void scaleSignal(double* outputSignal, Wav* inputFile, int outputSize){
    double inputMaxValue = 0.0;
    double outputMaxValue = 0.0;

    //check for max value in both original and output signals
    for(int i = 0; i < outputSize; i++){
        if(inputFile->signal[i] > inputMaxValue)
            inputMaxValue = inputFile->signal[i];

        if(outputSignal[i] > outputMaxValue)
            outputMaxValue = outputSignal[i];
    }
    
    for(int i  = 0; i < outputSize; i++){
        outputSignal[i] = outputSignal[i] / outputMaxValue * inputMaxValue;
    }
}

/****************************************************************************
Converts .wav into an array
*****************************************************************************/

void signalToDouble(Wav* wav, double signalDouble[]) {
    for(int i  = 0; i < (wav->signalSize); i++){
        signalDouble[i] = ((double) wav->signal[i])/32678.0;
    }
}

/*****************************************************************************
*
*    Function:     convolve
*
*    Description:  Convolves two signals, producing an output signal.
*                  The convolution is done in the time domain using the
*                  "Input Side Algorithm" (see Smith, p. 112-115).
*
*    Parameters:   x[] is the signal to be convolved
*                  N is the number of samples in the vector x[]
*                  h[] is the impulse response, which is convolved with x[]
*                  M is the number of samples in the vector h[]
*                  y[] is the output signal, the result of the convolution
*                  P is the number of samples in the vector y[].  P must
*                       equal N + M - 1
*
*****************************************************************************/

void convolve(double x[], int N, double h[], int M, double y[], int P)
{
  int n, m;

  /*  Make sure the output buffer is the right size: P = N + M - 1  */
  if (P != (N + M - 1)) {
    printf("Output signal vector is the wrong size\n");
    printf("It is %-d, but should be %-d\n", P, (N + M - 1));
    printf("Aborting convolution\n");
    return;
  }

  /*  Clear the output buffer y[] to all zero values  */  
  for (n = 0; n < P; n++)
    y[n] = 0.0;

  /*  Do the convolution  */
  /*  Outer loop:  process each input value x[n] in turn  */
  for (n = 0; n < N; n++) {
    /*  Inner loop:  process x[n] with each sample of h[]  */
    for (m = 0; m < M; m++)
      y[n+m] += x[n] * h[m];
  }
}

/******************************************************************************
*
*       function:       fwriteShortLSB
*
*       purpose:        Writes a 2-byte integer to the file stream, starting
*                       with the least significant byte (i.e. writes the int
*                       in little-endian form).  This routine will work on both
*                       big-endian and little-endian architectures.
*
*       internal
*       functions:      none
*
*       library
*       functions:      fwrite
*
******************************************************************************/

size_t fwriteShortLSB(short int data, FILE *stream)
{
    unsigned char array[2];

    array[1] = (unsigned char)((data >> 8) & 0xFF);
    array[0] = (unsigned char)(data & 0xFF);
    return fwrite(array, sizeof(unsigned char), 2, stream);
}

/******************************************************************************
*
*       function:       fwriteIntLSB
*
*       purpose:        Writes a 4-byte integer to the file stream, starting
*                       with the least significant byte (i.e. writes the int
*                       in little-endian form).  This routine will work on both
*                       big-endian and little-endian architectures.
*
*       internal
*       functions:      none
*
*       library
*       functions:      fwrite
*
******************************************************************************/

size_t fwriteIntLSB(int data, FILE *stream)
{
    unsigned char array[4];

    array[3] = (unsigned char)((data >> 24) & 0xFF);
    array[2] = (unsigned char)((data >> 16) & 0xFF);
    array[1] = (unsigned char)((data >> 8) & 0xFF);
    array[0] = (unsigned char)(data & 0xFF);
    return fwrite(array, sizeof(unsigned char), 4, stream);
}

/******************************************************************************
*
*       function:       writeWaveFileHeader
*
*       purpose:        Writes the header in WAVE format to the output file.
*
*       arguments:      channels:  the number of sound output channels
*                       numberSamples:  the number of sound samples
*                       outputRate:  the sample rate
*                       outputFile:  the output file stream to write to
*                       
*       internal
*       functions:      fwriteIntLSB, fwriteShortLSB
*
*       library
*       functions:      ceil, fputs
*
******************************************************************************/

void writeWaveFileHeader(int channels, int numberSamples,
                         int bitsPerSample, double outputRate, FILE *outputFile)
{
    /*  Calculate the total number of bytes for the data chunk  */
    int dataChunkSize = channels * numberSamples * (bitsPerSample/8);
	
    /*  Calculate the total number of bytes for the form size  */
    int formSize = 36 + dataChunkSize;
	
    /*  Calculate the total number of bytes per frame  */
    short int frameSize = channels * (bitsPerSample/8);
	
    /*  Calculate the byte rate  */
    int bytesPerSecond = (int)ceil(outputRate * frameSize);

    /*  Write header to file  */
    /*  Form container identifier  */
    fputs("RIFF", outputFile);
      
    /*  Form size  */
    fwriteIntLSB(formSize, outputFile);
      
    /*  Form container type  */
    fputs("WAVE", outputFile);

    /*  Format chunk identifier (Note: space after 't' needed)  */
    fputs("fmt ", outputFile);
      
    /*  Format chunk size (fixed at 16 bytes)  */
    fwriteIntLSB(16, outputFile);

    /*  Compression code:  1 = PCM  */
    fwriteShortLSB(1, outputFile);

    /*  Number of channels  */
    fwriteShortLSB((short)channels, outputFile);

    /*  Output Sample Rate  */
    fwriteIntLSB((int)outputRate, outputFile);

    /*  Bytes per second  */
    fwriteIntLSB(bytesPerSecond, outputFile);

    /*  Block alignment (frame size)  */
    fwriteShortLSB(frameSize, outputFile);

    /*  Bits per sample  */
    fwriteShortLSB(bitsPerSample, outputFile);

    /*  Sound Data chunk identifier  */
    fputs("data", outputFile);

    /*  Chunk size  */
    fwriteIntLSB(dataChunkSize, outputFile);
}
