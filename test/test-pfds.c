#include "../unity/unity.h"
#include "../src/pfds.h"

void test_pfd_create_array(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = pfd_create_array(nfds, args, windows);

  TEST_ASSERT_NOT_NULL(pfds);
  for (int i = 0; i < nfds; i++) {
    TEST_ASSERT_EQUAL_INT(-1, pfds[i].fd);
    TEST_ASSERT_EQUAL_INT(0, pfds[i].events);
    TEST_ASSERT_EQUAL_INT(0, pfds[i].revents);
  }

  // of size ?
  free(windows);
  free(args);
  free(pfds);
}



void test_pfd_insert(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  nfds_t fd_count = 0;
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = pfd_create_array(nfds, args, windows);

  int new_fd1 = 5;
  int new_fd2 = 6;
  int new_fd3 = 7;
  int new_fd4 = 8;

  pfd_insert(&pfds, new_fd1, &fd_count, &nfds);
  pfd_insert(&pfds, new_fd2, &fd_count, &nfds);
  pfd_insert(&pfds, new_fd3, &fd_count, &nfds);

  TEST_ASSERT_EQUAL_INT(new_fd1, pfds[0].fd);
  TEST_ASSERT_EQUAL_INT(new_fd2, pfds[1].fd);
  TEST_ASSERT_EQUAL_INT(new_fd3, pfds[2].fd);

  // will realloc if fd_count == nfds (new fds are -1)
  pfd_insert(&pfds, new_fd4, &fd_count, &nfds);
  TEST_ASSERT_EQUAL_INT(6, nfds); // doubles arr size

  for (int i = 5; i < nfds; i++) {
    TEST_ASSERT_EQUAL_INT(-1, pfds[i].fd);
    TEST_ASSERT_EQUAL_INT(0, pfds[i].events);
    TEST_ASSERT_EQUAL_INT(0, pfds[i].revents);
  }
  
  free(windows);
  free(args);
  free(pfds);
}



void test_pfd_destroy(void) {
  WINDOW **windows = window_create_array(N_WINDOWS);
  memset(windows, '\0', sizeof(*windows) * N_WINDOWS);

  args_t *args = malloc(sizeof(args_t));
  nfds_t fd_count = 0;
  nfds_t nfds = N_PFDS;
  struct pollfd *pfds = pfd_create_array(nfds, args, windows);

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

  free(windows);
  free(args);
  free(pfds);
}
