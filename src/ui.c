#include "ui.h"

/* init_curses */

void init_curses(WINDOW **windows) {
  initscr();
  welcome_screen(windows);
  init_ui(windows);
  print_usage(windows[USAGE]);
}

/* welcome_screen */

void welcome_screen(WINDOW **windows) {
  dim_t *welcome_dim = init_dim(LINES, COLS, 0, 0);
  WINDOW *welcome_win = window_create(welcome_dim, TRUE, windows);

  int y, x;
  y = (LINES / 2) - 4;
  x = COLS / 2;

  mvwprintw(welcome_win, y,     x - (strlen(WELCOME_MSG) / 2),        WELCOME_MSG);
  mvwprintw(welcome_win, y + 2, x - (strlen(HOSTNAME_MSG) / 2),       HOSTNAME_MSG);
  mvwprintw(welcome_win, y + 3, x - (strlen(CLIENTNAME_MSG) / 2),     CLIENTNAME_MSG);
  mvwprintw(welcome_win, y + 4, x - (strlen(SELECT_CLIENT_MSG) / 2),  SELECT_CLIENT_MSG);
  mvwprintw(welcome_win, y + 5, x - (strlen(CONNET_MSG) / 2),         CONNET_MSG);
  mvwprintw(welcome_win, y + 6, x - (strlen(QUIT_MSG) / 2),           QUIT_MSG);
  mvwprintw(welcome_win, y + 8, x - (strlen(CONTINUE_MSG) / 2),       CONTINUE_MSG);

  wrefresh(welcome_win);
  wgetch(welcome_win);
  werase(welcome_win); 
  wrefresh(welcome_win); // removes artefacts
  delwin(welcome_win);
}

/* window_creates_array */

void exit_screen(WINDOW **windows) {
  dim_t *exit_dim = init_dim(0, 0, 0, 0);
  WINDOW *exit_win = window_create(exit_dim, TRUE, windows);

  int y, x;
  y = (LINES / 2) / 2;
  x = COLS / 2;

  mvwprintw(exit_win, y, x - (strlen(SHUTDOWN_MSG) / 2), SHUTDOWN_MSG);
  wrefresh(exit_win);
  sleep(1);
  werase(exit_win); 
  wrefresh(exit_win);
  delwin(exit_win);
}

/* window_creates_array */

WINDOW **window_create_array(int n) {
  WINDOW **window_array = malloc(sizeof(WINDOW *) * n);
  if (window_array == NULL) {
    perror("window_create_array malloc");
    exit(EXIT_FAILURE);
  }
  return window_array;
}

/* window_create */

WINDOW *window_create(dim_t *dim, int box, WINDOW **windows) {
  WINDOW *new_win = newwin(dim->height, dim->width, dim->starty, dim->startx);
  if (new_win == NULL) {
    // window_free(windows); // TODO this will fail if not all created
    free(windows);
    perror("create window");
    exit(EXIT_FAILURE);
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
  dim_t *dim;
  if ((dim = malloc(sizeof(dim_t))) == NULL) { // ptr free'd in window_create
    perror("init dim");
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
  clients = init_dim(LINES - HEIGHT_USE, L_WIDTH, 0, 0);
  usage = init_dim(HEIGHT_USE, L_WIDTH, LINES - HEIGHT_USE, 0);
  info = init_dim(HEIGHT_UPP, R_WIDTH, 0, L_WIDTH);
  history = init_dim(HEIGHT_MID, R_WIDTH, HEIGHT_UPP, L_WIDTH);
  input = init_dim(HEIGHT_LOW - BORDER, R_WIDTH - BORDER, LINES - HEIGHT_LOW + 1, L_WIDTH + 1);
  input_border = init_dim(HEIGHT_LOW, R_WIDTH, LINES - HEIGHT_LOW, L_WIDTH);

  // windows ordered from left to right, top to bottom
  windows[0] = window_create(clients, TRUE, windows);
  windows[1] = window_create(info, TRUE, windows);
  windows[2] = window_create(history, TRUE, windows);
  windows[3] = window_create(usage, TRUE, windows);
  windows[4] = window_create(input, FALSE, windows);
  windows[5] = window_create(input_border, TRUE, windows);

  // check for NULL ptrs
  for (int i = 0; i < N_WINDOWS; i++) {
    if (windows[i] == NULL) {
      perror("create window");
      exit(EXIT_FAILURE);
    }
  }

  keypad(windows[4], TRUE); // enable function keys in input box
}

/* print_usage */

void print_usage(WINDOW *win) {
  mvwprintw(win, 1, 2, "$hostname");
  mvwprintw(win, 3, 2, "#clientname");
  mvwprintw(win, 2, 2, "@select_client");
  mvwprintw(win, 4, 2, "!IP PORT");
  mvwprintw(win, 5, 2, "/quit");
  wrefresh(win);
};

/* free_windows_array */

void window_free(WINDOW **window) {
  for (int i = 0; i < N_WINDOWS; i++) {
    werase(window[i]); 
    wrefresh(window[i]);
    delwin(window[i]);
  }
}
