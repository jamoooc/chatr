#include "client.h"
#include "utils.h"
#include "connect.h"
#include "pfds.h"


/* create_client */


client_t *create_client(int socket, char *username) {
  // allocate memory for new client
  client_t *client;
  client = malloc(sizeof(client_t));
  if (client == NULL) {
    perror("create_client");
    exit(EXIT_FAILURE);
  }

  history_t *history_head = NULL;

  client->socket = socket;
  client->next = NULL;
  client->unread_msg = 0;
  client->history = history_head;
  strcpy(client->username, username);

  return client;
}


/* append_client to linked list */


void append_client(client_t *new_client, args_t *args, WINDOW **windows) {
  client_t **tmp = args->client_list;
  while (*tmp != NULL) {
    tmp = &(*tmp)->next;
  }
  new_client->next = *tmp;  // *tmp is NULL
  *tmp = new_client;

  print_clients(args->active_client, args->client_list, windows);
}


/* add_client */


int add_client(char *input, args_t *args, WINDOW **windows) {
  // remove '!' and '\n'
  remove_first_char(input);
  remove_newline(input);

  // split input string into IP/port
  char *ip, *port, *tofree;       // strdup allocates memory for copy of string
  tofree = port = strdup(input);  // port will be modified by strsep so set extra ptr
  ip = strsep(&port, " ");
  free(tofree);

  if (port == NULL) { // port contains portion of string after '\0' separator
    werase(windows[INFO]);
    mvwprintw(windows[INFO], 1, 1, "%s.", port);
    box(windows[INFO], 0, 0);
    wrefresh(windows[INFO]);
    return 2;
  }

  if (valid_port(port) == false) {
    mvwprintw(windows[INFO], 1, 1, "%s.\n", INVALID_PORT);
    box(windows[INFO], 0, 0);
    wrefresh(windows[INFO]);
    return 3;
  };

  // socket info for remote host
  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(atoi(port));

  // validate ip
  int rv;
  rv = inet_pton(AF_INET, ip, &client_addr.sin_addr.s_addr);
  if (rv == 0) { // invalid address family
    werase(windows[INFO]);
    mvwprintw(windows[INFO], 1, 1, "%s.\n", INVALID_IPV4_ADDR);
    box(windows[INFO], 0, 0);
    wrefresh(windows[INFO]);
    return 4;
  }
  if (rv == -1) { // system error, sets errno
    perror("inet_pton");
    exit(EXIT_FAILURE);
  }

  // create a socket to connect to remote server
  int client_socket;
  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // attempt to connect to remote host
  int connect_status;
  if ((connect_status = connect(client_socket, (struct sockaddr *)&client_addr, sizeof(client_addr))) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  set_nonblock(client_socket);

  // destination string for inet_ntop
  char address_string[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, address_string, INET6_ADDRSTRLEN);
  
  // set default username
  char username[8];
  snprintf(username, 8, "user%i", client_socket);

  // new client notification
  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "Connected to client on %s.\n", address_string);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);

  // add to client list and pfds
  client_t *client = create_client(client_socket, username);
  append_client(client, args, windows);
  insert_pfd(&args->pfds, client_socket, args->fd_count, args->nfds);

  // if no client, set to active client
  if (args->active_client == NULL) {
    set_active_client(username, args, windows);
  }
  return 0;
}


/* set_active_client */


int set_active_client(char *username, args_t *args, WINDOW **windows) {
  // remove '@' and '\n' before set username;
  remove_first_char(username);
  remove_newline(username);

  // validate input username
  if (!valid_username(username)) {
    werase(windows[INFO]);
    mvwprintw(windows[INFO], 1, 1, INVALID_USERNAME);
    box(windows[INFO], 0, 0);
    wrefresh(windows[INFO]);
    return 1;
  }

  // find client in ll
  client_t *client = *args->client_list; 
  while (client != NULL) {
    if (strcmp(username, client->username) == 0) {
      args->active_client = client;
      werase(windows[INFO]);
      mvwprintw(windows[INFO], 1, 1, "%s: %s.\n", ACTIVE_CLIENT_SET, username);
      box(windows[INFO], 0, 0);
      wrefresh(windows[INFO]);
      print_history(client, args, windows);    
      return 0;
    } 
    client = client->next;
  }

  // if username not found
  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "%s. %s: %s.\n", 
    UNKNOWN_CLIENT, ACTIVE_CLIENT_SET, 
    args->active_client->username);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);
  return 2;
}


