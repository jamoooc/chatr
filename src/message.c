#include "message.h"

/* history_insert */

void history_insert(packet_t *packet, client_t *client, args_t *args, WINDOW **windows) {
  // create new history item
  history_t *new_msg;
  if ((new_msg = malloc(sizeof(history_t))) == NULL) {
    handle_error(-1, "history_insert: malloc,", args, windows);
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

  // if no active client, set to target client
  if (args->active_client == NULL) {
    args->active_client = client;
  }

  // refresh msg history if active client or indicate waiting message
  if (strcmp(client->username, args->active_client->username) == 0) {
    history_print(client, args, windows);
  } else {
    client->unread_msg = 1;
    client_print(args->active_client, args->client_list, windows);
  }
}

/* history_print */

int history_print(client_t *client, args_t *args, WINDOW **windows) {
  if (client->history == NULL) {
    return 1;
  }

  // window offset used to print messages from bottom up
  int y = HISTORY_OFFSET;
  int x = 1;
  int w_offset = HISTORY_OFFSET;

  client->unread_msg = 0;
  client_print(args->active_client, args->client_list, windows);

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
  return 0;
}

/* message_create */

msg_t *message_create(char *message_body, args_t *args, WINDOW **windows) {
  // create packet for message struct
  packet_t *packet;
  if ((packet = malloc(sizeof(packet_t))) == NULL) {
    handle_error(-1, "message_create: malloc,", args, windows);
    exit(EXIT_FAILURE);
  }

  remove_newline(message_body);
  strcpy(packet->body, message_body);
  strcpy(packet->username, args->host_username);

  // create message for queue
  msg_t *message;
  if ((message = malloc(sizeof(msg_t))) == NULL) {
    handle_error(-1, "message_create: malloc,", args, windows);
    exit(EXIT_FAILURE);
  }

  message->client = args->active_client;
  message->packet = packet;
  message->next = NULL;

  return message;
}

/* message_append */

void message_append(msg_t *new_message, msg_t **message_queue, WINDOW **windows) {
  msg_t **msg = message_queue;
  while (*msg != NULL) {
    msg = &(*msg)->next;
  }
  new_message->next = *msg; // *msg is NULL
  *msg = new_message;
}

/* remove message from queue */

void message_destroy(msg_t *message, msg_t **message_queue, WINDOW **windows) {
  // TODO not super happy with this strcmp 
  msg_t *del, **p = message_queue;
  while (*p != NULL && strcmp((**p).packet->body, message->packet->body) != 0) {
    p = &(*p)->next;
  }
  if (p) {  // p is null if target not found
    del = *p;
    *p = del->next;
    free(del);
  }
}

/* free remaining messages in queue */

int message_free(msg_t **message_queue) {
  if (message_queue == NULL) {
    return 1;
  }
  
  msg_t *del, *message = *message_queue;
  while (message != NULL) {
    del = message;
    message = del->next;
    free(del);
  }
  
  *message_queue = NULL;
  return 0;
}
