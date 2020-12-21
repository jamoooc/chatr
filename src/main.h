#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>         // malloc, free, exit,
#include <string.h>         // strlen
#include <fcntl.h>          // fcntl
#include <arpa/inet.h>      // inet_pton, inet_ntop
#include <poll.h>           // nfds_t
#include <libgen.h>         // basename
#include <errno.h>          // errno
#include <getopt.h>         // getopt


// #include "client.h"
// #include "connect.h"
// #include "message.h"
// #include "pfds.h"
// #include "ui.h"
// #include "utils.h"

// #include <ncurses.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <unistd.h>         
// #include <netinet/in.h>
// #include <assert.h>
// #include <ctype.h>
// #include <netdb.h>          

/* integer constants */

#define BACKLOG           10
#define USERNAME_LEN      30
#define BUFFER_LEN        256
#define MIN_CLIENT_SOCK   2
#define N_WINDOWS         6
#define N_PFDS            3

/* string constants */

#define OPTSTR            ":p:hv" // template string for getopt
#define USAGE_FMT         "%s [-p localport 0-65535] [-h help] [-v version]\n"
#define DEFAULT_PROGNAME  "chatr"
#define VERSION           "0.1.0"
#define INVALID_PORT      "Invalid port: 0 - 65535"
#define INVALID_IPV4_ADDR "Invalid IPv4 address"
#define INVALID_USERNAME  "Username can only contain alphanumeric characters"
#define ACTIVE_CLIENT_SET "Active client is"
#define UNKNOWN_CLIENT    "Unknown client"

extern char *optarg;                // ptr to option argument
extern int opterr, optind, optopt;  // used by getopt()

/* windows */

enum WIN_INDEX {
  CLIENTS,
  INFO,
  HISTORY,
  USAGE,
  INPUT,
  I_BORDER,
};


/* structs */


/* options_t */

typedef struct options {
  // int                 verbose;
  unsigned int        port;
} options_t;

/* packet_t */

typedef struct packet {
  char                username[USERNAME_LEN];
  char                body[BUFFER_LEN];
} packet_t;

/* history_t */

typedef struct history {
  packet_t            *packet;
  struct history      *next;
} history_t;

/* client_t - connected clients linked list */

typedef struct client {
  int                 socket;
  int                 unread_msg;
  char                username[USERNAME_LEN];
  history_t           *history;
  struct client       *next;
} client_t; 

/* msg_t - queue messages linked list */

typedef struct message {
  int                 socket;
  client_t            *client;
  packet_t            *packet;
  struct message      *next;
} msg_t;

/* arguments */

typedef struct args {
  int                 quit;
  unsigned int        port;
  struct sockaddr_in  server_addr;
  int                 *server_socket;
  char                host_username[USERNAME_LEN];
  client_t            *active_client;
  struct pollfd       *pfds;
  nfds_t              *nfds;
  nfds_t              *fd_count;
  client_t            **client_list;
  msg_t               **message_queue;
} args_t;

/* pollfds */

typedef struct pollfds {
  struct pollfd       *pfds;
  nfds_t              *nfds;
  nfds_t              *fd_count;
} pollfds_t;

#endif /* MAIN_H */