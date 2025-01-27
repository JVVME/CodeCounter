INCLUDES = $(wildcard include/*.h)

all: cdctr

cdctr: main.c file_pro.c thread_c.c $(INCLUDES)
	gcc -o $@ main.c file_pro.c thread_c.c
