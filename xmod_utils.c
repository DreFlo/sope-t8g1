#include "xmod_utils.h"

mode_t bitmask_permission(enum file_class c, enum file_permission p) {
    return 1 << ((3 - p) + (2 - c) * 3);
}

bool check_permission(mode_t m, enum file_class c, enum file_permission p) {
    return m & bitmask_permission(c, p);
}

mode_t add_permission(mode_t m, enum file_class c, enum file_permission p) {
    return m | bitmask_permission(c, p);
}

int is_directory(const char * path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) return -1;
    return S_ISDIR(path_stat.st_mode);
}

int get_mode_from_string(const char * rx, mode_t * new_mode, const mode_t old_mode) {
    if (new_mode == NULL) return 1;
    return 0;
}


int start_log_file(char* path){
    for(size_t i = 0; i < strlen(path); i++){
        if(path[i] == '='){
            memmove(path, path + i + 1, strlen(path) - i);
            break;
        }
    }
    char* folder_path = (char*) malloc(strlen(path));

    switch(is_directory(path)){
        case -1:
            return -1;
        case 0:;
            char* txt = ".txt";
            strcat(path, txt);
            break;
        default:;
            char* file_name = (char*) malloc(strlen(path));
            file_name = "logs.txt";
            if(path[strlen(path) - 1] != '/'){
                char* add = "/";
                strcat(path, add);
            }
            strcat(path, file_name);
    }

    if(is_directory(folder_path) == -1){
        FILE *nf = fopen(path, "w");
        fclose(nf);
    }

    return 0;
}

int write_exec_register(char *path, enum event ev){
    //TO DO
    return 0;
}