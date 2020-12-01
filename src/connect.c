#include "connect.h"
#include "utils.h"
#include "client.h"
#include "pfds.h"

// receive and transmit proba shoulnt be in here....


/* transmit_packet */


void transmit_packet(message_t *message, args_t *args, WINDOW **windows) {
  // check socket is valid and send packet // TODO is err check here worthwhile
  int send_bytes;
  if (message->client->socket > MIN_CLIENT_SOCK) {
    if ((send_bytes = send(message->client->socket, message->packet, sizeof(packet_t), 0)) == -1) {
      perror("send");
      exit(EXIT_FAILURE);
      // TODO handle if client has gone offline since message entered queue?? will be -1. mayb no exit(), return?
    }
  }

  // store packet before message free'd
  packet_t *packet = malloc(sizeof(packet_t));
  if (packet == NULL) {
    perror("malloc receive_packet");
    exit(EXIT_FAILURE);
  }
  *packet = *message->packet;

  // add to history and remove message from queue
  // TODO is it better to pass the whole message and to input_history????
  insert_history(message->packet, message->client, args, windows);
  remove_message(message, args->message_queue, windows);
}


/* receive_packet */

// TODO don't really need socket here if passing pfds and index... OR pass pfds and client list separately...
void receive_packet(int pfd_index, args_t *args, WINDOW **windows) {
  // printf("recieve_packet\n");
  int recv_bytes = 0;
  int recv_flags = 0;

  packet_t *packet = malloc(sizeof(packet_t));
  if (packet == NULL) {
    perror("malloc receive_packet");
    exit(EXIT_FAILURE);
  }

  if ((recv_bytes = recv(args->pfds[pfd_index].fd, packet, sizeof(packet_t), recv_flags)) == -1) {
    perror("recv");
    // exit(EXIT_FAILURE); // TODO don't quit on error,do summin??? look into this
  }

  if (recv_bytes > 0) {
    // TODO is there a more efficient way of doing this...?
    client_t *client = *args->client_list;
    while (client->socket != args->pfds[pfd_index].fd) {
      client = client->next;
    }
    insert_history(packet, client, args, windows);
  } else {
    // catch disconnects missed by pollhup and errors
    if (recv_bytes < 0) {
      perror("recv");
    }
    disconnect_client(args->pfds[pfd_index].fd, pfd_index, args, windows);
  }
}


/* init_server */


void init_server(int *server_socket, args_t *args, WINDOW **windows) {
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

  // specify willingness for server_socket to accept incoming connections 
  if (listen(*server_socket, BACKLOG) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  };

  args->server_addr = server_address;
  char address_buffer[INET_ADDRSTRLEN];
  socklen_t addr_len = strlen(address_buffer);
  inet_ntop(AF_INET, &server_address.sin_addr.s_addr, address_buffer, INET_ADDRSTRLEN);

  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "Host server started on %s port %u\n", 
    address_buffer, ntohs(server_address.sin_port));
  mvwprintw(windows[INFO], 2, 1, "Waiting for client connection...\n");
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);
}


/* accept_connection - accept incoming client connections */


void accept_connection(int server_socket, args_t *args, WINDOW **windows) {
  // store client address info returned by accept
  struct sockaddr_in client_addr; // TODO should this be memset
  socklen_t addr_len = sizeof(client_addr);
  memset(&client_addr, 0, addr_len);
  int client_socket;

  // accept incoming connection
  if ((client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_len)) == -1) {
    perror("accept");
    exit(EXIT_FAILURE);
  };

  set_nonblock(client_socket);

  //TODO do I need this string, on client_T????DODODO
  // destination string for inet_ntop TODO  use INET6 for when using getaddrinfo
  char address_string[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, address_string, INET6_ADDRSTRLEN);

  werase(windows[INFO]);
  mvwprintw(windows[INFO], 1, 1, "New client connected from %s.\n", address_string);
  box(windows[INFO], 0, 0);
  wrefresh(windows[INFO]);

  char username[8];
  snprintf(username, 8, "user%i", client_socket);

  // add to client list and pfds
  client_t *client = create_client(client_socket, username, client_addr);
  append_client(client, args->client_list, windows);
  insert_pfd(&args->pfds, client_socket, args->fd_count, args->nfds);

  // if first client, set to active user
  if (args->active_client == NULL) {
    set_active_client(username, args, windows);
  }

}


/* socket set_nonblock TODO rename */


void set_nonblock(int socket) {
  int flags;
  flags = fcntl(socket, F_GETFL, 0); // get any existing flags
  // assert(flags != -1);
  fcntl(socket, F_SETFL, flags | O_NONBLOCK); // set non-blocking flag
}


/* get sockaddr, IPv4 or IPv6 (from beej network guide) */ 


void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
