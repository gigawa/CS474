#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"

using namespace std;

int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);
void swap(int*, int*);
int bubbleSort(int[], int);

//MedianFiltering.cpp -
// To use: ./MedianFiltering image1.PGM int_val image2.PGM
//          where image1.PGM is the image to be filtered
//          int_val is the size of the filter mask (has to be odd)
//          image2.PGM is the output image name to be written on

int main(int argc, char *argv[])
{

    int i, j; //i -> N, j -> M
    int N, M, Q; // N is rows, M is columns, Q is gray levels
    bool type;
    int val;
    int filterSize; //holds the size of the filter specified through the command-line. Has to be odd
    int padding; //holds the padding size
    
    filterSize = atoi(argv[2]); //grabs the filter size specified by the user
    
    if(filterSize % 2 == 0){
        cout << "ERROR: Please use an odd integer for the filter size. (i.e. if you enter '3', the filter size will be '3x3')" << endl;
        return(-1);
    }
    else{
        cout << endl << "Filter size: " << filterSize << endl;
        padding = (filterSize - 1)/2; //set padding size based off of filter
    }

    
    // read image header
    readImageHeader(argv[1], N, M, Q, type);
    cout << "Image read." << endl;
    cout << "N: " << N << " M: " << M << " Q: " << Q << endl;
    
    ImageType inputImage(N, M, Q);
    readImage(argv[1], inputImage);
    
    ImageType outputImage(N, M, Q); //will hold the median filter results
    
    
    //Create padded image
    cout << " Creating padded image..." << endl;
    int TotalPadding = padding * 2;
    
    ImageType * paddedImage = new ImageType(N + TotalPadding, M + TotalPadding, Q);

    for(int i = 0 ; i < N; i++) {
        for(int j = 0; j < M; j++) {
          inputImage.getPixelVal(i, j, val);
          paddedImage->setPixelVal(i + padding, j + padding, val);
        }
    }
    
    cout << " Padded image created" << endl;
    
    
    //Perform Median Filtering
    val = 0;
    int indexer = 0; //1-D index tracker for our pixelList below
    int sizeOfList = filterSize * filterSize;
    int pixelList[sizeOfList] = {0}; //creates an array that holds nxn elements where n is the filter size
    
    
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
        
          paddedImage->getPixelVal(i, j, val);

          int v = 0;
          indexer = 0;

          //iterate through area on image that mask covers
          for(int k = 0; k < filterSize; k++) {
            for(int l = 0; l < filterSize; l++) {
            
              paddedImage->getPixelVal(i + k, j + l, val);
             
              //Now store it in an array
              pixelList[indexer] = val;
              indexer++;
              
            }
          }
          
          // MEDIAN CODE
          //Perform sort and store value:
          //Stores median value from resulting sorted list in v
          v = bubbleSort(pixelList, sizeOfList);
            
          //Reset array values
          for(int h = 0; h < sizeOfList; h++){
            pixelList[h] = 0;
          }          

          outputImage.setPixelVal(i, j, v);

          //Write median (stored in v) into the output image
          
          // AVERAGING CODE
          /*
          int accumulator = 0;
          
          for(int h = 0; h < sizeOfList; h++){
            accumulator += pixelList[h];
          }   
          
          for(int h = 0; h < sizeOfList; h++){
            pixelList[h] = 0;
          }     
          
          v = accumulator/sizeOfList;
          
          outputImage.setPixelVal(i, j, v);*/

        }
    }
    
    /*
    int testList[9] = {9,8,7,6,5,4,3,2,1};
    int size = 9;
    cout << "Testing bubbleSort on testList: " << endl;
    bubbleSort(testList,size);
    cout << "Sorting done. Printing array: " << endl;
    for(int i = 0; i < size; i++){
        cout << testList[i] << " ";
    }
    cout << "Done." << endl;*/
    
    //Write results to argv[3] which holds the output image name specified by the user
    cout << " Writing resulting image... " << endl;
    writeImage(argv[3], outputImage);
    cout << " Done. " << endl;
    
    return(1);
}
    
// swap
// Swaps *xp and *yp
void swap(int *xp, int *yp){

    int temp = *xp;
    *xp = *yp;
    *yp = temp;
    
}



// bubbleSort:
// Takes in an array of integers to be sorted
// size - integer value denoting how many elements in the array (should be passed in as a square of the filterSize)
// returns the median value (stored at [size - 1] / 2)
int bubbleSort(int pixels[], int size){

    for(int i = 0; i < size - 1; i++){
    
        for(int j = 0; j < size - i - 1; j++){
            if(pixels[j] > pixels[j+1]){
                swap(&pixels[j], &pixels[j+1]);
            }  
        } 
    }


    int middle = (size - 1)/2;
    return pixels[middle];
    
}













