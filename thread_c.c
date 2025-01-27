#include "include/thread_control.h"

file_node flist;
counter ctr;

sem_t work_lock;


void* pasrse_thread(void* arg){
    char *path = (char *)arg;
    find_files(path, &flist);
}

void* counter_thread(void *arg){
    char *file;
    bool finished = false;
    while (true)
    {
        sem_wait(&work_lock);
        if(flist.work_at >= flist.count){
            finished = true;
        }
        else{
            file = flist.file_paths[flist.work_at];
            flist.work_at++;
        }
        sem_post(&work_lock);

        if(finished){
            break;
        }

        single_file_count(file, &ctr);
    }
    
    return NULL;

}

void pcenter(char const* path){
    pthread_t fp_parse;

    init_file_list(&flist, 10);
    init_counter(&ctr);
    sem_init(&work_lock, 0, 1);
    sem_init(&write_lock, 0, 1);

    pthread_create(&fp_parse, NULL, pasrse_thread, (void *)path);
    pthread_join(fp_parse, NULL);

    printf("-----------------------------------------------\n");
    printf("Total file  %8d \n", flist.count);
    printf("-----------------------------------------------\n");

    pthread_t counters[8];
    for(int i = 0; i < 8; i++){
        pthread_create(&counters[i], NULL, counter_thread, NULL);
    }
    for(int i = 0; i < 8; i++){
        pthread_join(counters[i], NULL);
    }
    char *sp, *bk, *ct, *ce, *tt, *sm;
    sp = "";
    bk = "Blank";
    ct = "Comment";
    ce = "Code";
    tt = "Total";
    sm = "Sum";
    printf("%3s %10s %10s %10s %10s\n",sp, bk, ct, ce, tt);
    printf("%-3s %10d %10d %10d %10d\n", sm,ctr.blank, ctr.comment, ctr.code, ctr.line);
    printf("-----------------------------------------------\n");
}