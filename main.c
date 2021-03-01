#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "xmod_utils.h"

#define PROGRAM_NAME "xmod"
#define ARG_NO 2

int main(int argc, char **argv, char **envp) {
    if (argc < ARG_NO + 1) {
        printf("Incorrect arguments!\n");
        exit(EXIT_FAILURE);
    }

    mode_t old_mode, new_mode;
    struct stat path_stat;
    char *path = argv[argc - 1];

    //  load current path status into path_stat
    if (stat(path, &path_stat)) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    //  store current path permission mode
    old_mode = path_stat.st_mode;

    if (sscanf(argv[argc - 2], "%o", &new_mode) != 1 && get_mode_from_string(argv[argc - 2], &new_mode, old_mode)) {
        exit(EXIT_FAILURE);
    }

    if (chmod(argv[2], new_mode)) {
        perror("chmod");
        exit(EXIT_FAILURE);
    }
    /*
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
    */
    exit(EXIT_SUCCESS);
}