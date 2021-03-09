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
#include <signal.h>
#include "xmod_utils.h"
#include "xmod_sig_handlers.h"
#include "xmod_macros.h"

bool main_proc;                                             /* Is the main process */
pid_t proc_id;                                              /* Proccess id */
char * proc_start_path;                                     /* Path the process was started with */
unsigned int nftot;                                         /* Total number of files found */
unsigned int nfmod;                                         /* Total number of files modified */

pid_t children[128];                                        /* Array containing the pids of all the children of the process */
int child_no;                                               /* Number of child processes */

char *log_path;                                             /* Logfile path */
clock_t begin;                                              /* beggining time of the program */
bool log_filename;                                          /* Is logfile defined or not */

/**
 * @brief Struct with user settable flags
 */
typedef struct {
    bool v;                                                 /* Verbose */ 
    bool c;                                                 /* Changes */
    bool r;                                                 /* Recursive */
}flag_t, * flag_p;

int main(int argc, char **argv, char **envp) {
    begin = clock();
    
    // Find envp to generate and store records
    for (int i = 0; envp[i] != NULL; i++){
        if(strstr(envp[i], "LOG_FILENAME") != NULL){
            log_path = envp[i];
            if (start_log_file() != 0){
                printf("Incorrect path in LOG_FILENAME envp!\n");
                exit(EXIT_FAILURE);
            }
            log_filename = true;
            break;
        }
    }

    // Write PROC_CREAT event
    if (log_filename){
        char *arg = (char*) malloc (argc * strlen(argv[argc - 1]));
        char *space = " ";
        for (int i = 0; i < argc; i++){
            strncat(arg, argv[i], strlen(argv[i]));
            strncat(arg, space, strlen(space));
        }
        write_exec_register(2, PROC_CREAT, arg);
    }

    struct sigaction new;                                   /* sigaction struct for signal beahviour */
    sigset_t smask;                                         /* smask for signal behaviour */

    // Set sig handlers

    // SIGINT
    if (sigemptyset(&smask) == -1) perror("sigsetfunctions()");
    new.sa_handler = sigint_handler;
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (sigaction(SIGINT, &new, NULL) == -1) perror("sigaction");

    // SIGQUIT
    if (sigemptyset(&smask) == -1) perror("sigsetfunctions()");
    new.sa_handler = sigquit_handler;
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (sigaction(SIGQUIT, &new, NULL) == -1) perror("sigaction");

    // SIGCONT
    if (sigemptyset(&smask) == -1) perror("sigsetfunctions()");
    new.sa_handler = sigcont_handler;
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (sigaction(SIGCONT, &new, NULL) == -1) perror("sigaction");

    //--------------------

    main_proc = true;
    proc_id = getpid();
    proc_start_path = argv[argc - 1];
    nfmod = 0;
    nftot = 0;

    if (argc < ARG_NO + 1) {
        printf("Incorrect arguments!\n");
        exit(EXIT_FAILURE);
    }

    mode_t old_mode, new_mode;                              /* File permission info struct */
    struct stat path_stat;                                  /* Initial status of the argument path */
    char *path = argv[argc - 1];                            /* Path specified in command line arguments */
    flag_t flags = {false, false, false};                   /* Command line options flags */

    // Load current path status into path_stat
    if (stat(path, &path_stat)) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // Store current path permission mode
    old_mode = path_stat.st_mode % EXTRA_MODE_INFO;

    // Set all new_mode bits to 0
    memset(&new_mode, 0, sizeof(mode_t));

    // Store new mode specified by command line arguments (either OCTAL-MODE or MODE) 
    if (sscanf(argv[argc - 2], "%o", &new_mode) != 1 && get_mode_from_string(argv[argc - 2], &new_mode, old_mode)) {
        printf("Error mode\n");
        exit(EXIT_FAILURE);
    }

    // Set flags from command line options
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

    // For tests with more than one process
    /*
    int id = fork();

    switch (id)
    {
    case -1:
        exit(1);
    case 0:
        main_proc = false;
        proc_id = getpid();
        proc_start_path = "Child";
        nfmod = 0;
        nftot = 0;
        pause();
        exit(0);
    default:
        raise(SIGINT);
        break;
    }
    */

    // Change permissions
    if (chmod(path, new_mode)) {
        perror("chmod");
        exit(EXIT_FAILURE);
    }

    // Writes FILE_MODF register
    if(log_filename) write_exec_register(3, FILE_MODF, old_mode, new_mode);

    // Get mode strings
    char buf1[10], buf2[10];
    str_mode(old_mode, buf1);
    str_mode(new_mode, buf2);
    
    // Print messages if flags are set

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

    // Writes successful PROC_EXIT register
    if(log_filename) write_exec_register(2, PROC_EXIT, EXIT_SUCCESS);

    // Wait for all children to terminate
    wait(NULL);
    
    return 0;
}