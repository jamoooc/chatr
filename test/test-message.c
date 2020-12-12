#include "../unity/unity.h"
#include "../src/message.h"


void test_insert_history(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);
  
  client_t *client = malloc(sizeof(client_t));
  client_t *client_list = NULL;
  strcpy(client->username, "user1");
  client->history = NULL;

  args_t *args = malloc(sizeof(args_t));
  args->active_client = client;
  args->client_list = &client_list;

  packet_t *packet3 = malloc(sizeof(packet_t));
  packet_t *packet2 = malloc(sizeof(packet_t));
  packet_t *packet1 = malloc(sizeof(packet_t));

  strcpy(packet3->body, "test3");
  strcpy(packet2->body, "test2");
  strcpy(packet1->body, "test1");

  insert_history(packet3, client, args, windows);
  insert_history(packet2, client, args, windows);
  insert_history(packet1, client, args, windows);

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



void test_print_history(void) {
  // if history NULL, return err val
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);
  args_t *args = malloc(sizeof(args_t));
  
  client_t *client = malloc(sizeof(client_t));
  client->history = NULL;

  int rv = 0;
  rv = print_history(client, args, windows);
  TEST_ASSERT_EQUAL_INT(1, rv);

  // TODO ncurses tests

  free(windows);
  free(client);
  free(args);
}



void test_create_message(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  strcpy(args->host_username, "host");

  client_t *client = malloc(sizeof(client_t));
  strcpy(client->username, "user1");
  client->socket = 4;
  client->history = NULL;
  args->active_client = client;

  char *msg = "test message!";
  msg_t *t_msg = create_message(msg, args, windows);

  TEST_ASSERT_EQUAL_STRING(msg, t_msg->packet->body);
  TEST_ASSERT_EQUAL_STRING("user1", t_msg->client->username);
  TEST_ASSERT_NULL(t_msg->next);

  free(windows);
  free(client);
  free(args);
}



void test_append_message(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  msg_t *head = NULL;
  args_t *args = malloc(sizeof(args_t));
  args->message_queue = &head;
  int i = 0, j = 0;

  // create message queue
  char msg[7];
  while (i < 3) {
    snprintf(msg, sizeof(msg), "test%i", i);
    client_t *client = malloc(sizeof(client_t));
    msg_t *new = create_message(msg, args, windows);
    append_message(new, args->message_queue, windows);
    i++;
  }

  char *expect[3] = { "test0", "test1", "test2" };

  // check queue vals
  msg_t **tmp = &head;
  while (*tmp != NULL) {
    TEST_ASSERT_EQUAL_STRING(expect[j], (*tmp)->packet->body);
    tmp = &(*tmp)->next;
    j++;
  }
  TEST_ASSERT_EQUAL_INT(i, j);
}



void test_remove_message(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);
  
  client_t *active_client = malloc(sizeof(client_t));
  strcpy(active_client->username, "user1");
  active_client->socket = 4;
  active_client->history = NULL;

  args_t *args = malloc(sizeof(args_t));
  args->active_client = active_client;

  msg_t *head = NULL;
  int rv, i, a, b, c, d; // rv & counters
  rv = a = b = c = d = i = 0;
  char msg[6];

  // create client list
  while (i < 5) {
    client_t *client = malloc(sizeof(client_t));
    client->socket = i;
    snprintf(msg, sizeof(msg), "msg%i", i);
    msg_t *new = create_message(msg, args, windows);
    
    msg_t **tmp = &head;
    while (*tmp != NULL) {
      tmp = &(*tmp)->next;
    }
    new->next = *tmp; // NULL
    *tmp = new;
    i++;
  }

  // check message list is as expected
  msg_t **one = &head;
  char *expects[5] = { "msg0", "msg1", "msg2", "msg3", "msg4" };
  while (*one != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects[a], (*one)->packet->body);
    one = &(*one)->next;
    a++;
  }

  // remove last msg
  msg_t *tmp1 = create_message("msg4", args, windows);
  remove_message(tmp1, &head, windows);

  char *expects1[4] = { "msg0", "msg1", "msg2", "msg3" };
  int last = 0; // track last entry
  msg_t **two = &head;
  while (*two != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects1[b], (*two)->packet->body);
    two = &(*two)->next;
    b++;
  }

  // // removes client from middle
  msg_t *tmp2 = create_message("msg1", args, windows);
  remove_message(tmp2, &head, windows);

  msg_t **three = &head;
  char *expects2[3] = { "msg0", "msg2", "msg3" };
  int last2 = 0; // track last entry
  while (*three != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects2[c], (*three)->packet->body);
    three = &(*three)->next;
    c++;
  }

  // // removes client from start
  msg_t *tmp3 = create_message("msg0", args, windows);
  remove_message(tmp3, &head, windows);

  msg_t **four = &head;
  char *expects3[2] = { "msg2", "msg3" };
  int last3 = 0;
  while (*four != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects3[d], (*four)->packet->body);
    d++;
    four = &(*four)->next;
  }

  free(tmp1);
  free(tmp2);
  free(tmp3);
  free_messages(&head);
  free(args);
  free(windows);
}


/* test_free_messages */


void test_free_messages(void) {
  msg_t *head = NULL;
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  client_t *client = malloc(sizeof(client_t));
  char msg[15];
  int i, k, rv;
  rv = k = i = 0;

  while (i < 5) {
    snprintf(msg, 15, "test message%i", i);
    msg_t *new = create_message(msg, args, windows);
    msg_t **tmp = &head;
    while (*tmp != NULL) {
      tmp = &(*tmp)->next;
    }
    new->next = *tmp; // NULL
    *tmp = new;
    i++;
  }

  // message list is as expected
  char *expects[5] = { "test message0", "test message1", "test message2", "test message3", "test message4" };
  msg_t *p = head;
  while (p != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects[k], p->packet->body);
    TEST_ASSERT_NOT_NULL(p);
    p = p->next;
    k++;
  }

  rv = free_messages(&head);
  TEST_ASSERT_EQUAL_INT(0, rv);
  TEST_ASSERT_NULL(head);
}

// int main(void) {
//   printf("test-message.c\n");
//   UNITY_BEGIN();
//   RUN_TEST(test_insert_history);
//   RUN_TEST(test_print_history);
//   RUN_TEST(test_create_message);
//   RUN_TEST(test_append_message);
//   RUN_TEST(test_remove_message);
//   RUN_TEST(test_free_messages);
//   return UNITY_END();
// }