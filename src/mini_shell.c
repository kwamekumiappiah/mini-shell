#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mini_shell.h"




int get_command(char **line, size_t *len, FILE *stream) {
    ssize_t nread;
    nread = getline(line, len, stream);

    // Check for Ctrl+D (EOF) or reading error
    if (nread == -1) {
        return 1;
    }

    (*line)[strcspn(*line, "\n")] = '\0';  // clear the trailling new-line character with a null terminator
    return 0;
}



int parse_args(char *args[], size_t arr_size, char *command) {
    if (arr_size == 0) return 1; 
    int i = 0;

    // Get the first token
    char *token = strtok(command, " \t\r\n");

    // Loop through and store each token
    while (token != NULL && i < arr_size - 1) {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t\r\n"); // Get the next token
    }

    args[i] = NULL; // NULL-terminate the argument array for execvp

    return 0;
}