#include "connect.h"
#include "utils.h"
#include "client.h"

/* transmit_packet */



void transmit_packet(int socket, message_t **message_queue) {
  // check socket is valid and send packet
  message_t *message = *message_queue;
  int send_bytes;
  if (message->socket > 2) {
    if ((send_bytes = send(message->socket, message->packet, sizeof(packet_t), 0)) == -1) {
      perror("send");
      exit(EXIT_FAILURE);
    }
  }
  // remove message from queue
  remove_message(message, message_queue);
}


/* receive_packet */


// int i is gross here... whats it for, couldn't i jsut pass the correct pfd directly....
void receive_packet(int socket, int pfd_index, packet_t *packet, args_t *args) {
  // printf("recieve_packet\n");
  int recv_bytes = 0;
  int recv_flags = 0;

  if ((recv_bytes = recv(socket, packet, sizeof(packet_t), recv_flags)) == -1) {
    perror("recv");
    // exit(EXIT_FAILURE);
  }

  // catch disconnects missed by pollhup and errors
  if (recv_bytes > 0) {
    printf("%s: %s\n", packet->username, packet->body);
  } else {
    if (recv_bytes < 0) {
      perror("recv");
    }
    disconnect_client(args->pfds[pfd_index].fd, pfd_index, args);
  }
}


/* init_server */


void init_server(int *server_socket, args_t *args) {
  // file descriptor for listening socket
  if ((*server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  };

  // create servers IP/port tuple to bind to listening socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(args->port);
  server_address.sin_addr.s_addr = INADDR_ANY;

  // allow bind to reuse local socket address 
  int opt_val = 1;
  if (setsockopt(*server_socket, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int)) == -1) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  // bind socket to IP/port tuple
  if ((bind(*server_socket, (struct sockaddr *) &server_address, sizeof(server_address))) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  // 
  char address_buffer[INET_ADDRSTRLEN];
  socklen_t addr_len = strlen(address_buffer);
  inet_ntop(AF_INET, &server_address.sin_addr.s_addr, address_buffer, INET_ADDRSTRLEN);
  printf("Host server started on %s port %u\n", 
    address_buffer, ntohs(server_address.sin_port));

  args->server_addr = server_address;

  // specify willingness for server_socket to accept incoming connections 
  if (listen(*server_socket, BACKLOG) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  };

  printf("Waiting for client connection...\n");
}


/* accept_connection - accept incoming client connections */


void accept_connection(int server_socket, args_t *args) {

  // structure to store client address info returned by accept
  struct sockaddr_in client_addr; // TODO should this be memset
  socklen_t addr_len = sizeof(client_addr);
  memset(&client_addr, 0, addr_len);
  int client_socket;

  // accept incoming connection (blocking)
  if ((client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_len)) == -1) {
    perror("accept");
    exit(EXIT_FAILURE);
  };

  set_nonblock(client_socket);

  // destination string for inet_ntop - use INET6 for when using getaddrinfo
  char address_string[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, address_string, INET6_ADDRSTRLEN);
  printf("New client connected from %s.\n", address_string);

  // add to client list and pfds
  client_t *client = create_client(client_socket, address_string, client_addr);
  append_client(client, args->client_list);
  insert_pfd(&args->pfds, client_socket, args->fd_count, args->nfds);

  // if first client, set to active user
  if (args->active_socket ==-1-1) {
    select_active_client(address_string, args);
  }
}


/* socket set_nonblock TODO rename */


void set_nonblock(int socket) {
  int flags;
  flags = fcntl(socket, F_GETFL, 0); // get any existing flags
  assert(flags != -1);
  fcntl(socket, F_SETFL, flags | O_NONBLOCK); // set non-blocking flag
}




/* get sockaddr, IPv4 or IPv6 (from beej network guide) */ 


void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

