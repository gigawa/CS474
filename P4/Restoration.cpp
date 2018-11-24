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
void Apply2DFFTfloat(float ** realPart, float ** imaginaryPart, int N, int M, int isign);
void WriteImage(float ** realPart, int N, int Q, char * file);
void NormalizeImage(ImageType & image, int N, int min, int max);
float box_muller(float m, float s);
float generateGaussianNoise(float mu, float sigma);
void InverseFilter(float ** realPart, float ** imaginaryPart, float ** blurReal, float ** blurImaginary, int ** noiseReal, int ** noiseImaginary, int N, int Q);
void WienerFilter(float ** realPart, float ** imaginaryPart, float ** blurReal, float ** blurImaginary, int N, int Q);

#define PI 3.14159
int cutoff;
float k;

int main(int argc, char * argv[]) {
  int M, N, Q; // N is rows, M is columns, Q is gray levels
  bool type;
  int val;

  cout << "Enter Cutoff: ";
  cin >> cutoff;

  readImageHeader(argv[1], N, M, Q, type); //read in the input image, store rows to N, cols to M, gray lvl to Q

  cout << " Header values ... N: " << N << " M: " << M << " Q: " << Q << endl;


      // Now create a 2D matrix, store all the pixel values from the image into this matrix to use in Apply2DFFTfloat
  ImageType inputImage(N, M, Q);
  ImageType outputImage(N, M, Q);

  readImage(argv[1], inputImage);
  cout << " '" << argv[1] << "'" << " read into inputImage ... " << endl;


      //Creating the 2D array that will hold the results in between FFT steps as well as the final results
  float ** realPart = new float*[N];
  float ** imaginaryPart = new float*[N];

  //Allocate memory for double pointers (2D Arrays)
  //Stick the real values into realPart and make all values in imaginaryPart 0
  for(int i = 0; i < N; i++){
    realPart[i] = new float[N];
    imaginaryPart[i] = new float[N];
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

      //Now call Apply2DFFTfloat, do the calculations there, then output them in main afterwards
  Apply2DFFTfloat(realPart, imaginaryPart, N, M, -1);

  //Create and Apply blur function
  for(int i = 0; i < N; i++){

    blurReal[i] = new float[N];
    blurImaginary[i] = new float[N];
    blurMag[i] = new int[N];

    for(int j = 0; j < N; j++){

      //translate to center
      float u = i - N/2;
      float v = j - N/2;

      float a = 0.1;

      float inside = PI * ((a * u) + (a * v));
      float fraction = 1 / inside;

      float sinFunction = sin(inside);
      complex<float> blur = fraction * sinFunction * cexp(-I * inside);

      //if inside is 0 - fraction is NaN, set blur to 1 so F doesn't change
      if(inside == 0) {
        blur = (1,1);
      }

      complex<float> F (realPart[i][j], imaginaryPart[i][j]);
      complex<float> G = F * blur;

      //cout << "F: " << F << endl;
      //cout << "G: " << G << endl;

      //G /= blur;
      //cout << "FA: " << G << endl;

      realPart[i][j] = real(G);
      imaginaryPart[i][j] = imag(G);

      blurReal[i][j] = real(blur);
      blurImaginary[i][j] = imag(blur);
    }
  }

  Apply2DFFTfloat(realPart, imaginaryPart, N, M, 1);

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
  float s = 1000;

  for(int i = 0; i < N; i++) {
    noiseReal[i] = new int[N];
    noiseImaginary[i] = new int[N];

    for(int j = 0; j < N; j++) {
      noiseReal[i][j] = generateGaussianNoise(m, s);
      noiseImaginary[i][j] = 0;
      realPart[i][j] += noiseReal[i][j];
      if(realPart[i][j] > 255) {
        realPart[i][j] = 255;
      }
    }
  }

  WriteImage(realPart, N, Q, "AddedNoise.PGM");

  //Shift
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      realPart[i][j] *= pow(-1, i+j);
      imaginaryPart[i][j] *= pow(-1, i+j);
      noiseReal[i][j] *= pow(-1, i+j);
      noiseImaginary[i][j] *= pow(-1, i+j);
    }
  }

  Apply2DFFTfloat(realPart, imaginaryPart, N, M, -1);
  Apply2DFFT(noiseReal, noiseImaginary, N, M, -1);

  InverseFilter(realPart, imaginaryPart, blurReal, blurImaginary, noiseReal, noiseImaginary, N, Q);

  cout << "Enter k: ";
  cin >> k;
  while(k != -1) {
    WienerFilter(realPart, imaginaryPart, blurReal, blurImaginary, N, Q);
    cout << "Enter k: ";
    cin >> k;
  }

  return 0;
}

void InverseFilter(float ** realPart, float ** imaginaryPart, float ** blurReal, float ** blurImaginary, int ** noiseReal, int ** noiseImaginary, int N, int Q) {
  float ** resultReal = new float*[N];
  float ** resultImaginary = new float*[N];

  for(int i = 0; i < N; i++) {

    resultReal[i] = new float[N];
    resultImaginary[i] = new float[N];

    for(int j = 0; j < N; j++) {

      float magnitude = sqrt(pow(i, 2) + pow(j, 2));

      if(magnitude < cutoff) {
        complex<float> blur (blurReal[i][j], blurImaginary[i][j]);
        complex<float> G (realPart[i][j], imaginaryPart[i][j]);
        complex<float> N (noiseReal[i][j], noiseImaginary[i][j]);
        complex<float> F = G / blur;

        resultReal[i][j] = real(F);
        resultImaginary[i][j] = imag(F);

        if(isnan(realPart[i][j]) || isnan(imaginaryPart[i][j])) {
          cout << "i, j" << i << ", " << j << endl;
          cout << "G: " << G << endl;
        }
      }
    }
  }

  Apply2DFFTfloat(resultReal, resultImaginary, N, N, 1);

  //Shift
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      resultReal[i][j] *= pow(-1, i+j);
      resultImaginary[i][j] *= pow(-1, i+j);
    }
  }

  WriteImage(resultReal, N, Q, "Inverse.PGM");
}

void WienerFilter(float ** realPart, float ** imaginaryPart, float ** blurReal, float ** blurImaginary, int N, int Q) {
  float ** resultReal = new float*[N];
  float ** resultImaginary = new float*[N];

  for(int i = 0; i < N; i++) {

    resultReal[i] = new float[N];
    resultImaginary[i] = new float[N];

    for(int j = 0; j < N; j++) {
      complex<float> blur (blurReal[i][j], blurImaginary[i][j]);
      float blurMag = sqrt(pow(blurReal[i][j], 2) + pow(blurImaginary[i][j], 2));

      complex<float> G (realPart[i][j], imaginaryPart[i][j]);
      complex<float> F1 = (pow(blurMag, 2) / (pow(blurMag, 2) + k));
      complex<float> F =  F1 * (G / blur);

      resultReal[i][j] = real(F);
      resultImaginary[i][j] = imag(F);

      if(isnan(realPart[i][j]) || isnan(imaginaryPart[i][j])) {
        cout << "i, j" << i << ", " << j << endl;
        cout << "G: " << G << endl;
      }
    }
  }

  Apply2DFFTfloat(resultReal, resultImaginary, N, N, 1);

  //Shift
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      resultReal[i][j] *= pow(-1, i+j);
      resultImaginary[i][j] *= pow(-1, i+j);
    }
  }

  WriteImage(resultReal, N, Q, "Wiener.PGM");
}

void WriteImage(float ** realPart, int N, int Q, char * file) {
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
