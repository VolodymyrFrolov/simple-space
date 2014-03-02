TARGET = simplespace

SOURCES =   ./src/main.cpp                           \
            ./src/simplespace/simplespace.cpp        \
            ./src/simplespace/physics.cpp            \
            ./src/simplespace/planet.cpp             \
            ./src/simplespace/controls.cpp           \
            ./src/simplespace/mouse_and_keyboard.cpp \
            ./src/simplespace/timer.cpp              \
            ./src/wrappers/wrp_mutex.c               \
            ./src/wrappers/wrp_thread.c              \
            ./src/wrappers/wrp_cond.c                \
            ./src/wrappers/wrp_common.c              \
            ./src/logs/logs.c

OBJECTS_STRIPPED_NAMES += $(patsubst %.c,   %.o, $(notdir $(filter %.c,   $(SOURCES))))
OBJECTS_STRIPPED_NAMES += $(patsubst %.cpp, %.o, $(notdir $(filter %.cpp, $(SOURCES))))

MAIN_DIR =        ./src
SS_DIR = ./src/simplespace
WRP_DIR =    ./src/wrappers
LOGS_DIR =        ./src/logs

OBJ_DIR = ./obj
BIN_DIR = ./bin

OBJECTS = $(addprefix $(OBJ_DIR)/, $(OBJECTS_STRIPPED_NAMES))

CC_C = gcc
CC_CPP = g++

# Common for all platforms
CFLAGS = -g -Wall -MD -I$(SS_DIR) -I$(WRP_DIR) -I$(LOGS_DIR)
#Later replace -std=c++0x with -std=c++11, when use compiler version that supports it
CPP_SPECIFIC_FLAGS = -std=c++0x
LFLAGS =

# Platform specific
ifeq ($(OS), Windows_NT)
    @echo "Compiling for win: "$(OS)
else
    LFLAGS += -lpthread
    UNAME_S := $(firstword $(shell uname -s))
    ifeq ($(UNAME_S), Linux)
        LFLAGS += -lGL -lglut
        #Check these: -lglut -lGLU -lGL -L/usr/X11R6/lib/ -lXmu -lXi -lXext -lX11 -lXt
    endif
    ifeq ($(UNAME_S), Darwin)
        CFLAGS += -I/opt/X11/include -stdlib=libc++
        LFLAGS += -framework GLUT -framework OpenGL
        #Check these: -framework GLU
    endif
endif


.PHONY: all

all: create_folders $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking:"
	$(CC_CPP) $(OBJECTS) -o $@ $(LFLAGS)

create_folders:
	@echo "Creating folders"
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

.PHONY: clean
clean:
	@echo "Removing all"
	rm -rf $(OBJ_DIR) $(BIN_DIR)


$(OBJ_DIR)/%.o: $(MAIN_DIR)/%.cpp
	@echo "Compiling $<"
	$(CC_CPP) -c $(CFLAGS) $(CPP_SPECIFIC_FLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SS_DIR)/%.cpp
	@echo "Compiling $<"
	$(CC_CPP) -c $(CFLAGS) $(CPP_SPECIFIC_FLAGS) $< -o $@
	
$(OBJ_DIR)/%.o: $(WRP_DIR)/%.c
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(LOGS_DIR)/%.c
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@
