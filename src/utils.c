#include "utils.h"
#include "message.h"
#include "client.h"


/* process_input */


int process_input(args_t *args, WINDOW **windows) {

  char input_buffer[BUFFER_LEN];
  wgetnstr(windows[INPUT], input_buffer, BUFFER_LEN);
  
  werase(windows[INPUT]);
  wmove(windows[INPUT], 0, 0);
  wrefresh(windows[INPUT]);
  
  // check for exit condition
  if (strcmp(input_buffer, "quit") == 0) {
    wprintw(windows[INFO], EXIT_MESSAGE);// TODO sleep(1)?
    wrefresh(windows[INFO]);
    args->quit = 0;
    return 0;
    // TODO: FREE EVERYTHING
    // TODO: PRINT LEAVING MESSAGE
    // TODO exit gracefully
    // exit(EXIT_SUCCESS);
  }
// TODOD better comment here
  // check for user change, username, or create new message
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
          // TEMP
            // werase(windows[INFO]);
            // mvwprintw(windows[INFO], 1, 1, "PROCESS INPUT CALLED CREATE MSG\n");
            // box(windows[INFO], 0, 0);
            // wrefresh(windows[INFO]);
            // sleep(1);
        // create message
        // TODO review this - now passing active client to create msg, seems ok
        message_t *message = create_message(input_buffer, args->active_client, args, windows);
        append_message(message, args->message_queue, windows);
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
  // TODO
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

// TODO - maybe use this instead of remove_newline as it removes newline too
void remove_whitespace(char *input) {
  int len = strlen(input);
  for (int i = 0; i >= 0; i++) {
    if (isspace(input[i])) {
      input[i] = '\0';
    }
  }
}


// 

void handle_error(int e, const char *str) {
  FILE *log_file = fopen("logfile.txt", "w");
  fprintf(log_file, "%s: %s", str, strerror(e));
  fclose(log_file);
  exit(EXIT_FAILURE);
}


