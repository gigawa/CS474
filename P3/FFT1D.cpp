#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace std;

void fft(float data[], unsigned long nn, int isign);
void PrintFunction(float function[], int nn, string output);
void ApplyFFT(float function[], int N, string Output);
void SampleCos();

int main(int argc, char *argv[])
{
  float function[9] = {-1, 2, 0, 3, 0, 4, 0, 4, 0};

  ApplyFFT(function, 4, "Function1.txt");

  //Function 2 - Cosine function
  float function2[257];

  ifstream fin("cosFunction.txt");
  for(int i = 1; i < 257; i += 2) {
    fin >> function2[i];

    //shift to center
    function2[i] *= pow(-1, i-1);
  }
  fin.close();
  for(int i = 0; i < 257; i += 2) {
    function2[i] = 0;
  }

  ApplyFFT(function2, 128, "Function2.txt");

  //Function 3 - Rectangular function
  float function3[257];

  fin.open("Rect_128.dat");
  for(int i = 1; i < 257; i += 2) {
    fin >> function3[i];

    //shift to center
    function3[i] *= pow(-1, i-1);
  }
  for(int i = 0; i < 257; i += 2) {
    function3[i] = 0;
  }
  fin.close();

  ApplyFFT(function3, 128, "Function3.txt");

}

void ApplyFFT(float function[], int N, string Output) {
  int arrayLength = N * 2 + 1;

  //forward fft
  fft(function, N, -1);

  //Multiply by normalization factor
  for(int i = 1; i < arrayLength; i++) {
    function[i] /= N;
  }

  PrintFunction(function, N, Output);
}

//Samples cosine function and prints as text
void SampleCos() {
  ofstream file("cosFunction.txt");
  float pi = 3.14;
  for(int i = 0; i < 128; i++) {
    float val = cos((2 * pi * 8 * i)/128);
    file << val << endl;
  }
  file.close();
}

//prints function's real, imaginary, and magnitude values
void PrintFunction(float function[], int nn, string output) {
  ofstream file(output);
  int arrayLength = nn * 2 + 1;

  /*//print total
  file << "Total: " << endl;
  for(int i = 1; i < arrayLength; i++) {
    file << function[i] << endl;
  }

  file << endl;*/

  //print real
  file << "Real: " << endl;
  for(int i = 1; i < arrayLength; i += 2) {
    file << function[i] << endl;
  }

  file << endl << endl;

  //print imaginary
  file << "Imaginary: " << endl;
  for(int i = 2; i < arrayLength; i += 2) {
    file << function[i] << endl;
  }

  file << endl << endl;

  //print magnitude
  file << "Magnitude: " << endl;
  for(int i = 1; i < arrayLength; i += 2) {
    float magnitude = sqrt((function[i] * function[i]) + (function[i+1] * function[i+1]));
    file << magnitude << endl;
  }

  file << endl << endl;

  file.close();
}
