#include "curses.h"

int main()
{
  initscr();

  raw();
  attron(A_STANDOUT | A_UNDERLINE);
  mvprintw(10, 10, "HELLO WORLD");
  attroff(A_STANDOUT | A_UNDERLINE);
  refresh();
  getchar();

  endwin();
  return 0;
}