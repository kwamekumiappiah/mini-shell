#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mini_shell.h"

#define MAX_ARGS 64

int main(void) {
    char *line = NULL;  // getline will automatically allocate memory
    size_t len = 0;     // getline updates this with buffer size

    while (1) {
        printf("myshell> ");
        fflush(stdout); // Ensures prompt appears immediately

        if (get_command(&line, &len, stdin) == 1){
            printf("\n[!] Exiting shell..\n");
            return 1;
        }


        // break the array into tokens and store them in a character pointer array,

        char *args[MAX_ARGS];
        if (parse_args(args, MAX_ARGS, line) == 1) {
            printf("\n[!] Error occured\n[!] Exiting shell...\n");
        }
    }

    free(line); // Clean up allocated memory before exiting
    return 0;
}