#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "mini_shell.h"

/* ==========================================================================
 * Return-value convention used throughout this file:
 *   1 -> shell should keep looping
 *   0 -> shell should exit (only shell_exit returns this)
 * This lets main() decide when to stop just by checking shell_execute()'s
 * return value, instead of any function calling exit() on its own.
 * ========================================================================== */


/* --------------------------------------------------------------------------
 * Builtin: cd
 * -------------------------------------------------------------------------- */
int shell_cd(char **args) {
    // args[0] is "cd", args[1] is the target directory (if any)
    if (!args[1]) {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
        return 1; // missing arg is a user error, not a fatal one
    }

    if (chdir(args[1]) != 0) {
        perror("shell"); // e.g. "No such file or directory"
        return 1;        // failed cd shouldn't kill the shell
    }

    return 1; // success — keep running
}


/* --------------------------------------------------------------------------
 * Builtin: exit
 * -------------------------------------------------------------------------- */
int shell_exit(char **args) {
    (void)args; // unused — silences -Wunused-parameter warning

    // Deliberately does NOT call exit()/exit(0) here. Returning 0 lets
    // main()'s loop condition handle shutdown, keeping one single place
    // (main) responsible for the program's lifetime.
    return 0;
}


/* --------------------------------------------------------------------------
 * Input: read one line from the given stream
 * -------------------------------------------------------------------------- */
int get_command(char **line, size_t *len, FILE *stream) {
    // getline() allocates/reallocs *line as needed and updates *len
    // with the current buffer capacity.
    ssize_t nread = getline(line, len, stream);

    if (nread == -1) {
        // Ctrl+D (EOF) or an actual read error both land here
        return 1;
    }

    // getline() keeps the trailing '\n'; replace it with '\0' so
    // downstream code (strtok, execvp args, etc.) doesn't see it.
    (*line)[strcspn(*line, "\n")] = '\0';

    return 0;
}


/* --------------------------------------------------------------------------
 * Parsing: split a raw command line into an argv-style array
 * -------------------------------------------------------------------------- */
int parse_args(char *args[], size_t arr_size, char *command) {
    if (arr_size == 0) return 1; // no room to even store a NULL terminator

    size_t i = 0;

    // strtok mutates `command` in place, inserting '\0' after each token
    char *token = strtok(command, " \t\r\n");

    // Reserve the last slot (arr_size - 1) for the required NULL terminator,
    // since execvp() expects a NULL-terminated argv array.
    while (token != NULL && i < arr_size - 1) {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t\r\n");
    }

    args[i] = NULL; // terminate the array

    return 0;
}


/* --------------------------------------------------------------------------
 * Execution: run a builtin or fork/exec an external command
 * -------------------------------------------------------------------------- */
int shell_execute(char **args) {
    // Empty input (user just hit Enter) — nothing to do
    if (!args[0]) return 1;

    // --- Builtins are handled directly in the parent process, no fork ---
    if (strcmp(args[0], "cd") == 0) {
        return shell_cd(args);
    }

    if (strcmp(args[0], "exit") == 0) {
        return shell_exit(args);
    }

    // --- Anything else: treat as an external program ---
    pid_t pid = fork();

    if (pid == 0) {
        // ---- Child process ----
        // execvp() replaces this process's image entirely; it only
        // returns if it failed.
        if (execvp(args[0], args) == -1) {
            perror("shell");
            // Safe to call exit() here: this is the forked CHILD, not
            // the main shell process, so it doesn't affect the shell loop.
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        // ---- Parent process ----
        // Block until the child finishes so the prompt doesn't reappear
        // before the command has actually run.
        int status;
        waitpid(pid, &status, 0);
    } else {
        // pid < 0: fork() itself failed
        perror("fork");
        return 1;
    }

    return 1; // keep the shell running
}