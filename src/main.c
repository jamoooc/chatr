#include "main.h"
#include "message.h"
#include "utils.h"
#include "client.h"
#include "connect.h"
#include "pfds.h"
#include "ui.h"

static void usage(char *progname, int opt);

/* main */

int main(int argc, char *argv[]) {
  options_t options = { 0 };
  int opt = 0; // holds option flag, set to -1 at end of argv
  opterr = 0;

  // step through argv 
  while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
    switch (opt) {
      case 'p':
        if ((valid_port(optarg)) == false) {
          printf("%s.\n", INVALID_PORT);
          usage(basename(argv[0]), opt);
          exit(EXIT_FAILURE);
        } else {
          options.port = strtol(optarg, NULL, 10);
        }
        break;
      case 'v':
        printf("%s version %s\n", basename(argv[0]), VERSION);
        exit(EXIT_SUCCESS);
        break;
      case 'h':
      default:
        usage(basename(argv[0]), opt);
        exit(EXIT_FAILURE);
        break;
    }
  }

  if (argc < 3) {
    usage(argv[0], opt);
    exit(EXIT_FAILURE);
  }

  // linked lists
  int client_socket; // copied to pfd for each client
  int server_socket;
  client_t *client_list = NULL;
  msg_t *message_queue = NULL;

  // poll
  nfds_t nfds = N_PFDS;
  nfds_t fd_count = 0;
  struct pollfd *pfds = create_pfds_array(nfds); 
  
  // ncurses
  WINDOW **windows = create_windows_array(N_WINDOWS);
  init_curses(windows);

  // args object
  args_t *args = malloc(sizeof(args_t));
  args->quit = 1;
  args->port = options.port;
  args->server_socket = &server_socket;
  args->client_list = &client_list;
  args->message_queue = &message_queue;
  args->pfds = pfds;
  args->fd_count = &fd_count;
  args->nfds = &nfds;
  args->active_client = NULL;

  // start listening server
  init_server(&server_socket, args, windows);
  set_nonblock(server_socket);

  // add stdin and server socket to pfds
  insert_pfd(&args->pfds, STDIN_FILENO, &fd_count, &nfds);
  insert_pfd(&args->pfds, server_socket, &fd_count, &nfds);

  while (args->quit) {
    // move cursor to input window
    wmove(windows[INPUT], 0, 0);
    wrefresh(windows[INPUT]);

    // poll file descriptors for sockets ready to read
    int poll_count = poll(args->pfds, fd_count, 0);
    if (poll_count == -1) {
      perror("poll");
      exit(EXIT_FAILURE);
    }

    // stdin
    if (args->pfds[0].revents & POLLIN) {
      get_input(args, windows);
    }

    // server socket for client connection
    if (args->pfds[1].revents & POLLIN) {
      accept_connection(args->pfds[1].fd, args, windows);
      print_clients(args->active_client, args->client_list, windows);
    }

    // client fds
    for (int i = MIN_CLIENT_SOCK; i < fd_count; i++) {
      // unset fds are < 0
      if (args->pfds[i].fd < 0) {
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
        disconnect_client(i, args, windows);
        i++;
        continue;
      }

      // socket ready to recieve
      if (args->pfds[i].revents & POLLIN) {
        receive_packet(i, args, windows);
      }

      // if message in queue & socket ready, send any msg for that socket
      if (args->pfds[i].revents & POLLOUT) {
        msg_t *message = *args->message_queue;
        while (message != NULL) {
          if (message->client->socket == args->pfds[i].fd) {
            transmit_packet(message, args, windows);
          }
          message = message->next;
        }
      }
    }
  }

  free(args->pfds);
  free_clients(args->client_list);
  free_messages(args->message_queue);
  free_windows(windows); // individuals windows
  free(windows); // windows array
  exit_screen();
  endwin(); // end curses
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
}