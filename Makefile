CC=g++
CFLAGS=-Wall
SOURCES=src/phys.h src/main.cpp

all: simple-space

simple-space:
	$(CC) $(SOURCES) -o simple-space

clean:
	rm -rf simple-space *.c
