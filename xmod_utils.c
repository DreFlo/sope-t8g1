#include "xmod_utils.h"

mode_t permission(enum file_class c, enum file_permission p) {
    return 1 << ((3 - p) + (2 - c) * 3);
}

bool check_permission(mode_t m, enum file_class c, enum file_permission p) {
    return m & permission(c, p);
}

mode_t set_permission(mode_t m, enum file_class c, enum file_permission p) {
    return m | permission(c, p);
}

int is_directory(const char * path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) return -1;
    return S_ISDIR(path_stat.st_mode);
}
