#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

// Placeholder
unsigned int runtime = 0;
// Placeholder
char fifoname[256];

/**
 * @brief Prints the format of the command line arguments of the program and exits. 
 */
void print_usage() {
    printf("Usage: c <-t nsecs> fifoname\n\n");
    printf("nsecs - number of seconds (approx.) the program will run\n");
    printf("fifoname - name (absolute or relative) of the public channel through which the Client sends requests to the Server\n\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char ** argv, char ** envp) {
    // check and read command line arguments

    if (argc != 4 || (argc == 4 && strcmp(argv[1], "-t") != 0)) print_usage();

    sscanf(argv[2], "%u", &runtime);
    sscanf(argv[3], "%s", fifoname);

    // end command line arguments section

    printf("%u %s\n", runtime, fifoname);

    return 0;
}