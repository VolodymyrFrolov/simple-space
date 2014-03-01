prog_name = wrp_test_app

src_dir = .
inc_dir = .
obj_dir = obj
bin_dir = bin

CC_C = gcc
CC_CPP = g++
CFLAGS = -g -Wall -MD -I$(inc_dir)
ifneq ($(OS),Windows_NT)
LFLAGS = -lpthread -ldl
endif

files_c := $(wildcard $(src_dir)/*.c)
files_cpp += $(wildcard $(src_dir)/*.cpp)
objects := $(patsubst $(src_dir)/%.c, $(obj_dir)/%.o, $(files_c))
objects += $(patsubst $(src_dir)/%.cpp, $(obj_dir)/%.o, $(files_cpp))
target := $(bin_dir)/$(prog_name)

.PHONY : all
all : create_folders $(target)

$(target) : $(objects) 
	@echo "Linking"
	$(CC_C) $(objects) -o $@ $(LFLAGS)
	@echo

$(obj_dir)/%.o : $(src_dir)/%.c
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@
	@echo

$(obj_dir)/%.o : $(src_dir)/%.cpp
	@echo "Compiling $<"
	$(CC_CPP) -c $(CFLAGS) $< -o $@
	@echo

create_folders :
	@echo "Creating folders"
	mkdir -p $(obj_dir) $(bin_dir)
	@echo

.PHONY : clean
clean :
	@echo "Removing all"
	rm -rf $(obj_dir) $(bin_dir)
	@echo