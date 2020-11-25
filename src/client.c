#include "client.h"

#include "utils.h"
#include "connect.h"

/* create_client */


client_t *create_client(int socket, char *username, struct sockaddr_in client_addr) {
  // allocate memory for new client
  client_t *client;
  client = malloc(sizeof(client_t));
  if (client == NULL) {
    perror("create_client");
    exit(EXIT_FAILURE);
  }

  client->socket = socket;
  client->addr = client_addr;
  client->next = NULL;
  strcpy(client->username, username);

  return client;
}


/* append_client to linked list */


void append_client(client_t *new_client, client_t **client) {
  client_t **tmp = client;  // double ptr to first client node
  while (*tmp != NULL) {
    tmp = &(*tmp)->next;
  }
  new_client->next = *tmp;  // *tmp is currently NULL
  *tmp = new_client;  
}




/* add_client */


// TODO this func is amess
int add_client(char *input, args_t *args) {
  printf("add_client: %s", input);

  // remove '!' and '\n'
  remove_first_char(input);
  remove_newline(input);

  // TODO SEG ERROR IF PORT MISSING!
  
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
  rv = inet_pton(AF_INET, ip, &(client_addr.sin_addr.s_addr));
  
  printf("RV %i\n", rv);

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
    printf("%s.\n", INVALID_PORT);
    return EXIT_FAILURE;
  };


  printf("TEST %s\n", ip);
  printf("port %s\n", port);

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
  printf("Connected to client on %s.\n", address_string);

  // add to client list and pfds
  client_t *client = create_client(client_socket, address_string, client_addr);
  append_client(client, args->client_list);
  insert_pfd(&args->pfds, client_socket, args->fd_count, args->nfds);

  return 0;
}


/* select_active_client */


int select_active_client(char *input_user, args_t *args) {
  // remove '@' and '\n' before set username;
  remove_first_char(input_user);
  remove_newline(input_user);

  // validate input username
  int input_length = strlen(input_user);
  for (int i = 0; i < input_length; i++) {
    if (isalnum(input_user[i] == 0)) {
      printf(INVALID_USERNAME);
      return 1;
    }
  }

  // find client in ll
  client_t *client = *args->client_list; 
  while (client != NULL) {
    // if username match
    if (strcmp(input_user, client->username) == 0) {
      // set active user and socket
      strcpy(args->active_username, input_user);
      strcpy(args->active_alias, client->alias);
      args->active_socket = client->socket;
      printf("%s: %s.\n", ACTIVE_CLIENT_SET, input_user);
      break;
    } 
    // if alias match
    if (strcmp(input_user, client->alias) == 0) {
      // set active user and socket
      strcpy(args->active_username, client->username);
      strcpy(args->active_alias, input_user);
      args->active_socket = client->socket;
      printf("%s: %s.\n", ACTIVE_CLIENT_SET, input_user);
      break;
    }
    client = client->next;
  }

  // if username or alias not found
  if (client == NULL) {
    printf("%s. %s: %s.\n", 
      UNKNOWN_CLIENT, 
      ACTIVE_CLIENT_SET,
      strlen(args->active_alias) > 0 ? args->active_alias : args->active_username);
    return 1;
  }

  return 0;
}




/* set_client_alias */


int set_client_alias(char *input, args_t *args) {
  // active user must be set to create alias
  if (strlen(args->active_username) < 1) {
    printf("Select a client with '@ip' before creating an alias.\n");
    return 1;
  }

  // remove '#' and '\n' before set alias;
  remove_first_char(input);
  remove_newline(input);

  // validate input
  int length = strlen(input);
  for (int i = 0; i < length; i++) {
    if (isalnum(input[i] == 0)) {
      printf("Username can only contain alphanumeric characters.\n");
      return 1;
    }
  }

  // set active_client and client->alias
  client_t *client = *args->client_list;
  while (client != NULL) {
    if (strcmp(client->username, args->active_username) == 0) {
      strcpy(client->alias, input);
      strcpy(args->active_alias, input);
      printf("Client %s alias set to '%s'.\n", client->username, client->alias);
      return 0;
    }
    client = client->next;
  }

  // no client found
  if (client == NULL) {
    printf("Username not found.");
    return 1;
  }

  return 1;
}





/* disconnect_client */


void disconnect_client(int socket, int pfd_index, args_t *args) {
  // get disconnected client username
  client_t *client = *args->client_list;
  while (client->socket != socket) {
    client = client->next;
  }
  printf("Client username: %s, socket %i disconnected.\n", client->username, client->socket);

  // reset active user
  args->active_socket = -1; // TODO if remaining pfds, set to next available?
  memset(args->active_username, 0, USERNAME_LEN);

  // remove from client / pfd list and close socket
  close(args->pfds[pfd_index].fd);
  remove_pfd(args->pfds, pfd_index, args->fd_count);
  remove_client(socket, args->client_list);
}


/* free single client sockets linked list */

// not relinking?
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


void print_clients(client_t **client) {
  printf("\nAvailable clients:\n");
  client_t *tmp = *client;
  while (tmp != NULL) {
    printf("username: %s, ", tmp->username);
    printf("socket: %d\n", tmp->socket);
    tmp = tmp->next;
  }
  puts("");
}


