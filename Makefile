CC = g++
CFLAGS = -c -Wall -Iinc
LDFLAGS =
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
	$(CC) $(LDFLAGS) -o $(TARGET) main.o physics.o simplespace.o planet.o $(LDLIBS)

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp

simplespace.o: src/simplespace.cpp
	$(CC) $(CFLAGS) src/simplespace.cpp
	
planet.o: src/planet.cpp
	$(CC) $(CFLAGS) src/planet.cpp

physics.o: src/physics.cpp
	$(CC) $(CFLAGS) src/physics.cpp

clean:
	rm -rf $(TARGET) *.o
