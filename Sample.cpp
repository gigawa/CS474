#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);

//3 output image files entered in command line
//ex: ./Sample aerial.PGM aerial1.PGM aerial2.PGM aerial3.PGM

int main(int argc, char *argv[])
{
  int i, j;
  int M, N, Q;
  bool type;
  int val;
  int sample = 2;

  // read image header
  readImageHeader(argv[1], N, M, Q, type);

  // allocate memory for the image array

  ImageType image(N, M, Q);

  // read image
  readImage(argv[1], image);

  //Sample image for 128, 64, 32
  for(int m = 0; m < 3; m++) {
    // Sample each image
    for(i=0; i<N; i += sample) {
      for(j=0; j<M; j += sample) {
        image.getPixelVal(i, j, val);
        for(int k = 0; k < sample; k++) {
          for(int l = 0; l < sample; l++) {
            if(i+k < 256 && j+l < 256) {
              image.setPixelVal(i+k, j+l, val);
            }
          }
        }
      }
    }

    // write each sampled image
    writeImage(argv[2+m], image);
    cout << "Output: " << argv[2+m] << endl;
    sample++;
  }

  return (1);
}
