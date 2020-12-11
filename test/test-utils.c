#include "../unity/unity.h"
#include "../src/utils.h"
#include "../src/client.h"
#include "../src/connect.h"
#include "../src/main.h"
#include "../src/message.h"
#include "../src/pfds.h"
#include "../src/ui.h"

// gcc ../src/pfds.c test-something.c ../unity/unity.c -o test
// gcc ../test/test-something.c ../src/utils.c ../src/main.c ../src/main.h ../src/connect.c ../src/client.c ../src/message.c ../src/pfds.c ../src/ui.c ../unity/unity.c -lncurses

// this works MAKEFILE!
// gcc ../test/test-something.c ../src/utils.c ../src/client.c ../src/message.c ../src/pfds.c ../src/connect.c ../unity/unity.c -lncurses


void setUp() {
  /* run before each test */
}

void tearDown() {
  /* run after each test */
}

void test_process_input(void) {
  TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose");
}

void test_set_host_username(void) {
  char *input = "username";
  args_t *args = malloc(sizeof(args_t));
  WINDOW **windows = create_windows_array(N_WINDOWS);
  set_host_username(input, args, windows);

  TEST_ASSERT_EQUAL_STRING(input, args->host_username);
  TEST_ASSERT_EQUAL_STRING_LEN(input, args->host_username, 9);
  
  free(args);
  free_windows(windows);
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

  TEST_ASSERT_TRUE(valid_username(a));
  TEST_ASSERT_TRUE(valid_username(b));

  TEST_ASSERT_FALSE(valid_username(c));
  TEST_ASSERT_FALSE(valid_username(d));
  TEST_ASSERT_FALSE(valid_username(e));
  TEST_ASSERT_FALSE(valid_username(f));
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
  TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose");
}

int main(void) {
  printf("unity\n");
  UNITY_BEGIN();
  RUN_TEST(test_process_input);
  RUN_TEST(test_set_host_username);
  RUN_TEST(test_valid_port);
  RUN_TEST(test_valid_username);
  RUN_TEST(test_remove_first_char);
  RUN_TEST(test_remove_newline);
  RUN_TEST(test_remove_trailing_whitespace);
  return UNITY_END();
}