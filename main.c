#include <stdio.h>
#include <stdlib.h>

#define ARG_NO 1

int main(int argc, char** argv) {
    if (argc != ARG_NO + 1) {
        printf("Incorrect arguments!\n");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", argv[1]);
    exit(EXIT_SUCCESS);
}