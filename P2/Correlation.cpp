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

  // read image header
  readImageHeader(argv[1], N, M, Q, type);

  // allocate memory for the image array

  ImageType image(N, M, Q);

  // read image
  readImage(argv[1], image);

    for(i=0; i<N; i++) {
      for(j=0; j<M; j++) {
        image.getPixelVal(i, j, val);
      }
    }

    // write each sampled image
    writeImage(argv[2], image);
    cout << "Output: " << argv[2] << endl;
    sample *= 2;

  return (1);
}
