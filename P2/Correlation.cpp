#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
ImageType & ImagePadding(ImageType & image, int N, int M, int Q, int P);

//3 output image files entered in command line
//ex: ./Sample aerial.PGM aerial1.PGM aerial2.PGM aerial3.PGM

int main(int argc, char *argv[])
{
  int i, j;
  int M, N, Q;
  bool type;
  int val;

  // read image header
  readImageHeader(argv[1], N, M, Q, type);

  // allocate memory for the image array

  ImageType image(N, M, Q);

  // read image
  readImage(argv[1], image);

  ImageType paddedImage(ImagePadding(image, N, M, Q, 8));

  // write each sampled image
  writeImage(argv[2], paddedImage);
  cout << "Output: " << argv[2] << endl;

  return (1);
}

ImageType & ImagePadding(ImageType & image, int N, int M, int Q, int P) {
  int TotalPadding = P * 2;
  ImageType * paddedImage = new ImageType(N + TotalPadding, M + TotalPadding, Q);
  int val;

  for(int i = 0; i < N + TotalPadding; i++) {
    for(int j = 0; j < N + TotalPadding; j++) {
      paddedImage->setPixelVal(i, j, 0);
    }
  }

  for(int i=0; i<N; i++) {
    for(int j=0; j<M; j++) {
      image.getPixelVal(i, j, val);
      paddedImage->setPixelVal(i+P, j+P, val);
    }
  }

  return (*paddedImage);
}
