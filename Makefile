SOURCES = main.cpp simplespace.cpp planet.cpp physics.cpp timer.cpp controls.cpp mouse.cpp
#SOURCES_FULL_PATHS = $(addprefix src/, $(SOURCES))
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = simple-space

#Later replace with -std=c++11, when use compiler version that supports it
CFLAGS = -std=c++0x -Iinc

UNAME := $(firstword $(shell uname -s))

ifeq ($(UNAME), Linux)
CC = g++
CFLAGS += -Wall -c
LDFLAGS = -pthread
LDLIBS = -lGL -lglut
#Check these: -lglut -lGLU -lGL -L/usr/X11R6/lib/ -lXmu -lXi -lXext -lX11 -lXt
endif

ifeq ($(UNAME), Darwin)
CC = cc
CFLAGS += -stdlib=libc++ -I/opt/X11/include
LDLIBS = -framework GLUT -framework OpenGL
#Check these: -framework GLU
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(TARGET)

main.o: src/main.cpp inc/simplespace.h inc/planet.h inc/physics.h inc/timer.h inc/controls.h inc/mouse.h
	$(CC) $(CFLAGS) src/main.cpp

simplespace.o: src/simplespace.cpp inc/simplespace.h
	$(CC) $(CFLAGS) src/simplespace.cpp
	
planet.o: src/planet.cpp inc/planet.h
	$(CC) $(CFLAGS) src/planet.cpp

physics.o: src/physics.cpp inc/physics.h
	$(CC) $(CFLAGS) src/physics.cpp

timer.o: src/timer.cpp inc/timer.h
	$(CC) $(CFLAGS) src/timer.cpp

controls.o : src/controls.cpp inc/controls.h
	$(CC) $(CFLAGS) src/controls.cpp

mouse.o : src/mouse.cpp inc/mouse.h
	$(CC) $(CFLAGS) src/mouse.cpp

clean:
	rm -rf $(TARGET) *.o
