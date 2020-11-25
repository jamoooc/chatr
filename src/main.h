#ifndef MAIN_H
  #define MAIN_H

// all these includes not needed here? move to correct headers

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>
#include <libgen.h>
#include <errno.h>
#include <getopt.h>


#define OPTSTR ":p:hv" // template for getopt - 
#define USAGE_FMT "%s [-p localport 0-65535] [-h help] [-v version]\n"

// integer constants
#define BACKLOG 10
// #define DEFAULT_PORT 9876 // TODO use default if not set??
#define USERNAME_LEN 30
#define BUFFER_LEN 256
#define MIN_CLIENT_SOCK 2

// string constants
#define DEFAULT_PROGNAME "chatr"
#define VERSION "0.1.0"
#define INVALID_PORT "Invalid port: 0 - 65535"
#define INVALID_IPV4_ADDR "Invalid IPv4 address"
#define INVALID_USERNAME "Username can only contain alphanumeric characters"
#define ACTIVE_CLIENT_SET "Active client set to"
#define UNKNOWN_CLIENT "Unknown client"

extern int errno; // used by std C lib for error communication
extern char *optarg; // ptr to option argument
extern int opterr, optind; // used by getopt()


/* structs */


/* options_t */
typedef struct options {
  int verbose;
  unsigned int port;
} options_t;

/* packet_t */
typedef struct packet {
  char username[USERNAME_LEN];
  char body[BUFFER_LEN];
} packet_t;

/* message_t - queue messages linked list */
typedef struct message {
  int socket;
  packet_t *packet;
  struct message *next;
} message_t;


/* history_t - message history linked list */
typedef struct history {
  time_t timestamp;
  packet_t *packet;
  struct history *next;
} history_t;

/* client_t - connected clients linked list */
typedef struct client {
  int socket;
  char username[USERNAME_LEN]; // host - set to IP if unset
  char alias[USERNAME_LEN];
  struct sockaddr_in addr;
  struct client *next;
} client_t;

/* arguments */
typedef struct args {
  int                 quit;
  // WINDOW              *clients_win; // temp ( windows structs)
  unsigned int        port;
  struct sockaddr_in  server_addr; // maybe!
  int                 *server_socket;
  int                 active_socket;
  char                username[USERNAME_LEN]; // host
  char                active_username[USERNAME_LEN];
  char                active_alias[USERNAME_LEN];
  struct pollfd       *pfds; // pfd struct needed
  nfds_t              *nfds;
  nfds_t              *fd_count;
  client_t            **client_list;
  message_t           **message_queue;
} args_t;


/* prototypes */

void usage(char *progname, int opt);
// description

#endif /* MAIN_H */