#include "client.h"

#include "utils.h"
#include "connect.h"
#include "pfds.h"


/* create_client */


client_t *create_client(int socket, char *username, struct sockaddr_in client_addr) {
  // allocate memory for new client
  client_t *client;
  client = malloc(sizeof(client_t));
  if (client == NULL) {
    perror("create_client");
    exit(EXIT_FAILURE);
  }

  history_t *history_head = NULL;

  client->socket = socket;
  client->addr = client_addr;
  client->next = NULL;
  client->history = history_head;
  strcpy(client->username, username);

  return client;
}


/* append_client to linked list */


void append_client(client_t *new_client, client_t **client_list, WINDOW **windows) {
  client_t **tmp = client_list;  // double ptr to first client node
  while (*tmp != NULL) {
    tmp = &(*tmp)->next;
  }
  new_client->next = *tmp;  // *tmp is currently NULL
  *tmp = new_client;  

  print_clients(client_list, windows);
}


/* add_client */


// TODO this func is amess
int add_client(char *input, args_t *args, WINDOW **windows) {
  // printf("add_client: %s", input);

  // remove '!' and '\n'
  remove_first_char(input);
  remove_newline(input);

  // TODO SEG ERROR IF PORT MISSING!
  // for (int i = 0, len = strlen(input); i < len; i++) {
    // probably want to check for the space separator? followed by 4 or 5 nums... 
    // OR use inet_pton and hotns() or somthing
  // }

  // split input string into IP/port - TODO figure out why this doesn't work on the pi
  char *ip, *port, *tofree;                   // strdup allocates memory for copy of string
  tofree = port = strdup(input);              // port will be modified by strsep so set extra ptr
  assert(port != NULL);
  ip = strsep(&port, " ");
  free(tofree);


// port should be validatedhere!!

  // socket info for remote host
  int rv;
  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(client_addr));
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(atoi(port));
  rv = inet_pton(AF_INET, ip, &(client_addr.sin_addr.s_addr)); // TODO extra brakcet?
  
  // printf("RV %i\n", rv);

  // validate ip
  rv = inet_pton(AF_INET, ip, &client_addr.sin_addr.s_addr);
  // invalid address family
  if (rv == 0) {
    printf(INVALID_IPV4_ADDR);
    return EXIT_FAILURE;
  }
  // system error
  if (rv == -1) {
    perror("inet_pton");
    return EXIT_FAILURE;
  }

  // validate port
  if ((valid_port(atoi(port))) == false) {
    wprintw(windows[INFO], "%s.\n", INVALID_PORT);
    wrefresh(windows[INFO]);
    return EXIT_FAILURE;
  };

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
  
  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "Connected to client on %s.\n", address_string);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);

  // add to client list and pfds
  client_t *client = create_client(client_socket, address_string, client_addr);
  append_client(client, args->client_list, windows);
  insert_pfd(&args->pfds, client_socket, args->fd_count, args->nfds);


  // if no client, set to active client
  if (!args->active_client) {
    select_active_client(address_string, args, windows);
  }
  return 0;
}


/* select_active_client */


int select_active_client(char *input_user, args_t *args, WINDOW **windows) {
  // remove '@' and '\n' before set username;
  remove_first_char(input_user);
  remove_newline(input_user);

  // validate input username TODO check min max lens etc. too
  int input_length = strlen(input_user);
  for (int i = 0; i < input_length; i++) {
    if (isalnum(input_user[i] == 0)) {
      werase(windows[INFO]);
      mvwprintw(windows[INFO], 1, 1, INVALID_USERNAME);
      box(windows[INFO], 0, 0);
      wrefresh(windows[INFO]);
      return 1;
    }
  }

  // hardcode start co-ordinates????
  // TODO - WONT BE SETTING ALL THIS, just have active_client ptr
  // can probably change the ifs
  // find client in ll
  client_t *client = *args->client_list; 
  while (client != NULL) {
    // if username match
    if (strcmp(input_user, client->username) == 0) {
      // set active user and socket
      // TODO these 2 ifs are just repeats, make a func
      strcpy(args->active_username, input_user);
      strcpy(args->active_alias, client->alias);
      args->active_socket = client->socket;
      args->active_client = client;
      werase(windows[INFO]);
      mvwprintw(windows[INFO], 1, 1, "%s: %s.\n", ACTIVE_CLIENT_SET, input_user);
      box(windows[INFO], 0, 0);
      wrefresh(windows[INFO]);
      break;
    } 
    // if alias match
    if (strcmp(input_user, client->alias) == 0) {
      // set active user and socket
      strcpy(args->active_username, client->username);
      strcpy(args->active_alias, input_user);
      args->active_socket = client->socket;
      werase(windows[INFO]);
      args->active_client = client;
      mvwprintw(windows[INFO], 1, 1, "%s: %s.\n", ACTIVE_CLIENT_SET, input_user);
      box(windows[INFO], 0, 0);
      wrefresh(windows[INFO]);
      break;
    }
    client = client->next;
  }

  // if username or alias not found
  if (client == NULL) {
    werase(windows[INFO]);
    mvwprintw(windows[INFO], 1, 1, "%s. %s: %s.\n", UNKNOWN_CLIENT, ACTIVE_CLIENT_SET,
      strlen(args->active_alias) > 0 ? args->active_alias : args->active_username);
    box(windows[INFO], 0, 0);
    wrefresh(windows[INFO]);
    return 1;
  }

  print_history(client->history, windows);
  return 0;
}


