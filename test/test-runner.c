#include "../unity/unity.h"

#include "test-client.c"
#include "test-pfds.c"
#include "test-utils.c"
#include "test-message.c"

void setUp() {

}

void tearDown() {

}

int main(void) {
  UNITY_BEGIN();
  
  printf("\ntest_pfds\n\n");
  
  RUN_TEST(test_create_pfds_array);
  RUN_TEST(test_insert_pfd);
  RUN_TEST(test_remove_pfd);

  printf("\ntest_client\n\n");

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

  printf("\nutils\n\n");

  // RUN_TEST(test_process_input);
  // RUN_TEST(test_set_host_username); // SEG!
  RUN_TEST(test_valid_port);
  RUN_TEST(test_valid_username);
  RUN_TEST(test_remove_first_char);
  RUN_TEST(test_remove_newline);
  RUN_TEST(test_remove_trailing_whitespace);

  printf("\ntest-message\n\n");

  RUN_TEST(test_insert_history);
  RUN_TEST(test_print_history);
  RUN_TEST(test_create_message);
  RUN_TEST(test_append_message);
  RUN_TEST(test_remove_message);
  RUN_TEST(test_free_messages);

  return UNITY_END();
}