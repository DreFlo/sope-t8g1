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

int main(int argc, char** argv) {
    if (argc != ARG_NO + 1) {
        printf("Incorrect arguments!\n");
        exit(EXIT_FAILURE);
    }

    int mode;

    if (sscanf(argv[1], "%o", &mode) != 1) {
        perror("sscanf");
        exit(EXIT_FAILURE);
    }

    if (chmod(argv[2], mode)) {
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