#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

using namespace std;

void fft(float data[], unsigned long nn, int isign);

int main(int argc, char *argv[])
{
  float function[9] = {-1, 2, 0, 3, 0, 4, 0, 4, 0};
  int N = 4;
  int arrayLength = N * 2 + 1;

  //forward fft
  fft(function, N, -1);

  //print total and multiply by normalization factor
  cout << "Total: ";
  for(int i = 1; i < arrayLength; i++) {
    function[i] /= N;
    cout << function[i] << ", ";
  }

  cout << endl;

  //print real
  cout << "Real: ";
  for(int i = 1; i < arrayLength; i += 2) {
    cout << function[i] << ", ";
  }

  cout << endl;

  //print imaginary
  cout << "Imaginary: ";
  for(int i = 2; i < arrayLength; i += 2) {
    cout << function[i] << ", ";
  }

  cout << endl;

  //print magnitude
  cout << "Magnitude: ";
  for(int i = 1; i < arrayLength; i += 2) {
    float magnitude = sqrt((function[i] * function[i]) + (function[i+1] * function[i+1]));
    cout << magnitude << ", ";
  }

  cout << endl;

  //inverse fft
  cout << "Inverse: ";
  fft(function, 4, 1);

  for(int i = 1; i < arrayLength; i++) {
    cout << function[i] << ", ";
  }

  cout << endl;
}
