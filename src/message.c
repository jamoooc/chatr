#include "message.h"
#include "utils.h"

// TODO better name - 
void insert_history(int socket, packet_t *packet, client_t *client) {
  // create new history item
  history_t *new_msg = malloc(sizeof(history_t));
  if (new_msg == NULL) {
    perror("malloc insert_history");
    exit(EXIT_FAILURE);
  }
  memset(new_msg, 0, sizeof(history_t));

  // most recent item to top of LL for simple access
  history_t *prev_head = client->history;
  new_msg->packet = packet;
  new_msg->next = prev_head;
  client->history = new_msg;

  printf("client history: %s\n", strlen(client->alias) > 0 ? client->alias : client->username);
  print_history(client);
}


// TEMP print_history

void print_history(client_t *client) {
  history_t *msg = client->history;
  while (msg != NULL) {
    printf("%s\n", msg->packet->body);
    msg = msg->next;
  }
}

// TODO free_history


/* create_message */


message_t *create_message(int socket, char *message_body, client_t *client) {
  // create packet for message struct
  packet_t *packet = malloc(sizeof(packet_t));
  if (packet == NULL) {
    perror("malloc packet_t");
    exit(EXIT_FAILURE);
  }

  remove_newline(message_body);
  strcpy(packet->body, message_body);

  // create message for queue
  message_t *message = malloc(sizeof(message_t));
  if (message == NULL) {
    perror("malloc message_t");
    exit(EXIT_FAILURE);
  }

  message->client = client;
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

// TODO check this actually works in a different prog
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
    // printf("username: %s\n", tmp->packet->username);
    printf("packet: %s\n", tmp->packet->body);
    // printf("socket: %i\n", tmp->socket);
    tmp = tmp->next;
  }
}


/* 
  add to history 

create array client history LL 
  same size as pfds? when pfd is resized probably a good time to resize chat_history..
  maybe make a resize util func
insert_to_history
  LL - insert at end with *prev ptr OR insert at head no need for extra ptr
display history
  depending on size of curses window will need to display last n * (LINES - othercrap)
  THUS history can be simplified inserting at HEAD so theres no need for *prev ptr


existing funcs needing altered

transmit - insert_to_history after success
recieve - insert_to_history for specific client
    how to determine specific client? 
      use client IP so easier to add persistence
        either have HEAD struct with IP and username, then use packet_t
        no packet_t does not have a *next
        make history_t 
recieve packet - username displayed will need to come from history now

typedef struct history_head {
  username[SIZE];
  ip_address[INET6bla];
  history_t **history;
} hist_head_t;

typedef struct history {
  packet_t *packet;
  hist_t *next;
} hist_t;


remove username from packet_t??

*/

