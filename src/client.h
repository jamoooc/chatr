#ifndef CLIENT_H
#define CLIENT_H

#include "main.h"


client_t *create_client(int socket, char *username, struct sockaddr_in client_addr);
void append_client(client_t *new_client, client_t **client);
int add_client(char *input, args_t *args);
int select_active_client(char *input_user, args_t *args) ;
int set_client_alias(char *input, args_t *args);
void disconnect_client(int socket, int pfd_index, args_t *args);
void remove_client(int socket, client_t **client_list);
void print_clients(client_t **client);


#endif /* CLIENT_H */