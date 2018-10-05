#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);

//SaltPepper.cpp -
// To use: ./SaltPepper image1.PGM output.PGM
//          where image1.PGM is the image to be salt & peppered, resulting in output.PGM

int main(int argc, char *argv[])
{

    //cout << "get" << endl;
    int i, j; //i -> N, j -> M
    int M, N, Q; // N is rows, M is columns, Q is gray levels
    bool type;
    int replace; //true 30% or 50% of the time
    int whiteOrBlack; //chosen on 50% chance
    int val;
    int spVal;

    // read image header
    readImageHeader(argv[1], N, M, Q, type);
    
    ImageType inputImage(N, M, Q);
    ImageType outputImage(N, M, Q);
    
    //cout << "asdsd" << endl;
    readImage(argv[1], inputImage);
    //cout << "aszzzz" << endl;
    
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
        
            inputImage.getPixelVal(i, j, val);
            //now val holds the pixel value at i, j
            //we need to randomly decide whether or not to fill in the corresponding
            //pixel in 'outputImage' with the actual value, or with 0/255 depending on the random
            //integer values stored in 'replace' and 'whiteOrBlack'
            
            replace = rand() % 100; //Produces a random integer from 1-100
            whiteOrBlack = rand() % 100;
            
            //50% chance whiteOrBlack is below 50)
            if(whiteOrBlack < 50){
                spVal = 255;
            }
            else{
                spVal = 0;
            }
            
            //30% or 50% chance replace is less than 30 or 50 respectively
            //if(replace < 30 or 50 for 30% or 50% chance at replacing)
            if(replace < 50){
                //Then salt and pepper instead of real pixel val
                outputImage.setPixelVal(i, j, spVal);
            }
            else{
                outputImage.setPixelVal(i, j, val);
            }
        }
    }
    
    writeImage(argv[2], outputImage);
    
    cout << "Done. Image written to " << argv[2] << endl;
  
    return(1);  
}

