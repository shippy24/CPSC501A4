
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

   usage: ./convolveTest <baseFile> <fftFile>
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

int compareSubchunk1(Wav* baseFile, Wav* fftFile);
int compareSubchunk2(Wav* baseFile, Wav* fftFile);
int compareSignalData(Wav* baseFile, Wav* fftFile);

Wav* originalFile = new Wav();
Wav* fftFile = new Wav();
int equivalentFiles;

int main(int argc, char* argv[]){
    char* baseFileName = argv[1];
    char* fftFileName = argv[2];

    //check number of command-line args
    if(argc <= 2){
        printf("Usage: ./regressionTest <outputFileBase.wav> <outputFileFFT.wav>\n");
        return 0;
    }

    originalFile->readWavFile(argv[1]);
    fftFile->readWavFile(argv[2]);

    if(compareSignalData(originalFile, fftFile) != 0){
        printf("output signals not equal\n");
        equivalentFiles = 1;
    }
    else
        printf("output signals equal\n");

    if (compareSubchunk1(originalFile, fftFile) != 0) {
        printf("fmt subchunks not equal\n");
        equivalentFiles = 1;
    }
    else
        printf("fmt subchunks equal\n");

    if (compareSubchunk2(originalFile, fftFile) != 0) {
        printf("data subchunks not equal\n");
        equivalentFiles = 1;
    }
    else
        printf("data subchunks equal\n"); 

    if(equivalentFiles != 0)
        cout << "File: " << baseFileName << " & " << fftFileName << " are not the same" << endl;
    else
        cout << "File: " << baseFileName << " & " << fftFileName << " are the same" << endl;
       
}

int compareSubchunk1(Wav* baseFile, Wav* fftFile){
    int flagSubChunk1 = 0;

    //compare subchunk fields
    if(strcmp(baseFile->subChunk1ID, fftFile->subChunk1ID) != 0)
        flagSubChunk1 = 1;

    if(baseFile->subChunk1Size != fftFile->subChunk1Size)
        flagSubChunk1 = 1;

    if(baseFile->blockAlign != fftFile->blockAlign)
        flagSubChunk1 = 1;

    if(baseFile->numChannels != fftFile->numChannels)
        flagSubChunk1 = 1;

    if(baseFile->audioFormat != fftFile->audioFormat)
        flagSubChunk1 = 1;

    if(baseFile->bitsPerSample != fftFile->bitsPerSample)
        flagSubChunk1 = 1;
    
    if(baseFile->sampleRate != fftFile->sampleRate)
        flagSubChunk1 = 1;

    if(baseFile->byteRate != fftFile->byteRate)
        flagSubChunk1 = 1;

    return flagSubChunk1;
}

int compareSubchunk2(Wav* baseFile, Wav* fftFile){
    int flagSubChunk2 = 0;

    //compare subchunk fields
    if(strcmp(baseFile->subChunk2ID, fftFile->subChunk2ID) != 0)
        flagSubChunk2 = 1;

    if(baseFile->subChunk2Size != fftFile->subChunk2Size)
        flagSubChunk2 = 1;

    return flagSubChunk2;
}

int compareSignalData(Wav* baseFile, Wav* fftFile){
    int flagSignalData = 0;

    int baseSignalSize = baseFile->signalSize;
    int fftSignalSize = fftFile->signalSize;

    if(baseSignalSize != fftSignalSize){
        printf("baseFile signal size (%d) NOT equal to fftFile signal size (%d)\n", baseSignalSize, fftSignalSize);
        flagSignalData = 1;
    }
    else {
        for(int i = 0; i < baseSignalSize; i++){
            if(baseFile->signal[i] != fftFile->signal[i])
                flagSignalData = 1;
        }

        if(flagSignalData == 1)
            printf("base signalData NOT identical to fft signalData!\n");
    }

    return flagSignalData;
}
  