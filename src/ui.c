#include "ui.h"
#include "utils.h"

// put these in header? // TODO REVIEW 
// windows grid dimensions

// TODO add err checks to ptrs

void init_curses(WINDOW **windows) {
  initscr();
  welcome_screen();
  init_ui(windows);
  print_usage(windows[USAGE]);
}


/* welcome_screen */


void welcome_screen(void) {
  dim_t *welcome_dim = init_dim(LINES, COLS, 0, 0);
  WINDOW *welcome_win = create_window(welcome_dim, TRUE);

  int y, x;
  y = LINES / 2;
  x = (COLS / 2) - (strlen(WELCOME_MSG) / 2);

  mvwprintw(welcome_win, y, x, WELCOME_MSG);
  wrefresh(welcome_win);
  sleep(1); // TODO cheap?, can I do this with ncurses?
  werase(welcome_win); 
  wrefresh(welcome_win); // removes artefacts
  delwin(welcome_win);
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


WINDOW *create_window(dim_t *dim, int box) {
  WINDOW *new_win = newwin(dim->height, dim->width, dim->starty, dim->startx);
  if (new_win == NULL) {
    wprintw(stdscr, "Error creating new window"); // TODO proper error handling
  }
  if (box > 0) {
    box(new_win, 0, 0); // draw outline
  }
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


void init_ui(WINDOW **windows) {
  dim_t *clients, *history, *info, *input, *usage, *input_border;
  clients = init_dim(LINES - HEIGHT_LOW_WIN - 3, WIDTH_L_WIN, 0, 0);
  info = init_dim(HEIGHT_UPP_WIN, WIDTH_R_WIN, 0, WIDTH_L_WIN);
  history = init_dim(HEIGHT_MID_WIN, WIDTH_R_WIN, HEIGHT_UPP_WIN, WIDTH_L_WIN);
  //FIX THESE shits
  usage = init_dim(HEIGHT_LOW_WIN + 3, WIDTH_L_WIN, LINES - HEIGHT_LOW_WIN - 3, 0);
  input = init_dim(HEIGHT_LOW_WIN - 2, WIDTH_R_WIN - 2, LINES - HEIGHT_LOW_WIN + 1, WIDTH_L_WIN +1);
  input_border = init_dim(HEIGHT_LOW_WIN, WIDTH_R_WIN, LINES - HEIGHT_LOW_WIN, WIDTH_L_WIN);
  // TODO hardcode these!

  // windows ordered from left to right, top to bottom
  windows[0] = create_window(clients, TRUE);
  windows[1] = create_window(info, TRUE);
  windows[2] = create_window(history, TRUE);
  windows[3] = create_window(usage, TRUE);
  windows[4] = create_window(input, FALSE);
  windows[5] = create_window(input_border, TRUE);

  // enable function keys in input box
  keypad(windows[4], TRUE);
}


/* print_usage renmae command list or someting */

// this will change when ncurses added/
void print_usage(WINDOW *win) {
  mvwprintw(win, 1, 1, "'$user' hostname");
  mvwprintw(win, 3, 1, "'#user' clientname");
  mvwprintw(win, 2, 1, "'@user' cur client");
  mvwprintw(win, 4, 1, "'!IP PORT' connect");
  mvwprintw(win, 5, 1, "'quit' exit");
  wrefresh(win);
};