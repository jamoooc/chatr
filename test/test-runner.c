#include "../unity/unity.h"

#include "test-client.c"
#include "test-pfds.c"
#include "test-utils.c"
#include "test-message.c"
#include "test-connect.c"

void setUp() {

}

void tearDown() {

}

int main(void) {
  UNITY_BEGIN();
  
  printf("\ntest_pfds\n\n");
  
  RUN_TEST(test_pfd_create_array);
  RUN_TEST(test_pfd_insert);
  RUN_TEST(test_pfd_destroy);

  printf("\ntest_client\n\n");

  RUN_TEST(test_client_create);
  RUN_TEST(test_client_append);
  RUN_TEST(test_client_connect);
  RUN_TEST(test_client_print);
  RUN_TEST(test_client_disconnect);
  RUN_TEST(test_client_destroy);
  RUN_TEST(test_client_history_free);
  RUN_TEST(test_client_free);
  RUN_TEST(test_set_client_username);
  RUN_TEST(test_set_active_client);

  printf("\nutils\n\n");

  RUN_TEST(test_process_input);
  RUN_TEST(test_set_host_username);
  RUN_TEST(test_valid_port);
  RUN_TEST(test_valid_username);
  RUN_TEST(test_remove_first_char);
  RUN_TEST(test_remove_newline);
  RUN_TEST(test_remove_trailing_whitespace);

  printf("\ntest-message\n\n");

  RUN_TEST(test_history_insert);
  RUN_TEST(test_history_print);

  RUN_TEST(test_message_create);
  RUN_TEST(test_message_append);
  RUN_TEST(test_message_destroy);
  RUN_TEST(test_message_free);

  printf("\nconnect\n\n");

  RUN_TEST(test_accept_connection);
  RUN_TEST(test_packet_transmit);
  RUN_TEST(test_packet_receive);
  RUN_TEST(test_init_server);
  
  return UNITY_END();
}