#include "../unity/unity.h"
#include "../src/utils.h"
#include "../src/client.h"
#include "../src/connect.h"
#include "../src/main.h"
#include "../src/message.h"
#include "../src/pfds.h"
#include "../src/ui.h"


void setUp() {

}

void tearDown() {

}

void test_create_client(void) {
  client_t *client;
  client = create_client(5, "test");

  TEST_ASSERT_NOT_NULL(client);
  TEST_ASSERT_EQUAL_INT(5, client->socket);
  TEST_ASSERT_NULL(client->history);
  TEST_ASSERT_EQUAL_STRING("test", client->username);

  free(client);
}

void test_append_client(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));

  client_t *client_list = NULL;
  args->client_list = &client_list;
  args->active_client = NULL;
  
  client_t *client1 = create_client(5, "one");
  client_t *client2 = create_client(6, "two");

  append_client(client1, args, windows);
  append_client(client2, args, windows);

  TEST_ASSERT_NOT_NULL(client_list);
  TEST_ASSERT_EQUAL_INT(5, client_list->socket);
  TEST_ASSERT_NULL(client_list->history);
  TEST_ASSERT_EQUAL_STRING("one", client_list->username);
  
  client_list = client_list->next;

  TEST_ASSERT_NOT_NULL(client_list);
  TEST_ASSERT_EQUAL_INT(6, client_list->socket);
  TEST_ASSERT_NULL(client_list->history);
  TEST_ASSERT_EQUAL_STRING("two", client_list->username);

  free(windows);
  free_clients(args->client_list);
  free(args);
}



void test_add_client(void) {
 /* 
  *  err codes 
  *  0 = OK
  *  1 = ERR
  *  2 = NO PORT SUPPLIED
  *  3 = INVALID ADDR
  *  4 = INVALID PORT
  */

  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  client_t *client_list = NULL;

  args->client_list = &client_list;
  args->active_client = NULL;

  char *missing_port =    "127.0.0.1";
  char *missing_space =   "127.0.0.19999";
  char *missing_ip =      "9999";
  // char *double_space =    "127.0.0.1  9999"; // is OK, but need to mock connection
  char *slash_sep =       "127.0.0.1/9999";
  char *colon_sep =       "127.0.0.1:9999";
  char *port_exceeded =   "127.0.0.1 65536";
  char *random_sep =      "127.0.0.1/ 9999";
  char *invalid_ip =      "127.0.0.256 65535";

  TEST_ASSERT_EQUAL_INT(2, add_client(missing_port, args, windows));
  TEST_ASSERT_EQUAL_INT(2, add_client(missing_space, args, windows));
  TEST_ASSERT_EQUAL_INT(2, add_client(missing_ip, args, windows));
  // TEST_ASSERT_EQUAL_INT(0, add_client(double_space, args, windows)); // TODO mock?
  TEST_ASSERT_EQUAL_INT(2, add_client(slash_sep, args, windows));
  TEST_ASSERT_EQUAL_INT(2, add_client(colon_sep, args, windows));
  TEST_ASSERT_EQUAL_INT(3, add_client(port_exceeded, args, windows));
  TEST_ASSERT_EQUAL_INT(4, add_client(random_sep, args, windows));
  TEST_ASSERT_EQUAL_INT(4, add_client(invalid_ip, args, windows));

  free(windows);
  free(args);
}



