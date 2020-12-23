#ifndef CONNECT_H
#define CONNECT_H

#include "main.h"
#include "message.h"
#include "pfds.h"

#define HOST_ON   "Host listening on:"
#define LISTENING "Waiting for client connection..."

void server_init(int *server_socket, args_t *args, WINDOW **windows);
// description
void accept_connection(int server_socket, args_t *args, WINDOW **windows);
// description
void packet_receive(int pfd_index, args_t *args, WINDOW **windows);
// description
void packet_transmit(msg_t *message, args_t *args, WINDOW **windows);
// description
void socket_set_nonblock(int socket);
// description

#endif /* CONNECT_H */