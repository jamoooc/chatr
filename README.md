# chatr

Multi-user chat client in C, using the ncurses library for UI, and Unity for tests.

Run `make` to compile or `make test` to run the tests.

## usage

`./chatr [-p localport 0-65535] [-h help] [-v version]`

- Set the host name with:         `$hostname`.
- Set the client name with:       `#clientname`.
- Select the active client with:  `@clientname`.
- Connect to a new client with:   `!IP PORT`.
- Exit with:                      `/quit`.

## structure

```
get_opt
  valid_port
pfd_create_array
window_create_array
args
init_curses
init_server
pfd_insert
poll
  get_input
    handle_input
      client_set_active
        valid_username
      set_host_username
      set_client_username
        valid_username
      client_connect
        client_create
        pfd_insert
      message_create
  accept_connection
    client_create
    pfd_insert
      client_print
  client_disconnect
    client_destroy
    pfd_destroy
    client_print
  packet_receive
    history_insert
    client_disconnect
  packet_transmit
    history_insert
    message_destroy
client_free
  client_history_free
message_free
window_free
free(windows)
free(args)
endwin
close(server)
exit
```
