#include "utils.h"

/* process_input */

void get_input(args_t *args, WINDOW **windows) {
  char input_buffer[BUFFER_LEN];
  wgetnstr(windows[INPUT], input_buffer, BUFFER_LEN);
  
  werase(windows[INPUT]);
  wmove(windows[INPUT], 0, 0);
  wrefresh(windows[INPUT]);
 
  process_input(input_buffer, args, windows);
}

int process_input(char *input_buffer, args_t *args, WINDOW **windows) {
  // check for exit condition
  if (strcmp(input_buffer, "/quit") == 0) {
    args->quit = 0;
    return 1;
  }

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
      client_connect(input_buffer, args, windows);
      break;
    default:
      // create message for queue
      if (args->active_client != NULL) {
        // create message
        msg_t *message = message_create(input_buffer, args, windows);
        message_append(message, args->message_queue, windows);
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
      werase(windows[INFO]);
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


int valid_port(char *input) {
  // no trailing whitespace
  int len = strlen(input);
  if (isspace(input[len]) != 0) {
    // remove_trailing_whitespace(input);
    return false;
  }

  if (len < 1 || len > 5) {
    return false;
  }

  int zeros = 0, spaces = 0;
  for (int i = 0; i < len; i++) {
    if (!isdigit(input[i]) && input[i] != ' ') {
      return false;
    }
    if (input[i] == '0') {
      zeros++;
    }
    if (input[i] == ' ') {
      zeros++;
    }
  }
  // don't accept only 0 or spaces
  if (zeros == len && len > 1) {
    return false;
  }
  if (spaces == len && len > 1) {
    return false;
  }

  // strtol will strip leading whitespace and account for a positive '+' or negative '-'
  long port = strtol(input, NULL, 10);
  if (port < 0 || port > 65535) {
    return false;
  }
  return true;
}

/* check for valid_username */

bool valid_username(char *username) {
  int len = strlen(username);
  if (len < 1 || len > USERNAME_LEN) {
    return false;
  }
  for (int i = 0; i < len; i++) {
    if (!isalnum(username[i])) {
      return false;
    }
  }
  return true;
}

/* remove_first_char if @#!$ */

void remove_first_char(char *input) {
  char c = input[0];
  if (c == '!' || c == '$' || c == '@' || c == '#') {
    memmove(input, input + 1, strlen(input));
  }
}

/* remove_newline */

int remove_newline(char *input) {
  int len = strlen(input);
  if (len <= 0) {
    return 1;
  }
  int i = len - 1;
  if (input[i] == '\n') {
    input[i] = '\0';
  }
  return 0;
}

// TODO


/* remove_trailing_whitespace */

// removes any whitespace character
void remove_trailing_whitespace(char *input) {
  int i = 0, last = 0;
  while (input[i] != '\0') {
    if ((isspace(input[i]) == 0)) {
      last = i;
    }
    i++;
  }
  input[last + 1] = '\0';
}

/* handle_error */

// will need to get return vales form funcs for error codes etc.
void handle_error(int e, const char *str) {
  FILE *log_file = fopen("logfile.txt", "w");
  fprintf(log_file, "%s: %s", str, strerror(e));
  fclose(log_file);
  exit(EXIT_FAILURE);
}


