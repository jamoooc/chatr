#include "../unity/unity.h"
#include "../src/utils.h"

void test_get_input(void) {
  TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose");
}

void test_handle_input(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  client_t *client_list = NULL;
  msg_t *message_queue = NULL;
  args_t *args = malloc(sizeof(args_t));

  args->message_queue = &message_queue;
  args->client_list = &client_list;
  args->active_client = NULL;
  args->quit = 1;

  // create clients
  client_create(4, "user1", args, windows);
  client_create(5, "user2", args, windows);

  // if input /quit, exit
  char *exit = "/quit";
  handle_input(exit, args, windows);
  TEST_ASSERT_EQUAL_INT(0, args->quit);

  // sets active client
  char *set_active = malloc(sizeof(char) * USERNAME_LEN);
  char *expect_active = malloc(sizeof(char) * USERNAME_LEN);
  strcpy(set_active, "@user2");
  strcpy(expect_active, "user2");
  handle_input(set_active, args, windows);
  TEST_ASSERT_EQUAL_STRING(expect_active, args->active_client->username);

  // sets host username
  char *set_host = malloc(sizeof(char) * 30);
  char *expect_host = malloc(sizeof(char) * USERNAME_LEN);
  strcpy(set_host, "$hostname");
  strcpy(expect_host, "hostname");
  handle_input(set_host, args, windows);
  TEST_ASSERT_EQUAL_STRING(set_host, args->host_username);
  
  // adds new client
  // TODO

  // adds message to queue
  char *q_msg = malloc(sizeof(char) * BUFFER_LEN);
  char *expect_q_msg = malloc(sizeof(char) * BUFFER_LEN);
  strcpy(q_msg, "this is a message");
  strcpy(expect_q_msg, "this is a message");
  handle_input(q_msg, args, windows);
  msg_t *msg = *args->message_queue;
  TEST_ASSERT_EQUAL_STRING(expect_q_msg, msg->packet->body);
  TEST_ASSERT_EQUAL_STRING(args->active_client->username, msg->client->username);
  TEST_ASSERT_EQUAL_STRING(args->host_username, msg->packet->username);

  free(set_active);
  free(expect_active);
  free(set_host);
  free(expect_host);
  free(q_msg);
  free(expect_q_msg);
  window_free(windows);
  free(args);
}



void test_set_host_username(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);
  args_t *args = malloc(sizeof(args_t));

  char *input = malloc(sizeof(char) * 9);
  strcpy(input, "username");

  set_host_username(input, args, windows);

  TEST_ASSERT_EQUAL_STRING(input, args->host_username);
  TEST_ASSERT_EQUAL_STRING_LEN(input, args->host_username, 9);
  
  free(input);
  free(args);
  window_free(windows);
}

void test_valid_port(void) {
  // true
  char *a = "0";
  char *b = "2345";
  char *c = "65535";

  // false
  char *d = "-1";
  char *e = "+1234";
  char *f = "-1234";
  char *g = "65536";
  char *h = "+65536";
  char *i = "-65535";
  char *j = "str";
  char *k = "long_string!";
  char *l = "65535     ";
  char *m = "    ";
  char *n = "!@£$*";
  char *o = "   1234";
  char *p = "00000";
  char *q = "345 45";
  
  TEST_ASSERT_TRUE(valid_port(a));
  TEST_ASSERT_TRUE(valid_port(b));
  TEST_ASSERT_TRUE(valid_port(c));

  TEST_ASSERT_FALSE(valid_port(d));
  TEST_ASSERT_FALSE(valid_port(e));
  TEST_ASSERT_FALSE(valid_port(f));
  TEST_ASSERT_FALSE(valid_port(g));
  TEST_ASSERT_FALSE(valid_port(h));
  TEST_ASSERT_FALSE(valid_port(i));
  TEST_ASSERT_FALSE(valid_port(j));
  TEST_ASSERT_FALSE(valid_port(k));
  TEST_ASSERT_FALSE(valid_port(m));
  TEST_ASSERT_FALSE(valid_port(n));
  TEST_ASSERT_FALSE(valid_port(o));
  TEST_ASSERT_FALSE(valid_port(p));
  TEST_ASSERT_FALSE(valid_port(q));
}

void test_valid_username(void) {
  char *a = "valid";
  char *b = "abcdefghijklmnopqrstuvwxyz1234";
  char *c = "abcdefghijklmnopqrstuvwxyz12345";
  char *d = "!nvalid";
  char *e = "!@£$^&*";
  char *f = "i n v a l i d";
  char *g = "i_n_v_a-l-i-d";

  TEST_ASSERT_TRUE(valid_username(a));
  TEST_ASSERT_TRUE(valid_username(b));

  TEST_ASSERT_FALSE(valid_username(c));
  TEST_ASSERT_FALSE(valid_username(d));
  TEST_ASSERT_FALSE(valid_username(e));
  TEST_ASSERT_FALSE(valid_username(f));
  TEST_ASSERT_FALSE(valid_username(g));
}

