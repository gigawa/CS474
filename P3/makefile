CFLAGS = -g

ReadImage.o:	image.h ReadImage.cpp
	g++ -c $(CFLAGS) ReadImage.cpp

ReadImageHeader.o:	image.h ReadImageHeader.cpp
	g++ -c $(CFLAGS) ReadImageHeader.cpp

WriteImage.o:	image.h WriteImage.cpp
	g++ -c $(CFLAGS) WriteImage.cpp

image.o:	image.h image.cpp
	g++ -c $(CFLAGS) image.cpp

fft.o:		fft.c
	g++ -c fft.c

FFT1D:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o fft.o \
		FFT1D.cpp
	g++ -o FFT1D $(CFLAGS) image.o ReadImage.o ReadImageHeader.o fft.o \
					WriteImage.o FFT1D.cpp
