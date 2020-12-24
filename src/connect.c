#include "connect.h"

/* packet_transmit */

void packet_transmit(msg_t *message, args_t *args, WINDOW **windows) {
  // check socket is valid and send packet
  int send_bytes;
  if ((send_bytes = send(message->client->socket, message->packet, sizeof(packet_t), 0)) == -1) {
    perror("send");
    handle_error(send_bytes, "packet_transmit: send,", args, windows);
    exit(EXIT_FAILURE);
  }

  // store packet in user history and free msg
  packet_t *packet;
  if ((packet = malloc(sizeof(packet_t))) == NULL) {
    handle_error(send_bytes, "packet_transmit: malloc,", args, windows);
    exit(EXIT_FAILURE);
  }
  *packet = *message->packet;

  // add to history and remove message from queue
  history_insert(message->packet, message->client, args, windows);
  message_destroy(message, args->message_queue, windows);
}

/* packet_receive */

void packet_receive(int pfd_index, args_t *args, WINDOW **windows) {
  int recv_bytes = 0;
  int recv_flags = 0;

  packet_t *packet;
  if ((packet = malloc(sizeof(packet_t))) == NULL) {
    handle_error(-1, "packet_receive: malloc,", args, windows);
    exit(EXIT_FAILURE);
  }

  if ((recv_bytes = recv(args->pfds[pfd_index].fd, packet, sizeof(packet_t), recv_flags)) == -1) {
    handle_error(recv_bytes, "packet_receive: recv,", args, windows);
    exit(EXIT_FAILURE);
  }

  if (recv_bytes > 0) {
    // get target client 
    client_t *client = *args->client_list;
    while (client->socket != args->pfds[pfd_index].fd) {
      client = client->next;
    }
    history_insert(packet, client, args, windows);
  } else {
    // catch disconnects missed by pollhup (recv'd 0 bytes)
    client_disconnect(pfd_index, args, windows);
  }
}

/* server_init */

void server_init(int *server_socket, args_t *args, WINDOW **windows) {
  // file descriptor for listening socket
  if ((*server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    handle_error(-1, "server_init: socket,", args, windows);
    exit(EXIT_FAILURE);
  };

  // create server IP/port tuple to bind to listening socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(args->port);
  server_address.sin_addr.s_addr = INADDR_ANY;

  // allow bind to reuse local socket address 
  int opt_val = 1;
  if (setsockopt(*server_socket, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int)) == -1) {
    handle_error(-1, "server_init: setsockopt,", args, windows);
    exit(EXIT_FAILURE);
  }

  // bind socket to IP/port tuple
  if ((bind(*server_socket, (struct sockaddr *) &server_address, sizeof(server_address))) == -1) {
    handle_error(-1, "server_init: bind,", args, windows);
    exit(EXIT_FAILURE);
  }

  // specify willingness for server_socket to accept incoming connections 
  if (listen(*server_socket, BACKLOG) == -1) {
    handle_error(-1, "server_init: listen,", args, windows);
    exit(EXIT_FAILURE);
  };

  char address_buffer[INET_ADDRSTRLEN];
  socklen_t addr_len = strlen(address_buffer);
  inet_ntop(AF_INET, &server_address.sin_addr.s_addr, address_buffer, INET_ADDRSTRLEN);
  strcpy(args->host_username, address_buffer);

  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "%s %s/%u", HOST_ON, address_buffer, ntohs(server_address.sin_port));
  mvwprintw(windows[INFO], 2, 1, "%s", LISTENING);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);
}

/* accept_connection - accept incoming client connections */

void accept_connection(int server_socket, args_t *args, WINDOW **windows) {
  // store client address info returned by accept
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);
  memset(&client_addr, 0, addr_len);
  int client_socket;

  // accept incoming connection
  if ((client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_len)) == -1) {
    handle_error(server_socket, "accept_connection: accept,", args, windows);
    exit(EXIT_FAILURE);
  };

  socket_set_nonblock(client_socket);

  // destination string for inet_ntop
  char address_string[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, address_string, INET6_ADDRSTRLEN);

  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "New client connected from %s.\n", address_string);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);

  // set default username
  char username[8];
  snprintf(username, 8, "user%i", client_socket);

  // add to client list and pfds
  client_create(client_socket, username, args, windows);
  pfd_insert(&args->pfds, client_socket, args->fd_count, args->nfds);

  // if only client, set to active user
  if (args->active_client == NULL) {
    set_active_client(username, args, windows);
  }
}

/* socket set_nonblock TODO rename */

void socket_set_nonblock(int socket) {
  int flags;
  flags = fcntl(socket, F_GETFL, 0); // get any existing flags
  fcntl(socket, F_SETFL, flags | O_NONBLOCK); // set non-blocking flag
}
