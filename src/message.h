#ifndef MESSAGE_H
#define MESSAGE_H

#include "main.h"

void insert_history(int socket, packet_t *packet, client_t *client);
void print_history(client_t *client);
message_t *create_message(int socket, char *message_body, client_t *client);
void append_message(message_t *new_message, message_t **message_queue);
void remove_message(message_t *message, message_t **message_queue);
void print_messages(message_t **message_queue);

#endif /* MESSAGE_H */
