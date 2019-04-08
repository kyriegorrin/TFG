CC=gcc
CXX=g++
CPP=$(CXX)

IDIR=./Include
LDIR=.
LIBS=-lOpenNI2
ODIR=.
OBJ=test.o

CFLAGS=-I$(IDIR) -L$(LDIR)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS) -Wl,-rpath=./

.PHONY: clean

clean:
	rm -f test.o
