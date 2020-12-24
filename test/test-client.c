#include "../unity/unity.h"
#include "../src/client.h"

/* test_client_create */

void test_client_create(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  client_t *client_list = NULL;
  args->client_list = &client_list;

  char *username = malloc(sizeof(char) * 9);
  int i;

  // create client list
  for (i = 0; i < 5; i++) {
    snprintf(username, sizeof(username), "client%i", i);
    client_create(i, username, args, windows);
  }

  // client_list has correct values
  client_t *client = *args->client_list;
  for (i = 0; client != NULL; i++) {
    snprintf(username, sizeof(username), "client%i", i);

    TEST_ASSERT_NOT_NULL(client);
    TEST_ASSERT_EQUAL_INT(i, client->socket);
    TEST_ASSERT_NULL(client->history);
    TEST_ASSERT_EQUAL_STRING(username, client->username);
    client = client->next;
  }
  // list has correct num of clients
  TEST_ASSERT_EQUAL_INT(5, i);

  free(args);
  free(windows);
  free(username);
  client_free(args->client_list);
}

/* test_client_connect */

void test_client_connect(void) {
 /* 
  *  err codes 
  *  0 = OK
  *  1 = ERR
  *  2 = NO PORT SUPPLIED
  *  3 = INVALID PORT
  *  4 = INVALID ADDR
  */

  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  client_t *client_list = NULL;

  args->client_list = &client_list;
  args->active_client = NULL;

  // OK
  // char *double_space = "127.0.0.1  9999"; // is OK, but need to mock
  // no port
  char *missing_port =    "127.0.0.1";
  char *missing_space =   "127.0.0.19999";
  char *missing_ip =      "9999";
  char *slash_sep =       "127.0.0.1/9999";
  char *colon_sep =       "127.0.0.1:9999";
  // invalid port
  char *port_exceeded =   "127.0.0.1 65536";
  char *random_sep =      "127.0.0.1/ 9999";
  // invalid addr
  char *invalid_ip =      "127.0.0.256 65535";

  // ok
  // TEST_ASSERT_EQUAL_INT(0, client_connect(double_space, args, windows)); // TODO mock?
  // no port
  TEST_ASSERT_EQUAL_INT(2, client_connect(missing_port, args, windows));
  TEST_ASSERT_EQUAL_INT(2, client_connect(missing_space, args, windows));
  TEST_ASSERT_EQUAL_INT(2, client_connect(missing_ip, args, windows));
  TEST_ASSERT_EQUAL_INT(2, client_connect(slash_sep, args, windows));
  TEST_ASSERT_EQUAL_INT(2, client_connect(colon_sep, args, windows));
  // invalid port
  TEST_ASSERT_EQUAL_INT(3, client_connect(port_exceeded, args, windows));
  // invalid addr
  TEST_ASSERT_EQUAL_INT(4, client_connect(random_sep, args, windows));
  TEST_ASSERT_EQUAL_INT(4, client_connect(invalid_ip, args, windows));

  free(windows);
  free(args);
}

/* test_set_active_client */

void test_set_active_client(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS); 

  args_t *args = malloc(sizeof(args_t));
  client_t *client_list = NULL;
  msg_t *message_queue = NULL;
  nfds_t nfds = N_PFDS;
  nfds_t fd_count = 0;

  char *user1 = "user1";
  char *user2 = "user2";

  args->client_list = &client_list;
  args->active_client = NULL;

  client_create(4, user1, args, windows);
  client_create(5, user2, args, windows);

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

/* test_set_client_username */

void test_set_client_username(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  client_t *client_list = NULL;

  args_t *args = malloc(sizeof(args_t));
  args->client_list = &client_list;
  args->active_client = NULL;

  char *user1 = "user1";
  char *user2 = "user2";

  client_create(4, user1, args, windows);
  client_create(5, user2, args, windows);

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
  set_active_client(user2, args, windows);

  // will set to valid username
  char *valid = "user2";
  rv = set_active_client(valid, args, windows);
  TEST_ASSERT_EQUAL_STRING(valid, args->active_client->username);
  TEST_ASSERT_EQUAL_INT(rv, 0);

  client_free(args->client_list);
  free(args);
  free(windows);
}

/* test_client_disconnect */

void test_client_disconnect(void) {
  TEST_IGNORE_MESSAGE("This test was ignored on purpose.");
}

/* test_client_destroy */

