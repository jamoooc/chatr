# chatr

Multi-user chat client written in C, using the ncurses library for UI, and Unity for tests.

Run `make` to compile or `make test` to run the test suite.

## usage

`./chatr [-p localport 0-65535] [-h help] [-v version]`

Set the host name with:         `$hostname`.
Set the client name with:       `#clientname`.
Select the active client with:  `@clientname`.
Connect to a new client with:   `!IP PORT`.
Exit with:                      `/quit`.

## program structure

```
get_opt
  valid_port
create_pfds_array
create_windows_array
args
init_curses
init_server
insert_pfd
  poll
    get_input
      process_input
        set_active_client
          valid_username
        set_host_username
        set_client_username
          valid_username
        add_client
          create_client
          append_client
          insert_pfd
        create_message
          append_message
      accept_connection
        create_client
        append_client
        insert_pfd
          print_clients
      disconnect_client
        close
        remove_client
        remove_pfd
        print_clients
      receive_packet
        insert_history
        disconnect_client
      transmit_packet
        insert_history
        remove_message
free(args)
free_client
  free_history
free_messages
free_windows
free(windows_arr)
close(server)
exit
```
