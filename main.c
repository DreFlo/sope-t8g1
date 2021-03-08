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
#define EXTRA_MODE_INFO 0100000

typedef struct {
    bool v;                                                 /* Verbose */ 
    bool c;                                                 /* Changes */
    bool r;                                                 /* Recursive */
}flag_t, * flag_p;

int main(int argc, char **argv, char **envp) {
    clock_t begin = clock();

    if (argc < ARG_NO + 1) {
        printf("Incorrect arguments!\n");
        exit(EXIT_FAILURE);
    }

    mode_t old_mode, new_mode;                              /* File permission info struct */
    struct stat path_stat;                                  /* Initial status of the argument path */
    char *path = argv[argc - 1];                            /* Path specified in command line arguments */
    flag_t flags = {false, false, false};                   /* Command line options flags */
    char *log_path;
    bool log_filename = false;

    //Find envp to generate and store records
    for (int i = 0; envp[i] != NULL; i++){
        if(strstr(envp[i], "LOG_FILENAME") != NULL){
            log_path = envp[i];
            if (start_log_file(log_path) != 0){
                printf("Incorrect path in LOG_FILENAME envp!\n");
                exit(EXIT_FAILURE);
            }
            log_filename = true;
            break;
        }
    }

    //Write PROC_CREAT event
    if (log_filename){
        char *arg = (char*) malloc (argc * strlen(argv[argc - 1]));
        char *space = " ";
        for (int i = 0; i < argc; i++){
            strncat(arg, argv[i], strlen(argv[i]));
            strncat(arg, space, strlen(space));
        }
        write_exec_register(4, log_path, PROC_CREAT, begin, arg);
    }

    //  load current path status into path_stat
    if (stat(path, &path_stat)) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    //  store current path permission mode
    old_mode = path_stat.st_mode % EXTRA_MODE_INFO;

    //set all new_mode bits to 0
    memset(&new_mode, 0, sizeof(mode_t));

    // store new mode specified by command line arguments (either OCTAL-MODE or MODE) 
    if (sscanf(argv[argc - 2], "%o", &new_mode) != 1 && get_mode_from_string(argv[argc - 2], &new_mode, old_mode)) {
        printf("Error mode\n");
        exit(EXIT_FAILURE);
    }

    // set flags from command line options
    for (int i = 1; i < argc - 2; i++) {
        char str[2];
        sscanf(argv[i], "%s", str);
        if (strcmp(str, "-v") == 0) flags.v = true;
        else if (strcmp(str, "-c") == 0) flags.c = true;
        else if (strcmp(str, "-R") == 0) flags.r = true;
        else {
            printf("%s is not a specified argument\n", str);
            exit(EXIT_FAILURE);
        }
    }

    // change permissions
    if (chmod(path, new_mode)) {
        perror("chmod");
        exit(EXIT_FAILURE);
    }

    char buf1[10], buf2[10];
    str_mode(old_mode, buf1);
    str_mode(new_mode, buf2);
    
    // print messages if flags are set

    if ((flags.v || flags.c) && new_mode != old_mode) {
        printf("mode of '%s' changed from %.4o ('%s') to %.4o ('%s')\n", path, old_mode, buf1, new_mode, buf2);
    }

    else if ((flags.v && !flags.c) && new_mode == old_mode) {
        printf("mode of '%s' retained as %.4o ('%s')\n", path, new_mode, buf1);
    }

    // -----------end message printing

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

    return 0;
}