#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
void Apply2DFFT(int ** realPart, int ** imaginaryPart, int N, int M, int isign);
void WriteImage(int ** realPart, int N, int Q, char * file);
void NormalizeImage(ImageType & image, int N, int min, int max);
float box_muller(float m, float s);
float generateGaussianNoise(float mu, float sigma);

#define PI 3.14

int main(int argc, char * argv[]) {
  int M, N, Q; // N is rows, M is columns, Q is gray levels
  bool type;
  int val;

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
      imaginaryPart[i][j] *= pow(-1, i+j);
    }
  }

  float ** blurReal = new float*[N];
  float ** blurImaginary = new float*[N];
  int ** blurMag = new int*[N];

      //Now call Apply2DFFT, do the calculations there, then output them in main afterwards
  Apply2DFFT(realPart, imaginaryPart, N, M, -1);

  //Create and Apply blur function
  for(int i = 0; i < N; i++){

    blurReal[i] = new float[N];
    blurImaginary[i] = new float[N];
    blurMag[i] = new int[N];

    for(int j = 0; j < N; j++){

      float u = i - N/2;
      float v = j - N/2;
      float a = 0.1;
      float inside = PI * ((a * u) + (a * v));

      float fraction = 1 / inside;
      float sinFunction = sin(inside);
      float first = fraction * sinFunction;

      complex<float> blur = cexp(-I * inside);
      complex<float> F (realPart[i][j], imaginaryPart[i][j]);
      complex<float> G = F * blur;

      realPart[i][j] = int(real(G));
      imaginaryPart[i][j] = int(imag(G));
      blurReal[i][j] = real(blur);
      blurImaginary[i][j] = imag(blur);
    }
  }

  Apply2DFFT(realPart, imaginaryPart, N, M, 1);

  //Shift
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      realPart[i][j] *= pow(-1, i+j);
      imaginaryPart[i][j] *= pow(-1, i+j);
    }
  }

  WriteImage(realPart, N, Q, "Blurred.PGM");

  //Apply noise to blurred Image
  int ** noiseReal = new int*[N];
  int ** noiseImaginary = new int*[N];

  float m = 0;
  float s = 1;

  for(int i = 0; i < N; i++) {
    noiseReal[i] = new int[N];
    noiseImaginary[i] = new int[N];

    for(int j = 0; j < N; j++) {
      noiseReal[i][j] = generateGaussianNoise(m, s);
      noiseImaginary[i][j] = 0;
      //realPart[i][j] += noiseReal[i][j];
    }
  }

  WriteImage(realPart, N, Q, "AddedNoise.PGM");

  Apply2DFFT(realPart, imaginaryPart, N, M, -1);
  Apply2DFFT(noiseReal, noiseImaginary, N, M, -1);

  //Inverse filter
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      int cutoff = 300;
      float magnitude = sqrt(pow(i, 2) + pow(j, 2));
      if(magnitude < cutoff) {
        complex<float> blur (blurReal[i][j], blurImaginary[i][j]);
        complex<float> F (realPart[i][j], imaginaryPart[i][j]);
        complex<float> G = F / blur;

        realPart[i][j] = int(real(G));
        imaginaryPart[i][j] = int(imag(G));
      }
    }
  }

  Apply2DFFT(realPart, imaginaryPart, N, M, 1);

  //Shift
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      realPart[i][j] *= pow(-1, i+j);
      imaginaryPart[i][j] *= pow(-1, i+j);
    }
  }

  WriteImage(realPart, N, Q, "Inverse.PGM");

  return 0;
}

/*void MultiplyComplex(float real1, float imaginary1, float real2, float imaginary2, float & real, float & imaginary) {
  float tempReal = (real1 * real2) - (imaginary1 * imaginary2);
  float tempImaginary = (imaginary1 * real2) + (real1 * imaginary2);
  real = tempReal;
  imaginary = tempImaginary;
}

void DivideComplex(float real1, float imaginary1, float real2, float imaginary2, float & real, float & imaginary) {
  float realConj = real2;
  float imaginaryConj = -1 * imaginary2;
  MultiplyComplex(real1, imaginary1, realConj, imaginaryConj, real1, imaginary1);
  MultiplyComplex(real2, imaginary2, realConj, imaginaryConj, real2, imaginary2);

  if(imaginary2 != 0) {
    cout << "Imaginary: " << imaginary2 << endl;
  }

  real = real1 / real2;
  imaginary = imaginary1 / real2;
}*/

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

  //cout << min << ", " << max << endl;

  if(min < 0 || max > Q) {
    cout << "Normalized" << endl;
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
