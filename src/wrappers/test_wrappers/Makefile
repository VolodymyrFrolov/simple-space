target = test_wrappers

src_dir = ..
inc_dir = ..
obj_dir = ./obj
bin_dir = ./bin

ifndef LOG_LEVEL
LOG_LEVEL = 2
endif

ifndef LOG_ENTER_EXIT
LOG_ENTER_EXIT = 0
endif

CC_C = gcc
CC_CPP = g++
CFLAGS = -g -Wall -MMD -MP -I$(inc_dir)

ifeq ($(OS), Windows_NT)
@echo "Compiling for Windows"
else
LFLAGS = -lpthread -ldl
endif

files_c = $(wildcard $(src_dir)/*.c)
files_cpp = $(wildcard $(src_dir)/*.cpp)
objects = $(patsubst $(src_dir)/%.c, $(obj_dir)/%.o, $(files_c))
objects += $(patsubst $(src_dir)/%.cpp, $(obj_dir)/%.o, $(files_cpp))
objects += $(obj_dir)/test_wrappers.o

.PHONY: all
all: create_folders $(target)

$(target): $(objects) 
	@echo "Linking"
	$(CC_C) $(objects) -o $(bin_dir)/$@ $(LFLAGS)

-include $(objects:%.o=%.d)

$(obj_dir)/%.o: $(src_dir)/%.c
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@

$(obj_dir)/%.o: $(src_dir)/%.cpp
	@echo "Compiling $<"
	$(CC_CPP) -c $(CFLAGS) $< -o $@

$(obj_dir)/test_wrappers.o: ./test_wrappers.c	
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@

create_folders:
	mkdir -p $(obj_dir) $(bin_dir)

.PHONY: clean
clean:
	rm -rf $(obj_dir) $(bin_dir)

