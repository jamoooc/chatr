#ifndef PFDS_H
#define PFDS_H

#include <poll.h>
#include <stdlib.h>
#include <stdio.h>

void insert_pfd(struct pollfd *pfds[], int new_fd, nfds_t *nfds, nfds_t *fd_count);
// description
void remove_pfd(struct pollfd pfds[], int i, nfds_t *fd_count);
// description
struct pollfd *create_pfds_array(nfds_t nfds);
// description
void free_pfds(struct pollfd *pfds[], nfds_t *nfds);

#endif