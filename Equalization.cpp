#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <math.h>
#include <iomanip>
#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);

//Command-line arguments: 1 input file and 1 output file to place the equalized image out
//ex: ./Equalization aerial.PGM aerialOutput.PGM

int main(int argc, char *argv[])
{
  int i, j; //i -> N, j -> M
  int M, N, Q; // N is rows, M is columns, Q is gray levels
  bool type;
  int val;
  
  //image.pixelvalue[i][j] where 0 <= i <= 255 (our PGMs are 256x256), and 0 <= j <= 255 
  
  // read image header
  readImageHeader(argv[1], N, M, Q, type);

    
  int pixelCount[Q+1] = {0}; //Holds # of pixels of that gray level in its own index (each gray level has its own spot)
  float probabilityDensity[Q+1] = {0}; //Holds probability that this particular gray level is present out of all 256x256 pixels
  float cumulativeDistribution[Q+1] = {0}; //Holds cumulative probability distribution for all gray levels up to 256
  float newFloatValues[Q+1] = {0};
  int newFlooredIntValues[Q+1] = {0};
  int equalizer = Q+1; //Integer used to scale cumulativeDistribution[] to re-map new image with (256 in our case)
  float totalPixels = N*M;

  
  cout << endl << "N is: " << N << endl << "M is: " << M << endl;
  cout << "Total pixels: " << totalPixels << endl;
  cout << "Q is: " << Q+1 << endl << endl;

  // allocate memory for the image array
  ImageType image(N, M, Q);
  
  // read image
  readImage(argv[1], image);
  
  
  
  // Parse through image.pixelvalue[0..255][0..255] and increment each index in pixelCount
  for(int i = 0; i < N; i++){
    for(int j = 0; j < M; j++){
        //val = image.pixelValue[i][j]; //holds the gray level value at that matrix cell
        image.getPixelVal(i, j, val);
        pixelCount[val]++; //increments gray value at that index/gray value
    }
  }
  
  /*for(int i = 0; i < Q+1; i++){
    cout << pixelCount[i] << " ";
  }
  cout << endl;*/ //PRINTS ALL COUNTED PIXEL VALUES
  
  
  
  
  // Calculate the individual probability of each pixel value up to Q+1
  float calculatedProbability = 0.0;
  for(int i = 0; i < Q+1; i++){
    calculatedProbability = pixelCount[i] / totalPixels;
    probabilityDensity[i] = calculatedProbability;
  }
  
  /*for(int i = 0; i < Q+1; i++){
    cout << fixed << setprecision(6) << probabilityDensity[i] << " ";
  }*/ //PRINTS ALL PDF VALUES
  
  
  
  // Adds up all pdf values cumulatively
  float temp; //holds previous value as we iterate
  for(int i = 0; i < Q+1; i++){
    temp += probabilityDensity[i];
    cumulativeDistribution[i] = temp;
  }
  
  /*cout << "Cumulative Dist. : " << endl;
  for(int i = 0; i < Q+1; i++){
    cout << fixed << setprecision(6) << cumulativeDistribution[i] << " ";
  }
  cout << endl;*/ //PRINTS ALL CUMULATIVE PROB. VALUES
  
  
  
  // Copies cumulativeDistribution[] to newValues[] so we can use cumulativeDistribution[]'s data later if we need it
  for(int i = 0; i < Q+1; i++){
    newFloatValues[i] = cumulativeDistribution[i];
  }
  
  // Scales all values in cumulativeDistribution[Q+1] up to the specified equalization value (gray level, or Q+1)
  for(int i = 0; i < Q+1; i++){
    newFloatValues[i] = newFloatValues[i] * equalizer;
    newFloatValues[i] = floor(newFloatValues[i]);
  }
  
  // Copies from the float array to an int array just for the possibility of no floats allowed 
  for(int i = 0; i < Q+1; i++){
      newFlooredIntValues[i] = newFloatValues[i];
  }
  
  /*cout << "newFlooredIntValues : " << endl;
  for(int i = 0; i < Q+1; i++){
    cout << newFlooredIntValues[i] << " ";
  }
  cout << endl;*/ //PRINTS ALL THE EQUALIZED HISTOGRAM VALUES (INT)
  
  
  // Run through original image.pixelValues[][], store that in val, replace value at new cell with newFlooredIntValues[val];
  ImageType newImage(N, M, Q);
  
  for(int i = 0; i < N; i++){
    for(int j = 0; j < M; j++){
        image.getPixelVal(i, j, val);
        newImage.setPixelVal(i, j, newFlooredIntValues[val]);
    }
  }
  
  writeImage(argv[2], newImage);
  cout << "Output: " << argv[2] << endl;
  
  
  return (1);
}









