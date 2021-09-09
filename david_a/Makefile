GXX=g++
CXXFLAGS=-Wall -pedantic -std=c++14
DEL=rm -f

# Adaptez ces deux variables avec vos propres chemins s'ils sont différents :
INCPATH_JPEG=
LDPATH_JPEG=

.PHONY: all clean

all: essai.exe correction.exe

Image.o : Image.cpp Image.hpp
	$(GXX) -g $(CXXFLAGS) $(INCPATH_JPEG) -c Image.cpp

essai.exe : essai.cpp Image.hpp Image.o
	$(GXX) $(CXXFLAGS) $(INCPATH_JPEG) -g -o essai.exe essai.cpp Image.o $(LDPATH_JPEG) -ljpeg

correction.o : compilator.exe Image.hpp
	./compilator.exe $(INCPATH_JPEG)

correction.exe : correction.o Image.o
	$(GXX) $(CXXFLAGS) -o correction.exe correction.o Image.o $(LDPATH_JPEG) -ljpeg

clean:
	$(DEL) *.o essai.exe correction.exe
