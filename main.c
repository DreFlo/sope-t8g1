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

bool main_proc;        /* Is the main process */
pid_t proc_id;         /* Proccess id */
char *proc_start_path; /* Path the process was started with */
unsigned int nftot;    /* Total number of files found */
unsigned int nfmod;    /* Total number of files modified */

pid_t children[256]; /* Array containing the pids of all the children of the process */
int child_no;        /* Number of child processes */

char *log_path;    /* Logfile path */
clock_t begin;     /* beggining time of the program */
bool log_filename; /* Is logfile defined or not */

int s_argc;    /* Number of command line arguments */
char **s_argv; /* Pointer to command line arguments */
char **s_envp; /* Pointer to environment variables */

char msg[1024]; /* Message to be printed on the screen when interrupted */

sem_t *semaphore; /* Pointer to sempaphore for writing registers synchronization */

int main(int argc, char **argv, char **envp)
{
    struct sigaction new; /* sigaction struct for signal beahviour */
    sigset_t smask;       /* smask for signal behaviour */

    semaphore = sem_open("/semaphore", O_CREAT, 0777, 1);

    // Set sig handlers

    // SIGINT
    if (sigemptyset(&smask) == -1)
        perror("sigsetfunctions()");
    new.sa_handler = sigint_handler;
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (sigaction(SIGINT, &new, NULL) == -1)
        perror("sigaction");

    // SIGQUIT
    if (sigemptyset(&smask) == -1)
        perror("sigsetfunctions()");
    new.sa_handler = sigquit_handler;
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (sigaction(SIGQUIT, &new, NULL) == -1)
        perror("sigaction");

    // SIGCONT
    if (sigemptyset(&smask) == -1)
        perror("sigsetfunctions()");
    new.sa_handler = sigcont_handler;
    new.sa_mask = smask;
    new.sa_flags = 0;
    if (sigaction(SIGCONT, &new, NULL) == -1)
        perror("sigaction");

    //--------------------

    // Determine if it is main process by comparing process and group IDs
    main_proc = getpid() == getpgid(getpid());

    // Store beginning time of program
    begin = clock();

    // Allocate memory
    char *path = malloc(1024); /* Path specified in command line arguments */
    proc_start_path = malloc(1024);

    memcpy(path, argv[argc - 1], strlen(argv[argc - 1]) + 1);
    memcpy(proc_start_path, path, strlen(path) + 1);

    // Store main() arguments statically
    s_argc = argc;
    s_argv = argv;
    s_envp = envp;

    // Find envp to generate and store records
    if ((log_path = getenv("LOG_FILENAME")) != NULL)
    {   
        if (main_proc)
        {
            if (start_log_file() != 0)
            {
                printf("Incorrect path in LOG_FILENAME envp!\n");
                exit_plus(EXIT_FAILURE);
            }
        }
        log_filename = true;
    }

    // Write PROC_CREAT event
    if (log_filename)
    {
        char *arg = (char *)malloc(argc * strlen(argv[argc - 1]));
        char *space = " ";
        for (int i = 0; i < argc; i++)
        {
            strncat(arg, argv[i], strlen(argv[i]));
            strncat(arg, space, strlen(space));
        }
        write_exec_register(2, PROC_CREAT, arg);
    }

    if (argc < ARG_NO + 1)
    {
        printf("Incorrect arguments!\n");
        exit_plus(EXIT_FAILURE);
    }

    proc_id = getpid();
    nfmod = 0;
    nftot = 0;
    child_no = 0;

    mode_t old_mode, new_mode;            /* File permission info struct */
    struct stat path_stat;                /* Initial status of the argument path */
    flag_t flags = {false, false, false}; /* Command line options flags */

    // Load current path status into path_stat
    if (stat(path, &path_stat))
    {
        perror("stat");
        exit_plus(EXIT_FAILURE);
    }

    // Store current path permission mode
    old_mode = path_stat.st_mode % EXTRA_MODE_INFO;

    // Set all new_mode bits to 0
    memset(&new_mode, 0, sizeof(mode_t));

    // Store new mode specified by command line arguments (either OCTAL-MODE or MODE)
    if ((!check_octal_mode_format(argv[argc - 2]) || sscanf(argv[argc - 2], "%o", &new_mode) != 1) && get_mode_from_string(argv[argc - 2], &new_mode, old_mode))
    {
        printf("Error mode\n");
        exit_plus(EXIT_FAILURE);
    }

    // Set flags from command line options
    for (int i = 1; i < argc - 2; i++)
    {
        char str[2];
        sscanf(argv[i], "%s", str);
        if (strcmp(str, "-v") == 0)
            flags.v = true;
        else if (strcmp(str, "-c") == 0)
            flags.c = true;
        else if (strcmp(str, "-R") == 0)
            flags.r = true;
        else
        {
            printf("%s is not a specified argument\n", str);
            exit_plus(EXIT_FAILURE);
        }
    }

    // If -R and is directory do recursively, else change mode
    if (flags.r && S_ISDIR(path_stat.st_mode))
    {
        // change path permissions
        xmod(path, new_mode, old_mode, flags);
        DIR *dir;
        if (path[strlen(path) - 1] != '/')
            strncat(path, "/", 2);
        if ((dir = opendir(path)) != NULL)
            recursive_xmod(path, dir, new_mode, old_mode, flags);
        else
            perror("xmod");
    }
    else
    {
        xmod(path, new_mode, old_mode, flags);
    }

    free(path);
    free(proc_start_path);

    exit_plus(EXIT_SUCCESS);
}