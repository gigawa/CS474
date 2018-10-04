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

  int n;
	//input number of rows and columns
	cout << "Enter Number of rows: ";
	cin >> n;

	//pointer to 2D array
	int ** mask = new int*[n];

	//pointer initialization
	for(int i=0;i<n;i++)
	{
		mask[i] = new int[n];
	}

  cout << "Enter Mask: ";
	//input array elements
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
		{
			cin >> mask[i][j];
		}
	}

  //int padding = 3;
  int padding = (n - 1)/2;
  //int mask [7][7] = {1,1,2,2,2,1,1,1,2,2,4,2,2,1,2,2,4,8,4,2,2,2,4,8,16,8,4,2,2,2,4,8,4,2,2,1,2,2,4,2,2,1,1,1,2,2,2,1,1};
  //int mask [3][3] = {0,1,0,1,-4,1,0,1,0};

  ApplyMask(image, mask, padding, N, M, Q);

  // write each sampled image
  writeImage(argv[2], image);
  cout << "Output: " << argv[2] << endl;

  return (1);
}
