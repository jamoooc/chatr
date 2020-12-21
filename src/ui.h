#ifndef NCURSES_H
#define NCURSES_H

#include <ncurses.h>

#include "main.h"

#define LEFT 0.30 * COLS
#define RIGHT 0.70 * COLS
#define MIN_L_WIDTH 18 // to fit usage reminder

#define L_WIDTH (MIN_L_WIDTH > LEFT ? MIN_L_WIDTH : LEFT)
#define R_WIDTH (L_WIDTH == MIN_L_WIDTH ? COLS - L_WIDTH : RIGHT)

#define HEIGHT_UPP 4
#define HEIGHT_MID (LINES - (HEIGHT_UPP + HEIGHT_LOW))
#define HEIGHT_LOW 4
#define HISTORY_OFFSET (HEIGHT_MID - 3)
#define HEIGHT_USE 7
#define BORDER 2

#define WELCOME_MSG       "Welcome to chatr!"
#define HOSTNAME_MSG      "Set the hosts name with: '$hostname'."
#define CLIENTNAME_MSG    "Set the client name with: '#clientname'."
#define SELECT_CLIENT_MSG "Select the active client with '@clientname'."
#define CONNET_MSG        "Connect to a new client with '!IP PORT'."
#define QUIT_MSG          "Exit with '/quit'."
#define CONTINUE_MSG      "Press any key to continue."
#define SHUTDOWN_MSG      "Shutting down..."


typedef struct win_dim {
  int height;
  int width;
  int starty;
  int startx;
} dim_t;

void welcome_screen(void);
void exit_screen(void);
void print_usage(WINDOW *win);
void init_curses(WINDOW **windows);
void init_ui(WINDOW **windows);
void free_windows(WINDOW **windows);
dim_t *init_dim(int h, int w, int y, int x);
WINDOW **create_windows_array(int n);
WINDOW *create_window(dim_t *dim, int draw_box);

#endif