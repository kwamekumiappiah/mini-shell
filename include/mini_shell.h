#ifndef SHELL_H
#define SHELL_H


int get_command(char **line, size_t *len, FILE *stream);

int parse_args(char *args[], size_t arr_size, char *command);

#endif // SHELL_H