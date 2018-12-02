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
void Apply2DFFTfloat(float ** realPart, float ** imaginaryPart, int N, int M, int isign);
void NormalizeImage(ImageType & image, int N, int min, int max);

/*  Homomorphic Filtering steps:
    1. Take ln (log in cmath) of input image's real values
    2. Apply 2D FFT to create a real and an imaginary part
    3. CENTER the real & imaginary, then apply high-freq. emphasis filtering (use equation)
       on both the real and imaginary parts at each [i][j]
    4. Take inverse of the image, supplying the high-passed real & imaginary parts
    5. Apply the exponential function (exp in cmath)
        a. Check the values and normalize if any values are above 255 or below 0
    6. Calculate and display magnitude of the image afterwards
*/
          

int main(int argc, char * argv[]) {
    // argv[1] file name - will hold the input image
    // argv[2] file name - output to this file

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
    float ** realPart = new float*[N];
    float ** imaginaryPart = new float*[N];
    float ** magnitudeValues = new float*[N];

        //Allocate memory for double pointers (2D Arrays)
    for(int i = 0 ; i < N ; i++){
            realPart[i] = new float[N];
            imaginaryPart[i] = new float[N];
            magnitudeValues[i] = new float[N];
    }
    
    
        //Stick the real values into realPart and make all values in imaginaryPart 0
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            inputImage.getPixelVal(i, j, val);
            realPart[i][j] = val;
            imaginaryPart[i][j] = 0;
            magnitudeValues[i][j] = 0; 
        }
    }
    
    //~~~~~~~~~~~~Begin steps here
    
        //Step 1: apply 'ln' function (do it to only realPart)
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            realPart[i][j] = log(realPart[i][j]);
        }
    }
    cout << "realPart at 0,0 (log): " << realPart[0][0] << endl; 
    cout << "imaginaryPart at 0,0 (log): " << imaginaryPart[0][0] << endl; 
    cout << "Step 1 done. " << endl;
    
    
        //Step 2: apply forward 2D FFT on realPart & imaginaryPart
        //Shift
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            realPart[i][j] *= pow(-1, i+j);
        }
    }      
        
    Apply2DFFTfloat(realPart, imaginaryPart, N, M, 1);
    cout << "realPart at 0,0 (fft): " << realPart[0][0] << endl; 
    cout << "imaginaryPart at 0,0 (fft): " << imaginaryPart[0][0] << endl; 
    cout << "Step 2 done. " << endl;
    
    
    
    
        //Step 3: CENTER the application of the high-emphasis filter by replacing u,v = i,j with u,v = i-N/2,j-N/2
        /*
        a. Store gammaH - gammaL as 1 variable, a
        b. Store '-c (where c = 1 like slides) * [ (u^2 + v^2) / Dsub0^2) ]' as another variable, b
        c. Perform variable a * variable b, store into another variable, c
        d. Do c + gammaL & store final values into real/imaginary @ [i][j]
        
        Dsub0 = 1.8, gammaH = 1.5, gammaL = 0.5, 
        */
    float dSubZero = 1.8;
    float gammaH = 1.5;
    float gammaL = 0.5;
    float c = 1.0;
    
    cout << "Enter gammaH (base is 1.5): " << endl;
    cin >> gammaH;
    
    cout << "Enter gammaL (base is 0.5): " << endl;
    cin >> gammaL;
    
    
    float gammaResult = gammaH - gammaL;
    
        //Shift
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            realPart[i][j] *= pow(-1, i+j);
        }
    }
    
    
    
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
        
            float u = i - N/2;
            float v = j - N/2;
            
            //float u = i;
            //float v = j;
            
            float bigTerm = 1 - exp(-c * ( (pow(u,2) + pow(v,2)) / pow(dSubZero,2)));
        
            realPart[i][j] = (gammaResult * bigTerm) + gammaL;
            imaginaryPart[i][j] = (gammaResult * bigTerm) + gammaL;
        }
    }
    cout << "realPart at 0,0 (filter): " << realPart[0][0] << endl; 
    cout << "imaginaryPart at 0,0 (filter): " << imaginaryPart[0][0] << endl; 
    cout << "Step 3 done. " << endl;
    
    
    
        //Step 4: Apply inverse FFT
    Apply2DFFTfloat(realPart, imaginaryPart, N, M, -1);
    cout << "realPart at 0,0 (inverse fft): " << realPart[0][0] << endl; 
    cout << "imaginaryPart at 0,0 (inverse fft): " << imaginaryPart[0][0] << endl; 
    cout << "Step 4 done. " << endl;
    
    
        //Step 5: apply exp
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            realPart[i][j] = exp(realPart[i][j]);
        }
    }
    cout << "realPart at 0,0 (exp): " << realPart[0][0] << endl; 
    cout << "imaginaryPart at 0,0 (exp): " << imaginaryPart[0][0] << endl; 
    cout << "Step 5 done. " << endl;
    
    
        //Step 6: apply magnitude sqrt(pow(realPart[i][j],2) + pow(imaginaryPart[i][j],2));
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            magnitudeValues[i][j] = sqrt(pow(realPart[i][j],2) + pow(imaginaryPart[i][j],2));
        }
    }
    cout << "realPart at 0,0 (magnitude): " << realPart[0][0] << endl; 
    cout << "imaginaryPart at 0,0 (magnitude): " << imaginaryPart[0][0] << endl; 
    cout << "Step 6 done. " << endl;
    
    
    
        //Write magnitude to output image
    float stretchWeight = 0;
    cout << "Input stretchWeight value: " << endl;
    cin >> stretchWeight;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            magnitudeValues[i][j] = stretchWeight * log(1 + magnitudeValues[i][j]);
        }    
    }
    
        
        
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            outputImage.setPixelVal(i, j, magnitudeValues[i][j]);
        }
    }    
    writeImage(argv[2], outputImage);
    cout << "Output: " << argv[2] << endl;
    
    return 0;
}


































