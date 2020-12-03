#ifndef NCURSES_H
#define NCURSES_H

#include <ncurses.h>
#include "main.h"

#define LEFT 0.30 * COLS
#define RIGHT 0.70 * COLS
#define MIN_L_WIDTH 15

#define L_WIDTH (MIN_L_WIDTH > LEFT ? MIN_L_WIDTH : LEFT)
#define R_WIDTH (L_WIDTH == MIN_L_WIDTH ? COLS - L_WIDTH : RIGHT)

#define HEIGHT_UPP 4
#define HEIGHT_MID (LINES - (HEIGHT_UPP + HEIGHT_LOW))
#define HEIGHT_LOW 4
#define HISTORY_OFFSET (HEIGHT_MID - 3)
#define HEIGHT_USE 7
#define BORDER 2

#define WELCOME_MSG "Welcome to chatr"


typedef struct win_dim {
  int height;
  int width;
  int starty;
  int startx;
  // int nlines; int ncols; int begin_y; int begin_x // from header definition
} dim_t;

// error values?!?!?!
void init_curses(WINDOW **windows);
void init_ui(WINDOW **windows);
void welcome_screen(void);
dim_t *init_dim(int h, int w, int y, int x);
WINDOW **create_windows_array(int n);
WINDOW *create_window(dim_t *dim, int box);
void print_usage(WINDOW *win);

#endif