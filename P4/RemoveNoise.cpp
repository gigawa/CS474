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
  int threshold;

  cout << "Enter threshold: ";
  cin >> threshold;

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

      //Allocate memory for double pointers (2D Arrays)
  for(int i = 0 ; i < N ; i++)
{
  realPart[i] = new int[N];
  imaginaryPart[i] = new int[N];
}

      //Stick the real values into realPart and make all values in imaginaryPart 0

  for(int i = 0; i < N; i++){
      for(int j = 0; j < M; j++){
          inputImage.getPixelVal(i, j, val);
          realPart[i][j] = val;
          imaginaryPart[i][j] = 0;
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
  //Check surrounding pixels for outliers representing noise
  //threshold difference of surrounding to check if significant
  for(int i = 1; i < N-2; i++) {
    for(int j = 1; j < N-2; j++) {

      int surrounding[8] = {realPart[i-1][j-1], realPart[i-1][j], realPart[i][j-1], realPart[i-1][j+1], realPart[i+1][j-1], realPart[i+1][j+1], realPart[i][j+1], realPart[i+1][j]};
      int surroundingImaginary[8] = {imaginaryPart[i-1][j-1], imaginaryPart[i-1][j], imaginaryPart[i][j-1], imaginaryPart[i-1][j+1], imaginaryPart[i+1][j-1], imaginaryPart[i+1][j+1], imaginaryPart[i][j+1], imaginaryPart[i+1][j]};
      int difference[8] = {realPart[i][j] - surrounding[0], realPart[i][j] - surrounding[1], realPart[i][j] - surrounding[2], realPart[i][j] - surrounding[3], realPart[i][j] - surrounding[4], realPart[i][j] - surrounding[5], realPart[i][j] - surrounding[6], realPart[i][j] - surrounding[7]};

      if(difference[0] > threshold && difference[1] > threshold && difference[2] > threshold && difference[3] > threshold && difference[4] > threshold && difference[5] > threshold && difference[6] > threshold && difference[7] > threshold) {
        realPart[i][j] = 0;
        imaginaryPart[i][j] = 0;
      }
    }
  }

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
