#include "pfds.h"

/* pfd_create_array */

struct pollfd *pfd_create_array(nfds_t nfds, args_t *args, WINDOW **windows) {
  struct pollfd *pfds;
  if ((pfds = malloc(sizeof(*pfds) * nfds)) == NULL) {
    handle_error(-1, "pfd_create_array: malloc,", args, windows); // TODO err val
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < nfds; i++) {
    pfds[i].fd = -1;
    pfds[i].events = 0;
    pfds[i].revents = 0;
  }
  return pfds;
}

/* pfd_insert */

void pfd_insert(struct pollfd *pfds[], int new_fd, nfds_t *fd_count, nfds_t *nfds) {
  // resize poll_fd array if necessary
  if (*fd_count == *nfds) {
    *nfds *= 2;
    *pfds = realloc(*pfds, sizeof(**pfds) * (size_t)(*nfds));

    // set new pfds null
    for (nfds_t i = *fd_count; i < *nfds; i++) {
      (*pfds)[i].fd = -1;
      (*pfds)[i].events = 0;
      (*pfds)[i].revents = 0;
    }
  }
  (*pfds)[*fd_count].fd = new_fd;
  (*pfds)[*fd_count].events = POLLIN | POLLOUT;
  (*pfds)[*fd_count].revents = 0;
  
  (*fd_count)++;
}

/* remove pfd */

void pfd_destroy(struct pollfd pfds[], int i, nfds_t *fd_count) {
  // copy last pfd over el to remove
  pfds[i] = pfds[*fd_count - 1];

  // reset copied pfd and decrement count
  pfds[*fd_count - 1].fd = -1; // negative fd ignored by poll
  pfds[*fd_count - 1].revents = 0;
  (*fd_count)--;
}
