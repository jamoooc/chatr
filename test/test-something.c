#include "../unity/unity.h"
#include "../src/utils.h"

// gcc ../src/pfds.c test-something.c ../unity/unity.c -o test

void setUp() {
  /* run before each test */
}

void tearDown() {
  /* run after each test */
}

void test_IgnoredTest(void)
{
  TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose");
}

int main(void)
{
  printf("unity\n");
  UNITY_BEGIN();
  RUN_TEST(test_IgnoredTest);
  return UNITY_END();
}