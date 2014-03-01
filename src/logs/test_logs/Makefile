prog_name = test_logs_app

src_dir = .
inc_dir = .
obj_dir = obj
bin_dir = bin

ifndef LOG_LEVEL
LOG_LEVEL = 2
endif

ifndef LOG_ENTER_EXIT
LOG_ENTER_EXIT = 0
endif

CC_C = gcc
CFLAGS = -g -Wall -MD -D"LOG_LEVEL=$(LOG_LEVEL)" -D"LOG_ENTER_EXIT=$(LOG_ENTER_EXIT)" -I$(inc_dir) -I../wrappers
LFLAGS =

files_c := $(wildcard $(src_dir)/*.c)
objects := $(patsubst $(src_dir)/%.c, $(obj_dir)/%.o, $(files_c))
objects += $(obj_dir)/wrp_mutex.o
target := $(bin_dir)/$(prog_name)

.PHONY : all
all : create_folders $(target)

$(target) : $(objects) 
	@echo "Linking"
	$(CC_C) $(objects) $(precompiled_objects) -o $@ $(LFLAGS)
	@echo

$(obj_dir)/%.o : $(src_dir)/%.c
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@
	@echo

# External sources start
$(obj_dir)/%.o : ../wrappers/%.c
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@
	@echo
# External sources end

create_folders :
	@echo "Creating folders"
	mkdir -p $(obj_dir) $(bin_dir)
	@echo

.PHONY : clean
clean :
	@echo "Removing all"
	rm -rf $(obj_dir) $(bin_dir)
	@echo