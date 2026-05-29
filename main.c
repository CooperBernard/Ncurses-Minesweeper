#include "main.h"

int main()
{
    char** gameboard;       // logical level of game state

    initscr();  // initialize ncurses screen
    cbreak();   // allow ctrl+c to exit program
    noecho();   // does not display user's inputs

    bool playing = true;

    while(playing)
    {
        main_menu(gameboard);    // display main menu
    }

    endwin();   // exit ncurses
    return 0;
}

// return to main menu of game
void main_menu(char** gb)
{
    clear();
    WINDOW* menu;   // window for main menu
    WINDOW* title;  // title of game
    int menu_height, menu_width, menu_start_y, menu_start_x;  // dimensions and coords of menu window
    menu_height = 14;
    menu_width = 25;
    menu_start_y = 0;
    menu_start_x = 0;
    menu = newwin(menu_height, menu_width, menu_start_y, menu_start_x);   // initialize menu
    int title_height, title_width, title_start_y, title_start_x;
    title_height = 7;
    title_width = 21;
    title_start_y = 0;
    title_start_x = 1;
    title = newwin(title_height, title_width, title_start_y, title_start_x);
    box(title, 0, 0);
    // print menu text
    mvwprintw(title, 2, 5, "Minesweeper");
    mvwprintw(title, 4, 2, "By Cooper Bernard");
    mvwprintw(menu, 8, 0, "1. Beginner       (9x9)");
    mvwprintw(menu, 9, 0, "2. Intermediate   (16x16)");
    mvwprintw(menu, 10, 0, "3. Expert         (16x30)");
    mvwprintw(menu, 11, 0, "0. Exit");
    move(13, 0);    // move cursor below menu
    refresh();
    wrefresh(menu);
    wrefresh(title);

    bool in_menu = true;    // bool for while loop
    int difficulty;         // game difficulty

    while(in_menu)
    {
        move(13, 0);    // move cursor below menu
        difficulty = getch();
        difficulty -= 48;   // convert from ASCII to numerical value
        if(difficulty < 0 || difficulty > 3)
        {
            refresh();
            wrefresh(menu);
            wrefresh(title);
            continue;
        }
        else if(difficulty == 0)
        {
            endwin();
            exit(EXIT_SUCCESS); // end program
        }
        else
        {
            in_menu = false;
            generate_board(gb, difficulty);
        }
    }
}

// generates mines and numbers for gameboard
void generate_board(char** gb, int difficulty)
{
    int gb_width, gb_height, mines;
    switch(difficulty)
    {
        case BEGINNER:
            gb_width = 9;
            gb_height = 9;
            mines = 10;
            break;
        case INTERMEDIATE:
            gb_width = 16;
            gb_height = 16;
            mines = 40;
            break;
        case EXPERT:
            gb_width = 30;
            gb_height = 16;
            mines = 99;
            break;
        default:
            clear();
            printw("Error: Invalid argument passed to generate_board(). Press any key to return to menu");
            getch();
            main_menu(gb);
    }
    // allocate memory for gameboard
    gb = (char**)calloc(gb_height, sizeof(char*));
    for(int i=0; i<gb_height; i++)
    {
        gb[i] = (char*)calloc(gb_width, sizeof(char));
    }

    // initialize empty board
    for(int y=0; y<gb_height; y++)
    {
        for(int x=0; x<gb_width; x++)
        {
            gb[y][x] = ' ';
        }
    }
    // randomly generate mines
    srand(time(NULL));
    int y,x;
    for(int i=0; i<mines; i++)
    {
        bool valid_coords = false;
        while(!valid_coords)
        {
            x = rand() % gb_width;
            y = rand() % gb_height;
            if(gb[y][x] != 'M')
            {
                valid_coords = true;
                gb[y][x] = 'M';     // M indicates mine
            }
        }
    }
    // assign numbers to mine-adjacent tiles
    for(int y = 0; y<gb_height; y++)
    {
        for(int x=0; x<gb_width; x++)
        {
            if(gb[y][x] != ' ') // ignore non-empty cells
            {
                continue;
            }
            int mines = num_adjacent_mines(gb, gb_height, gb_width, y, x);
            if(mines>0)
            {
                mines += 48;    // convert to ASCII representation
                gb[y][x] = mines;
            }
        }
    }
    clear();
    for(int y=0; y<gb_height; y++)
    {
        for(int x=0; x<gb_width; x++)
        {
            char c = gb[y][x];
            addch(c);
            addch(' ');
        }
        move(y, 0);
    }
    refresh();
    for(int y=0; y<gb_height; y++)
    {
        free(gb[y]);
    }
    free(gb);
    getch();
}

void display_board(char** gb, char** db)
{

}

int num_adjacent_mines(char** gb, int height, int width, int y, int x)
{
    int result = 0;
    for(int a=y-1; a<=y+1; a++)
    {
        for(int b=x-1; b<=x+1; b++)
        {
            if(a<0 || a>=height || b<0 || b>=width || (a==y && b==x))
            {
                continue;
            }
            if(gb[a][b] == 'M')
            {
                result++;
            }
        }
    }
    return result;
}