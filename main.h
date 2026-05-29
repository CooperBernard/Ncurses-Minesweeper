#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

enum difficulties
{
    EXIT = 0, BEGINNER = 1, INTERMEDIATE = 2, EXPERT = 3
};

void main_menu(char** gb);   // displays main menu

void generate_board(char** gb, int difficulty);  // generates game board

void display_board(char** gb, char** db);   // refreshes gameboard display

int num_adjacent_mines(char** gb, int height, int width, int y, int x);    // returns number of mines adjacent to tile with coords

void exit_program(char** gb, int gb_height);   // free all memory and exit program