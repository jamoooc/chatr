#include "../unity/unity.h"
#include "../src/message.h"

/* test_history_print */

void test_history_insert(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);
  
  args_t *args = malloc(sizeof(args_t));
  client_t *client_list = NULL;
  args->client_list = &client_list;

  client_create(4, "user", args, windows);
  set_active_client("user", args, windows);
  client_t *client = *args->client_list;

  // history inserted in reverse
  packet_t *packet3 = malloc(sizeof(packet_t));
  packet_t *packet2 = malloc(sizeof(packet_t));
  packet_t *packet1 = malloc(sizeof(packet_t));

  strcpy(packet3->body, "test3");
  strcpy(packet2->body, "test2");
  strcpy(packet1->body, "test1");

  history_insert(packet3, client, args, windows);
  history_insert(packet2, client, args, windows);
  history_insert(packet1, client, args, windows);

  int i = 0;
  char *expect[3] = { "test1", "test2", "test3" };

  history_t *hist = client->history;
  while (hist != NULL) {
    TEST_ASSERT_EQUAL_STRING(expect[i], hist->packet->body);
    hist = hist->next;
    i++;
  }
  TEST_ASSERT_EQUAL_INT(3, i);
}

/* test_history_print */

void test_history_print(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);
  args_t *args = malloc(sizeof(args_t));
  
  client_t *client = malloc(sizeof(client_t));
  client->history = NULL;
  
  int rv = 0;

  // if history NULL, return err val
  rv = history_print(client, args, windows);
  TEST_ASSERT_EQUAL_INT(1, rv);

  // TODO ncurses tests

  free(windows);
  free(client);
  free(args);
}

/* test_message_create */

void test_message_create(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  msg_t *message_queue = NULL;
  client_t *client_list = NULL;
  args_t *args = malloc(sizeof(args_t));

  strcpy(args->host_username, "host");
  args->message_queue = &message_queue;
  args->client_list = &client_list;

  char *msg = malloc(sizeof(char) * 6);
  char *client = malloc(sizeof(char) * 9);
  
  int i;

  for (i = 0; i < 5; i++) {
    snprintf(msg, sizeof(msg), "msg%i", i);
    snprintf(client, sizeof(client), "client%i", i);

    client_create(i, client, args, windows);
    set_active_client(client, args, windows);
    message_create(msg, args, windows);
  }

  msg_t *actual_msg = *args->message_queue;
  for (i = 0; i < 5; i++) {
    snprintf(msg, sizeof(msg), "msg%i", i);
    snprintf(client, sizeof(client), "client%i", i);

    TEST_ASSERT_EQUAL_STRING(msg, actual_msg->packet->body);
    TEST_ASSERT_EQUAL_STRING(client, actual_msg->client->username);
    if (i < 4) {
      TEST_ASSERT_NOT_NULL(actual_msg->next);
    } else {
      TEST_ASSERT_NULL(actual_msg->next);
    }
    actual_msg = actual_msg->next;
  }

  free(msg);
  free(client);
  free(windows);
  client_free(args->client_list);
  free(args);
}

/* void test_message_destroy */

void test_message_destroy(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  msg_t *message_queue = NULL;
  client_t *client_list = NULL;
  args_t *args = malloc(sizeof(args_t));

  strcpy(args->host_username, "host");
  args->message_queue = &message_queue;
  args->client_list = &client_list;

  char *msg = malloc(sizeof(char) * 6);
  char *client = malloc(sizeof(char) * 9);
  
  int rv, last, i; // rv & counters
  rv = last = i = 0;

  for (i = 0; i < 5; i++) {
    snprintf(msg, sizeof(msg), "msg%i", i);
    snprintf(client, sizeof(client), "client%i", i);

    client_create(i, client, args, windows);
    set_active_client(client, args, windows);
    message_create(msg, args, windows);
  }

  // check message list is as expected
  msg_t *message = *args->message_queue;
  char *expects[5] = { "msg0", "msg1", "msg2", "msg3", "msg4" };
  i = 0;
  while (message != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects[i], message->packet->body);
    message = message->next;
    i++;
  }

  // message to remove (reused)
  packet_t *packet = malloc(sizeof(packet_t));
  strcpy(packet->body, "msg4");
  msg_t *tmp = malloc(sizeof(msg_t));
  tmp->packet = packet;
  tmp->next = NULL;

  // remove last msg
  message_destroy(tmp, args->message_queue, windows);
  char *expects1[4] = { "msg0", "msg1", "msg2", "msg3" };
  last = 0, i = 0; // track last entry
  message = *args->message_queue;
  while (message != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects1[i], message->packet->body);
    message = message->next;
    i++;
  }

  // removes client from middle
  strcpy(tmp->packet->body, "msg1");
  message_destroy(tmp, args->message_queue, windows);
  message = *args->message_queue;
  char *expects2[3] = { "msg0", "msg2", "msg3" };
  last = 0, i = 0; // track last entry
  while (message != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects2[i], message->packet->body);
    message = message->next;
    i++;
  }

  // removes client from start
  strcpy(tmp->packet->body, "msg0");
  message_destroy(tmp, args->message_queue, windows);
  message = *args->message_queue;
  char *expects3[2] = { "msg2", "msg3" };
  last = 0, i = 0; // track last entry
  while (message != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects3[i], message->packet->body);
    message = message->next;
    i++;
  }

  free(tmp);
  free(msg);
  free(client);
  message_free(args->message_queue);
  client_free(args->client_list);
  free(args);
  free(windows);
}

/* test_message_free */

void test_message_free(void) {
  // msg_t *head = NULL;
  // WINDOW **windows = window_create_array(N_WINDOWS);
  // memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  // args_t *args = malloc(sizeof(args_t));
  // client_t *client = malloc(sizeof(client_t));

  // char msg[15];
  // int i, k, rv;
  // rv = k = i = 0;

  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  msg_t *message_queue = NULL;
  client_t *client_list = NULL;
  args_t *args = malloc(sizeof(args_t));

  strcpy(args->host_username, "host");
  args->message_queue = &message_queue;
  args->client_list = &client_list;

  char *msg = malloc(sizeof(char) * 6);
  char *client = malloc(sizeof(char) * 9);
  
  int rv, i; // rv & counters
  rv = i = 0;

  for (i = 0; i < 5; i++) {
    snprintf(msg, sizeof(msg), "msg%i", i);
    snprintf(client, sizeof(client), "client%i", i);

    client_create(i, client, args, windows);
    set_active_client(client, args, windows);
    message_create(msg, args, windows);
  }

  // message list is as expected
  char *expects[5] = { "msg0", "msg1", "msg2", "msg3", "msg4" };
  msg_t *message = *args->message_queue;
  i = 0;
  while (message != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects[i], message->packet->body);
    TEST_ASSERT_NOT_NULL(message);
    message = message->next;
    i++;
  }

  rv = message_free(args->message_queue);
  TEST_ASSERT_EQUAL_INT(0, rv);
  TEST_ASSERT_NULL(message_queue);
}
