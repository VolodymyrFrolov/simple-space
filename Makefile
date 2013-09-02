CFLAGS = -std=c++0x -stdlib=libc++ -Wall -c -Iinc
LDFLAGS = -pthread
SOURCES = main.cpp simplespace.cpp planet.cpp physics.cpp controlsmanager.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = simple-space

UNAME := $(firstword $(shell uname -s))
ifeq ($(UNAME), Linux)
CC = g++
LDLIBS = -lGL -lglut
#Check these: -lglut -lGLU -lGL -L/usr/X11R6/lib/ -lXmu -lXi -lXext -lX11 -lXt
endif
ifeq ($(UNAME), Darwin)
CC = cc
LDLIBS = -framework GLUT -framework OpenGL
#Check these: -framework GLU
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) main.o physics.o simplespace.o planet.o controlsmanager.o $(LDLIBS) -o $(TARGET)

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp

simplespace.o: src/simplespace.cpp inc/simplespace.h
	$(CC) $(CFLAGS) src/simplespace.cpp
	
planet.o: src/planet.cpp inc/planet.h
	$(CC) $(CFLAGS) src/planet.cpp

physics.o: src/physics.cpp inc/physics.h
	$(CC) $(CFLAGS) src/physics.cpp

controlsmanager.o : src/controlsmanager.cpp inc/controlsmanager.h
	$(CC) $(CFLAGS) src/controlsmanager.cpp

clean:
	rm -rf $(TARGET) *.o
