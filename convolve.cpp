
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

#define PI         3.141592653589793
#define TWO_PI     (2.0 * PI)
#define SIZE       8
#define SWAP(a,b)  tempr=(a);(a)=(b);(b)=tempr 

using namespace std;

//Function declarations
void convolve(double x[], double h[], double y[], int P);
void outputWAVFile(char* file, double* outputSignal, int numSamples);
void writeWaveFileHeader(int channels, int numberSamples, int bitsPerSample, double outputRate, FILE *outputFile);
size_t fwriteIntLSB(int data, FILE *stream);
size_t fwriteShortLSB(short int data, FILE *stream);
void signalToDouble(Wav* wav,double signalDouble[]);
void scaleSignal(double* outputSignal, Wav* inputFile, int outputSize);
void performFFT(char* inputfile, char* irFile, char* outFile);
void four1(double data[], int nn, int isign);

//Initializations of input files 
Wav *inputFile = new Wav();
Wav *IRFile = new Wav();

int main (int argc, char* argv[]) {

//Check command args
    if(argc <= 3){
        printf("Usage: ./convolve <inputfile.wav> <IRfile.wav> <outputfile.wav>\n");
        return 0;
    }

    double complexData[SIZE * 2];

    //reading input wav and IR files
    inputFile->readWavFile(argv[1]);
    IRFile->readWavFile(argv[2]);

    cout << "Input Wav File signal Size: " << inputFile->signalSize << endl;
    cout << "IR file signal Size: " << IRFile->signalSize << endl;

    performFFT(argv[1], argv[2], argv[3]);
    //create the result of convolution and output as wavFile
    //outputWAVFile(argv[3]);
}

void performFFT(char* inputfile, char* irFile, char* outFile) {
    double* x = new double[inputFile->signalSize];
    double* h = new double[IRFile->signalSize];

    signalToDouble(inputFile, x);
    signalToDouble(IRFile, h);

    cout << "signals converted" << endl;

    int sizeFreqX = inputFile->signalSize;
    int sizeFreqH = IRFile->signalSize;
            
    int maxLength = 0;
    if(sizeFreqX <= sizeFreqH)
        maxLength = sizeFreqH;
    else
        maxLength = sizeFreqX;

    printf("Max length of signals: %d\n", maxLength);

    //Find largest power of two that's less than or equal to maxLength
    int pow2 = 1;
    while (pow2 < maxLength) {
        pow2 *= 2;
    }

    int maxLengthtoPow2 = pow2*2;
    double* freqX = new double[maxLengthtoPow2];
    double* freqH = new double[maxLengthtoPow2];

    for(int i  = 0; i < maxLengthtoPow2; i++){
        freqX[i] = 0.0;
        freqH[i] = 0.0;
    }

    for(int i = 0; i < sizeFreqX; i++){
        freqX[i*2] = x[i];
    }

    for(int i = 0; i < sizeFreqH; i++){
        freqH[i*2] = h[i];
    }

    //Complete convolution using four1 algo
    //input .wav
    cout << "Frequency Domain Transformation for input signal: " << inputfile << endl;
    four1(freqX - 1, pow2, 1);

    //ir .wav
    cout << "Frequency Domain Transformation for input signal: " << irFile << endl;
    four1(freqH - 1, pow2, 1);

    cout << "\n FFT complete" << endl;
    double* freqY = new double[maxLengthtoPow2];

    convolve(freqX, freqH, freqY, maxLengthtoPow2);

    //Perform inverse FFT to use for comparisons
    cout << "\n IFFT OUTPUT: " << endl;
    four1(freqY - 1, pow2, -1);
    cout << "\n IFFT OUTPUT complete: " << endl;

//seperate real from complex values
    int outputSize = (inputFile->signalSize) + (IRFile->signalSize) - 1;   
    double outputMaxValue = 0.0;

    double* freqYSignal = new double[outputSize];
    for(int i = 0; i < outputSize; i++) {
        double signalSample = freqY[i*2];
        freqYSignal[i] = signalSample;

        if(outputMaxValue < abs(signalSample)){
            outputMaxValue = abs(signalSample);
        }
    }

    //Prevent signal cutting and create output file
    scaleSignal(freqYSignal, inputFile, outputSize);

    outputWAVFile(outFile, freqYSignal, outputSize);
}

/**
 four1 FFT from textbook 
 -1 represents inverse FFT
**/
void four1(double data[], int nn, int isign){

    unsigned long n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;

    n = nn << 1;
    j = 1;

    for (i = 1; i < n; i += 2) {
	    if (j > i) {
	        SWAP(data[j], data[i]);
	        SWAP(data[j+1], data[i+1]);
	    }
	    m = nn;
	    while (m >= 2 && j > m) {
	        j -= m;
	        m >>= 1;
	    }
	    j += m;
    }

    mmax = 2;

    while (n > mmax) {

        //Initializations
	    istep = mmax << 1;
	    theta = isign * (6.28318530717959 / mmax);
	    wtemp = sin(0.5 * theta);
	    wpr = -2.0 * wtemp * wtemp;
	    wpi = sin(theta);
	    wr = 1.0;
	    wi = 0.0;

	    for (m = 1; m < mmax; m += 2) {
	        for (i = m; i <= n; i += istep) {
		        j = i + mmax;
		        tempr = wr * data[j] - wi * data[j+1];
		        tempi = wr * data[j+1] + wi * data[j];
		        data[j] = data[i] - tempr;
		        data[j+1] = data[i+1] - tempi;
		        data[i] += tempr;
		        data[i+1] += tempi;
	        }
	        wr = (wtemp = wr) * wpr - wi * wpi + wr;
	        wi = wi * wpr + wtemp * wpi + wi;
	    }
	    mmax = istep;
    }
}
/*****************************************************************************
**** Passes data needed to convolve data to the respective convolve function

Parameters : just the output file name 
******************************************************************************/

void outputWAVFile(char* file, double* outputSignal, int dataSize) {

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

    //Make size comparisons
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

void convolve(double x[], double h[], double y[], int P)
{
    
  //int n, m;
//--------------------------------------------------------------
// PREVIOUS CONVOLUTION ALGO
//--------------------------------------------------------------
  /*  Make sure the output buffer is the right size: P = N + M - 1  
  if (P != (N + M - 1)) {
    printf("Output signal vector is the wrong size\n");
    printf("It is %-d, but should be %-d\n", P, (N + M - 1));
    printf("Aborting convolution\n");
    return;
  }
    **/
  /*  Clear the output buffer y[] to all zero values  */ 
//------------------------------------------------------------

//iNDEXES SWITCH BETWEEN REAL AND COMPLEX VALUES

   for(int i = 0; i < P; i+= 2) {

       //real values
        y[i] = x[i] * h[i] - x[i+1] * h[i+1]; 

        //imaginary values
        y[i+1] = x[i+1] * h[i] + x[i] * h[i+1]; 
    
        if((i%100000) == 0)
            printf("Convolving %d...\n", i);
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
