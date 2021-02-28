#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define PROGRAM_NAME "xmod"
#define ARG_NO 1

int is_directory(const char * path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) return -1;
    return S_ISDIR(path_stat.st_mode);
}

int main(int argc, char** argv) {
    if (argc != ARG_NO + 1) {
        printf("Incorrect arguments!\n");
        exit(EXIT_FAILURE);
    }

    switch (is_directory(argv[1])) {
        case -1:
            printf("Not a valid file or directory path\n");
            break;
        case 0:
            printf("-\n");
            break;
        default:
            printf("d\n");
            break;
    }

    

    exit(EXIT_SUCCESS);
}