#include "message.h"
#include "utils.h"

// TODO better name
void insert_history(packet_t *packet, client_t *client, args_t *args, WINDOW **windows) {
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

// TEMP
  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "1CLIENT USERNAME %s.\n", client->username);
  mvwprintw(windows[INFO], 2, 1, "2CLIENT USERNAME %s.\n", args->active_client->username);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);


  // append_history(new_msg, client->history);
  if (strcmp(client->username, args->active_client->username) == 0) {
    print_history(client->history, windows);
  }
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
  while (msg != NULL) {
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


message_t *create_message(char *message_body, client_t *client) {
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
