
# Target
TARGET := test_wrappers

# Directories
ROOT_DIR := ../..

ROOT_SRC_DIR      := $(ROOT_DIR)/src
WRP_SRC_DIR       := $(ROOT_SRC_DIR)/wrappers
TEST_SRC_DIR      := $(ROOT_DIR)/tests
TEST_WRP_SRC_DIR  := $(TEST_SRC_DIR)/wrappers

ROOT_INC_DIR := $(ROOT_DIR)/inc
WRP_INC_DIR  := $(ROOT_INC_DIR)/wrappers

OBJ_DIR = $(ROOT_DIR)/obj
BIN_DIR = $(ROOT_DIR)/bin

# Sources
SOURCES :=  $(TEST_WRP_SRC_DIR)/test_wrappers.cpp \
            $(WRP_SRC_DIR)/osWrappers.c           \
            $(WRP_SRC_DIR)/Timer.cpp              \
            $(WRP_SRC_DIR)/Stopwatch.cpp          \
            $(WRP_SRC_DIR)/FpsCounter.cpp


# Objects
OBJECTS_NOTDIR := $(patsubst %.c,   %.o, $(notdir $(filter %.c,   $(SOURCES))))
OBJECTS_NOTDIR += $(patsubst %.cpp, %.o, $(notdir $(filter %.cpp, $(SOURCES))))
OBJECTS := $(addprefix $(OBJ_DIR)/, $(OBJECTS_NOTDIR))

#Includes
INCLUDES := -I$(WRP_INC_DIR)

# Verbosity (use "V=1" for verbose output)
ifdef V
Q :=
else
Q := @
endif

# Compiler
CC_C = gcc
CC_CPP = g++

# Common flags
# Add  for release version need to add -DNDEBUG -Wno-unused-variable
CFLAGS := -g -O0 -c -Wall -Wextra -Werror
CPPSTD := -std=c++11
LFLAGS :=
LIBS   :=

# Platform specific flags
ifeq ($(OS), Windows_NT)
    # Windows
    # Empty
else
    LIBS += -lpthread -ldl
    UNAME_S := $(firstword $(shell uname -s))
    ifeq ($(UNAME_S), Linux)
        # Linux
    endif
    ifeq ($(UNAME_S), Darwin)
        # MacOS
    endif
endif

VPATH = $(BIN_DIR)
vpath %.c   $(TEST_WRP_SRC_DIR) $(WRP_SRC_DIR)
vpath %.cpp $(WRP_SRC_DIR)
vpath %.h   $(WRP_INC_DIR)
vpath %.o   $(OBJ_DIR)

.PHONY: all debug release
all: create_folders $(TARGET)

$(TARGET): $(OBJECTS_NOTDIR)
	@echo "Linking target: $@"
	$(Q)$(CC_CPP) $(LFLAGS) $(OBJECTS) $(LIBS) -o $(BIN_DIR)/$@

%.o: %.c
	@echo "Compiling: $(notdir $<)"
	$(Q)$(CC_C) $(CFLAGS) $(INCLUDES) $< -o $(OBJ_DIR)/$@

%.o: %.cpp
	@echo "Compiling: $(notdir $<)"
	$(Q)$(CC_CPP) $(CFLAGS) $(CPPSTD) $(INCLUDES) $< -o $(OBJ_DIR)/$@

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
