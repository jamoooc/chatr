#include "../unity/unity.h"
#include "../src/client.h"

void test_client_create(void) {
  client_t *client;
  client = client_create(5, "test");

  TEST_ASSERT_NOT_NULL(client);
  TEST_ASSERT_EQUAL_INT(5, client->socket);
  TEST_ASSERT_NULL(client->history);
  TEST_ASSERT_EQUAL_STRING("test", client->username);

  free(client);
}

void test_client_append(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  client_t *client_list = NULL;

  args_t *args = malloc(sizeof(args_t));
  args->client_list = &client_list;
  args->active_client = NULL;
  
  client_t *client1 = client_create(5, "one");
  client_t *client2 = client_create(6, "two");

  client_append(client1, args, windows);
  client_append(client2, args, windows);

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
  client_free(args->client_list);
  free(args);
}



void test_client_connect(void) {
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

  TEST_ASSERT_EQUAL_INT(2, client_connect(missing_port, args, windows));
  TEST_ASSERT_EQUAL_INT(2, client_connect(missing_space, args, windows));
  TEST_ASSERT_EQUAL_INT(2, client_connect(missing_ip, args, windows));
  // TEST_ASSERT_EQUAL_INT(0, client_connect(double_space, args, windows)); // TODO mock?
  TEST_ASSERT_EQUAL_INT(2, client_connect(slash_sep, args, windows));
  TEST_ASSERT_EQUAL_INT(2, client_connect(colon_sep, args, windows));
  TEST_ASSERT_EQUAL_INT(3, client_connect(port_exceeded, args, windows));
  TEST_ASSERT_EQUAL_INT(4, client_connect(random_sep, args, windows));
  TEST_ASSERT_EQUAL_INT(4, client_connect(invalid_ip, args, windows));

  free(windows);
  free(args);
}



void test_set_active_client(void) {
  WINDOW **windows = create_windows_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS); 

  client_t *client_list = NULL;
  msg_t *message_queue = NULL;
  nfds_t nfds = N_PFDS;
  nfds_t fd_count = 0;

  struct pollfd *pfds = pfd_create_array(nfds); 

  char *user1 = "user1";
  char *user2 = "user2";

  args_t *args = malloc(sizeof(args_t));
  args->pfds = pfds;
  args->client_list = &client_list;
  args->active_client = NULL;

  client_t *client = client_create(4, user1);
  client_t *client1 = client_create(5, user2);
  client_append(client, args, windows);
  client_append(client1, args, windows);

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
  
  client_free(args->client_list);
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

  client_t *client1 = client_create(4, user1);
  client_t *client2 = client_create(5, user2);
  client_append(client1, args, windows);
  client_append(client2, args, windows);

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

  client_free(args->client_list);
  free(args);
  free(windows);
}



void test_client_disconnect(void) {
  TEST_IGNORE_MESSAGE("This test was ignored on purpose.");
}



void test_client_destroy(void) {
  client_t *head = NULL;
  int a, b, c, d, e, f, g, h, i; // counters (could just reuse a couple...)
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
  rv = client_destroy(4, &head);
  int expect1[4] = { 0, 1, 2, 3 };
  client_t **second = &head;
  while (*second != NULL) {
    TEST_ASSERT_EQUAL_INT(expect1[b], (*second)->socket);
    second = &(*second)->next;
    b++;
  }
  TEST_ASSERT_EQUAL_INT(0, rv);

  // will remove client form middle - socks 0 2 3
  rv = client_destroy(1, &head);
  client_t **third = &head;
  int expect2[3] = { 0, 2, 3 };
  while (*third != NULL) {
    TEST_ASSERT_EQUAL_INT(expect2[c], (*third)->socket);
    third = &(*third)->next;
    c++;
  }
  TEST_ASSERT_EQUAL_INT(0, rv);

  // will remove client form start - socks 2 3
  rv = client_destroy(0, &head);
  client_t **fourth = &head;
  int expect3[2] = { 2, 3 };
  while (*fourth != NULL) {
    TEST_ASSERT_EQUAL_INT(expect3[d], (*fourth)->socket);
    fourth = &(*fourth)->next;
    d++;
  }
  TEST_ASSERT_EQUAL_INT(0, rv);
  
  // returns err if client sock doesn't exist
  // rv = client_destroy(5, &head);
  // client_t **fifth = &head;
  // int expect4[2] = { 2, 3 };
  // while (*fifth != NULL) {
  //   printf("TEST: %i\n", (*fifth)->socket);
  //   TEST_ASSERT_EQUAL_INT(expect4[e], (*fifth)->socket);
  //   fifth = &(*fifth)->next;
  //   e++;
  // }
  // TEST_ASSERT_EQUAL_INT(1, rv);

  client_free(&head);
}



void test_client_print(void)
{
  TEST_IGNORE_MESSAGE("TThis test was ignored on purpose.");
}



void test_client_free(void) {
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
    new->next = *tmp;
    *tmp = new;
    i++;
  }

  int rv = 0;
  rv = client_free(&head);
  TEST_ASSERT_EQUAL_INT(0, rv);
  TEST_ASSERT_NULL(head);
}



void test_client_history_free(void) {
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
  rv = client_history_free(head);
  TEST_ASSERT_EQUAL_INT(0, rv);
}
