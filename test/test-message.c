#include "../unity/unity.h"
#include "../src/utils.h"
#include "../src/client.h"
#include "../src/connect.h"
#include "../src/main.h"
#include "../src/message.h"
#include "../src/pfds.h"
#include "../src/ui.h"


// this works for now MAKEFILE!
// gcc ../test/test-message.c ../src/utils.c ../src/client.c ../src/message.c ../src/pfds.c ../src/connect.c ../src/ui.c ../unity/unity.c -lncurses


void setUp() {
  /* run before each test */
}

void tearDown() {
  /* run after each test */
}



void test_insert_history(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  
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
  args_t *args = malloc(sizeof(args_t));

  client_t *client = malloc(sizeof(client_t));
  strcpy(client->username, "user1");
  client->socket = 4;
  client->history = NULL;

  char *msg = "test message!";
  message_t *t_msg = create_message(msg, client, args, windows);

  TEST_ASSERT_EQUAL_STRING(msg, t_msg->packet->body);
  TEST_ASSERT_EQUAL_STRING("user1", t_msg->client->username);
  TEST_ASSERT_NULL(t_msg->next);

  free(windows);
  free(client);
  free(args);
}



void test_append_message(void) {
  WINDOW **wins = create_windows_array(N_WINDOWS);
  message_t *head = NULL;
  args_t *args = malloc(sizeof(args_t));
  args->message_queue = &head;
  int i = 0, j = 0;

  // create message queue
  char msg[7];
  while (i < 3) {
    snprintf(msg, sizeof(msg), "test%i", i);
    client_t *client = malloc(sizeof(client_t));
    message_t *new = create_message(msg, client, args, wins);
    append_message(new, args->message_queue, wins);
    i++;
  }

  char *expect[3] = { "test0", "test1", "test2" };

  // check queue vals
  message_t **tmp = &head;
  while (*tmp != NULL) {
    TEST_ASSERT_EQUAL_STRING(expect[j], (*tmp)->packet->body);
    tmp = &(*tmp)->next;
    j++;
  }
  TEST_ASSERT_EQUAL_INT(i, j);
}



void test_remove_message(void) {
  WINDOW **wins = create_windows_array(N_WINDOWS);
  args_t *args = malloc(sizeof(args_t));
  message_t *head = NULL;
  int rv, i, a, b, c, d; // rv & counters
  rv = a = b = c = d = i = 0;
  char msg[6];

  // create client list
  while (i < 5) {
    client_t *client = malloc(sizeof(client_t));
    client->socket = i;
    snprintf(msg, sizeof(msg), "msg%i", i);
    message_t *new = create_message(msg, client, args, wins);
    
    message_t **tmp = &head;
    while (*tmp != NULL) {
      tmp = &(*tmp)->next;
    }
    new->next = *tmp; // NULL
    *tmp = new;
    i++;
  }

  // check message list is as expected
  message_t **one = &head;
  char *expects[5] = { "msg0", "msg1", "msg2", "msg3", "msg4" };
  while (*one != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects[a], (*one)->packet->body);
    TEST_ASSERT_NOT_NULL(one);
    one = &(*one)->next;
    a++;
  }

  // remove last client
  client_t *client1 = malloc(sizeof(client_t));
  message_t *tmp1 = create_message("msg4", client1, args, wins);
  client1->socket = 1;

  remove_message(tmp1, &head, wins);

  char *expects1[4] = { "msg0", "msg1", "msg2", "msg3" };
  int last = 0; // track last entry
  message_t **two = &head;
  while (*two != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects1[b], (*two)->packet->body);
    last = (*two)->client->socket;
    two = &(*two)->next;
    b++;
  }
  TEST_ASSERT_EQUAL_INT(3, last);

  // removes client from middle
  client_t *client2 = malloc(sizeof(client_t));
  client1->socket = 1;
  message_t *tmp2 = create_message("msg1", client2, args, wins);

  remove_message(tmp2, &head, wins);

  message_t **three = &head;
  char *expects2[3] = { "msg0", "msg2", "msg3" };
  int last2 = 0; // track last entry
  while (*three != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects2[c], (*three)->packet->body);
    c++;
    last2 = (*three)->client->socket;
    three = &(*three)->next;
  }
  TEST_ASSERT_EQUAL_INT(3, last2);

  // removes client from start
  client_t *client3 = malloc(sizeof(client_t));
  client1->socket = 1;
  message_t *tmp3 = create_message("msg0", client3, args, wins);
  
  remove_message(tmp3, &head, wins);

  message_t **four = &head;
  char *expects3[2] = { "msg2", "msg3" };
  int last3 = 0;
  while (*four != NULL) {
    TEST_ASSERT_EQUAL_STRING(expects3[d], (*four)->packet->body);
    d++;
    last3 = (*four)->client->socket;
    four = &(*four)->next;
  }
  TEST_ASSERT_EQUAL_INT(3, last3);
}


/* test_free_messages */


void test_free_messages(void) {
  message_t *head = NULL;
  WINDOW **wins = create_windows_array(N_WINDOWS);
  args_t *args = malloc(sizeof(args_t));
  client_t *client = malloc(sizeof(client_t));
  char msg[15];
  int i, k, rv;
  rv = k = i = 0;

  while (i < 5) {
    snprintf(msg, 15, "test message%i", i);
    message_t *new = create_message(msg, client, args, wins);
    message_t **tmp = &head;
    while (*tmp != NULL) {
      tmp = &(*tmp)->next;
    }
    new->next = *tmp; // NULL
    *tmp = new;
    i++;
  }

  // message list is as expected
  char *expects[5] = { "test message0", "test message1", "test message2", "test message3", "test message4" };
  message_t *p = head;
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

int main(void) {
  printf("test-message.c\n");
  UNITY_BEGIN();
  RUN_TEST(test_insert_history);
  RUN_TEST(test_print_history);
  RUN_TEST(test_create_message);
  RUN_TEST(test_append_message);
  RUN_TEST(test_remove_message);
  RUN_TEST(test_free_messages);
  return UNITY_END();
}