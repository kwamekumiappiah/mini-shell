#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>
#include <stdio.h>

/* ------------------------------------------------------------------------
 * mini_shell.h
 *
 * Public interface for the mini shell library. main.c only needs to know
 * about these three functions — everything else in mini_shell.c is an
 * implementation detail.
 * ------------------------------------------------------------------------ */

/* Reads one line of input from `stream` into `*line` (auto-allocated/resized
 * by getline), strips the trailing newline, and stores the buffer size in
 * `*len`. Returns 0 on success, 1 on EOF or read error. */
int get_command(char **line, size_t *len, FILE *stream);

/* Splits `command` into whitespace-separated tokens and stores pointers to
 * them in `args`. `arr_size` is the capacity of `args` (including room for
 * the trailing NULL). Returns 0 on success, 1 if arr_size is 0. */
int parse_args(char *args[], size_t arr_size, char *command);

/* Dispatches a parsed command: runs a builtin (cd, exit) or forks+execs an
 * external program. Returns 1 to keep the shell loop running, 0 to signal
 * that the shell should exit. */
int shell_execute(char **args);

#endif // SHELL_H