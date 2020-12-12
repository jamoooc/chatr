#include "../unity/unity.h"
#include "../src/utils.h"
#include "../src/client.h"
#include "../src/connect.h"
#include "../src/main.h"
#include "../src/message.h"
#include "../src/pfds.h"
#include "../src/ui.h"

// #include "test-client.c"

// gcc ../test/test-client.c ../src/utils.c ../src/client.c ../src/message.c ../src/pfds.c ../src/connect.c ../src/ui.c ../unity/unity.c -lncurses

// void setUp() {

// }

// void tearDown() {

// }

void test_create_pfds_array(void) {
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = create_pfds_array(nfds);

  // creates array
  TEST_ASSERT_NOT_NULL(pfds);
  // of size ?
}



void test_insert_pfd(void) {
  nfds_t fd_count = 0;
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = create_pfds_array(nfds);

  int new_fd1 = 5;
  int new_fd2 = 6;
  int new_fd3 = 7;
  // int new_fd4 = 8;

  insert_pfd(&pfds, new_fd1, &fd_count, &nfds);
  insert_pfd(&pfds, new_fd2, &fd_count, &nfds);
  insert_pfd(&pfds, new_fd3, &fd_count, &nfds);

  TEST_ASSERT_EQUAL_INT(new_fd1, pfds[0].fd);
  TEST_ASSERT_EQUAL_INT(new_fd2, pfds[1].fd);
  TEST_ASSERT_EQUAL_INT(new_fd3, pfds[2].fd);

  // will resize pfd array
  // how to test size of array?
  // TEST_ASSERT_EQUAL_INT(sizeof(pfds),?!?!? sizeof(struct pollfd) * 3);
  // insert_pfd(&pfds, new_fd4, &fd_count, &nfds);
  // TEST_ASSERT_EQUAL_INT(new_fd1, pfds[3].fd);
}



void test_remove_pfd(void) {
  nfds_t fd_count = 0;
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = create_pfds_array(nfds);

  int new_fd1 = 5;
  int new_fd2 = 6;
  int new_fd3 = 7;

  insert_pfd(&pfds, new_fd1, &fd_count, &nfds);
  insert_pfd(&pfds, new_fd2, &fd_count, &nfds);
  insert_pfd(&pfds, new_fd3, &fd_count, &nfds);

  int i = 1;
  int last_fd = pfds[fd_count - 1].fd;
  nfds_t prev_count = fd_count;
  remove_pfd(pfds, i, &fd_count);

  // decrements fd_count
  TEST_ASSERT_EQUAL_INT(prev_count, fd_count + 1);

  // prev last pfd copied over pfd to be replaced
  TEST_ASSERT_EQUAL_INT(last_fd, pfds[i].fd);
}



// int main(void) {
//   printf("test_pfds\n");
//   UNITY_BEGIN();
//   RUN_TEST(test_create_pfds_array);
//   RUN_TEST(test_insert_pfd);
//   RUN_TEST(test_remove_pfd);


//   RUN_TEST(test_create_client);
//   RUN_TEST(test_append_client);
//   RUN_TEST(test_add_client);
//   RUN_TEST(test_set_active_client);
//   RUN_TEST(test_set_client_username);
//   RUN_TEST(test_remove_client);
//   RUN_TEST(test_print_clients);
//   RUN_TEST(test_disconnect_client);
//   RUN_TEST(test_free_clients);
//   RUN_TEST(test_free_history);


//   return UNITY_END();
// }