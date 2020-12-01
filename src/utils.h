#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"

#define EXIT_MESSAGE "Goodbye!"

bool valid_port(unsigned int port); 
// description
bool valid_username(char *username);
// TODO
void remove_first_char(char *input);
// description
void remove_newline(char *input);
// description
void assist(char *input, args_t *args, WINDOW **windows); // maybe not in here
// rename assist/delete
// description
int set_host_username(char *input, args_t *args, WINDOW **windows);
// description
int process_input(args_t *args, WINDOW **windows);
// description
void handle_error(char *err);
// description


#endif /* UTILS_H */