void test_remove_first_char(void) {
  char *str1 =    malloc(sizeof(char) * 6);
  char *str2 =    malloc(sizeof(char) * 2);
  char *str3 =    malloc(sizeof(char) * 2);
  char *str4 =    malloc(sizeof(char) * 3);
  char *str5 =    malloc(sizeof(char) * 3);
  char *str6 =    malloc(sizeof(char) * 12);
  char *str7 =    malloc(sizeof(char) * 6);
  char *str8 =    malloc(sizeof(char) * 5);
  char *str9 =    malloc(sizeof(char) * 21);
  char *str10 =   malloc(sizeof(char) * 21);
  char *str11 =   malloc(sizeof(char) * 5);
  
  strcpy(str1,    "!test");
  strcpy(str2,    "!");  
  strcpy(str3,    "@");  
  strcpy(str4,    "$");  
  strcpy(str5,    "#");  
  strcpy(str6,    "!@$#");
  strcpy(str7,    "last!");  
  strcpy(str8,    "s!cond char");  
  strcpy(str9,    "!return exact string");  
  strcpy(str10,   "@#with 2 characters!");  
  strcpy(str11,   "!OL$");

  remove_first_char(str1);
  remove_first_char(str2);
  remove_first_char(str3);
  remove_first_char(str4);
  remove_first_char(str5);
  remove_first_char(str6);
  remove_first_char(str7);
  remove_first_char(str8);
  remove_first_char(str9);
  remove_first_char(str10);
  remove_first_char(str11);

  TEST_ASSERT_EQUAL_STRING("test",                str1);
  TEST_ASSERT_EQUAL_STRING("",                    str2);
  TEST_ASSERT_EQUAL_STRING("",                    str3);
  TEST_ASSERT_EQUAL_STRING("",                    str4);
  TEST_ASSERT_EQUAL_STRING("",                    str5);
  TEST_ASSERT_EQUAL_STRING("@$#",                 str6);
  TEST_ASSERT_EQUAL_STRING("last!",               str7);
  TEST_ASSERT_EQUAL_STRING("s!cond char",         str8);
  TEST_ASSERT_EQUAL_STRING("return exact string", str9);
  TEST_ASSERT_EQUAL_STRING("#with 2 characters!", str10);
  TEST_ASSERT_EQUAL_STRING("OL$",                 str11);

  free(str1);
  free(str2);
  free(str3);
  free(str4);
  free(str5);
  free(str6);
  free(str7);
  free(str8);
  free(str9);
  free(str10);
  free(str11);
}

void test_remove_newline(void) {
  char *str1 = malloc(sizeof(char) * 2);
  char *str2 = malloc(sizeof(char) * 3);
  char *str3 = malloc(sizeof(char) * 5);
  char *str4 = malloc(sizeof(char) * 6);
  char *str5 = malloc(sizeof(char) * 7);

  strcpy(str1, "\n");
  strcpy(str2, " \n");
  strcpy(str3, "sdfg");
  strcpy(str4, "sdfg\n");
  strcpy(str5, "sdfg\r\n");

  remove_newline(str1);
  remove_newline(str2);
  remove_newline(str3);
  remove_newline(str4);
  remove_newline(str5);

  TEST_ASSERT_EQUAL_STRING("",        str1);
  TEST_ASSERT_EQUAL_STRING(" ",       str2);
  TEST_ASSERT_EQUAL_STRING("sdfg",    str3);
  TEST_ASSERT_EQUAL_STRING("sdfg",    str4);
  TEST_ASSERT_EQUAL_STRING("sdfg\r",  str5);

  free(str1);
  free(str2);
  free(str3);
  free(str4);
  free(str5);
}

