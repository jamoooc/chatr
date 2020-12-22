#include "../unity/unity.h"
#include "../src/pfds.h"

void test_pfd_create_array(void) {
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = pfd_create_array(nfds);

  // creates array
  TEST_ASSERT_NOT_NULL(pfds);
  // of size ?
  free(pfds);
}



void test_pfd_insert(void) {
  nfds_t fd_count = 0;
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = pfd_create_array(nfds);

  int new_fd1 = 5;
  int new_fd2 = 6;
  int new_fd3 = 7;
  // int new_fd4 = 8;

  pfd_insert(&pfds, new_fd1, &fd_count, &nfds);
  pfd_insert(&pfds, new_fd2, &fd_count, &nfds);
  pfd_insert(&pfds, new_fd3, &fd_count, &nfds);

  TEST_ASSERT_EQUAL_INT(new_fd1, pfds[0].fd);
  TEST_ASSERT_EQUAL_INT(new_fd2, pfds[1].fd);
  TEST_ASSERT_EQUAL_INT(new_fd3, pfds[2].fd);

  // test realloc by size of new array?

  free(pfds);
}



void test_pfd_destroy(void) {
  nfds_t fd_count = 0;
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = pfd_create_array(nfds);

  int new_fd1 = 5;
  int new_fd2 = 6;
  int new_fd3 = 7;

  pfd_insert(&pfds, new_fd1, &fd_count, &nfds);
  pfd_insert(&pfds, new_fd2, &fd_count, &nfds);
  pfd_insert(&pfds, new_fd3, &fd_count, &nfds);

  int i = 1;
  int last_fd = pfds[fd_count - 1].fd;
  nfds_t prev_count = fd_count;
  pfd_destroy(pfds, i, &fd_count);

  // decrements fd_count
  TEST_ASSERT_EQUAL_INT(prev_count, fd_count + 1);

  // prev last pfd copied over pfd to be replaced
  TEST_ASSERT_EQUAL_INT(last_fd, pfds[i].fd);

  free(pfds);
}
