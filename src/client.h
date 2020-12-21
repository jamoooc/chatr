#ifndef CLIENT_H
#define CLIENT_H

#include "main.h"
#include "ui.h"
#include "connect.h"

#define SET_CLIENT "Select a client with '@username' before editing a username."

client_t *create_client(int socket, char *username);
void disconnect_client(int pfd_index, args_t *args, WINDOW **windows);
void print_clients(client_t *active_client, client_t **client, WINDOW **windows);
void append_client(client_t *new_client, args_t *args, WINDOW **windows);
int add_client(char *input, args_t *args, WINDOW **windows);
int set_active_client(char *input_user, args_t *args, WINDOW **windows);
int set_client_username(char *input, args_t *args, WINDOW **windows);
int remove_client(int socket, client_t **client_list);
int free_clients(client_t **client_list);
int free_history(history_t *history);

#endif /* CLIENT_H */