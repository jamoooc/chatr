#include "main.h"
#include "message.h"
#include "utils.h"
#include "client.h"
#include "connect.h"


/* main */


int main(int argc, char *argv[]) {
  options_t options = { 0, 0 }; // TODO maybe put a ptr to this in args_t??? what need from it?
  int opt = 0;                  // to hold argv char, set to -1 at argv EOF
  opterr = 0;

  // minimum args
  if (argc < 2) {
    usage(argv[0], opt);
    exit(EXIT_FAILURE);
  }

  // step through argv 
  while ((opt = getopt(argc, argv, OPTSTR)) != EOF ) {
    switch (opt) {
      case 'p':
        options.port = atoi(optarg);
        if ((valid_port(options.port)) == false ) {
          usage(basename(argv[0]), opt);
          exit(EXIT_FAILURE);
        };
        printf("port is %d\n", options.port);
        break;
      case 'v':
        printf("%s version %s\n", basename(argv[0]), VERSION);
        exit(EXIT_SUCCESS);
        break;
      case 'h': // help
      default:
        usage(basename(argv[0]), opt);
        break;
    }
  }
  
  // non-option arguments are moved to end of argv
  if (optind < argc) {
    while (optind < argc) {
      printf("%s\n", argv[optind++]);
    }
  }

  // linked lists
  int client_socket;               // reused by accept_connection for pfds
  int server_socket;
  client_t *client_list = NULL;
  message_t *message_queue = NULL;
  nfds_t nfds = 3;                 // initial sizeof pfds array
  nfds_t fd_count = 0;
  
  // declare pfd array
  struct pollfd *pfds = malloc(sizeof(*pfds) * nfds);
  if (pfds == NULL) {
    perror("pfds");
    exit(EXIT_FAILURE);
  }

  // init pfd values
  for (int i = 0; i < nfds; i++) {
    pfds[i].fd = -1;
    pfds[i].events = 0;
    pfds[i].revents = 0;
  }
  
  // args object
  args_t *args = malloc(sizeof(args_t));
  args->quit = 1;
  args->port = options.port;
  args->server_socket = &server_socket;
  args->client_list = &client_list;
  args->message_queue = &message_queue;

  // TODO separate struct for pfds?
  args->pfds = pfds;
  args->active_socket = -1;
  args->fd_count = &fd_count;
  args->nfds = &nfds;

  // usage
  print_usage();
  
  // start listening server
  init_server(&server_socket, args);
  set_nonblock(server_socket);

  // add stdin and server socket to pfds
  insert_pfd(&args->pfds, STDIN_FILENO, &fd_count, &nfds);
  insert_pfd(&args->pfds, server_socket, &fd_count, &nfds);
  
  while (args->quit) {
    // poll file descriptors for sockets ready to read
    int poll_count = poll(args->pfds, fd_count, 0);
    if (poll_count == -1) {
      perror("poll");
      exit(EXIT_FAILURE);
    }

    // stdin
    if (args->pfds[0].revents & POLLIN) { 
      process_input(args);
    }

    // server socket for client connection
    if (args->pfds[1].revents & POLLIN) {
      accept_connection(args->pfds[1].fd, args);
      print_clients(&client_list);
    }

    // client fds
    for (int i = 2; i < fd_count; i++) {
      // unset fds are < 0
      if (pfds[i].fd < 0) {
        i++;
        continue;
      }

      // socket error
      if (args->pfds[i].revents & POLLERR) {
        perror("revents socket");
        exit(EXIT_FAILURE);
      }

      // check for client hangup 
      if (args->pfds[i].revents & POLLHUP) {
        disconnect_client(args->pfds[i].fd, i, args);
        i++;
        continue;
      }

      // socket ready to recieve
      if (args->pfds[i].revents & POLLIN) {
        receive_packet(args->pfds[i].fd, i, args); // TODO stupid args
      }

      // if message in queue & socket ready, send any msg for that socket
      if (*args->message_queue != NULL) {
        message_t *message = *args->message_queue;
        while (message != NULL) {
          if (args->pfds[i].revents & POLLOUT) {
            if (message->socket == args->pfds[i].fd) { 
              transmit_packet(args->pfds[i].fd, args->message_queue, message->client);
            }
          }
          message = message->next;
        }
      }
    }
  }

  // free clients and server
  // TODO free clients
  close(server_socket);
  exit(EXIT_SUCCESS);
}


/* usage */


void usage(char *progname, int opt) {
  if (opt == '?') { 
    fprintf(stderr, "Unknown option '-%c'.\n", optopt);
  }
  if (opt == ':') {
    fprintf(stderr, "Missing arguement for option '-%c'.\n", optopt);
  }
  fprintf(stderr, USAGE_FMT, progname ? progname : DEFAULT_PROGNAME);
  exit(EXIT_FAILURE);
}