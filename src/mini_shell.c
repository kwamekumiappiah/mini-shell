#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "mini_shell.h"


const int shell_cd(char **args) {
    if (!args[1]) {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
        return 1; // Keep shell running
    }
    if (chdir(args[1]) != 0) {
        perror("shell");
        return 1; // Keep shell running even if chdir fails
    }
    return 1; // Return 1 on success so the shell stays open!
}


const int shell_exit(char **args) {
    exit(EXIT_SUCCESS); // Return 0 to signal main() loop to exit!
}


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



int shell_execute(char **args) {
    if (!args[0]) return 1; // 1. Check for empty input BEFORE forking

// Check for Built-in commands
    if (strcmp(args[0], "cd") == 0) {
        return shell_cd(args);
    }

    if (strcmp(args[0], "exit") == 0) {
        return shell_exit(args);
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child process execution
        if (execvp(args[0], args) == -1) {
            perror("shell");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        // Parent process waiting
        int status;
        waitpid(pid, &status, 0);
    } else if (pid < 0) {
        // Fork failure
        perror("fork");
        return 1; // 2. Keep the shell running
    }

    return 1;
}

