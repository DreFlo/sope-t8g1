#include "xmod_utils.h"

mode_t bitmask_permission(enum file_class c, enum file_permission p) {
    return 1 << ((2 - p) + (2 - c) * 3);
}

bool check_permission(mode_t m, enum file_class c, enum file_permission p) {
    return m & bitmask_permission(c, p);
}

mode_t add_permission(mode_t m, enum file_class c, enum file_permission p) {
    return m | bitmask_permission(c, p);
}

mode_t subtract_permission(mode_t m, enum file_class c, enum file_permission p) {
    return m & ~bitmask_permission(c, p);
}

int is_directory(const char * path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) return -1;
    return S_ISDIR(path_stat.st_mode);
}

int get_mode_from_string(const char * rx, mode_t * new_mode, const mode_t old_mode) {
    
    // Check for users and permissions
    UsersAccess ua = {false, false, false};
    Permissions perm = {false, false, false};
    
    if (strlen(rx) < 3 || strlen(rx) > 5) return 1;

    // Check target
    switch (rx[0]) {
        case 'a':
            ua.owner = true;
            ua.group = true;
            ua.other = true;
            break;
        case 'u':
            ua.owner = true;
            break;
        case 'g':
            ua.group = true;
            break;
        case 'o':
            ua.other = true;
            break;
        default:
            return 1;
    }

    // Operation
    char op = rx[1];

    // Check for permissions 
    for(int j = 2; rx[j] != '\0'; j++) {
        // Set permissions
        switch (rx[j]) {
            case 'r':
                if (perm.read == true) return 1;
                perm.read = true;
                break;
            case 'w':
                if (perm.write == true) return 1;
                perm.write = true;
                break;
            case 'x':
                if (perm.execute == true) return 1;
                perm.execute = true;
                break;
            default:
                return 1;
        }
    }
    

    // Change the new_mode
    switch(op){
        case '+':
            *new_mode = old_mode;

            if(ua.owner) {
                if(perm.read) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_READ);
                if(perm.write) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_WRITE);
                if(perm.execute)
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_EXEC);
            }
            if(ua.group) {
                if(perm.read) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_READ);
                if(perm.write) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_WRITE);
                if(perm.execute)
                    *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_EXEC);
            }
            if(ua.other) {
                if(perm.read) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_READ);
                if(perm.write) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_WRITE);
                if(perm.execute)
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_EXEC);
            }
            break;

        case '-':
            *new_mode = old_mode;

            if(ua.owner) {
                if(perm.read) 
                    *new_mode = subtract_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_READ);
                if(perm.write) 
                    *new_mode = subtract_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_WRITE);
                if(perm.execute)
                    *new_mode = subtract_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_EXEC);
            }
            if(ua.group) {
                if(perm.read) 
                    *new_mode = subtract_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_READ);
                if(perm.write) 
                    *new_mode = subtract_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_WRITE);
                if(perm.execute)
                    *new_mode = subtract_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_EXEC);
            }
            if(ua.other) {
                if(perm.read) 
                    *new_mode = subtract_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_READ);
                if(perm.write) 
                    *new_mode = subtract_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_WRITE);
                if(perm.execute)
                    *new_mode = subtract_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_EXEC);
            }
            break;

        case '=':
            if(ua.owner) {
                if(perm.read) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_READ);
                if(perm.write) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_WRITE);
                if(perm.execute)
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, FILE_PERMISSION_EXEC);
            } else {
                for(int i = FILE_PERMISSION_READ; i <= FILE_PERMISSION_EXEC; i++) 
                    if(check_permission(old_mode, FILE_CLASS_OWNER, i))
                        *new_mode = add_permission(*new_mode, FILE_CLASS_OWNER, i);
            }
            if(ua.group) {
                if(perm.read) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_READ);
                if(perm.write) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_WRITE);
                if(perm.execute)
                    *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, FILE_PERMISSION_EXEC);
            } else {
                for(int i = FILE_PERMISSION_READ; i <= FILE_PERMISSION_EXEC; i++) 
                    if(check_permission(old_mode, FILE_CLASS_GROUP, i))
                        *new_mode = add_permission(*new_mode, FILE_CLASS_GROUP, i);
            }
            if(ua.other) {
                if(perm.read) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_READ);
                if(perm.write) 
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_WRITE);
                if(perm.execute)
                    *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, FILE_PERMISSION_EXEC);
            } else {
                for(int i = FILE_PERMISSION_READ; i <= FILE_PERMISSION_EXEC; i++) 
                    if(check_permission(old_mode, FILE_CLASS_OTHER, i))
                        *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, i);
            }
            break;
        default:
            return 1;

    }

    return 0;
}

