#ifndef CLIENT_H
#define CLIENT_H

#include "main.h"
#include "ui.h"
#include "connect.h"

#define SET_CLIENT "Select a client with '@username' before editing a username."

client_t *client_create(int socket, char *username);
int client_connect(char *input, args_t *args, WINDOW **windows);
int client_destroy(int socket, client_t **client_list);
void client_append(client_t *new_client, args_t *args, WINDOW **windows);
void client_print(client_t *active_client, client_t **client, WINDOW **windows);
void client_disconnect(int pfd_index, args_t *args, WINDOW **windows);

int set_active_client(char *input_user, args_t *args, WINDOW **windows);
int set_client_username(char *input, args_t *args, WINDOW **windows);

int client_free(client_t **client_list);
int client_history_free(history_t *history);

#endif /* CLIENT_H */