/* set_client_alias */

// TODO set_client_username
int set_client_alias(char *input, args_t *args, WINDOW **windows) {
  // active user must be set to create alias
  if (strlen(args->active_username) < 1) {
    werase(windows[INFO]);
    mvwprintw(windows[INFO], 1, 1, "Select a client with '@ip' before creating an alias.\n");
    box(windows[INFO], 0, 0);
    wrefresh(windows[INFO]);
    return 1;
  }

  // remove '#' and '\n' before set alias;
  remove_first_char(input);
  remove_newline(input);

  // validate input
  int length = strlen(input);
  for (int i = 0; i < length; i++) {
    if (isalnum(input[i] == 0)) {
      werase(windows[INFO]);
      mvwprintw(windows[INFO], 1, 1, "Username can only contain alphanumeric characters.\n");
      box(windows[INFO], 0, 0);
      wrefresh(windows[INFO]);
      return 1;
    }
  }

  // set active_client and client->alias
  client_t *client = *args->client_list;
  while (client != NULL) {
    if (strcmp(client->username, args->active_username) == 0) {
      strcpy(client->alias, input);
      strcpy(args->active_alias, input);
      werase(windows[INFO]);
      mvwprintw(windows[INFO], 1, 1, "Client %s alias set to '%s'.\n", client->username, client->alias);
      box(windows[INFO], 0, 0);
      wrefresh(windows[INFO]);
      print_clients(args->client_list, windows);
      return 0;
    }
    client = client->next;
  }

  // no client found
  if (client == NULL) {
    werase(windows[INFO]);
    mvwprintw(windows[INFO], 1, 1, "Username not found.");
    box(windows[INFO], 0, 0);
    wrefresh(windows[INFO]);
    return 1;
  }
  return 1;
}


/* disconnect_client */


void disconnect_client(int socket, int pfd_index, args_t *args, WINDOW **windows) {
  // get disconnected client username
  client_t *client = *args->client_list;
  while (client->socket != socket) {
    client = client->next;
  }
  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "Client username: %s, socket %i disconnected.\n", client->username, client->socket);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);

  // reset active user
  args->active_socket = -1; // TODO if remaining pfds, set to next available?
  memset(args->active_username, 0, USERNAME_LEN);

  // remove from client / pfd list and close socket
  close(args->pfds[pfd_index].fd);
  remove_pfd(args->pfds, pfd_index, args->fd_count);
  remove_client(socket, args->client_list);
  print_clients(args->client_list, windows);
}


/* free single client sockets linked list */


// TODO not relinking?
// TODO this is removing all clients if removing any but last client-unlinknig somewhere?
void remove_client(int socket, client_t **client_list) {
  client_t *del, **p = client_list;
  while (*p && (**p).socket != socket) {    // while ptr is not null, and next doesnt match target  
    p = &(*p)->next;                        // set p to the address of the next el until the next el is target
  }
  if (p) {                                  // if not null (will be null (->next) if target not found)
    del = *p;
    *p = del->next;
    free(del);
  }
}


/* print client */


void print_clients(client_t **client_list, WINDOW **windows) {
  // printf("\nAvailable client_lists:\n");
  client_t *client = *client_list;
  int y = 1, x = 1;

  werase(windows[CLIENTS]);
  while (client != NULL) {
    mvwprintw(windows[CLIENTS], y, x, "%s", strlen(client->alias) > 0 ? client->alias : client->username);
    client = client->next;
    y++;
  }
  box(windows[CLIENTS], 0, 0);
  wrefresh(windows[CLIENTS]);
}


