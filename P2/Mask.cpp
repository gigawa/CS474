#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"

int ** ImagePadding(ImageType & image, int N, int M, int P);
ImageType & ImagePadding(ImageType & image, int N, int M, int Q, int P);

using namespace std;

void ApplyMask(ImageType & image, int ** mask, int padding, int tempN, int tempM, int Q) {
  int N = tempN + (padding * 2);
  int M = tempM + (padding * 2);
  int maskSize = (padding * 2) + 1;
  int val;

  //max and min values of the pixel values after applying mask. Used to convert to correct range 0 - 255
  int max = 0;
  int min = 1000;

  ImageType paddedArray(ImagePadding(image, tempN, tempM, Q, padding));

  //iterate through image to apply mask
  for(int i = 0; i < tempN; i++) {
    for(int j = 0; j < tempM; j++) {
      paddedArray.getPixelVal(i, j, val);

      int v = 0;

      //iterate through area on image that mask covers
      for(int k = 0; k < maskSize; k++) {
        for(int l = 0; l < maskSize; l++) {
          paddedArray.getPixelVal(i + k, j + l, val);
          v += (mask[k][l] * val);
        }
      }

      //set min and max values
      if(v < min) {
        min = v;
      }else if(v > max) {
        max = v;
      }

      image.setPixelVal(i, j, v);
    }
  }

  cout << "Max: " << max << endl << "Min: " << min << endl;

  if(min < 0) {
    max -= min;
  }

  //divide each value by d to return values to correct range
  float d = ((float)255/max);

  cout << "D: " << d << endl;

  //iterate through image returning values to correct range
  for(int i = 0; i < tempN; i++) {
    for(int j = 0; j < tempM; j++) {
      image.getPixelVal(i, j, val);
      if(min < 0) {
        val -= min;
      }
      val *= d;
      image.setPixelVal(i, j, val);
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
