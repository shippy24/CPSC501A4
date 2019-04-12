
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

