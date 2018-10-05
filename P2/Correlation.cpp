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

//Correlation execution command line example:
// ./Correlation Pattern.pgm Image.pgm CorrOut.PGM
// where "Pattern" is the image to look for in "Image" and results saved to "CorrOut"
// so: argv[1] is the pattern image that defines the mask
//     argv[2] is the image to look for the pattern
//     argv[3] is the output image where correlation results are stored and normalized
//            to be viewed as a PGM greyscale image

int main(int argc, char *argv[])
{
  int i, j;
  int N, M, Q; // refers to the pattern, argv[1], and its dimensions/grey levels (row, col, grey)
  int J, K, L; // refers to the input image, argv[2] (row, col , grey)
               // also the same dimensions to use for output image
  bool type;
  int val;

  // read image header
  readImageHeader(argv[1], N, M, Q, type);
  readImageHeader(argv[2], J, K, L, type);

  // allocate memory for the image array
  ImageType patternImage(N, M, Q);
  ImageType inputImage(J, K, L);
  
  //Creates new image with square dimensions to store input image
  int outputDimensions = 0;
  
    if(J > K){
        outputDimensions = J;
        ImageType outputImage(J, J, L);
    }
    else if(K > J){
        outputDimensions = K;
        ImageType outputImage(K, K, L);
    }
    
  ImageType outputImage(outputDimensions, outputDimensions, L);
    
    

  // read image
  readImage(argv[1], patternImage);
  readImage(argv[2], inputImage);
  
  
  //Debug cout's
  cout << endl;
  cout << "Format: 'Image' 'Rows' 'Columns' 'Grey Levels' " << endl;
  cout << "__________________" << endl;
  cout << "Pattern N M Q: " << N << "  " << M << "  " << Q << endl;
  cout << "Input J K L: " << J << "  " << K << "  " << L << endl;
  
  
  //Find mask dimensions to make it into a square for image traversal
  int maskDimensions = 0;
  
  if(N > M && N%2 != 0){
    //Then N is larger, and is odd. Use this to create the mask matrix with both 
    //rows and columns set to N, with all extra cells filled with 0s
    maskDimensions = N;
  }
  else if(M > N && M%2 != 0){
    //Then M is larger, and is odd. Use this as the dimensions for the mask.
    maskDimensions = M;
  }
  else{
    cout << "ERROR: Neither the rows or columns are odd, cannot make a square matrix." << endl;
    return(0);
  }
  
  
    //Mask Creation
	//pointer to 2D array
	int ** mask = new int*[maskDimensions];

	//pointer initialization
	for(int i = 0 ; i < maskDimensions ; i++)
	{
		mask[i] = new int[maskDimensions];
	}

    //Set all cells to 0 
    for(int i = 0; i < maskDimensions; i++)
	{
		for(int j = 0; j < maskDimensions; j++)
		{
			mask[i][j] = 0;
		}
	}


    cout << endl << "Inputting mask weights from pattern image (argv[1])..." << endl;
	//Fill mask weights, make sure to only fill in however many rows and columns are in
	//pattern image, as this matrix is larger than that image. To do this, utilize
	//previously stored row and col values for argv[1] in N and M
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < M; j++)
		{
		    patternImage.getPixelVal(i, j, val);
			mask[i][j] = val;
			//cout << mask[i][j] << " ";
		}
		//cout << endl;
	}
	cout << endl << "...mask weights stored. " << endl;
	

	//Now copy in the inputImage into the cells of outputImage
	//Make sure to only work around inputImage's dimensions or else you'll be OOB (use J & K)
	val = 0;
	for(int i = 0; i < J; i++){
	    for(int j = 0; j < K; j++){
	        inputImage.getPixelVal(i, j, val);
	        outputImage.setPixelVal(i, j, val);
	        //cout << val << " ";
	    }
	   //cout << endl;
	}
	
	for(int i = 0; i < outputDimensions; i++){
	    for(int j = 0; j < outputDimensions; j++){
	        outputImage.getPixelVal(i, j, val);
	        //cout << val << " ";
	    }
	   //cout << endl;
	}




    int padding = (maskDimensions - 1)/2; //Using J as that corresponds to rows in the input image argv[2]

    cout << "Applying mask" << endl;
    ApplyMask(inputImage, mask, padding, J, K, L);
    cout << "...mask applied" << endl;


    //Write image into argv[3] using inputImage which is now mask-applied
    writeImage(argv[3], inputImage);
    cout << "Output: " << argv[3] << endl;

  return (1);
}
