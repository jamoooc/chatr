#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "main.h"
#include "client.h"
#include "ui.h"

#define EXIT_MESSAGE "Goodbye!"

int valid_port(char *port); 
// description
bool valid_username(char *username);

void remove_first_char(char *input);
// description
int remove_newline(char *input);
// description
int set_host_username(char *input, args_t *args, WINDOW **windows);
// description
void get_input(args_t *args, WINDOW **windows);
// description
int process_input(char *input, args_t *args, WINDOW **windows);
// description
void handle_error(int e, const char *str);
// description
void remove_trailing_whitespace(char *input);


#endif /* UTILS_H */