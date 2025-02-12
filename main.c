#include "include/thread_control.h"

bool recursion;

int main(int argc, char const *argv[])
{
    if(argc <= 1){
        printf("too few argments, use -h for help.\n");
        return -1;
    }

    bool single_file  = false;
    recursion = true;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0) {
            single_file = true;
        }
        if (strcmp(argv[i], "--norecursion") == 0) {
            recursion = false;
        }
    }
    
    char const *file_path = argv[1];
    double elapsed;

#if defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER frequency, start, end;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
#elif defined(__linux__)
    struct timeval start, end;
    long seconds, microseconds;
    gettimeofday(&start, NULL);
#endif

    if(single_file){
        counter ctr;
        init_counter(&ctr); 
        sem_init(&write_lock, 0, 1);
        single_file_count(file_path, &ctr);
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
    else{
        pcenter(file_path);  
    }

#if defined(_WIN32) || defined(_WIN64)
    QueryPerformanceCounter(&end);
    elapsed = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
#elif defined(__linux__)
    gettimeofday(&end, NULL);
    seconds = end.tv_sec - start.tv_sec;
    microseconds = end.tv_usec - start.tv_usec;
    elapsed = seconds + microseconds * 1e-6;
#endif

    printf("C file      %8d \nHeader file %8d \n", cfile, hfile);
    printf("-----------------------------------------------\n");
    printf("Function execution time: %.6f seconds\n", elapsed);
    printf("-----------------------------------------------\n");
    
    return 0;
}
