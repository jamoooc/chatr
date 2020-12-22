#ifndef CONNECT_H
#define CONNECT_H

#include "main.h"
#include "message.h"
#include "pfds.h"

#define HOST_ON   "Host listening on:"
#define LISTENING "Waiting for client connection..."

void init_server(int *server_socket, args_t *args, WINDOW **windows);
void accept_connection(int server_socket, args_t *args, WINDOW **windows);
void *get_in_addr(struct sockaddr *sa);
void packet_receive(int pfd_index, args_t *args, WINDOW **windows);
void packet_transmit(msg_t *message, args_t *args, WINDOW **windows);

void set_nonblock(int socket); // rename
// description
// description



#endif /* CONNECT_H */