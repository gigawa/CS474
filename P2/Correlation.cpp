#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
ImageType & ImagePadding(ImageType & image, int N, int M, int Q, int P);
void ApplyMask(ImageType & image, int N, int M, int Q);

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

  ImageType paddedImage(ImagePadding(image, N, M, Q, 1));

  ApplyMask(image, N, M, Q);

  // write each sampled image
  writeImage(argv[2], image);
  cout << "Output: " << argv[2] << endl;

  return (1);
}
