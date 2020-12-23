#ifndef PFDS_H
#define PFDS_H

#include <poll.h>
#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "ui.h"
#include "utils.h"

void pfd_insert(struct pollfd *pfds[], int new_fd, nfds_t *nfds, nfds_t *fd_count);
// description
void pfd_destroy(struct pollfd pfds[], int i, nfds_t *fd_count);
// description
void pfd_free(struct pollfd *pfds[], nfds_t *nfds);
// description
struct pollfd *pfd_create_array(nfds_t nfds, args_t *args, WINDOW **windows);
// description

#endif