void test_remove_trailing_whitespace(void) {
  char *str1 = malloc(sizeof(char) * 6);
  char *str2 = malloc(sizeof(char) * 11);
  char *str3 = malloc(sizeof(char) * 15);
  char *str4 = malloc(sizeof(char) * 10);
  char *str5 = malloc(sizeof(char) * 19);
  char *str6 = malloc(sizeof(char) * 25);
  char *str7 = malloc(sizeof(char) * 30);
  char *str8 = malloc(sizeof(char) * 22);
  char *str9 = malloc(sizeof(char) * 27);
  char *str10 = malloc(sizeof(char) * 15);
  char *str11 = malloc(sizeof(char) * 19);

  strcpy(str1,    "hello");
  strcpy(str2,    "one_space ");
  strcpy(str3,    "multi_space   ");
  strcpy(str4,    "mid space");
  strcpy(str5,    "mid space_trail   ");
  strcpy(str6,    "multi mid space trail   ");
  strcpy(str7,    "don't remove special char$   ");
  strcpy(str8,    "even at the end     !");
  strcpy(str9,    "   leave leading space    ");
  strcpy(str10,   "removes tabs\t\t\t");
  strcpy(str11,   "removes newlines\n\n\n");

  remove_trailing_whitespace(str1);
  remove_trailing_whitespace(str2);
  remove_trailing_whitespace(str3);
  remove_trailing_whitespace(str4);
  remove_trailing_whitespace(str5);
  remove_trailing_whitespace(str6);
  remove_trailing_whitespace(str7);
  remove_trailing_whitespace(str8);
  remove_trailing_whitespace(str9);
  remove_trailing_whitespace(str10);
  remove_trailing_whitespace(str11);

  TEST_ASSERT_EQUAL_STRING("hello",                       str1);
  TEST_ASSERT_EQUAL_STRING("one_space",                   str2);
  TEST_ASSERT_EQUAL_STRING("multi_space",                 str3);
  TEST_ASSERT_EQUAL_STRING("mid space",                   str4);
  TEST_ASSERT_EQUAL_STRING("mid space_trail",             str5);
  TEST_ASSERT_EQUAL_STRING("multi mid space trail",       str6);
  TEST_ASSERT_EQUAL_STRING("don't remove special char$",  str7);
  TEST_ASSERT_EQUAL_STRING("even at the end     !",       str8);
  TEST_ASSERT_EQUAL_STRING("   leave leading space",      str9);
  TEST_ASSERT_EQUAL_STRING("removes tabs",                str10);
  TEST_ASSERT_EQUAL_STRING("removes newlines",            str11);

  free(str1);
  free(str2);
  free(str3);
  free(str4);
  free(str5);
  free(str6);
  free(str7);
  free(str8);
  free(str9);
  free(str10);
  free(str11);
}

void test_handle_error() {
  WINDOW **windows = window_create_array(N_WINDOWS);
  // memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  // init manually as cannot run ncurses within test
  windows[0] = newwin(1,1,1,1);
  windows[1] = newwin(1,1,1,1);
  windows[2] = newwin(1,1,1,1);
  windows[3] = newwin(1,1,1,1);
  windows[4] = newwin(1,1,1,1);
  windows[5] = newwin(1,1,1,1);

  args_t *args = malloc(sizeof(args_t));
  msg_t *message_queue = NULL;
  client_t *client_list = NULL;
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = pfd_create_array(nfds, args, windows); 

  // init args freed by handle_error
  args->client_list = &client_list;
  args->message_queue = &message_queue;
  args->pfds = pfds;

  // active user
  char *test_user = malloc(sizeof(char) * 9);
  strcpy(test_user, "testuser");
  client_create(4, test_user, args, windows);
  args->active_client = client_list;

  // for msg in queue
  char *hostname = malloc(sizeof(char) * 9);
  strcpy(hostname, "hostname");
  strcpy(args->host_username, hostname);
  
  // msg in queue 
  char *msg = malloc(sizeof(char) * 5);
  strcpy(msg, "msg");
  message_create(msg, args, windows);

  TEST_ASSERT_NOT_NULL(windows);
  TEST_ASSERT_NOT_NULL(args->pfds);
  TEST_ASSERT_NOT_NULL(client_list);
  TEST_ASSERT_NOT_NULL(message_queue);
  TEST_ASSERT_NOT_NULL(args);

  errno = 5; // !?
  int rv = 1;

  handle_error(rv, "func-name", args, windows);

  // check logfile produced by handle_error
  FILE *file = fopen("logfile.txt", "r");

  char c;
  int i = 0;
  char actual[100] = { '\0' };
  while ((c = fgetc(file)) != EOF) {
    actual[i++] = c;
  }

  char *expected = "func-name failed with error code 1. errno: Input/output error.";
  TEST_ASSERT_EQUAL_STRING(expected, actual);

  TEST_ASSERT_NULL(windows[0]);
  TEST_ASSERT_NULL(windows[1]);
  TEST_ASSERT_NULL(windows[2]);
  TEST_ASSERT_NULL(windows[3]);
  TEST_ASSERT_NULL(windows[4]);
  TEST_ASSERT_NULL(windows[5]);

  // windows, pfds, client_list, msg_queue & args freed and set to NULL in handle_error
  TEST_ASSERT_NULL(*windows);
  TEST_ASSERT_NULL(args->pfds);
  TEST_ASSERT_NULL(client_list);
  TEST_ASSERT_NULL(message_queue);
  // TEST_ASSERT_NULL(args);

  free(test_user);
  free(hostname);
  free(msg);
}
