#ifndef CONNECT_H
#define CONNECT_H

#include "main.h"
#include "message.h"
#include "pfds.h"

void init_server(int *server_socket, args_t *args, WINDOW **windows); // probably don't need a ptr here....
void accept_connection(int server_socket, args_t *args, WINDOW **windows);
void set_nonblock(int socket); // rename
void *get_in_addr(struct sockaddr *sa);
void receive_packet(int socket, int pfd_index, args_t *args, WINDOW **windows);
void transmit_packet(int socket, message_t **message_queue, client_t *client, args_t *args, WINDOW **windows);
// description
// description



#endif /* CONNECT_H */