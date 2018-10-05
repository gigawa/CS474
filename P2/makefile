CFLAGS = -g

ReadImage.o:	image.h ReadImage.cpp
	g++ -c $(CFLAGS) ReadImage.cpp

ReadImageHeader.o:	image.h ReadImageHeader.cpp
	g++ -c $(CFLAGS) ReadImageHeader.cpp

WriteImage.o:	image.h WriteImage.cpp
	g++ -c $(CFLAGS) WriteImage.cpp

image.o:	image.h image.cpp
	g++ -c $(CFLAGS) image.cpp

Mask.o:		image.h Mask.cpp
	g++ -c $(CFLAGS) Mask.cpp

Correlation:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o Mask.o \
		Correlation.cpp
	g++ -o Correlation $(CFLAGS) image.o ReadImage.o ReadImageHeader.o Mask.o \
					WriteImage.o Correlation.cpp

SaltPepper:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o Mask.o \
		SaltPepper.cpp
	g++ -o SaltPepper $(CFLAGS) image.o ReadImage.o ReadImageHeader.o Mask.o \
					WriteImage.o SaltPepper.cpp


Smoothing:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o Mask.o \
		Smoothing.cpp
	g++ -o Smoothing $(CFLAGS) image.o ReadImage.o ReadImageHeader.o Mask.o \
					WriteImage.o Smoothing.cpp

Sharpening:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o Mask.o \
		Sharpening.cpp
	g++ -o Sharpening $(CFLAGS) image.o ReadImage.o ReadImageHeader.o Mask.o \
					WriteImage.o Sharpening.cpp
