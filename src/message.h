#ifndef MESSAGE_H
#define MESSAGE_H

#include "main.h"

void insert_history(packet_t *packet, client_t *client, args_t *args, WINDOW **windows);
void print_history(history_t *history, WINDOW **windows);
void append_history(history_t *new, history_t *history);

void append_message(message_t *new_message, message_t **message_queue);
void remove_message(message_t *message, message_t **message_queue, WINDOW **windows);
void print_messages(message_t **message_queue);
message_t *create_message(char *message_body, client_t *client);
// temp REMOVE WINDWOS HERE ^
#endif /* MESSAGE_H */
