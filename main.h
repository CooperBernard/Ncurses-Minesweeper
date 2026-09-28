#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

enum difficulties
{
    EXIT = 0, BEGINNER = 1, INTERMEDIATE = 2, EXPERT = 3
};

enum color_pairs    // A_NORMAL is default
{
    CURSOR = 0, NUMBER = 1, FLAG = 2, MINE = 3
};

void main_menu(char** gb, char** db);   // displays main menu

void generate_board(char** gb, char** db, int difficulty);  // generates game board

void display_controls();    // prints menu of controls

void game_start(char** gb, char** db, int height, int width);   // begin game after board generation

void display_board(WINDOW* win, char** gb, char** db, int height, int width);   // refreshes gameboard display

void move_highlighted_tile(WINDOW* win, char** db, int start_y, int start_x, int y, int x); // moves highlited tile based on user input

void click_tile(WINDOW* win, char** gb, char** db, int y, int x, int height, int width);   // reveals tile when spacebar pressed

void recursive_reveal(WINDOW* win, char** gb, char** db, int start_y, int start_x, int height, int width); // recursively reveals adjacent tiles for all adjacent empty tiles

void click_number(WINDOW* win, char** gb, char** db, int y, int x, int height, int width);  // reveals adjacent non-flagged tiles

bool check_winner(char** db, int height, int width); // returns true if all non-mine cells are revealed and all mines are flagged

void game_over(WINDOW* win, char** gb, char** db, int height, int width, bool winner);  // called when game ends by loss or win

int num_adjacent_mines(char** gb, int height, int width, int y, int x);    // returns number of mines adjacent to tile with coords

void exit_program(char** gb, char** db, int height);   // free all memory and exit program