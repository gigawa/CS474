CFLAGS = -g

ReadImage.o:	image.h ReadImage.cpp
	g++ -c $(CFLAGS) ReadImage.cpp

ReadImageHeader.o:	image.h ReadImageHeader.cpp
	g++ -c $(CFLAGS) ReadImageHeader.cpp

WriteImage.o:	image.h WriteImage.cpp
	g++ -c $(CFLAGS) WriteImage.cpp

image.o:	image.h image.cpp
	g++ -c $(CFLAGS) image.cpp

Threshold:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o \
		Threshold.cpp
	g++ -o Threshold $(CFLAGS) image.o ReadImage.o ReadImageHeader.o \
					WriteImage.o Threshold.cpp

Sample:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o \
		Sample.cpp
	g++ -o Sample $(CFLAGS) image.o ReadImage.o ReadImageHeader.o \
					WriteImage.o Sample.cpp

Quantization:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o \
		Quantization.cpp
	g++ -o Quantization $(CFLAGS) image.o ReadImage.o ReadImageHeader.o \
					WriteImage.o Quantization.cpp
					
Equalization:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o \
		Equalization.cpp
	g++ -o Equalization $(CFLAGS) image.o ReadImage.o ReadImageHeader.o \
					WriteImage.o Equalization.cpp
