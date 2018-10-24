#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

void fft(float data[], unsigned long nn, int isign);

int main(int argc, char *argv[])
{
  float function[9] = {-1, 2, 0, 3, 0, 4, 0, 4, 0};
  int N = 4;
  int arrayLength = N * 2 + 1;

  fft(function, N, -1);

  for(int i = 1; i < arrayLength + 1; i++) {
    function[i] /= N;
    cout << function[i] << ", ";
  }

  cout << endl;

  fft(function, 4, 1);

  for(int i = 1; i < arrayLength; i++) {
    cout << function[i] << ", ";
  }

  cout << endl;
}
