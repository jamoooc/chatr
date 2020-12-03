#include "main.h"
#include "message.h"
#include "utils.h"
#include "client.h"
#include "connect.h"
#include "pfds.h"
#include "ui.h"


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
  while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
    switch (opt) {
      case 'p':
        options.port = atoi(optarg);
        if ((valid_port(options.port)) == false) {
          usage(basename(argv[0]), opt);
          exit(EXIT_FAILURE);
        };
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
      // printf("%s\n", argv[optind++]);
    }
  }

  // linked lists
  int client_socket; // reused by accept_connection for pfds
  int server_socket;
  client_t *client_list = NULL;
  message_t *message_queue = NULL;

  // poll
  nfds_t nfds = N_PFDS;
  nfds_t fd_count = 0;
  struct pollfd *pfds = create_pfds_array(nfds); 
  
  // ncurses
  WINDOW **windows = create_windows_array(N_WINDOWS);
  init_curses(windows);

  // args object // TODO put this in a func?
  args_t *args = malloc(sizeof(args_t));
  args->quit = 1;
  args->port = options.port;
  args->server_socket = &server_socket;
  args->client_list = &client_list;
  args->message_queue = &message_queue;

  // TODO separate struct for pfds?
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

  // this needs its own func
  while (args->quit) {
    // move cursor to input window   // TODO utility functions for this stuff...?
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
      process_input(args, windows);
    }

    // server socket for client connection
    if (args->pfds[1].revents & POLLIN) {
      accept_connection(args->pfds[1].fd, args, windows);
                  // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "ACCEPT CONNECTION\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
      print_clients(args->active_client, args->client_list, windows);
                  // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "ACCEPT CONNECTION DONE\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
    }

    // client fds
    for (int i = MIN_CLIENT_SOCK; i < fd_count; i++) {
            //     // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "LOOPING FDS: i: %i, fd: %i\n", i, args->pfds[i].fd);
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
      // unset fds are < 0 // TODOD IS THIS CAUSING FREEZE?
      if (args->pfds[i].fd < 0) {
          //  werase(windows[INFO]);
          //   mvwprintw(windows[INFO], 1, 1, "PFD < 0 I: %i fd: %i\n", i , args->pfds[i].fd);
          //   box(windows[INFO], 0, 0);
          //   wrefresh(windows[INFO]);
          //   sleep(1);
        i++;
        continue;
      }

      // socket error
      if (args->pfds[i].revents & POLLERR) {
        // TEMP
        // werase(windows[INFO]);
        // mvwprintw(windows[INFO], 1, 1, "POLLERR\n");
        // box(windows[INFO], 0, 0);
        // wrefresh(windows[INFO]);
        // sleep(1);
        perror("revents socket");
        exit(EXIT_FAILURE);
      }

      // check for client hangup 
      if (args->pfds[i].revents & POLLHUP) {
        // TEMP
        // werase(windows[INFO]);
        // mvwprintw(windows[INFO], 1, 1, "POLLHUP\n");
        // box(windows[INFO], 0, 0);
        // wrefresh(windows[INFO]);
        // sleep(1);
        disconnect_client(args->pfds[i].fd, i, args, windows); // TODO dont use duplicate args...
        i++;
        continue;
      }

      // socket ready to recieve
      if (args->pfds[i].revents & POLLIN) {
        // TEMP
        // werase(windows[INFO]);
        // mvwprintw(windows[INFO], 1, 1, "POLLIN RECEIVE PACKET CALLED\n");
        // box(windows[INFO], 0, 0);
        // wrefresh(windows[INFO]);
        // sleep(1);
        receive_packet(i, args, windows);

        // werase(windows[INFO]);
        // mvwprintw(windows[INFO], 1, 1, "POLLIN RECEIVE PACKET DONE\n");
        // box(windows[INFO], 0, 0);
        // wrefresh(windows[INFO]);
        // sleep(1);
      }


// REORDER THIS NEXT? LOOP MSGS THEN CHECK FD
      // if message in queue & socket ready, send any msg for that socket
      if (args->pfds[i].revents & POLLOUT) {
        // TEMP
        // werase(windows[INFO]);
        // mvwprintw(windows[INFO], 1, 1, "POLLOUT\n");
        // box(windows[INFO], 0, 0);
        // wrefresh(windows[INFO]);
        // sleep(1);

        message_t *message = *args->message_queue;
        while (message != NULL) {
            // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "POLLOUT MSG IN Q\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
          if (message->client->socket == args->pfds[i].fd) {
            // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "TRANSMIT CALLED\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
            transmit_packet(message, args, windows);
            // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "TRANSMIT DONE\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
          }
          message = message->next;
        }
      }
    }
  }

  // free clients and server
  // TODO free clients
  endwin();
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