#include "message.h"
#include "utils.h"
#include "ui.h"
#include "client.h"


/* insert_history */


void insert_history(packet_t *packet, client_t *client, args_t *args, WINDOW **windows) {
          // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "INSERT HISTORY CALLED\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
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

  // refresh msg history if active client or indicate waiting message
  if (strcmp(client->username, args->active_client->username) == 0) {
      // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "PRINT HISTORY IF CALLED\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
    print_history(client, args, windows);
      // TEMP
      // werase(windows[INFO]);
      // mvwprintw(windows[INFO], 1, 1, "PRINT HISTORY DONE\n");
      // box(windows[INFO], 0, 0);
      // wrefresh(windows[INFO]);
      // sleep(1);
  } else {
      // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "PRINT CLIENTS CALLED\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
    client->unread_msg = 1;
    print_clients(args->active_client, args->client_list, windows);
     // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "PRINT CLIENTS CALLED\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
  }
    // TEMP
    // werase(windows[INFO]);
    // mvwprintw(windows[INFO], 1, 1, "INSERT HISTORY DONE\n");
    // box(windows[INFO], 0, 0);
    // wrefresh(windows[INFO]);
    // sleep(1);
}


/* print_history */


void print_history(client_t *client, args_t *args, WINDOW **windows) {
          // TEMP
    // werase(windows[INFO]);
    // mvwprintw(windows[INFO], 1, 1, "PRINT HISTORY CALLED\n");
    // box(windows[INFO], 0, 0);
    // wrefresh(windows[INFO]);
    // sleep(1);

  // handle more gracefully if active client null??
  int y = HISTORY_OFFSET;
  int x = 1;
  int w_offset = HISTORY_OFFSET;

  client->unread_msg = 0;
  print_clients(args->active_client, args->client_list, windows);
        // TEMP
    // werase(windows[INFO]);
    // mvwprintw(windows[INFO], 1, 1, "RECEIVE PACKET CALLED PRINT CLIENTS\n");
    // box(windows[INFO], 0, 0);
    // wrefresh(windows[INFO]);
    // sleep(1);
  // get offset if total msg history > window height
  history_t *msg = client->history;
  while (msg != NULL && w_offset >= 0) {
    msg = msg->next;
    w_offset--;
  }

  werase(windows[HISTORY]);
  msg = client->history; // reset ptr
  while (msg != NULL) {
    mvwprintw(windows[HISTORY], y - w_offset, x, "%s: %s\n", msg->packet->username, msg->packet->body);
    msg = msg->next;
    y--;
  }

  box(windows[HISTORY], 0, 0);
  wrefresh(windows[HISTORY]);
  // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "PRINT HISTORY DONE\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);

}


// TODO 
// void free_message_history(void) { }


/* create_message */


message_t *create_message(char *message_body, client_t *client, args_t *args, WINDOW **windows) {
    // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "CALL CREATE MSG\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
  // create packet for message struct
  packet_t *packet = malloc(sizeof(packet_t));
  if (packet == NULL) {
    perror("malloc packet_t");
    exit(EXIT_FAILURE);
  }

  remove_newline(message_body);
  strcpy(packet->body, message_body);
  strcpy(packet->username, args->host_username);

  // create message for queue
  message_t *message = malloc(sizeof(message_t));
  if (message == NULL) {
    perror("malloc message_t");
    exit(EXIT_FAILURE);
  }

  message->client = client;
  message->packet = packet;
  message->next = NULL;

  //   // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "CREATE MSG PACKET: %s\n", message->packet->body);
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
  //           sleep(1);
  // // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "CREATE MSG DONE\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
  return message;
}


/* append_message */


void append_message(message_t *new_message, message_t **message_queue, WINDOW **windows) {
                  // TEMP
        // werase(windows[INFO]);
        // mvwprintw(windows[INFO], 1, 1, "APPEND MSG CALLED\n");
        // box(windows[INFO], 0, 0);
        // wrefresh(windows[INFO]);
        // sleep(1);
  
  message_t **tmp = message_queue; // TODO rename tmp, msg?
  while (*tmp != NULL) {
    tmp = &(*tmp)->next;
  }
  new_message->next = *tmp; // *tmp == NULL
  *tmp = new_message;
    // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "APPENDING MSG: %s\n", (*tmp)->packet->body);
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
}


/* remove message from queue */


void remove_message(message_t *message, message_t **message_queue, WINDOW **windows) {
        // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "REMOVE MSG CALLED\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
  // TODO not super happy with this strcmp 
  message_t *del, **p = message_queue;
  while (*p != NULL && strcmp((**p).packet->body, message->packet->body) != 0) {
    p = &(*p)->next;                                        // set p to the address of the next el until the next el is target
  }
  if (p) {                                                  // if not null (will be null (->next) if target not found)
    del = *p;
    *p = del->next;
    free(del);
  }
    // TEMP
    // werase(windows[INFO]);
    // mvwprintw(windows[INFO], 1, 1, "REMOVe MSG DONE\n");
    // box(windows[INFO], 0, 0);
    // wrefresh(windows[INFO]);
    // sleep(1);
}


/* free remaining messages in queue */


void free_messages(message_t **message_queue) {
  message_t *del, *message = *message_queue;
  while (message != NULL) {
    del = message;
    message = del->next;
    free(del);
  }
}
