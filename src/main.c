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
          fprintf(stderr, "%s.\n", INVALID_PORT);
          usage(basename(argv[0]), opt);
          exit(EXIT_FAILURE);
        } else {
          options.port = strtol(optarg, NULL, 10);
        }
        break;
      case 'v':
        fprintf(stdout, "%s version %s\n", basename(argv[0]), VERSION);
        exit(EXIT_SUCCESS);
      case 'h':
        usage(basename(argv[0]), opt);
        exit(EXIT_SUCCESS);
      default:
        usage(basename(argv[0]), opt);
        exit(EXIT_FAILURE);
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
  
  // ncurses
  WINDOW **windows = window_create_array(N_WINDOWS);
  init_curses(windows);

  // args object
  args_t *args = malloc(sizeof(args_t));
  args->quit = 1;
  args->port = options.port;
  args->server_socket = &server_socket;
  args->client_list = &client_list;
  args->message_queue = &message_queue;
  args->active_client = NULL;
  
  // poll
  int poll_count;
  nfds_t nfds = N_PFDS;
  nfds_t fd_count = 0;
  struct pollfd *pfds = pfd_create_array(nfds, args, windows); 
  args->pfds = pfds;
  args->fd_count = &fd_count;
  args->nfds = &nfds;

  // start listening server
  server_init(&server_socket, args, windows);
  socket_set_nonblock(server_socket);

  // add stdin and server socket to pfds
  pfd_insert(&args->pfds, STDIN_FILENO, &fd_count, &nfds);
  pfd_insert(&args->pfds, server_socket, &fd_count, &nfds);

  while (args->quit) {
    // move cursor to input window
    wmove(windows[INPUT], 0, 0);
    wrefresh(windows[INPUT]);

    // poll file descriptors for sockets ready to read
    if ((poll_count = poll(args->pfds, fd_count, 0)) == -1) {
      handle_error(poll_count, "poll", args, windows);
      exit(EXIT_FAILURE);
    }

    // stdin
    if (args->pfds[0].revents & POLLIN) {
      get_input(args, windows);
    }

    // server socket for client connection
    if (args->pfds[1].revents & POLLIN) {
      accept_connection(args->pfds[1].fd, args, windows);
      client_print(args->active_client, args->client_list, windows);
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
        handle_error(poll_count, "revents socket", args, windows);
        endwin();
        exit(EXIT_FAILURE);
      }

      // check for client hangup 
      if (args->pfds[i].revents & POLLHUP) {
        client_disconnect(i, args, windows);
        i++;
        continue;
      }

      // socket ready to recieve
      if (args->pfds[i].revents & POLLIN) {
        packet_receive(i, args, windows);
      }

      // if message in queue & socket ready, send any msg for that socket
      if (args->pfds[i].revents & POLLOUT) {
        msg_t *message = *args->message_queue;
        while (message != NULL) {
          if (message->client->socket == args->pfds[i].fd) {
            packet_transmit(message, args, windows);
          }
          message = message->next;
        }
      }
    }
  }

  free(args->pfds);
  client_free(args->client_list);
  message_free(args->message_queue);
  exit_screen(windows);
  window_free(windows);
  free(args);
  endwin(); // end curses
  close(server_socket);
  exit(EXIT_SUCCESS);
}

/* usage */

static void usage(char *progname, int opt) {
  if (opt == '?') { 
    fprintf(stderr, "Unknown option '-%c'.\n", optopt);
  }
  if (opt == ':') {
    fprintf(stderr, "Missing argument for option '-%c'.\n", optopt);
  }
  fprintf(stdout, USAGE_FMT, progname ? progname : DEFAULT_PROGNAME);
}