void test_set_active_client(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS); 

  args_t *args = malloc(sizeof(args_t));
  client_t *client_list = NULL;
  msg_t *message_queue = NULL;

  nfds_t nfds = N_PFDS;
  nfds_t fd_count = 0;

  struct pollfd *pfds = create_pfds_array(nfds); 

  char *user1 = "user1";
  char *user2 = "user2";

  args->pfds = pfds;
  args->client_list = &client_list;
  args->active_client = NULL;

  client_t *client = create_client(4, user1);
  client_t *client1 = create_client(5, user2);
  append_client(client, args, windows);
  append_client(client1, args, windows);

  int rv = 0;

  // set to valid user OK
  rv = set_active_client(user1, args, windows);
  TEST_ASSERT_EQUAL_STRING(user1, args->active_client->username);
  TEST_ASSERT_EQUAL_INT(0, rv);

  // doesn't accept invalid string (user not changed)
  char *invalid1 = "u$er2!";
  rv = set_active_client(invalid1, args, windows);
  TEST_ASSERT_EQUAL_STRING(user1, args->active_client->username);
  TEST_ASSERT_EQUAL_INT(1, rv);

  // valid username which doesn't exist
  char *invalid2 = "user3";
  rv = set_active_client(invalid2, args, windows);
  TEST_ASSERT_EQUAL_STRING(user1, args->active_client->username);
  TEST_ASSERT_EQUAL_INT(rv, 2);

  // will set to valid user
  char *valid = "user2";
  rv = set_active_client(valid, args, windows);
  TEST_ASSERT_EQUAL_STRING(valid, args->active_client->username);
  TEST_ASSERT_EQUAL_INT(rv, 0);
  
  // free_clients(args->client_list);
  free(args);
  free(windows);
}



void test_set_client_username(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  client_t *client_list = NULL;

  args_t *args = malloc(sizeof(args_t));
  args->client_list = &client_list;
  args->active_client = NULL;

  char *user1 = "user1";
  char *user2 = "user2";

  client_t *client1 = create_client(4, user1);
  client_t *client2 = create_client(5, user2);
  append_client(client1, args, windows);
  append_client(client2, args, windows);

  int rv = 0;

  // err if active client not set
  rv = set_client_username(user1, args, windows);
  TEST_ASSERT_EQUAL_INT(2, rv);

  // sets first client to active
  rv = set_active_client(user1, args, windows);
  TEST_ASSERT_EQUAL_STRING(user1, args->active_client->username);
  TEST_ASSERT_EQUAL_INT(0, rv);

  // can change value
  char *new_username = "test";
  rv = set_client_username(new_username, args, windows);
  TEST_ASSERT_EQUAL_STRING(new_username, args->active_client->username);
  TEST_ASSERT_EQUAL_INT(0, rv);

  // doesn't accept invalid string (user not changed)
  char *invalid1 = "u$er2!";
  rv = set_client_username(invalid1, args, windows);
  TEST_ASSERT_EQUAL_STRING(new_username, args->active_client->username);
  TEST_ASSERT_EQUAL_INT(1, rv);

  // set second client to active
  set_active_client(client2->username, args, windows);

  // will set to valid username
  char *valid = "user2";
  rv = set_active_client(valid, args, windows);
  TEST_ASSERT_EQUAL_STRING(valid, args->active_client->username);
  TEST_ASSERT_EQUAL_INT(rv, 0);
}



void test_disconnect_client(void) {
  // TODO 
  
  TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose");
}



