#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "main.h"
#include "client.h"
#include "ui.h"

#define ERROR_EXIT_MESSAGE "Chatr exited unexpectedly. Error log in logfile.txt"
#define EXIT_MESSAGE "Goodbye!"

int valid_port(char *port); 
// description
bool valid_username(char *username);
// description
void remove_first_char(char *input);
// description
int remove_newline(char *input);
// description
int set_host_username(char *input, args_t *args, WINDOW **windows);
// description
void get_input(args_t *args, WINDOW **windows);
// description
int handle_input(char *input, args_t *args, WINDOW **windows);
// description
void handle_error(int rv, const char *func, args_t *args, WINDOW **windows);
// description
void remove_trailing_whitespace(char *input);
// description

#endif /* UTILS_H */