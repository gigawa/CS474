#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
void fft(float data[], unsigned long nn, int isign);
void Apply2DFFT(int ** realPart, int ** imaginaryPart, int N, int M, int isign);

/*
    How to use:
        "./FFT2D whiteSquare'x'.pgm 'output filename'.pgm"
            - where 'x' is the white square's dimensions, 32, 64, or 128


    To do this:
        Function will look like this:
            Apply2DFFT(float function[], int N, string Output, int isign)
                inside: call fft( function[], N, isign);
    
    At the beginning of Apply2DFFT, we create an array[2*N + 1]
    then we initialize this array with {-1, x, 0, x, 0, ... , 0}; <- filling the first spot with a -1
    and the even spots with 0 (imaginary part).
    
    Afterwards pull the row information from the image one row at a time up to N times
    then call fft, normalize, then store into new matrix.
    
    Afterwards, do the same thing but using columns and using the new matrix values, normalize
    then you have your frequency domain.
    
    Experiments:
        2a. Use 512x512 image with a 32x32 white square at the center. Compute magnitude
            before shifting, then compute magnitude after shifting. 
                NOTE: Stretch the magnitude of the original magnitude values before each part (pre-shift and post-shift);
            
        2b. Do the same with the 64x64 white square image. How does this compare to 2a?
        
        2c. Do the same with the 128x128 white square image. How does this compare to both 2a and 2b?
                    
*/

int main(int argc, char *argv[])
{
    
    // argv[1] file name - will hold the input image
    // argv[2] file name - will contain un-shifted magnitude output 
    // argv[3] file name - will hold the magnitude output 

    int M, N, Q; // N is rows, M is columns, Q is gray levels
    bool type;
    int val;
    int isign = -1;
    float stretchWeight = 40;

    readImageHeader(argv[1], N, M, Q, type); //read in the input image, store rows to N, cols to M, gray lvl to Q
    
    cout << " Header values ... N: " << N << " M: " << M << " Q: " << Q << endl;

    
        // Now create a 2D matrix, store all the pixel values from the image into this matrix to use in Apply2DFFT
    ImageType inputImage(N, M, Q);
    ImageType outputImage(N, M, Q);
    
    readImage(argv[1], inputImage);   
    cout << " '" << argv[1] << "'" << " read into inputImage ... " << endl;
        
        
        //Creating the 2D array that will hold the results in between FFT steps as well as the final results
    int ** realPart = new int*[N];
    int ** imaginaryPart = new int*[N];
    int ** magnitudeValues = new int*[N];
    
        //Allocate memory for double pointers (2D Arrays)
    for(int i = 0 ; i < N ; i++)
	{
		realPart[i] = new int[N];
		imaginaryPart[i] = new int[N];
		magnitudeValues[i] = new int[N];
	}

        //Stick the real values into realPart and make all values in imaginaryPart 0
        
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            inputImage.getPixelVal(i, j, val);
            realPart[i][j] = val;
            imaginaryPart[i][j] = 0;
            magnitudeValues[i][j] = 0;
            //cout << val << " ";
        }
    }
    
    
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
        //                                  //
        //    APPLY SHIFT TO CENTER HERE    //
        //                                  //
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
        
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            realPart[i][j] *= pow(-1, j);
        }
    }
    
        //Now call Apply2DFFT, do the calculations there, then output them in main afterwards
    Apply2DFFT(realPart, imaginaryPart, N, M, isign);
    
    
        //Perform magnitude calculations (sqrt(a^2 + b^2))
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            magnitudeValues[i][j] = sqrt((realPart[i][j] * realPart[i][j]) + (imaginaryPart[i][j] * imaginaryPart[i][j]));
        }    
    }
        
        //Now perform stretching to properly view magnitude as an image

    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            magnitudeValues[i][j] = stretchWeight * log(1 + magnitudeValues[i][j]);
        }    
    }
        
        
        //Write pixel values to outputImage 
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            outputImage.setPixelVal(i, j, magnitudeValues[i][j]);
        }
    }

    writeImage(argv[2], outputImage);

    return 0;
}
    

void Apply2DFFT(int ** realPart, int ** imaginaryPart, int N, int M, int isign){

    //Make a new array sized to N * 2 + 1
    //initialize with:
    //   -1 at [0]  <- fft.c does not utilize [0] indexing
    //   0 at all [even#]  <- imaginary part
    //   image.getPixelVal[i,j] at all [odd#]  <- real part based off of image values

    float currentArray[(N * 2) + 1] = {0};
    int arraySize = (N * 2) + 1;
    int val = 0;
    int oddSpots = 1; // Reals start at 1, +2 per step
    int evenSpots = 2; // Imaginary start at 2, +2 per step
    
        //Initialize currentArray with directions from above:
        
    currentArray[0] = -1; //stores the faux -1 value so that fft from fft.c can receive our array
    
    
        //Now build the array up from each row then call fft on it
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            currentArray[oddSpots] = realPart[i][j];
            currentArray[evenSpots] = imaginaryPart[i][j];
            
            oddSpots += 2;
            evenSpots += 2;
        }
        
        oddSpots = 1;
        evenSpots = 2;
        
        
            //Call FFT on currentArray
        fft(currentArray, N, isign);
        
        
            //Store the transformed values back into the same row/column it got pulled from
        for(int k = 0; k < M; k++){
            realPart[i][k] = currentArray[oddSpots];
            imaginaryPart[i][k] = currentArray[evenSpots];
            
            oddSpots += 2;
            evenSpots += 2;
        }
        
        oddSpots = 1;
        evenSpots = 2;
    }
    
        //To be safe, reset oddSpots and evenSpots in case above loop short-circuits somehow
    oddSpots = 1;
    evenSpots = 2;
    
        //Now do FFT on columns
    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            currentArray[oddSpots] = realPart[j][i];
            currentArray[evenSpots] = imaginaryPart[j][i];
            
            oddSpots += 2;
            evenSpots += 2;
        }
        
        oddSpots = 1;
        evenSpots = 2;
        
        
        
            //Call FFT on currentArray
        fft(currentArray, N, isign);
        
            //Normalize the array before storage
        for(int h = 1; h < arraySize; h++) {
            currentArray[h] /= N;
        }
        
        
            //Store the transformed values back into the same row/column it got pulled from
        for(int k = 0; k < N; k++){
            realPart[k][i] = currentArray[oddSpots];
            imaginaryPart[k][i] = currentArray[evenSpots];
            
            oddSpots += 2;
            evenSpots += 2;
        }
        
        oddSpots = 1;
        evenSpots = 2;
    }
    
        //Some values go < 0, can't write image using negative values, so we set those to 0
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            if(realPart[i][j] < 0){
                realPart[i][j] = 0;
            }
        }
    }
}

    
    
    
    
    
    
    
    
    
    
    
    





