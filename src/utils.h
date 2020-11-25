#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"

bool valid_port(unsigned int port); 
// description
bool valid_username(char *username);
// TODO
void remove_first_char(char *input);
// description
void remove_newline(char *input);
// description
void assist(char *input, args_t *args); // maybe not in here
// rename assist/delete
void print_usage(void); 
// description
int set_host_username(char *input, args_t *args);
// description
int process_input(args_t *args);
// description
void insert_pfd(struct pollfd *pfds[], int new_fd, nfds_t *nfds, nfds_t *fd_count);
// description
void remove_pfd(struct pollfd pfds[], int i, nfds_t *fd_count);
// description



#endif /* UTILS_H */