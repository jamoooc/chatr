#include "ui.h"

// put these in header? // TODO REVIEW 
// windows grid dimensions

void init_curses(WINDOW **windows) {
  initscr();
  // nocbreak();
  // echo();
  // ???? TODO what other stuff needs init?
  // welcome_screen();
  refresh();

  init_ui(windows);
}


/* welcome_screen */

// TODO something cool....
void welcome_screen(void) {
  // TODO no stdscr now?
  // make new screen - full size, welcome message
  wprintw(stdscr, "Welcome to chatr...");
  sleep(1); // TODO cheap, figure out how to do this with ncurses
  // delwin(welcome_scr);
}


/* create_windows_array */


WINDOW **create_windows_array(int n) {
  WINDOW **window_array = malloc(sizeof(WINDOW *) * n);
  if (window_array == NULL) {
    perror("malloc create_windows_array");
    exit(EXIT_FAILURE);
  }
  return window_array;
}


/* create_window */


WINDOW *create_window(dim_t *dim) {
  WINDOW *new_win = newwin(dim->height, dim->width, dim->starty, dim->startx);
  // no stdscr
  if (new_win == NULL) {
    wprintw(stdscr, "Error creating new window"); // TODO proper error handling
  }
  box(new_win, 0, 0); // draw outline
  wrefresh(new_win);
  free(dim);

  return new_win;
}


/* init_dimensions */


dim_t *init_dim(int h, int w, int y, int x) {
  dim_t *dim = malloc(sizeof(dim_t)); // ptr free'd in create_window
  if (dim == NULL) {
    wprintw(stdscr, "Error creating new window"); // TODO proper error handling!!!
    exit(EXIT_FAILURE);
  }
  dim->height = h;
  dim->width = w;
  dim->starty = y;
  dim->startx = x;
  return dim;
}


/* init_ui */

// probably want a return val here to exit on err
void init_ui(WINDOW **windows) {
  dim_t *clients, *history, *info, *input;
  clients = init_dim(LINES, WIDTH_L_WIN, 0, 0);
  info = init_dim(HEIGHT_UPP_WIN, WIDTH_R_WIN, 0, WIDTH_L_WIN);
  history = init_dim(HEIGHT_MID_WIN, WIDTH_R_WIN, HEIGHT_UPP_WIN, WIDTH_L_WIN);
  input = init_dim(HEIGHT_LOW_WIN, WIDTH_R_WIN, LINES - HEIGHT_LOW_WIN, WIDTH_L_WIN);
  
  // windows ordered from left to right, top to bottom
  windows[0] = create_window(clients);
  windows[1] = create_window(info);
  windows[2] = create_window(history);
  windows[3] = create_window(input); // TODO maybe have sub window here for text box? 

  // return SUCCESS or fail??
}