mode_t get_umask() {
    mode_t mask = umask(0000);
    umask(mask);
    return mask;
}

void str_mode(mode_t mode, char * buf) {
  const char permissions[] = "rwxrwxrwx";
  for (size_t i = 0; i < 9; i++) {
    buf[i] = (mode & (1 << (8-i))) ? permissions[i] : '-';
  }
  buf[9] = '\0';
}

int start_log_file(){
    for(size_t i = 0; i < strlen(log_path); i++){
        if(log_path[i] == '='){
            memmove(log_path, log_path + i + 1, strlen(log_path) - i);
            break;
        }
    }

    char* folder_path = (char*) malloc(strlen(log_path));
    char* file_path = (char*) malloc(strlen(log_path) + 4*sizeof(char));

    switch(is_directory(log_path)){
        case -1:;
            for(size_t i = strlen(log_path) - 1; i >= 0; i--){
                if(log_path[i] == '/'){
                    memmove(folder_path, log_path, i + 1);
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
            char* file_name = (char*) malloc(strlen(log_path));
            file_name = "logs";
            if(log_path[strlen(log_path) - 1] != '/'){
                char* add = "/";
                strcat(log_path, add);
            }
            strcat(log_path, file_name);
    }

    int file = open(log_path, O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    close(file);

    return 0;
}

int write_exec_register(int argc, ...){
    va_list args;
    va_start(args, argc);

    enum event ev = va_arg(args, enum event);

    pid_t pid = getpid();
    char *event_s, *info;
    char* bet = " : ";
    switch (ev)
    {
        case PROC_CREAT: //(..., char* cmd_args)
            event_s = "PROC_CREAT";
            info = va_arg(args, char*);
            break;
        case PROC_EXIT: //(..., int exit)
            event_s = "PROC_EXIT";
            sprintf(info, "%d", va_arg(args, int));
            break;
        case SIGNAL_RECV: //(..., char* signal)
            event_s = "SIGNAL_RECV";
            info = va_arg(args, char*);
            break;
        case SIGNAL_SENT: //(..., char*signal, pid_t pid)
            event_s = "SIGNAL_SENT";
            char* signal = va_arg(args, char*);
            pid_t target_pid = va_arg(args, pid_t);
            char* target_pid_s = (char*) malloc(sizeof(target_pid_s));
            sprintf(target_pid_s, "%i", target_pid);
            info = (char*) malloc(sizeof(signal) + sizeof(target_pid_s) + sizeof(bet));
            memmove(info, signal, strlen(signal));
            strncat(info, bet, strlen(bet));
            strncat(info, target_pid_s, strlen(target_pid_s));
            break;
        case FILE_MODF: //(..., mode_t old, mode_t new)
            event_s = "FILE_MODF";
            char *zero = "0";
            mode_t old = va_arg(args, mode_t);
            mode_t new = va_arg(args, mode_t);
            char* old_s = (char*) malloc(4 * sizeof(mode_t));
            char* new_s = (char*) malloc(4 * sizeof(mode_t));
            char* old_s1 = (char*) malloc(3 * sizeof(mode_t));
            char* new_s1 = (char*) malloc(3 * sizeof(mode_t));
            sprintf(old_s1, "%o", old);
            sprintf(new_s1, "%o", new);
            strncat(old_s, zero, strlen(zero));
            strncat(old_s, old_s1, strlen(old_s1));
            strncat(new_s, zero, strlen(zero));
            strncat(new_s, new_s1, strlen(new_s1));
            info = (char*) malloc(sizeof(char) * strlen(log_path) + 15*sizeof(char));
            memmove(info, log_path, strlen(log_path));
            strncat(info, bet, strlen(bet));
            strncat(info, old_s, strlen(old_s));
            strncat(info, bet, strlen(bet));
            strncat(info, new_s, strlen(new_s));
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

    int file = open(log_path, O_WRONLY | O_APPEND);

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

void register_new_child(pid_t pid) {
    children[child_no] = pid;
    child_no++;
}
