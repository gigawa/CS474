#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace std;

void fft(float data[], unsigned long nn, int isign);
void PrintFunction(float function[], int nn, string output);

int main(int argc, char *argv[])
{
  float function[9] = {-1, 2, 0, 3, 0, 4, 0, 4, 0};
  int N = 4;
  int arrayLength = N * 2 + 1;

  //forward fft
  fft(function, N, -1);

  //Multiply by normalization factor
  for(int i = 1; i < arrayLength; i++) {
    function[i] /= N;
  }

  PrintFunction(function, N, "Function1.txt");

  //inverse fft
  cout << "Inverse: ";
  fft(function, 4, 1);

  for(int i = 1; i < arrayLength; i++) {
    cout << function[i] << ", ";
  }

  cout << endl;
}

void PrintFunction(float function[], int nn, string output) {
  ofstream file(output);
  int arrayLength = nn * 2 + 1;

  //print total
  file << "Total: " << endl;
  for(int i = 1; i < arrayLength; i++) {
    file << function[i] << endl;
  }

  file << endl;

  //print real
  file << "Real: " << endl;
  for(int i = 1; i < arrayLength; i += 2) {
    file << function[i] << endl;
  }

  file << endl;

  //print imaginary
  file << "Imaginary: " << endl;
  for(int i = 2; i < arrayLength; i += 2) {
    file << function[i] << endl;
  }

  file << endl;

  //print magnitude
  file << "Magnitude: " << endl;
  for(int i = 1; i < arrayLength; i += 2) {
    float magnitude = sqrt((function[i] * function[i]) + (function[i+1] * function[i+1]));
    file << magnitude << endl;
  }

  file << endl;

  file.close();
}
