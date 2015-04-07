
# Targets
MAIN_TARGET  := simplespace
TESTS_TARGET := tests

# Directories
ROOT_DIR := .

ROOT_SRC_DIR      := $(ROOT_DIR)/src
SS_SRC_DIR        := $(ROOT_SRC_DIR)/simplespace
WRP_SRC_DIR       := $(ROOT_SRC_DIR)/wrappers
LOGS_SRC_DIR      := $(ROOT_SRC_DIR)/logs
TEST_SRC_DIR      := $(ROOT_DIR)/tests
TEST_WRP_SRC_DIR  := $(TEST_SRC_DIR)/wrappers
TEST_LOGS_SRC_DIR := $(TEST_SRC_DIR)/logs

ROOT_INC_DIR := $(ROOT_DIR)/inc
SS_INC_DIR   := $(ROOT_INC_DIR)/simplespace
WRP_INC_DIR  := $(ROOT_INC_DIR)/wrappers
LOGS_INC_DIR := $(ROOT_INC_DIR)/logs
MISC_INC_DIR := $(ROOT_INC_DIR)/misc

OBJ_DIR = $(ROOT_DIR)/obj
BIN_DIR = $(ROOT_DIR)/bin

# Sources
SOURCES :=  $(ROOT_SRC_DIR)/main.cpp             \
            $(SS_SRC_DIR)/simplespace.cpp        \
            $(SS_SRC_DIR)/physics.cpp            \
            $(SS_SRC_DIR)/planet.cpp             \
            $(SS_SRC_DIR)/controls.cpp           \
            $(SS_SRC_DIR)/mouse_and_keyboard.cpp \
            $(WRP_SRC_DIR)/timer.cpp             \
            $(WRP_SRC_DIR)/wrp_mutex.c           \
            $(WRP_SRC_DIR)/wrp_thread.c          \
            $(WRP_SRC_DIR)/wrp_cond.c            \
            $(WRP_SRC_DIR)/wrp_common.c          \
            $(LOGS_SRC_DIR)/logs.c

# Objects
OBJECTS_NOTDIR := $(patsubst %.c,   %.o, $(notdir $(filter %.c,   $(SOURCES))))
OBJECTS_NOTDIR += $(patsubst %.cpp, %.o, $(notdir $(filter %.cpp, $(SOURCES))))
OBJECTS := $(addprefix $(OBJ_DIR)/, $(OBJECTS_NOTDIR))

#Includes
INCLUDES := -I$(SS_INC_DIR)     \
            -I$(WRP_INC_DIR)    \
            -I$(LOGS_INC_DIR)   \
            -I$(MISC_INC_DIR)

# Verbosity (use "V=1" for verbose output)
ifdef V
Q :=
else
Q := @
endif

# Logging
ifndef LOG_LEVEL
LOG_LEVEL = 2
endif

# Compiler
CC_C = gcc
CC_CPP = g++

# Common flags
CFLAGS := -g -c -Wall -D"LOG_LEVEL=$(LOG_LEVEL)"
LFLAGS :=
LIBS   :=
CPPSTD := -std=c++0x

# Platform specific flags
ifeq ($(OS), Windows_NT)
    # Windows
    # Empty
else
    LIBS += -lpthread
    UNAME_S := $(firstword $(shell uname -s))
    ifeq ($(UNAME_S), Linux)
        # Linux
        LIBS += -lGL -lglut
    endif
    ifeq ($(UNAME_S), Darwin)
        # MacOS
        CFLAGS += -I/opt/X11/include
        LFLAGS += -framework GLUT -framework OpenGL
    endif
endif

VPATH = $(BIN_DIR)
vpath %.c   $(WRP_SRC_DIR) $(LOGS_SRC_DIR)
vpath %.cpp $(ROOT_SRC_DIR) $(SS_SRC_DIR) $(WRP_SRC_DIR)
vpath %.h   $(SS_INC_DIR) $(WRP_INC_DIR) $(LOGS_INC_DIR) $(MISC_INC_DIR)
vpath %.o   $(OBJ_DIR)

.PHONY: all
all: create_folders $(MAIN_TARGET)

$(MAIN_TARGET): $(OBJECTS_NOTDIR)
	@echo "Linking target: $@"
	$(Q)$(CC_CPP) $(LFLAGS) $(OBJECTS) $(LIBS) -o $(BIN_DIR)/$@

%.o: %.c
	@echo "Compiling: $(notdir $<)"
	$(Q)$(CC_C) $(CFLAGS) $(INCLUDES) $< -o $(OBJ_DIR)/$@

%.o: %.cpp
	@echo "Compiling: $(notdir $<)"
	$(Q)$(CC_CPP) $(CFLAGS) $(CPPSTD) $(INCLUDES) $< -o $(OBJ_DIR)/$@

.PHONY: $(TESTS_TARGET)
$(TESTS_TARGET):
	@echo "Calling make in subfolder: $(TEST_WRP_SRC_DIR)"
	$(Q)@$(MAKE) -C $(TEST_WRP_SRC_DIR)
	@echo "Calling make in subfolder: $(TEST_LOGS_SRC_DIR)"
	$(Q)@$(MAKE) -C $(TEST_LOGS_SRC_DIR) LOG_LEVEL=$(LOG_LEVEL)

MAKE_DIR_P := mkdir -p

.PHONY: create_folders
create_folders: $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR):
	$(MAKE_DIR_P) $(OBJ_DIR)
	
$(BIN_DIR):
	$(MAKE_DIR_P) $(BIN_DIR)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

#
# TODO:
#
# Check for headers to be modified:
# try -MMD -MP flags and 
# -include $(OBJECTS:%.o=%.d)
# after linkage rule
#
# Check if those flags are needed:
# -stdlib=libc++
# -std=c++11
# -framework GLU
# -lglut -lGLU -lGL -L/usr/X11R6/lib/ -lXmu -lXi -lXext -lX11 -lXt
#
# Add and satisfy -Wextra & -pedantic
#
# VPATH may be used for vpath purpose (vpath looks to be better choice)
#
