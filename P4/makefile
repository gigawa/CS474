CFLAGS = -g

all: clean

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

noise.o:		noise.cpp
	g++ -c noise.cpp

FFT2D.o:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o fft.o \
		FFT2D.cpp
	g++ -c $(CFLAGS) image.o ReadImage.o ReadImageHeader.o fft.o \
					WriteImage.o FFT2D.cpp

FFT2Dfloat.o:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o fft.o \
		FFT2Dfloat.cpp
	g++ -c $(CFLAGS) image.o ReadImage.o ReadImageHeader.o fft.o \
					WriteImage.o FFT2Dfloat.cpp

RemoveNoise:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o fft.o FFT2D.o\
		RemoveNoise.cpp
	g++ -o RemoveNoise $(CFLAGS) FFT2D.o image.o ReadImage.o ReadImageHeader.o fft.o \
					WriteImage.o RemoveNoise.cpp

Restoration:	image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o fft.o noise.o FFT2D.o FFT2Dfloat.o\
		Restoration.cpp
	g++ -o Restoration $(CFLAGS) FFT2D.o FFT2Dfloat.o image.o ReadImage.o ReadImageHeader.o fft.o noise.o \
					WriteImage.o Restoration.cpp

Homomorphic:    image.h image.o ReadImage.o ReadImageHeader.o WriteImage.o fft.o FFT2Dfloat.o\
		Homomorphic.cpp
	g++ -o Homomorphic $(CFLAGS) FFT2Dfloat.o image.o ReadImage.o ReadImageHeader.o fft.o \
					WriteImage.o Homomorphic.cpp

clean:
	rm -f *o Restoration RemoveNoise
