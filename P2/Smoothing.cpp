#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
ImageType & ImagePadding(ImageType & image, int N, int M, int Q, int P);
void ApplyMask(ImageType & image, int ** mask, int padding, int tempN, int tempM, int Q);
void Gaussian(ImageType & image, bool fifteen, int N, int M, int Q, char output []);
void Averaging(ImageType & oldImage, int maskSize, int N, int M, int Q, char output []);

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

  ImageType image(N, M, Q);

  // read image
  readImage(argv[1], image);

  Gaussian(image, true, N, M, Q, argv[2]);
  Gaussian(image, false, N, M, Q, argv[3]);
  Averaging(image, 7, N, M, Q, argv[4]);
  Averaging(image, 15, N, M, Q, argv[5]);

  return (1);
}

void Averaging(ImageType & oldImage, int maskSize, int N, int M, int Q, char output []) {
  ImageType image(oldImage);
  int ** mask;
  int currSize = maskSize;
  int padding = (currSize - 1)/2;

  mask = new int*[currSize];
  for(int i=0;i<currSize;i++)
  {
    mask[i] = new int[currSize];
  }

  //input array elements
  for(int i = 0; i < currSize; i++)
  {
    for(int j = 0; j < currSize; j++)
    {
      mask[i][j] = 1;
    }
  }

  ApplyMask(image, mask, padding, N, M, Q);

  // write each sampled image
  writeImage(output, image);
  cout << "Output: " << output << endl;

}

void Gaussian(ImageType & oldImage, bool fifteen, int N, int M, int Q, char output []) {
  ImageType image(oldImage);
  int ** mask;
  int currSize;
  if(fifteen) {
    currSize = 15;
    int currMask [15][15] = {2,2,3,4,5,5,6,6,6,5,5,4,3,2,2,
                            2,3,4,5,7,7,8,8,8,7,7,5,4,3,2,
                            3,4,6,7,9,10,10,11,10,10,9,7,6,4,3,
                            4,5,7,9,10,12,13,13,13,12,10,9,7,5,4,
                            5,7,9,11,13,14,15,16,15,14,13,11,9,7,5,
                            5,7,10,12,14,16,17,18,17,16,15,12,10,7,5,
                            6,8,10,13,15,17,19,10,10,17,15,13,10,8,6,
                            6,8,11,13,16,18,19,20,19,18,16,13,11,8,6,
                            6,8,10,13,15,17,19,10,10,17,15,13,10,8,6,
                            5,7,10,12,14,16,17,18,17,16,15,12,10,7,5,
                            5,7,9,11,13,14,15,16,15,14,13,11,9,7,5,
                            4,5,7,9,10,12,13,13,13,12,10,9,7,5,4,
                            3,4,6,7,9,10,10,11,10,10,9,7,6,4,3,
                            2,3,4,5,7,7,8,8,8,7,7,5,4,3,2,
                            2,2,3,4,5,5,6,6,6,5,5,4,3,2,2};

    //pointer initialization
    mask = new int*[currSize];
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

  }else {
    int currMask [7][7] = {1,1,2,2,2,1,1,1,2,2,4,2,2,1,2,2,4,8,4,2,2,2,4,8,16,8,4,2,2,2,4,8,4,2,2,1,2,2,4,2,2,1,1,1,2,2,2,1,1};
    currSize = 7;

    //pointer initialization
    mask = new int*[currSize];
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
  }

  int padding = (currSize - 1)/2;

  ApplyMask(image, mask, padding, N, M, Q);

  // write each sampled image
  writeImage(output, image);
  cout << "Output: " << output << endl;
}
