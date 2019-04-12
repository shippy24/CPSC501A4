#ifndef _WAV_H
#define _WAV_H

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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

class Wav {
    public:
    //Audio information
        short audioFormat;
        int sampleRate;
        int byteRate;
        short numChannels;
        short bitsPerSample;
        int signalSize;

    //Byte data chunks
        char chunkID[4];
        int chunkSize;
        char format[4];
        char subChunk1ID[4];
        int subChunk1Size;
        char subChunk2ID[4];
        int subChunk2Size;
        short blockAlign;
        
        char* dataArray;
        short* signal;  

    //Function declaration
    void readWavFile(char* file);
    void displayWavFileDetails();
    void convertSignal();          
};

//Displays details of file
void Wav::displayWavFileDetails() {
    cout << "AudioFormat: " << audioFormat << endl;
    cout << "SampleRate: " << sampleRate << endl;
    cout << "ByteRate:  " << byteRate << endl;
    cout << "BitsPerSample:  " << bitsPerSample << endl;
    cout << "NumofChannels:  " << numChannels << endl;

//Chunk details
    cout << "ChunkID:  " << chunkID << endl;
    cout << "ChunkSize:  " << chunkSize << endl;
    cout << "Format:  " << format << endl;
    cout << "SubChunk1ID:  " << subChunk1ID << endl;
    cout << "SubChunk1Size:  " << subChunk1Size << endl;
    cout << "SubChunk2ID:  " << subChunk1ID << endl;
    cout << "SubChunk2Size:  " << subChunk1Size << endl;
      
}

//converts signal based on sample
void Wav::convertSignal() {
    signal = NULL;

    if(bitsPerSample == 8) {
        signalSize = subChunk2Size;
        signal = new short[signalSize];
        for(int i = 0; i < subChunk2Size; i++) {
            signal[i] = (short) ((unsigned char) dataArray[i]);
        }
    }
    else {
        //perform byte shift
        signalSize = subChunk2Size/2;
        signal = new short[signalSize];
        short headerData;
        for(int i = 0; i < subChunk2Size; i+= 2){
            headerData = (short) ((unsigned char) dataArray[i]);
            headerData = (short) ((unsigned char) dataArray[i+1]) *256; 
            signal[i/2] = headerData;
        }
    }
}

void Wav::readWavFile(char* file) {

    cout << "Reading file :" << file << endl;

    //Open file with binary options enabled
    ifstream wavFile(file, ios::binary | ios::in);

    //Read subchunk data
    wavFile.seekg(0, ios::beg);
    wavFile.read(chunkID, 4);
    wavFile.read((char*) &chunkSize, 4);
    wavFile.read(format, 4);
    wavFile.read(subChunk1ID, 4);
    wavFile.read((char*) &subChunk1Size, 4);
    wavFile.read((char*) &audioFormat, sizeof(short));
    wavFile.read((char*) &numChannels, sizeof(short));
    wavFile.read((char*) &sampleRate, sizeof(int));
    wavFile.read((char*) &byteRate, sizeof(int));
    wavFile.read((char*) &blockAlign, sizeof(short));
    wavFile.read((char*) &bitsPerSample, sizeof(short));

    //Read remainder
    if(subChunk1Size == 18){
        short padBytes;
        wavFile.read((char*) &padBytes, sizeof(short));
    }

    //subchunk2 data
    wavFile.read(subChunk2ID, 4);
    wavFile.read((char*) &subChunk2Size, sizeof(int));

    displayWavFileDetails();

    dataArray = new char[subChunk2Size];
    wavFile.read(dataArray, subChunk2Size); 
    wavFile.close();

    convertSignal();

    cout << "File Read complete" << endl;

}


#endif
