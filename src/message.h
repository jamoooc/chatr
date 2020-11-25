#ifndef MESSAGE_H
#define MESSAGE_H

#include "main.h"

// /* packet_t */
// typedef struct packet {
//   char username[USERNAME_LEN];
//   char body[BUFFER_LEN];
// } packet_t;

// /* message_t - queue messages in linked list */

// typedef struct message {
//   int socket;
//   packet_t *packet;
//   struct message *next;
// } message_t;

// /* history_t - message history linked list */
// typedef struct history {
//   time_t timestamp;
//   packet_t *packet;
//   struct history *next;
// } history_t;


message_t *create_message(int socket, char *username, char *message_body);
void append_message(message_t *new_message, message_t **message_queue);
void remove_message(message_t *message, message_t **message_queue);
void print_messages(message_t **message_queue);

#endif /* MESSAGE_H */
