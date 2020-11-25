#include "message.h"


/* create_message */


message_t *create_message(int socket, char *username, char *message_body) {
  // create packet for message struct
  packet_t *packet = malloc(sizeof(packet_t));
  if (packet == NULL) {
    perror("malloc packet_t");
    exit(EXIT_FAILURE);
  }
  strcpy(packet->username, username);
  strcpy(packet->body, message_body);
  
  // create message for queue
  message_t *message = malloc(sizeof(message_t));
  if (message == NULL) {
    perror("malloc message_t");
    exit(EXIT_FAILURE);
  }

  message->socket = socket;
  message->packet = packet;
  message->next = NULL;

  return message;
}


/* append_message */


void append_message(message_t *new_message, message_t **message_queue) {
  message_t **tmp = message_queue;
  while (*tmp != NULL) {
    tmp = &(*tmp)->next;
  }
  new_message->next = *tmp; // *tmp == NULL
  *tmp = new_message;
}


/* remove message from queue */


void remove_message(message_t *message, message_t **message_queue) {
  message_t *del, **p = message_queue;
  while (*p && strcmp((**p).packet->body, message->packet->body)) {     // while ptr is not null, and next doesnt match target  
    p = &(*p)->next;                                                    // set p to the address of the next el until the next el is target
  }
  if (p) {                                                              // if not null (will be null (->next) if target not found)
    del = *p;
    *p = del->next;
    free(del);
  }
}


/* print message */


void print_messages(message_t **message_queue) {
  // printf("message_queue\n"); // DEL
  message_t *tmp = *message_queue;
  while (tmp != NULL) {
    printf("username: %s\n", tmp->packet->username);
    printf("packet: %s\n", tmp->packet->body);
    printf("socket: %i\n", tmp->socket);
    tmp = tmp->next;
  }
}
