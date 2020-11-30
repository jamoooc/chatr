#include "message.h"
#include "utils.h"

// TODO better name
void insert_history(int socket, packet_t *packet, client_t *client, args_t *args, WINDOW **windows) {
  // create new history item
  history_t *new_msg = malloc(sizeof(history_t));
  if (new_msg == NULL) { // make helper func for these
    perror("malloc insert_history");
    exit(EXIT_FAILURE);
  }
  memset(new_msg, 0, sizeof(history_t));
  new_msg->packet = packet;
  new_msg->next = NULL;
  
  // most recent item to top of LL for simple access
  history_t *prev_head = client->history;
  new_msg->packet = packet;
  new_msg->next = prev_head;
  client->history = new_msg;

  // append_history(new_msg, client->history);
  // if (strcmp(client->username, args->active_username)) {
    print_history(client->history, windows);
  // }
}


/* append_history */


// void append_history(history_t *new, history_t *history) {
//   history_t *tmp = history;
//   while (tmp != NULL) {
//     tmp = tmp->next;
//   }
//   new->next = tmp;
// }


// print_history

// review this whole func, dbl ptrs history please
void print_history(history_t *history, WINDOW **windows) {
  history_t *msg = history;
  werase(windows[HISTORY]);

  // TODO set y and counter to LINES * 0.8
  int y = 7, x = 1;
  int counter = 7;

  // offset for printing msgs if total msgs < 7 
  while (msg != NULL && counter >= 0) {
    msg = msg->next;
    counter--;
  }

  msg = history; // reset ptr
  while (msg != NULL && counter < 8) {
    mvwprintw(windows[HISTORY], y - counter, x, "%s\n", msg->packet->body);
    msg = msg->next;
    y--;
  }

  box(windows[HISTORY], 0, 0);
  wrefresh(windows[HISTORY]);
}


// TODO 
// void free_message_history(void) { }


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

// TODO REMOVE
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

