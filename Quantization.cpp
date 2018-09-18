#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);

//4 output image files entered in command line
//ex: ./Quantization aerial.PGM aerial1.PGM aerial2.PGM aerial3.PGM aerial4.PGM

int main(int argc, char *argv[])
{
  int i, j;
  int M, N, Q;
  bool type;
  int val;
  int newVal;
  int quantMult [] = {2, 8, 32, 128};

  // read image header
  readImageHeader(argv[1], N, M, Q, type);

  // allocate memory for the image array

  ImageType image(N, M, Q);

  // read image
  readImage(argv[1], image);

  //Quantize image for 128, 32, 8, 2
  for(int m = 0; m < 4; m++) {
    // Quantize each image
    ImageType newImage(N, M, (Q/quantMult[m]));
    for(i=0; i<N; i++) {
      for(j=0; j<M; j++) {
        image.getPixelVal(i, j, val);
        newVal = val/quantMult[m];
        newImage.setPixelVal(i, j, newVal);
      }
    }

    // write each quantized image
    writeImage(argv[2+m], newImage);
    cout << "Output: " << argv[2+m] << endl;
  }

  return (1);
}
