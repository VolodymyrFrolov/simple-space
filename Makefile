CC = g++
CFLAGS = -c -Wall -Iinc
LDFLAGS = -framework GLUT -framework OpenGL
TARGET=simple-space

all: $(TARGET)

$(TARGET): main.o simplespace.o planet.o physics.o
	$(CC) $(LDFLAGS) main.o physics.o simplespace.o planet.o -o simple-space

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp

simplespace.o: src/simplespace.cpp
	$(CC) $(CFLAGS) src/simplespace.cpp
	
planet.o: src/planet.cpp
	$(CC) $(CFLAGS) src/planet.cpp

physics.o: src/physics.cpp
	$(CC) $(CFLAGS) src/physics.cpp

clean:
	rm -rf simple-space *.o
