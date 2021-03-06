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
    
    bool error = true;

    // Check if has an =, + or -
    for(int j = 0; rx[j] != '\0';j++) {
        if(rx[j] != '=' && rx[j] != '+' && rx[j] != '-') 
            continue;
        error = false;
        break;
    }
    if(error) return 1;

    // Check if =, + or - is the first char
    if(rx[0] == '=' || rx[0] == '+' || rx[0] == '-') {
        ua.owner = true;
        ua.group = true;
        ua.other = true;
    }

    // Check for other users
    int i = 0;
    for(; rx[i] != '=' && rx[i] != '+' && rx[i] != '-'; i++) {
        switch (rx[i]) {
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
    }

    char op = rx[i];    // Operation 

    // Check for permissions 
    for(int j = ++i; rx[j] != '\0'; j++) {
        switch (rx[j]) {
            case 'r':
                perm.read = true;
                break;
            case 'w':
                perm.write = true;
                break;
            case 'x':
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
                for(int i = FILE_PERMISSION_READ; i < FILE_PERMISSION_EXEC; i++) 
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
                for(int i = FILE_PERMISSION_READ; i < FILE_PERMISSION_EXEC; i++) 
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
                for(int i = FILE_PERMISSION_READ; i < FILE_PERMISSION_EXEC; i++) 
                    if(check_permission(old_mode, FILE_CLASS_OTHER, i))
                        *new_mode = add_permission(*new_mode, FILE_CLASS_OTHER, i);
            }
            break;
    }

    return 0;
}
