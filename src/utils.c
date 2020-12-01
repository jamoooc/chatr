#include "utils.h"
#include "message.h"
#include "client.h"


/* process_input */


int process_input(args_t *args, WINDOW **windows) {

  char input_buffer[BUFFER_LEN];
  wgetnstr(windows[INPUT], input_buffer, BUFFER_LEN);
  
  werase(windows[INPUT]);
  box(windows[INPUT], 0, 0);
  wmove(windows[INPUT], 1, 1);
  wrefresh(windows[INPUT]);
  
  // check for exit condition
  if (strcmp(input_buffer, "quit") == 0) {
    wprintw(windows[INFO], EXIT_MESSAGE);
    wrefresh(windows[INFO]);
    args->quit = 0;
    return 0;
    // TODO: FREE EVERYTHING
    // TODO: PRINT LEAVING MESSAGE
    // TODO exit gracefully
    // exit(EXIT_SUCCESS);
  }

  // check for user change, username, or create new message SWITCH?
  switch (input_buffer[0]) {
    case '@':
      set_active_client(input_buffer, args, windows);
      break;
    case '$':
      set_host_username(input_buffer, args, windows);
      break;
    case '#':
      set_client_username(input_buffer, args, windows);
      break;
    case '!':
      add_client(input_buffer, args, windows);
      break;
    case '/':
      // TODO quit should be in here
      // TODO add this back in a when u sort out your life
      // assist(input_buffer, args);
      break;
    default:
      // create message for queue
      if (args->active_client != NULL) {
        // create message
        // TODO review this - now passing active client to create msg, seems ok
        message_t *message = create_message(input_buffer, args->active_client);
        append_message(message, args->message_queue);
      } else {
        werase(windows[INFO]);
        mvwprintw(windows[INFO], 1, 1, "No active client.\n");
        box(windows[INFO], 0, 0);
        wrefresh(windows[INFO]);
        wrefresh(windows[INPUT]);
      }
    break;
  }
  return 0;
}


/* set_host_username */


int set_host_username(char *input, args_t *args, WINDOW **windows) {
  // remove '$' and '\n' before set username;
  remove_first_char(input);
  remove_newline(input);

  // validate input
  int length = strlen(input);
  for (int i = 0; i < length; i++) {
    if (isalnum(input[i] == 0)) {
      // TODO maybe clearing screen here etc.. 
      // need a func for reset screen
      wprintw(windows[INFO], "%s.\n", INVALID_USERNAME);
      wrefresh(windows[INFO]);
      return 1;
    }
  }
  
  // set username
  strcpy(args->host_username, input);
  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "Host username set to: '%s'.\n", args->host_username);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);
  return 0;
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



// TODO
// void remove_whitespace() { }


/* assist - print help messages */

// RENAME this 
void assist(char *input, args_t *args, WINDOW **windows) {
  switch (input[1]) {
    case 'm':
      print_messages(args->message_queue);
      break;
    case 'c': 
      print_clients(args->client_list, windows);
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

void handle_error(char *err) {
  perror(err);
  // TODO print to log file...
  exit(EXIT_FAILURE);
}