void test_client_destroy(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  client_t *client_list = NULL;
  args->client_list = &client_list;

  char *username = malloc(sizeof(char) * 9);
  int rv, i;

  for (i = 0; i < 5; i++) {
    snprintf(username, sizeof(username), "client%i", i);
    client_create(i, username, args, windows);
  }

  // client list is as expected - socks 0 1 2 3 4
  int expect[5] = { 0, 1, 2, 3, 4 };
  client_t *client = *args->client_list;
  i = 0;
  while (client != NULL) {
    TEST_ASSERT_EQUAL_INT(expect[i], client->socket);
    client = client->next;
    i++;
  }

  // // will remove client from end - socks 0 1 2 3
  rv = client_destroy(4, args->client_list);
  int expect1[4] = { 0, 1, 2, 3 };
  client = *args->client_list;
  i = 0;
  while (client != NULL) {
    TEST_ASSERT_EQUAL_INT(expect1[i], client->socket);
    client = client->next;
    i++;
  }
  TEST_ASSERT_EQUAL_INT(0, rv);

  // will remove client form middle - socks 0 2 3
  rv = client_destroy(1, args->client_list);
  client = *args->client_list;
  int expect2[3] = { 0, 2, 3 };
  i = 0;
  while (client != NULL) {
    TEST_ASSERT_EQUAL_INT(expect2[i], client->socket);
    client = client->next;
    i++;
  }
  TEST_ASSERT_EQUAL_INT(0, rv);

  // will remove client form start - socks 2 3
  rv = client_destroy(0, args->client_list);
  client = *args->client_list;
  int expect3[2] = { 2, 3 };
  i = 0;
  while (client != NULL) {
    TEST_ASSERT_EQUAL_INT(expect3[i], client->socket);
    client = client->next;
    i++;
  }
  TEST_ASSERT_EQUAL_INT(0, rv);
  
  // // returns err if client sock doesn't exist
  // // rv = client_destroy(5, &head);
  // // client_t **fifth = &head;
  // // int expect4[2] = { 2, 3 };
  // // while (*fifth != NULL) {
  // //   printf("TEST: %i\n", (*fifth)->socket);
  // //   TEST_ASSERT_EQUAL_INT(expect4[e], (*fifth)->socket);
  // //   fifth = &(*fifth)->next;
  // //   e++;
  // // }
  // // TEST_ASSERT_EQUAL_INT(1, rv);
  free(args);
  free(windows);
  free(username);
  client_free(args->client_list);
}

/* test_client_print */

void test_client_print(void)
{
  TEST_IGNORE_MESSAGE("This test was ignored on purpose.");
}

/* test_client_free */

void test_client_free(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  client_t *client_list = NULL;
  args->client_list = &client_list;

  char *username = malloc(sizeof(char) * 9);
  int rv, i;

  for (i = 0; i < 5; i++) {
    snprintf(username, sizeof(username), "client%i", i);
    client_create(i, username, args, windows);
  }

  TEST_ASSERT_NOT_NULL(*args->client_list);
  TEST_ASSERT_NOT_NULL(args->client_list);
  
  rv = client_free(args->client_list);
  TEST_ASSERT_EQUAL_INT(0, rv);

  TEST_ASSERT_NULL(*args->client_list);

  free(windows);
  client_free(args->client_list);
  free(args);
}

/* test_client_history_free */

void test_client_history_free(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  client_t *client_list = NULL;
  args->client_list = &client_list;

  int rv, i = 0;

  char *username = malloc(sizeof(char) * 7);
  snprintf(username, sizeof(username), "client");
  client_create(i, username, args, windows);
  client_t *client = *args->client_list;
  set_active_client("client", args, windows);

  // rv 1 if history is NULL
  rv = client_history_free(&client->history);
  TEST_ASSERT_EQUAL_INT(1, rv);

  // create client msg history
  packet_t *packet = malloc(sizeof(packet_t));
  while (i < 5) {
    snprintf(packet->body, BUFFER_LEN, "packet%i", i);
    snprintf(packet->username, USERNAME_LEN, "client");
    history_insert(packet, client, args, windows);
    i++;
  }

  i = 0;
  // client history is as expected
  history_t *hist = client->history;
  while (hist != NULL) {
    snprintf(packet->body, BUFFER_LEN, "packet%i", i);
    TEST_ASSERT_NOT_NULL(client->history);
    TEST_ASSERT_EQUAL_STRING(packet->body, hist->packet->body);
    TEST_ASSERT_EQUAL_STRING(username, hist->packet->username);
    hist = hist->next;
    i++;
  }

  TEST_ASSERT_NOT_NULL(client->history);

  rv = client_history_free(&client->history);
  TEST_ASSERT_EQUAL_INT(0, rv);

  hist = client->history;
  while (hist != NULL) {
    printf("TEST %s\n", hist->packet->body);
    hist = hist->next;
  }
  TEST_ASSERT_NULL(hist);

  free(args);
  free(windows);
  free(username);
}
