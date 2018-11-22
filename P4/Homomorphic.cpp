#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
void Apply2DFFT(int ** realPart, int ** imaginaryPart, int N, int M, int isign);
void NormalizeImage(ImageType & image, int N, int min, int max);

/*  Homomorphic Filtering steps:
    1. Take ln (log in cmath) of input image's real values
    2. Apply 2D FFT to create a real and an imaginary part
    3. CENTER the real & imaginary, then apply high-freq. emphasis filtering (use equation)
       on both the real and imaginary parts at each [i][j]
        a. Store gammaH - gammaL as 1 variable, a
        b. Store '-c (where c = 1 like slides) * [ (u^2 + v^2) / Dsub0^2) ]' as another variable, b
        c. Perform variable a * variable b, store into another variable, c
        d. Do c + gammaL & store final values into real/imaginary @ [i][j]
    4. Take inverse of the image, supplying the high-passed real & imaginary parts
    5. Apply the exponential function (exp in cmath)
        a. Check the values and normalize if any values are above 255 or below 0
    6. Calculate and display magnitude of the image afterwards
*/
          

int main(int argc, char * argv[]) {
    // argv[1] file name - will hold the input image

    int N, M, Q; //dimensions N & M and gray levels Q
    bool type;
    int val = 0;

        //Read image header values
    readImageHeader(argv[1], N, M, Q, type); //read in the input image, store rows to N, cols to M, gray lvl to Q

    cout << " Header values ... N: " << N << " M: " << M << " Q: " << Q << endl;


        //Create image data types to store the input image and the output image
    ImageType inputImage(N, M, Q);
    ImageType outputImage(N, M, Q);

    readImage(argv[1], inputImage);
    cout << " '" << argv[1] << "'" << " read into inputImage ... " << endl;
    
        //Apply natural logarithm to all real values in the image & create the matrices
    int ** realPart = new int*[N];
    int ** imaginaryPart = new int*[N];

        //Allocate memory for double pointers (2D Arrays)
    for(int i = 0 ; i < N ; i++){
            realPart[i] = new int[N];
            imaginaryPart[i] = new int[N];
    }
    
    
        //Stick the real values into realPart and make all values in imaginaryPart 0
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            inputImage.getPixelVal(i, j, val);
            realPart[i][j] = val;
            imaginaryPart[i][j] = 0;
        }
    }

    return 0;
}


































