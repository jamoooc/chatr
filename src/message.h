#ifndef MESSAGE_H
#define MESSAGE_H

#include "main.h"

void insert_history(packet_t *packet, client_t *client, args_t *args, WINDOW **windows);
int print_history(client_t *client, args_t *args, WINDOW **windows);
void append_history(history_t *new, history_t *history);
void append_message(msg_t *new_message, msg_t **message_queue, WINDOW **windows);
void remove_message(msg_t *message, msg_t **message_queue, WINDOW **windows);
void print_messages(msg_t **message_queue);
msg_t *create_message(char *message_body, client_t *active_client, args_t *args, WINDOW **windows);
int free_messages(msg_t **message_queue);

#endif /* MESSAGE_H */