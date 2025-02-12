INCLUDES = $(wildcard include/*.h)

all: cdctr

cdctr: main.c file_pro.c thread_c.c include/file_counter.h include/settings.h include/thread_control.h
	gcc -o $@ main.c file_pro.c thread_c.c
