#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
void Apply2DFFT(int ** realPart, int ** imaginaryPart, int N, int M, int isign);
void WriteImage(int ** realPart, int N, int Q, char * file);
void NormalizeImage(ImageType & image, int N, int min, int max);

int main(int argc, char * argv[]) {
  // argv[1] file name - will hold the input image
  // argv[2] file name - will hold the output image

  int M, N, Q; // N is rows, M is columns, Q is gray levels
  bool type;
  int val;
  int isign = -1;
  int filterSize = 40;
  int butterworth = 4;

  cout << "Enter band radius: ";
  cin >> filterSize;

  readImageHeader(argv[1], N, M, Q, type); //read in the input image, store rows to N, cols to M, gray lvl to Q

  cout << " Header values ... N: " << N << " M: " << M << " Q: " << Q << endl;


      // Now create a 2D matrix, store all the pixel values from the image into this matrix to use in Apply2DFFT
  ImageType inputImage(N, M, Q);
  ImageType outputImage(N, M, Q);

  readImage(argv[1], inputImage);
  cout << " '" << argv[1] << "'" << " read into inputImage ... " << endl;


      //Creating the 2D array that will hold the results in between FFT steps as well as the final results
  int ** realPart = new int*[N];
  int ** imaginaryPart = new int*[N];
  int ** magnitudeValues = new int*[N];

      //Allocate memory for double pointers (2D Arrays)
  for(int i = 0 ; i < N ; i++)
{
  realPart[i] = new int[N];
  imaginaryPart[i] = new int[N];
  magnitudeValues[i] = new int[N];
}

      //Stick the real values into realPart and make all values in imaginaryPart 0

  for(int i = 0; i < N; i++){
      for(int j = 0; j < M; j++){
          inputImage.getPixelVal(i, j, val);
          realPart[i][j] = val;
          imaginaryPart[i][j] = 0;
          magnitudeValues[i][j] = 0;
          //cout << val << " ";
      }
  }

  //Shift
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      realPart[i][j] *= pow(-1, i+j);
    }
  }

      //Now call Apply2DFFT, do the calculations there, then output them in main afterwards
  Apply2DFFT(realPart, imaginaryPart, N, M, isign);

  float ** filterRealPart = new float*[N];
  int ** filterImaginaryPart = new int*[N];
  int ** filterMagnitudeValues = new int*[N];

      //Allocate memory for double pointers (2D Arrays)
  for(int i = 0 ; i < N ; i++)
{
  filterRealPart[i] = new float[N];
  filterImaginaryPart[i] = new int[N];
  filterMagnitudeValues[i] = new int[N];
}

for(int i = 0; i < N; i++){
    for(int j = 0; j < M; j++){
      int mag = sqrt(i*i + j*j);
      filterRealPart[i][j] = 0;
      filterImaginaryPart[i][j] = 0;
    }
}

WriteImage(realPart, N, Q, "AfterFFT.PGM");

  //Create low pass filter
  for(int i = 0; i < N; i++){
      for(int j = 0; j < M; j++){
        //ideal
        /*int mag = sqrt(pow(i - N/2, 2) + pow(j - N/2, 2));
          if(mag < filterSize) {
            cout  << i << ", " << j << endl;
            filterRealPart[i][j] = 1;
          }*/

          //gaussian
          /*filterRealPart[i][j] = exp(-1 * (pow(i-N/2, 2) + pow(j-N/2, 2)) / (2 * pow(2 * filterSize, 2)));
          cout << "Filter: " << filterRealPart[i][j] << endl;
          */

          //Butterworth
          filterRealPart[i][j] = 1 / (1 + pow(sqrt(pow(i - N/2, 2) + pow(j - N/2, 2)) / filterSize, 2 * butterworth));

          filterImaginaryPart[i][j] = filterRealPart[i][j] * 255;
      }
  }

  WriteImage(filterImaginaryPart, N, Q, "Filter.PGM");

  //Apply Filter
  for(int i = 0; i < N; i++){
      for(int j = 0; j < M; j++){
          realPart[i][j] *= filterRealPart[i][j];
          imaginaryPart[i][j] *= filterRealPart[i][j];
      }
  }

  WriteImage(realPart, N, Q, "AfterFilter.PGM");

  Apply2DFFT(realPart, imaginaryPart, N, M, 1);

  //Shift
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      realPart[i][j] *= pow(-1, i+j);
    }
  }

  WriteImage(realPart, N, Q, "Done.PGM");
}

void WriteImage(int ** realPart, int N, int Q, char * file) {
  ImageType outputImage(N, N, Q);
  int max = 0;
  int min = 10000;

  for(int i = 0; i < N; i++){
      for(int j = 0; j < N; j++){

        if(realPart[i][j] < min) {
          min = realPart[i][j];
        }

        if(realPart[i][j] > max) {
          max = realPart[i][j];
        }
        outputImage.setPixelVal(i, j, realPart[i][j]);
      }
  }

  if(min < 0 || max > Q) {
    NormalizeImage(outputImage, N, min, max);
  }

  writeImage(file, outputImage);
  cout << "Output: " << file << endl;
}

void NormalizeImage(ImageType & image, int N, int min, int max) {
  int val;
  if(min < 0) {
    max -= min;
  }

  //divide each value by d to return values to correct range
  float d = ((float)255/max);

  //cout << "D: " << d << endl;

  //iterate through image returning values to correct range
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      image.getPixelVal(i, j, val);
      if(min < 0) {
        val -= min;
      }
      val *= d;
      image.setPixelVal(i, j, val);
    }
  }
}
