#ifndef CONNECT_H
#define CONNECT_H

#include "main.h"
#include "message.h"

void init_server(int *server_socket, args_t *args); // probably don't need a ptr here....
void accept_connection(int server_socket, args_t *args);
void receive_packet(int socket, int pfd_index, args_t *args);
void transmit_packet(int socket, message_t **message_queue, client_t *client);
void set_nonblock(int socket); // rename
// description
void *get_in_addr(struct sockaddr *sa);
// description

#endif /* CONNECT_H */