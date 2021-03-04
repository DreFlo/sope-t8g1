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
    char* file_path = (char*) malloc(strlen(path) + 4*sizeof(char));

    switch(is_directory(path)){
        case -1:;
            for(size_t i = strlen(path) - 1; i >= 0; i--){
                if(path[i] == '/'){
                    memmove(folder_path, path, i + 1);
                    break;
                }
            }
            if(is_directory(folder_path) == 1){
                break;
            }
            else{
                return -1;
            }
            break;
        case 0:;
            break;
        default:;
            char* file_name = (char*) malloc(strlen(path));
            file_name = "logs";
            if(path[strlen(path) - 1] != '/'){
                char* add = "/";
                strcat(path, add);
            }
            strcat(path, file_name);
    }

    int file = open(path, O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    printf("FILE: %d\n", file);
    close(file);

    return 0;
}

int write_exec_register(int argc, ...){
    va_list args;
    va_start(args, argc);

    char* path = va_arg(args, char*);
    enum event ev = va_arg(args, enum event);
    clock_t begin = va_arg(args, clock_t);

    pid_t pid = getpid();
    char *event_s, *info;
    switch (ev)
    {
        case PROC_CREAT:
            event_s = "PROC_CREAT";
            info = va_arg(args, char*);
            break;
        case PROC_EXIT:
            event_s = "PROC_EXIT";
            break;
        case SIGNAL_RECV:
            event_s = "SIGNAL_RECV";
            break;
        case SIGNAL_SENT:
            event_s = "SIGNAL_SENT";
            break;
        case FILE_MODF:
            event_s = "FILE_MODF";
            info = va_arg(args, char*);
            break;
    }
    char *pid_s = (char*) malloc(sizeof(int));
    sprintf(pid_s, "%d", pid);

    char *ms = "ms";
    char *instant_s = (char*) malloc(sizeof(double) + strlen(ms));
    double instant = (double)(clock() - begin) / 1000;
    sprintf(instant_s, "%f", instant);
    strncat(instant_s, ms, strlen(ms));
    
    char *between = " ; ";

    int file = open(path, O_WRONLY);

    write(file, instant_s, strlen(instant_s));
    write(file, between, strlen(between));
    write(file, pid_s, strlen(pid_s));
    write(file, between, strlen(between));
    write(file, event_s, strlen(event_s));
    write(file, between, strlen(between));
    write(file, info, strlen(info));
    write(file, "\n", strlen("\n"));
    
    close(file);
    
    va_end(args);
    return 0;
}

/**int write_exec_register(char *path, enum event ev, clock_t begin(), char **argv){
    FILE *fp = fopen(path, "a");
    char* line = "";
    pid_t pid = getpid();
    switch(ev){
        case PROC_CREAT:
            break;
        case PROC_EXIT:
            break;
        case SIGNAL_RECV:
            break;
        case SIGNAL_SENT:
            break;
        case FILE_MODF:
            break;
    }
    return 0;
}*/