void test_remove_client(void) {
  client_t *head = NULL;
  int a, b, c, d, e, f, g, h, i; // counters
  a = b = c = d = e = f = g = h = i = 0;
  int rv = 0;

  // TODO returns err if client list is NULL

  // create client list
  while (i < 5) {
    client_t *new = malloc(sizeof(client_t));
    history_t *history_head = NULL;
    new->socket = i;
    new->next = NULL;
    new->unread_msg = 0;
    new->history = history_head;

    client_t **tmp = &head;
    while (*tmp != NULL) {
      tmp = &(*tmp)->next;
    }
    new->next = *tmp; // tmp is null
    *tmp = new;
    i++;
  }

  // client list is as expected - socks 0 1 2 3 4
  int expect[5] = { 0, 1, 2, 3, 4 };
  client_t **first = &head;
  while (*first != NULL) {
    TEST_ASSERT_EQUAL_INT(expect[a], (*first)->socket);
    first = &(*first)->next;
    a++;
  }

  // will remove client from end - socks 0 1 2 3
  rv = remove_client(4, &head);
  int expect1[4] = { 0, 1, 2, 3 };
  client_t **second = &head;
  while (*second != NULL) {
    TEST_ASSERT_EQUAL_INT(expect1[b], (*second)->socket);
    second = &(*second)->next;
    b++;
  }
  TEST_ASSERT_EQUAL_INT(0, rv);

  // will remove client form middle - socks 0 2 3
  rv = remove_client(1, &head);
  client_t **third = &head;
  int expect2[3] = { 0, 2, 3 };
  while (*third != NULL) {
    TEST_ASSERT_EQUAL_INT(expect2[c], (*third)->socket);
    third = &(*third)->next;
    c++;
  }
  TEST_ASSERT_EQUAL_INT(0, rv);

  // will remove client form start - socks 2 3
  rv = remove_client(0, &head);
  client_t **fourth = &head;
  int expect3[2] = { 2, 3 };
  while (*fourth != NULL) {
    TEST_ASSERT_EQUAL_INT(expect3[d], (*fourth)->socket);
    fourth = &(*fourth)->next;
    d++;
  }
  TEST_ASSERT_EQUAL_INT(0, rv);
  
  // returns err if client sock doesn't exist
  // rv = remove_client(5, &head);
  // client_t **fifth = &head;
  // int expect4[2] = { 2, 3 };
  // while (*fifth != NULL) {
  //   printf("TEST: %i\n", (*fifth)->socket);
  //   TEST_ASSERT_EQUAL_INT(expect4[e], (*fifth)->socket);
  //   fifth = &(*fifth)->next;
  //   e++;
  // }
  // TEST_ASSERT_EQUAL_INT(1, rv);

  free_clients(&head);
}



void test_print_clients(void)
{
  // https://stackoverflow.com/a/36304428/11350846
  TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose");
}



void test_free_clients(void) {
  client_t *head = NULL;
  int i = 0;

  while (i < 5) {
    client_t *new = malloc(sizeof(client_t));
    history_t *history_head = NULL;
    new->socket = i;
    new->next = NULL;
    new->unread_msg = 0;
    new->history = history_head;

    client_t **tmp = &head;
    while (*tmp != NULL) {
      tmp = &(*tmp)->next;
    }
    new->next = *tmp; // tmp is null in empty list
    *tmp = new;
    i++;
  }

  int rv = 0;
  rv = free_clients(&head);
  TEST_ASSERT_EQUAL_INT(0, rv);
  TEST_ASSERT_NULL(head);
}



void test_free_history(void) {
  history_t *head = NULL;
  int i = 0;
  while (i < 5) {
    packet_t *packet = malloc(sizeof(packet_t));
    snprintf(packet->body, BUFFER_LEN, "packet%i", i);

    history_t *new_msg = malloc(sizeof(history_t));
    memset(new_msg, 0, sizeof(history_t));
    new_msg->packet = packet;
    new_msg->next = NULL;

    if (head == NULL) {
      head = new_msg;
    } else {
      history_t *tmp = head;
      while (tmp->next != NULL) {
        tmp = tmp->next;
      }
      tmp->next = new_msg;
    }
    i++;
  }

  int rv = 0;
  rv = free_history(head);
  TEST_ASSERT_EQUAL_INT(0, rv);
}


int main(void) {
  printf("test-client.c\n");
  UNITY_BEGIN();
  RUN_TEST(test_create_client);
  RUN_TEST(test_append_client);
  RUN_TEST(test_add_client);
  RUN_TEST(test_set_active_client);
  RUN_TEST(test_set_client_username);
  RUN_TEST(test_remove_client);
  RUN_TEST(test_print_clients);
  RUN_TEST(test_disconnect_client);
  RUN_TEST(test_free_clients);
  RUN_TEST(test_free_history);
  return UNITY_END();
}