#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"

int ** ImagePadding(ImageType & image, int N, int M, int P);
ImageType & ImagePadding(ImageType & image, int N, int M, int Q, int P);

using namespace std;

void ApplyMask(ImageType & image, int tempN, int tempM, int Q) {
  int padding = 3;
  int N = tempN + (padding * 2);
  int M = tempM + (padding * 2);
  int maskSize = 3;
  double mask [7][7] = {1,1,2,2,2,1,1,1,2,2,4,2,2,1,2,2,4,8,4,2,2,2,4,8,16,8,4,2,2,2,4,8,4,2,2,1,2,2,4,2,2,1,1,1,2,2,2,1,1};
  double maskSum = 0;
  int val;

  ImageType paddedArray(ImagePadding(image, tempN, tempM, Q, padding));

  for(int i = padding; i < tempN; i++) {
    for(int j = padding; j < tempM; j++) {
      paddedArray.getPixelVal(i, j, val);

      int v = 0;
      for(int k = 0; k < maskSize; k++) {
        for(int l = 0; l < maskSize; l++) {
          paddedArray.getPixelVal(i + k, j + l, val);
          v += (mask[k][l] * val)/16;
        }
      }
      cout << val << " " << v << endl;
      image.setPixelVal(i - padding, j - padding, v);
    }
  }
}

ImageType & ImagePadding(ImageType & image, int N, int M, int Q, int P) {
  int TotalPadding = P * 2;
  ImageType * paddedImage = new ImageType(N + TotalPadding, M + TotalPadding, Q);
  int val;

  for(int i=0; i<N; i++) {
    for(int j=0; j<M; j++) {
      image.getPixelVal(i, j, val);
      paddedImage->setPixelVal(i+P, j+P, val);
    }
  }

  return (*paddedImage);
}

int ** ImagePadding(ImageType & image, int N, int M, int P) {
  int TotalPadding = P * 2;
  int val;

  int ** paddedArray = new int* [N + TotalPadding];
  for(int i=0; i < N + TotalPadding; i++) {
    paddedArray[i] = new int[M];
    for(int j=0; j < M + TotalPadding; j++)
      paddedArray[i][j] = 0;
  }

  for(int i=0; i<N; i++) {
    for(int j=0; j<M; j++) {
      image.getPixelVal(i, j, val);
      paddedArray[i+P][j+P] = val;
    }
  }

  return paddedArray;
}
