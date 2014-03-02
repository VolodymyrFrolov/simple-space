target = test_logs

wrappers_dir = ../../wrappers
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
CFLAGS = -g -Wall -MMD -MP -D"LOG_LEVEL=$(LOG_LEVEL)" -D"LOG_ENTER_EXIT=$(LOG_ENTER_EXIT)" -I$(inc_dir) -I$(wrappers_dir)
LFLAGS =

files_c = $(wildcard $(src_dir)/*.c)
objects = $(patsubst $(src_dir)/%.c, $(obj_dir)/%.o, $(files_c))
objects += $(obj_dir)/wrp_mutex.o $(obj_dir)/test_logs.o

.PHONY: all
all: create_folders $(target)

$(target): $(objects) 
	@echo "Linking"
	$(CC_C) $(objects) -o $(bin_dir)/$@ $(LFLAGS)

-include $(objects:%.o=%.d)

$(obj_dir)/%.o: $(src_dir)/%.c
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@

$(obj_dir)/%.o: $(wrappers_dir)/%.c
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@

$(obj_dir)/test_logs.o: ./test_logs.c
	@echo "Compiling $<"
	$(CC_C) -c $(CFLAGS) $< -o $@

create_folders:
	mkdir -p $(obj_dir) $(bin_dir)

.PHONY: clean
clean:
	rm -rf $(obj_dir) $(bin_dir)

