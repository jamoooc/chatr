#include "../unity/unity.h"
#include "../src/utils.h"
#include "../src/client.h"
#include "../src/connect.h"
#include "../src/main.h"
#include "../src/message.h"
#include "../src/pfds.h"
#include "../src/ui.h"

// gcc ../test/test-client.c ../src/utils.c ../src/client.c ../src/message.c ../src/pfds.c ../src/connect.c ../src/ui.c ../unity/unity.c -lncurses

void setUp() {

}

void tearDown() {

}

void test_main(void) {
  TEST_IGNORE_MESSAGE("todo");
}


int main(void) {
  // printf("unity\n");
  UNITY_BEGIN();
  RUN_TEST(test_main);

  return UNITY_END();
}