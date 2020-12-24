#ifndef MESSAGE_H
#define MESSAGE_H

#include "main.h"
#include "ui.h"
#include "utils.h"

/* prototypes */

int message_create(char *message_body, args_t *args, WINDOW **windows);
void message_destroy(msg_t *message, msg_t **message_queue, WINDOW **windows);
int  message_free(msg_t **message_queue);
int  history_print(client_t *client, args_t *args, WINDOW **windows);
void history_insert(packet_t *packet, client_t *client, args_t *args, WINDOW **windows);

#endif /* MESSAGE_H */