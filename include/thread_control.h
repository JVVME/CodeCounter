#pragma once
#include <pthread.h>
#include <stdio.h>
#include "file_counter.h"

extern sem_t work_lock;

void* pasrse_thread(void* arg);

void* counter_thread(void* arg);

void pcenter(char const *path);