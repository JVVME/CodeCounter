#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <semaphore.h>
#include "settings.h"

extern int cfile;
extern int hfile;

extern sem_t write_lock;

typedef struct counter{
    int line;
    int comment;
    int blank;
    int code;
} counter;

typedef struct file_node{
    char **file_paths; 
    int count;         
    int capacity;
    int work_at;
}file_node;



void single_file_count(char const *file_path, counter *ctr);

void find_files(const char *path, file_node *flist);

void init_file_list(file_node *file_list, int capacity);
void add_file_to_list(file_node *file_list, const char *file_path);
void free_file_list(file_node *file_list);

void init_counter(counter* ctr);
