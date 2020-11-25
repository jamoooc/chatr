#include "utils.h"
#include "message.h"
#include "client.h"


/* process_input */


int process_input(args_t *args) { // message_queue **
  // get input from stdin
  char input_buffer[BUFFER_LEN];
  if (!fgets(input_buffer, BUFFER_LEN, stdin)) {
    printf("fgets error");
    exit(EXIT_FAILURE);
  };
  
  // check for exit condition
  if (strcmp(input_buffer, "quit\n") == 0) {
    printf("I QUIT!\n"); 
    args->quit = 0;
    return 0;
    // TODO: FREE EVERYTHING
    // TODO exit gracefully
    // exit(EXIT_SUCCESS);
  }

  // check for user change, username, or create new message SWITCH?
  switch (input_buffer[0]) {
    case '@':
      select_active_client(input_buffer, args);
      break;
    case '$':
      set_host_username(input_buffer, args);
      break;
    case '#':
      set_client_alias(input_buffer, args);
      break;
    case '!':
      add_client(input_buffer, args);
      break;
    case '/':
      assist(input_buffer, args);
      break;
    default:
      // if username not set, set to IP addr
      if (strlen(args->username) < 1) { 
        char address_buffer[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET, &args->server_addr.sin_addr.s_addr, address_buffer, INET6_ADDRSTRLEN);
        strcpy(args->username, address_buffer);
      }

      // create message for queue
      if (args->active_socket > MIN_CLIENT_SOCK) {
        // create message
        message_t *message = create_message(args->active_socket, args->username, input_buffer);
        append_message(message, args->message_queue);
      } else {
        printf("No active client.\n");
      }
    break;
  }
  return 0;
}


/* set_host_username */


int set_host_username(char *input, args_t *args) {
  // remove '$' and '\n' before set username;
  remove_first_char(input);
  remove_newline(input);

  // validate input
  int length = strlen(input);
  for (int i = 0; i < length; i++) {
    if (isalnum(input[i] == 0)) {
      printf("%s.\n", INVALID_USERNAME);
      return 1;
    }
  }
  
  // set username
  strcpy(args->username, input);
  printf("Host username set to: %s.\n", args->username);
  return 0;
}


/* insert_pfd */


void insert_pfd(struct pollfd *pfds[], int new_fd, nfds_t *fd_count, nfds_t *nfds) {
  // resize poll_fd array if necessary
  if (*fd_count == *nfds) {
    *nfds *= 2;
    *pfds = realloc(*pfds, sizeof(**pfds) * (size_t)(*nfds));

    // set new pfds null
    for (nfds_t i = *fd_count; i < *nfds; i++) {
      (*pfds)[i].fd = -1;
      (*pfds)[i].revents = 0;
    }
  }
  (*pfds)[*fd_count].fd = new_fd;
  (*pfds)[*fd_count].events = POLLIN | POLLOUT;
  (*pfds)[*fd_count].revents = 0;
  
  (*fd_count)++;
}


/* remove pfd */


void remove_pfd(struct pollfd pfds[], int i, nfds_t *fd_count) {
  // copy last pfd over el to remove
  pfds[i] = pfds[*fd_count - 1];

  // reset copied pfd and decrement count
  pfds[*fd_count - 1].fd = -1; // negative fd ignored by poll
  pfds[*fd_count - 1].revents = 0;
  (*fd_count)--;
}


/* check for valid_port */

// not sure about this, by definition anything but an unsigned int wont work? 0-65535
bool valid_port(unsigned int port) {
  if (port < 0 || port > 65535) {
    return false;
  }
  return true;
}


/* check for valid_username */


bool valid_username(char *username) {
  return true;
}


/* remove_first_char if @#!$ */

// TODO not a great name 
void remove_first_char(char *input) {
  char c = input[0];
  if (c == '!' || c == '$' || c == '@' || c == '#') {
    memmove(input, input + 1, strlen(input));
  }
}


/* remove_newline */


void remove_newline(char *input) {
  int i = strlen(input) - 1;
  if (input[i] == '\n') {
    input[i] = '\0';
  }
}



/* assist - print help messages */

// RENAME this 
void assist(char *input, args_t *args) {
  switch (input[1]) {
    case 'm':
      print_messages(args->message_queue);
      break;
    case 'c': 
      print_clients(args->client_list);
      break;
    case 'h':
    default:
      print_usage();
    break;
  }
}


/* print_usage renmae command list or someting */

// this will change when ncurses added/
void print_usage(void) {
  printf(
    "\n"
    "  ************************************************************\n"
    "  *                                                          *\n"
    "  *   '$username' - set host username.                       *\n"
    "  *   '!IP PORT' - add new remote client.                    *\n"
    "  *   '@ip' or '@alias' - select active client.              *\n"
    "  *   '#alias' - set active client's alias.                  *\n"
    "  *   'quit' - exit.                                         *\n"
    "  *   '/c' - print connected clients.                        *\n"
    "  *   '/m' - print message queue.                            *\n"
    "  *   '/h' - print this message.                             *\n"
    "  *                                                          *\n"
    "  ************************************************************\n"
    "\n"
  );

};

