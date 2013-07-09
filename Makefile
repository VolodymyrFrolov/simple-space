CC = g++
CFLAGS = -std=c++11 -Wall -c -Iinc
LDFLAGS = -pthread
SOURCES = main.cpp simplespace.cpp planet.cpp physics.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = simple-space

UNAME := $(firstword $(shell uname -s))
ifeq ($(UNAME), Linux)
LDLIBS = -lGL -lglut
endif
ifeq ($(UNAME), Darwin)
LDLIBS = -framework GLUT -framework OpenGL
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) main.o physics.o simplespace.o planet.o $(LDLIBS) -o $(TARGET)

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp

simplespace.o: src/simplespace.cpp inc/simplespace.h
	$(CC) $(CFLAGS) src/simplespace.cpp
	
planet.o: src/planet.cpp inc/planet.h
	$(CC) $(CFLAGS) src/planet.cpp

physics.o: src/physics.cpp inc/physics.h
	$(CC) $(CFLAGS) src/physics.cpp

clean:
	rm -rf $(TARGET) *.o
