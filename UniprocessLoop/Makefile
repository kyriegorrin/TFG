CC=gcc
CXX=g++
CPP=$(CXX)

IDIR=./Include
LDIR=.
LIBS=-lOpenNI2
ODIR=.
OBJ=main.o minilzo.o

CFLAGS=-I$(IDIR) -L$(LDIR)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

%.o: %.c
	$(CXX) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS) -Wl,-rpath=./

.PHONY: clean

clean:
	rm -f main.o minilzo.o
	rm main