/* set_client_username */


int set_client_username(char *input, args_t *args, WINDOW **windows) {
  // active user must be set to edit username
  if (args->active_client == NULL) {
    werase(windows[INFO]);
    mvwprintw(windows[INFO], 1, 1, SET_CLIENT);
    box(windows[INFO], 0, 0);
    wrefresh(windows[INFO]);
    return 2;
  }

  // remove '#' and '\n' before set alias;
  remove_first_char(input);
  remove_newline(input);

  if (!valid_username(input)) {
    werase(windows[INFO]);
    mvwprintw(windows[INFO], 1, 1, INVALID_USERNAME);
    box(windows[INFO], 0, 0);
    wrefresh(windows[INFO]);
    return 1;
  }

  // set client username to input
  char prev_uname[USERNAME_LEN];
  strcpy(prev_uname, args->active_client->username);
  strcpy(args->active_client->username, input);

  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "Client '%s' username set to '%s'.\n", prev_uname, args->active_client->username);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);
  print_clients(args->active_client, args->client_list, windows);
  return 0;
}


/* disconnect_client */


void disconnect_client(int pfd_index, args_t *args, WINDOW **windows) {
  // get disconnected client username
  client_t *client = *args->client_list;
  while (client->socket != args->pfds[pfd_index].fd) {
    client = client->next;
  }

  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "Client username: %s, socket %i disconnected.\n", client->username, client->socket);
  box(windows[INFO], 0, 0);

  // if active user, set active user NULL
  if (client == args->active_client) {
    mvwprintw(windows[INFO], 2, 1, "No active client.\n");
    box(windows[INFO], 0, 0);
    args->active_client = NULL;
  }

  wrefresh(windows[INFO]);
  werase(windows[HISTORY]);
  box(windows[HISTORY], 0, 0);
  wrefresh(windows[HISTORY]);

  // remove from client / pfd list and close socket
  close(args->pfds[pfd_index].fd);
  remove_client(args->pfds[pfd_index].fd, args->client_list);
  remove_pfd(args->pfds, pfd_index, args->fd_count);
  print_clients(args->active_client, args->client_list, windows);
}


/* free single client sockets linked list */


int remove_client(int socket, client_t **client_list) {
  if (socket < 0) {
    return 1;
  }

  client_t *del, **p = client_list;
  while (*p && (**p).socket != socket) {    // while ptr is not null, and next doesnt match target  
    p = &(*p)->next;                        // set p to the address of the next el
  }
  if (p) {                                  // if not null (is null if target not found)
    del = *p;
    *p = del->next;
    free(del);
    return 0;
  } else {
    return 1; // client doesn't exist
  }
}


/* print client */


void print_clients(client_t *active_client, client_t **client_list, WINDOW **windows) {
  client_t *client = *client_list;
  int y = 1, x = 1;

  werase(windows[CLIENTS]);
  while (client != NULL) {
    if (client == active_client) {
      wattron(windows[CLIENTS], A_STANDOUT);
      mvwprintw(windows[CLIENTS], y, x, "%s", client->username);
      wattroff(windows[CLIENTS], A_STANDOUT);
    } else {
      mvwprintw(windows[CLIENTS], y, x, "%s%s", client->username, client->unread_msg ? " *" : " ");
    }
    client = client->next;
    y++;
  }
  box(windows[CLIENTS], 0, 0);
  wrefresh(windows[CLIENTS]);
}


/* free_client-list */


int free_clients(client_t **client_list) {
  if (client_list == NULL) {
    return 1; // TODO err msg? this is not really an err
  }
  client_t *del, *client = *client_list;
  while(client != NULL) {
    del = client;
    client = del->next;
    free_history(del->history);
    free(del);
  }
  *client_list = NULL;
  return 0;
}


/* free client messages in queue */


int free_history(history_t *history) {
  if (history == NULL) {
    return 1;
  }
  history_t *del, *msg = history;
  while (msg != NULL) {
    del = msg;
    msg = del->next;
    free(del);
  }
  history = NULL;
  return 0;
}
