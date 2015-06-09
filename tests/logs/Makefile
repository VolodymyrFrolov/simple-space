
# Target
TARGET := test_logs

# Directories
ROOT_DIR := ../..

ROOT_SRC_DIR      := $(ROOT_DIR)/src
LOGS_SRC_DIR      := $(ROOT_SRC_DIR)/logs
WRP_SRC_DIR       := $(ROOT_SRC_DIR)/wrappers
TEST_SRC_DIR      := $(ROOT_DIR)/tests
TEST_LOGS_SRC_DIR := $(TEST_SRC_DIR)/logs

ROOT_INC_DIR := $(ROOT_DIR)/inc
LOGS_INC_DIR := $(ROOT_INC_DIR)/logs
WRP_INC_DIR  := $(ROOT_INC_DIR)/wrappers

OBJ_DIR = $(ROOT_DIR)/obj
BIN_DIR = $(ROOT_DIR)/bin

# Sources
SOURCES := $(TEST_LOGS_SRC_DIR)/test_logs.c \
           $(LOGS_SRC_DIR)/logs.c

# Objects
OBJECTS_NOTDIR := $(patsubst %.c, %.o, $(notdir $(filter %.c, $(SOURCES))))
OBJECTS := $(addprefix $(OBJ_DIR)/, $(OBJECTS_NOTDIR))

#Includes
INCLUDES := -I$(WRP_INC_DIR)  \
            -I$(LOGS_INC_DIR)

# Verbosity (use "V=1" for verbose output)
ifdef V
Q :=
else
Q := @
endif

# Logging
ifndef LOGLEVEL
LOGLEVEL = 2
endif

# Compiler
CC = gcc

# Common flags
# Add  for release version need to add -DNDEBUG -Wno-unused-variable
CFLAGS := -g -O0 -c -Wall -Wextra -Werror -D"LOGLEVEL=$(LOGLEVEL)"
LFLAGS :=
LIBS   :=

# Platform specific flags
ifeq ($(OS), Windows_NT)
    # Windows
    # Empty
else
    LIBS += -lpthread
    UNAME_S := $(firstword $(shell uname -s))
    ifeq ($(UNAME_S), Linux)
        # Linux
    endif
    ifeq ($(UNAME_S), Darwin)
        # MacOS
    endif
endif

VPATH = $(BIN_DIR)
vpath %.c   $(TEST_LOGS_SRC_DIR) $(WRP_SRC_DIR) $(LOGS_SRC_DIR)
vpath %.h   $(WRP_INC_DIR) $(LOGS_INC_DIR)
vpath %.o   $(OBJ_DIR)

.PHONY: all
all: create_folders $(TARGET)

$(TARGET): $(OBJECTS_NOTDIR) 
	@echo "Linking target: $@"
	$(Q)$(CC) $(LFLAGS) $(OBJECTS) $(LIBS) -o $(BIN_DIR)/$@

%.o: %.c
	@echo "Compiling: $(notdir $<)"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) $< -o $(OBJ_DIR)/$@

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
