#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
void ApplyMask(ImageType & image, int ** mask, int padding, int tempN, int tempM, int Q, bool normalize);
void Laplacian(ImageType & image, int N, int M, int Q, char output[]);
void Prewitt(ImageType & image, int N, int M, int Q, char output[]);
void Sobel(ImageType & image, int N, int M, int Q, char output[]);
void NormalizeImage(ImageType & image, int N, int M, int min, int max);
void NormalizeImage(ImageType & image, int N, int M);

int main(int argc, char *argv[])
{
  int i, j;
  int M, N, Q;
  bool type;
  int val;

  // read image header
  readImageHeader(argv[1], N, M, Q, type);

  ImageType image(N, M, Q);

  // read image
  readImage(argv[1], image);

  Laplacian(image, N, M, Q, argv[2]);
  Prewitt(image, N, M, Q, argv[3]);
  Sobel(image, N, M, Q, argv[4]);

  return (1);
}

void Laplacian(ImageType & image, int N, int M, int Q, char output[]) {
  ImageType maskedImage(image);

  int currMask [3][3] = {0,1,0,1,-4,1,0,1,0};
  //int currMask [3][3] = {0,-1,0,-1,5,-1,0,-1,0};
  int currSize = 3;
  int padding = (currSize - 1)/2;

  //pointer initialization
  int ** mask = new int*[currSize];
  for(int i=0;i<currSize;i++)
  {
    mask[i] = new int[currSize];
  }

  //input array elements
  for(int i = 0; i < currSize; i++)
  {
    for(int j = 0; j < currSize; j++)
    {
      mask[i][j] = currMask[i][j];
    }
  }

  ApplyMask(maskedImage, mask, padding, N, M, Q, false);

  for(int i = 0; i < N; i++) {
    for(int j = 0; j < M; j++) {
      int originalVal;
      int sharpenVal;
      int newVal;
      image.getPixelVal(i, j, originalVal);
      maskedImage.getPixelVal(i, j, sharpenVal);
      newVal = (5 * originalVal) - sharpenVal;
      image.setPixelVal(i, j, newVal);
    }
  }

  NormalizeImage(image, N, M);
  NormalizeImage(maskedImage, N, M);

  // write each sampled image
  writeImage("LapMask.PGM", maskedImage);
  writeImage(output, image);
  cout << "Output: " << output << endl;
}

void Prewitt(ImageType & image, int N, int M, int Q, char output[]) {
  ImageType deriv1(image);
  ImageType deriv2(image);

  int mask1 [3][3] = {-1,-1,-1,0,0,0,1,1,1};
  int mask2 [3][3] = {-1,0,1,-1,0,1,-1,0,1};
  int currSize = 3;
  int padding = (currSize - 1)/2;

  //pointer initialization
  int ** mask = new int*[currSize];
  for(int i=0;i<currSize;i++)
  {
    mask[i] = new int[currSize];
  }

  //input array elements
  for(int i = 0; i < currSize; i++)
  {
    for(int j = 0; j < currSize; j++)
    {
      mask[i][j] = mask1[i][j];
    }
  }

  ApplyMask(deriv1, mask, padding, N, M, Q, false);

  //input array elements
  for(int i = 0; i < currSize; i++)
  {
    for(int j = 0; j < currSize; j++)
    {
      mask[i][j] = mask2[i][j];
    }
  }

  ApplyMask(deriv2, mask, padding, N, M, Q, false);

  ImageType gradientMag(N, M, Q);

  for(int i = 0; i < N; i++) {
    for(int j = 0; j < M; j++) {
      int val1;
      int val2;
      int newVal;
      deriv1.getPixelVal(i, j, val1);
      deriv2.getPixelVal(i, j, val2);
      newVal = sqrt((val1 * val1) + (val2 * val2));
      gradientMag.setPixelVal(i, j, newVal);
    }
  }

  for(int i = 0; i < N; i++) {
    for(int j = 0; j < M; j++) {
      int val1;
      int val2;
      int newVal;
      image.getPixelVal(i, j, val1);
      gradientMag.getPixelVal(i, j, val2);
      newVal = (10 * val1) - val2;
      image.setPixelVal(i, j, newVal);
    }
  }

  NormalizeImage(image, N, M);
  NormalizeImage(gradientMag, N, M);
  NormalizeImage(deriv1, N, M);
  NormalizeImage(deriv2, N, M);

  // write each sampled image
  writeImage("prewittDeriv1.PGM", deriv1);
  writeImage("prewittDeriv2.PGM", deriv2);
  writeImage("prewittGrad.PGM", gradientMag);
  writeImage(output, image);
  cout << "Output: " << output << endl;
}

void Sobel(ImageType & image, int N, int M, int Q, char output[]) {
  ImageType deriv1(image);
  ImageType deriv2(image);

  int mask1 [3][3] = {-1,-2,-1,0,0,0,1,2,1};
  int mask2 [3][3] = {-1,0,1,-2,0,2,-1,0,1};
  int currSize = 3;
  int padding = (currSize - 1)/2;

  //pointer initialization
  int ** mask = new int*[currSize];
  for(int i=0;i<currSize;i++)
  {
    mask[i] = new int[currSize];
  }

  //input array elements
  for(int i = 0; i < currSize; i++)
  {
    for(int j = 0; j < currSize; j++)
    {
      mask[i][j] = mask1[i][j];
    }
  }

  ApplyMask(deriv1, mask, padding, N, M, Q, false);

  //input array elements
  for(int i = 0; i < currSize; i++)
  {
    for(int j = 0; j < currSize; j++)
    {
      mask[i][j] = mask2[i][j];
    }
  }

  ApplyMask(deriv2, mask, padding, N, M, Q, true);

  ImageType gradientMag(N, M, Q);

  for(int i = 0; i < N; i++) {
    for(int j = 0; j < M; j++) {
      int val1;
      int val2;
      int newVal;
      deriv1.getPixelVal(i, j, val1);
      deriv2.getPixelVal(i, j, val2);
      newVal = sqrt((val1 * val1) + (val2 * val2));
      gradientMag.setPixelVal(i, j, newVal);
    }
  }

  for(int i = 0; i < N; i++) {
    for(int j = 0; j < M; j++) {
      int val1;
      int val2;
      int newVal;
      image.getPixelVal(i, j, val1);
      gradientMag.getPixelVal(i, j, val2);
      newVal = (10 * val1) - val2;
      image.setPixelVal(i, j, newVal);
    }
  }

  NormalizeImage(image, N, M);
  NormalizeImage(gradientMag, N, M);
  NormalizeImage(deriv1, N, M);
  NormalizeImage(deriv2, N, M);

  // write each sampled image
  writeImage("sobelDeriv1.PGM", deriv1);
  writeImage("sobelDeriv2.PGM", deriv2);
  writeImage("sobelGrad.PGM", gradientMag);
  writeImage(output, image);
  cout << "Output: " << output << endl;
}
