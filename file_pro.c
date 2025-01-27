#include "include/file_counter.h"

int cfile = 0;
int hfile = 0;
sem_t write_lock;

void single_file_count(char const *file_path, counter* ctr){
    FILE *file = fopen(file_path,"r");
    if (file == NULL){
        printf("Error Reading the file %s.", file_path);
        return ;
    }

    char buffer[256];
    int line = 0;
    int code = 0;
    int comment = 0;
    int blank = 0;
    bool large_flag = false;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        line++;
        bool begin = false;
        bool comment_flag = large_flag;
        bool blank_flag = false;
        bool code_flag = false;

        bool first = true;

        for(int i = 0; i < 256; i++){
            if(!begin && !isspace(buffer[i])){
                begin = true;
                if(buffer[i] == '\0'){
                    blank_flag = true;
                    break;
                }
            }

            if (begin){
                if (buffer[i] == 0) {
                    break;
                }
                if(buffer[i] == '/' && buffer[i + 1] == '*'){
                    large_flag = true;
                    comment_flag = true;
                    i++;
                }
                else if(buffer[i] == '/' && buffer[i + 1] == '/'){
                    comment_flag = true;    
                    break;
                }
                if(buffer[i] == '*' && buffer[i + 1] == '/'){
                    large_flag = false;
                    break;
                }

                if(first && !blank_flag && !comment_flag){
                    code_flag = true;
                }
                if(code_flag){
                    break;
                }
                first = false;
            }
            
        }
        if(code_flag){
            code++;
        }
        else if(blank_flag){
            blank++;
        }
        else{
            comment++;
        }

    }

    sem_wait(&write_lock);
    ctr -> code += code;
    ctr -> blank += blank;
    ctr -> comment += comment;
    ctr -> line += line;
    sem_post(&write_lock);

    fclose(file);

}


void find_files(const char* path, file_node *flist){
#if defined(_WIN32) || defined(_WIN64)
    WIN32_FIND_DATA find_file_data;
    HANDLE h_find;
    char search_path[MAX_PATH];
    
    // 构造搜索路径，寻找当前目录下的所有文件和子目录
    snprintf(search_path, MAX_PATH, "%s\\*", path);

    h_find = FindFirstFile(search_path, &find_file_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        printf("unable to open the dir, did you input a file? (%s)\n", path);
        return ;
    }

    do {
        // 忽略 "." 和 ".." 目录
        if (strcmp(find_file_data.cFileName, ".") == 0 || strcmp(find_file_data.cFileName, "..") == 0) {
            continue;
        }

        // 构造完整路径
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s\\%s", path, find_file_data.cFileName);

        // 如果是文件并且以 .c 或 .h 结尾
        if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            size_t len = strlen(find_file_data.cFileName);
            // 检查是否以 .c 或 .h 结尾
            if (len > 2 && (strcmp(find_file_data.cFileName + len - 2, ".c") == 0)) {
                add_file_to_list(flist, full_path);
                cfile++;
            }
            else if (len > 2 && (strcmp(find_file_data.cFileName + len - 2, ".h") == 0)) {
                add_file_to_list(flist, full_path);
                hfile++;
            }
        }
        // 如果是目录，递归查找
        else if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            find_files(full_path, flist);  // 递归调用，查找子目录
        }

    } while (FindNextFile(h_find, &find_file_data) != 0);

    FindClose(h_find);

#elif defined(__linux__)
    DIR *dir = opendir(path);
    if (!dir) {
        printf("Unable to open the dir, did you input a file? (%s)\n", path);
        return;
    }

    struct dirent *entry;
    struct stat file_stat;
    char full_path[MAX_PATH];

    while ((entry = readdir(dir)) != NULL) {
        // 忽略 "." 和 ".." 目录
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 构造完整路径
        snprintf(full_path, MAX_PATH, "%s/%s", path, entry->d_name);

        // 获取文件属性
        if (stat(full_path, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        // 如果是文件并且以 .c 或 .h 结尾
        if (S_ISREG(file_stat.st_mode)) {  // 检查是否是普通文件
            size_t len = strlen(entry->d_name);
            if (len > 2 && strcmp(entry->d_name + len - 2, ".c") == 0) {
                add_file_to_list(flist, full_path);
            } else if (len > 2 && strcmp(entry->d_name + len - 2, ".h") == 0) {
                add_file_to_list(flist, full_path);
            }
        }
        // 如果是目录，递归查找
        else if (S_ISDIR(file_stat.st_mode)) {  // 检查是否是目录
            find_files(full_path, flist);  // 递归调用，查找子目录
        }
    }

    closedir(dir);
#endif
}

void init_file_list(file_node *file_list, int capacity) {
    file_list->file_paths = (char **)malloc(sizeof(char *) * capacity);
    file_list->count = 0;
    file_list->capacity = capacity;
    file_list->work_at = 0;
}

void add_file_to_list(file_node *file_list, const char *file_path) {
    // 如果容量不够，扩展容量
    if (file_list->count >= file_list->capacity) {
        file_list->capacity *= 2;
        file_list->file_paths = (char **)realloc(file_list->file_paths, sizeof(char *) * file_list->capacity);
    }
    
    // 为新文件路径分配内存并复制路径
    file_list->file_paths[file_list->count] = (char *)malloc(strlen(file_path) + 1);
    strcpy(file_list->file_paths[file_list->count], file_path);
    file_list->count++;
}

void free_file_list(file_node *file_list) {
    for (int i = 0; i < file_list->count; i++) {
        free(file_list->file_paths[i]);
    }
    free(file_list->file_paths);
}

void init_counter(counter* ctr){
    ctr -> line = 0;
    ctr -> comment = 0;
    ctr -> blank = 0;
    ctr -> code = 0;
}
