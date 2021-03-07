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
