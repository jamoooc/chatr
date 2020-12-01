#include "message.h"
#include "utils.h"
#include "ui.h"


/* insert_history */


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

  // refresh msg history if active client
  if (strcmp(client->username, args->active_client->username) == 0) {
    print_history(client->history, windows);
  }
}


/* print_history */


void print_history(history_t *history, WINDOW **windows) {
  int y = HISTORY_OFFSET;
  int x = 1;
  int w_offset = HISTORY_OFFSET;

  // get offset if total msg history > window height
  history_t *msg = history;
  while (msg != NULL && w_offset >= 0) {
    msg = msg->next;
    w_offset--;
  }

  werase(windows[HISTORY]);
  msg = history; // reset ptr
  while (msg != NULL) {
    mvwprintw(windows[HISTORY], y - w_offset, x, "%s: %s\n", msg->packet->username, msg->packet->body);
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
  strcpy(packet->username, client->username);

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


void remove_message(message_t *message, message_t **message_queue, WINDOW **windows) {
  // TODO not super happy with this strcmp 
  message_t *del, **p = message_queue;
  while (*p != NULL && 
    strcmp((**p).packet->body, message->packet->body)) {    // while ptr is not null, and next doesnt match target  
    p = &(*p)->next;                                        // set p to the address of the next el until the next el is target
  }
  if (p) {                                                  // if not null (will be null (->next) if target not found)
    del = *p;
    *p = del->next;
    free(del);
  }
}
