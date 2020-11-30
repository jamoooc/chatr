#ifndef NCURSES_H
#define NCURSES_H

#include <ncurses.h>
#include "main.h"

// #define HEIGHT_LOW_WIN 0.25 * LINES
// #define HEIGHT_UPP_WIN 0.75 * LINES
// #define WIDTH_L_WIN 0.25 * COLS
// #define WIDTH_R_WIN 0.75 * COLS


#define WIDTH_L_WIN 0.20 * COLS
#define WIDTH_R_WIN 0.80 * COLS
#define HEIGHT_UPP_WIN 4
#define HEIGHT_MID_WIN (LINES - (HEIGHT_UPP_WIN + HEIGHT_LOW_WIN))
#define HEIGHT_LOW_WIN 4 // LINES not zero based, +1 needed to fill screen if start at 0

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
WINDOW *create_window(dim_t *dim);

#endif