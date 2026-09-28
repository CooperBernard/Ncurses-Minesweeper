#include "main.h"

int main()
{
    char** gameboard;       // logical level of game state
    char** display_board;   // user level of game state

    initscr();  // initialize ncurses screen
    cbreak();   // allow ctrl+c to exit program
    noecho();   // does not display user's inputs
    start_color();  // enable colors

    // define color pairs
    init_pair(CURSOR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(NUMBER, COLOR_CYAN, COLOR_BLACK);
    init_pair(FLAG, COLOR_GREEN, COLOR_BLACK);
    init_pair(MINE, COLOR_RED, COLOR_BLACK);

    bool playing = true;

    while(playing)
    {
        main_menu(gameboard, display_board);    // display main menu
    }

    endwin();   // exit ncurses
    return 0;
}

// return to main menu of game
void main_menu(char** gb, char** db)
{
    clear();
    WINDOW* menu;   // window for main menu
    WINDOW* title;  // title of game
    int yBeg, xBeg, yMax, xMax; // screen dimensions
    getbegyx(stdscr, yBeg, xBeg);
    getmaxyx(stdscr, yMax, xMax);
    int title_height, title_width, title_start_y, title_start_x; // dimensions and coords of title window
    // define window size of title
    title_height = 7;
    title_width = 21;
    title_start_y = (yMax - yBeg) / 2 - (title_height);
    title_start_x = (xMax - xBeg + 1) / 2 - (title_width / 2);
    title = newwin(title_height, title_width, title_start_y, title_start_x);
    box(title, 0, 0);
    int menu_height, menu_width, menu_start_y, menu_start_x;  // dimensions and coords of menu window
    // define window size of menu
    menu_height = 14;
    menu_width = 25;
    menu_start_y = (yMax - yBeg) / 2 - (menu_height) + 6;
    menu_start_x = (xMax - xBeg + 1) / 2 - (menu_width / 2);
    menu = newwin(menu_height, menu_width, menu_start_y, menu_start_x);   // initialize menu
    // print menu text
    mvwprintw(title, 2, 5, "Minesweeper");
    mvwprintw(title, 4, 2, "By Cooper Bernard");
    mvwprintw(menu, 8, 0, "1. Beginner       (9x9)");
    mvwprintw(menu, 9, 0, "2. Intermediate   (16x16)");
    mvwprintw(menu, 10, 0, "3. Expert         (16x30)");
    mvwprintw(menu, 11, 0, "0. Exit");
    move(13, 0);    // move cursor below menu
    refresh();
    wrefresh(menu); // update display
    wrefresh(title);

    bool in_menu = true;    // bool for while loop
    int difficulty;         // game difficulty

    while(in_menu)
    {
        move(13, 0);    // move cursor below menu
        difficulty = getch();
        difficulty -= 48;   // convert from ASCII to numerical value
        // ignore invalid input
        if(difficulty < 0 || difficulty > 3)
        {
            refresh();
            wrefresh(menu);
            wrefresh(title);
            continue;
        }
        else if(difficulty == 0)    // exit upon input 0
        {
            exit_program(gb, db, 0);
        }
        else    // begin game with selected difficulty
        {
            in_menu = false;
            generate_board(gb, db, difficulty);
        }
    }
}

// initializes arrays for minefield and display board
void generate_board(char** gb, char** db, int difficulty)
{
    int gb_width, gb_height;    // dimensions of minefield
    switch(difficulty)  // initialize dimensions according to difficulty
    {
        case BEGINNER:
            gb_width = 9;
            gb_height = 9;
            break;
        case INTERMEDIATE:
            gb_width = 16;
            gb_height = 16;
            break;
        case EXPERT:
            gb_width = 30;
            gb_height = 16;
            break;
        default:
            clear();
            printw("Error: Invalid argument passed to generate_board(). Press any key to return to menu");
            getch();
            main_menu(gb, db);
    }
    // allocate memory for gameboard and display board
    gb = (char**)malloc(gb_height*sizeof(char*));
    db = (char**)malloc(gb_height*sizeof(char*));
    for(int i=0; i < gb_height; i++)
    {
        gb[i] = (char*)malloc(gb_width*sizeof(char));
        db[i] = (char*)malloc(gb_width*sizeof(char));
    }

    // initialize empty board
    for(int y=0; y < gb_height; y++)
    {
        for(int x=0; x<gb_width; x++)
        {
            gb[y][x] = ' ';
            db[y][x] = '*';
        }
    }
    game_start(gb, db, gb_height, gb_width);
}

// generates mines and numbers for gameboard
void populate_minefield(char** gb, int height, int width, int yCurrent, int xCurrent)
{
    int mines = 0;
    switch(width)   // number of mines based on difficulty-unique width
    {
        case 9:
            mines = 10;
            break;
        case 16:
            mines = 40;
        case 30:
            mines = 99;
            break;
        default:
            break;
    }
    // randomly generate mines
    srand(time(NULL));
    int y, x;
    for(int i=0; i<mines; i++)
    {
        bool valid_coords = false;
        while(!valid_coords)
        {
            x = rand() % width;
            y = rand() % height;
            // ignore 3x3 grid around first click
            if(gb[y][x] != 'M' && ((y < yCurrent-2 || y > yCurrent+2) || (x < xCurrent-2 || x > xCurrent+2)))
            {
                valid_coords = true;
                gb[y][x] = 'M';     // M indicates mine
            }
        }
    }
    // assign numbers to mine-adjacent tiles
    for(int y = 0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            if(gb[y][x] != ' ') // ignore non-empty cells
            {
                continue;
            }
            int mines = num_adjacent_mines(gb, height, width, y, x);
            if(mines>0)
            {
                mines += 48;    // convert to ASCII representation
                gb[y][x] = mines;
            }
        }
    }
}

// prints controls to screen
void display_controls()
{
    clear();
    int yBeg, xBeg, yMax, xMax; // screen dimensions
    getbegyx(stdscr, yBeg, xBeg);
    getmaxyx(stdscr, yMax, xMax);
    // assign window dimensions
    int height = 7;
    int width = 24;
    // calculate center of screen
    int yStart = (yMax - yBeg) / 2 - (height / 2);
    int xStart = (xMax - xBeg + 1) / 2 - (width / 2);
    // print menu
    WINDOW* controls = newwin(height, width, yStart, xStart);
    box(controls, 0, 0);
    mvwprintw(controls, 2, 2, "Arrow keys: Move");
    mvwprintw(controls, 3, 2, "Spacebar: Select");
    mvwprintw(controls, 4, 2, "F: Place/Remove Flag");
    refresh();
    wrefresh(controls);
    getch();
}

// initializes and maintains game state
void game_start(char** gb, char** db, int height, int width)
{
    clear();
    display_controls();
    bool playing = true;
    keypad(stdscr, true);   // enables arrow keys
    int yBeg, xBeg, yMax, xMax; // screen dimensions
    getbegyx(stdscr, yBeg, xBeg);
    getmaxyx(stdscr, yMax, xMax);
    // check screen size
    if((yMax-yBeg) < height || (xMax-xBeg) < (width * 2 - 1))
    {
        move(0, 0);
        printw("Error: Terminal too small. Press any key to exit program.");
        getch();
        exit_program(gb, db, height);
    }
    // calculate center of screen
    int yStart = (yMax - yBeg) / 2 - (height / 2);
    int xStart = (xMax - xBeg + 1) / 2 - width + 1;
    WINDOW* minefield = newwin(height, (width*2-1), yStart, xStart);
    refresh();
    wrefresh(minefield);
    int yCurrent = height / 2, xCurrent = width / 2;
    char c = db[yCurrent][xCurrent];
    // highlight cursor
    attron(A_STANDOUT);
    wprintw(minefield, "%c", c);
    attroff(A_STANDOUT);
    refresh();
    wrefresh(minefield);
    int yNext, xNext;
    bool first_input = true;
    int mines_remaining = 0;    // initialize mine counter
    switch(width)
    {
        case 9:
            mines_remaining = 10;
            break;
        case 16:
            mines_remaining = 40;
            break;
        case 30:
            mines_remaining = 99;
            break;
        default:
            break;
    }
    clear();
    // while loop until game over
    while(playing)
    {
        display_board(minefield, gb, db, height, width);    // print game state
        wmove(minefield, yCurrent, xCurrent*2); // move cursor to current position
        // print mines remaining
        move(0,0);
        clrtoeol();
        mvprintw(0,0, "Mines remaining: %d", mines_remaining);
        refresh();
        wrefresh(minefield);
        int input = getch();    // user input
        switch(input)
        {
            case KEY_UP:    // move cursor up
                if(yCurrent == 0)
                    yNext = height-1;
                else
                    yNext = yCurrent-1;
                move_highlighted_tile(minefield, db, yCurrent, xCurrent, yNext, xCurrent);
                yCurrent--;
                if(yCurrent < 0)    // check for out of bounds, wrap cursor to bottom
                    yCurrent = height-1;
                break;
            case KEY_DOWN:  // move cursor down
                if(yCurrent == height-1)
                    yNext = 0;
                else
                    yNext = yCurrent+1;
                move_highlighted_tile(minefield, db, yCurrent, xCurrent, yNext, xCurrent);
                yCurrent++;
                if(yCurrent >= height)  // check for out of bounds, move cursor to top
                    yCurrent = 0;
                break;
            case KEY_LEFT:  // move cursor left
                if(xCurrent == 0)
                    xNext = width-1;
                else
                    xNext = xCurrent-1;
                move_highlighted_tile(minefield, db, yCurrent, xCurrent, yCurrent, xNext);
                xCurrent--;
                if(xCurrent < 0)    // check for out of bounds, move cursor to rightmost column
                    xCurrent = width-1;
                break;
            case KEY_RIGHT: // move cursor right
                if(xCurrent == (width-1))
                    xNext = 0;
                else
                    xNext = xCurrent+1;
                move_highlighted_tile(minefield, db, yCurrent, xCurrent, yCurrent, xNext);
                xCurrent++;
                if(xCurrent >= width)   // check for out of bounds, move cursor to leftmost column
                    xCurrent = 0;
                break;
            case 'f':   // place/remove flag
                if(db[yCurrent][xCurrent] == 'F')
                {
                    db[yCurrent][xCurrent] = '*';
                    mines_remaining++;
                }
                else if(db[yCurrent][xCurrent] == '*')
                {
                    if(mines_remaining == 0)    // cannot place additional flag
                    {
                        continue;
                    }
                    db[yCurrent][xCurrent] = 'F';
                    mines_remaining--;
                }
                break;
            case ' ':   // reveal tile
                if(first_input) // populate mines on first input to guarantee first-click safety
                {
                    populate_minefield(gb, height, width, yCurrent, xCurrent);
                    wmove(minefield, yCurrent, xCurrent);
                    first_input = false;
                }
                if(db[yCurrent][xCurrent] == '*')
                {
                    click_tile(minefield, gb, db, yCurrent, xCurrent, height, width);   // normal reveal cell
                }
                else if(db[yCurrent][xCurrent] >= '1' && db[yCurrent][xCurrent] <= '9')
                {
                    click_number(minefield, gb, db, yCurrent, xCurrent, height, width); // special case: click number tile
                }
                refresh();
                wrefresh(minefield);
                break;
            default:
                break;
        }
        if(mines_remaining == 0)    // all flags placed
        {
            bool winner = check_winner(db, height, width);  // check game state for game win
            if(winner)
            {
                game_over(minefield, gb, db, height, width, winner);    // exit function
            }
        }
    }
}

// refresh display board
void display_board(WINDOW* win, char** gb, char** db, int height, int width)
{
    // nested for loop to print game state
    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            char c = db[y][x];
            mvwprintw(win, y, 2*x, "%c", c);
            if(x != width-1)
            {
                c = ' ';
                wprintw(win, "%c", c);
            }
        }
    }
}

// move cursor when arrow key pressed
void move_highlighted_tile(WINDOW* win, char** db, int start_y, int start_x, int y, int x)
{
    // remove highlight from current cell and highlight new position
    int yMin, yMax, xMin, xMax;
    getbegyx(win, yMin, xMin);
    getmaxyx(win, yMax, xMax);
    attron(A_NORMAL);
    char c = db[start_y][start_x];
    mvwprintw(win, start_y, start_x*2, "%c", c);
    attroff(A_NORMAL);
    attron(CURSOR);
    c = db[y][x];
    mvwprintw(win, y, x*2, "%c", c);
    attroff(CURSOR);
    refresh();
    wrefresh(win);
}

// reveals tile when spacebar pressed
void click_tile(WINDOW* win, char** gb, char** db, int y, int x, int height, int width)
{
    // check if cell has mine
    if(gb[y][x] == 'M' && db[y][x] != 'F')
    {
        game_over(win, gb, db, height, width, false);
    }
    // if empty, call recursive reveal
    else if(gb[y][x] == ' ')
    {
        recursive_reveal(win, gb, db, y, x, height, width);
    }
    // reveal number tile
    else
    {
        db[y][x] = gb[y][x];
        mvwprintw(win, y, x*2, "%c", gb[y][x]);
    }
}

void recursive_reveal(WINDOW* win, char** gb, char** db, int y, int x, int height, int width)
{
    // if current cell is unknown, reveal it
    if(db[y][x] == '*' && gb[y][x] != 'M')
    {
        db[y][x] = gb[y][x];
        mvwprintw(win, y, x*2, "%c", gb[y][x]);
    }
    // if empty cell, recursively reveal 8 adjacent cells
    if(gb[y][x] == ' ')
    {
        for(int r=y-1; r<=y+1; r++)
        {
            for(int c=x-1; c<=x+1; c++)
            {
                // check for valid bounds
                if(r>=0 && r<height && c>=0 && c<width && db[r][c] == '*')
                {
                    recursive_reveal(win, gb, db, r, c, height, width); // recursive call
                }
            }
        }
    }
}

// reveals adjacent non-flagged tiles
void click_number(WINDOW* win, char** gb, char** db, int y, int x, int height, int width)
{
    // count adjacent flags
    int adjacent_flags = 0;
    for(int r=y-1; r<=y+1; r++)
    {
        for(int c=x-1; c<=x+1; c++)
        {
            if(r>=0 && r<height && c>=0 && c<width)
            {
                if(db[r][c] == 'F')
                {
                    adjacent_flags++;
                }
            }
        }
    }
    // if adjacent flags not equal to number, exit
    if(adjacent_flags != (db[y][x] - '0'))
    {
        return;
    }
    // iterate over 8 adjacent tiles to reveal non-flagged tiles
    for(int r=y-1; r<=y+1; r++)
    {
        for(int c=x-1; c<=x+1; c++)
        {
            if(r>=0 && r<height && c>=0 && c<width)
            {
                if(db[r][c] != 'F')
                {
                    click_tile(win, gb, db, r, c, height, width);
                }
            }
        }
    }
}

// returns true if all non-mine cells are revealed and all mines are flagged
bool check_winner(char** db, int height, int width)
{
    // if board has all cells revealed, user wins
    bool result = true;
    for(int r=0; r<height; r++)
    {
        for(int c=0; c<width; c++)
        {
            if(db[r][c] == '*')
            {
                result = false;
            }
        }
    }
    return result;
}

// called when game ends by loss or win
void game_over(WINDOW* win, char** gb, char** db, int height, int width, bool winner)
{
    if(winner == true)
    {
        // if winner, print message
        int xMax = getmaxx(stdscr);
        int start_x = xMax/2 - 12;
        mvprintw(0, start_x, "CONGRATULATIONS! YOU WIN");
    }
    else
    {
        // if loss, reveal all mines and correct flags
        for(int y=0; y<height; y++)
        {
            for(int x=0; x<width; x++)
            {
                if(gb[y][x] == 'M' && db[y][x] != 'F')
                {
                    attron(MINE);
                    mvwprintw(win, y, 2*x, "M");
                    attroff(MINE);
                }
                else if(gb[y][x] == 'F' && db[y][x] != 'M')
                {
                    attron(MINE);
                    mvwprintw(win, y, 2*x, "F");
                    attroff(MINE);
                }
            }
        }
    }
    refresh();
    wrefresh(win);
    getch();
    // free memory of gameboard and display board
    if(height != 0)
    {
        for(int y=0; y<height; y++)
        {
            free(gb[y]);
            free(db[y]);
        }
        free(gb);
        free(db);
    }
    main_menu(gb, db);
}

// count number of mines adjacent to tile
int num_adjacent_mines(char** gb, int height, int width, int y, int x)
{
    int result = 0;
    // iterate over 8 adjacent tiles
    for(int a=y-1; a<=y+1; a++)
    {
        for(int b=x-1; b<=x+1; b++)
        {
            // check for valid coords
            if(a<0 || a>=height || b<0 || b>=width || (a==y && b==x))
            {
                continue;
            }
            // if mine, update counter
            if(gb[a][b] == 'M')
            {
                result++;
            }
        }
    }
    return result;
}

// close program
void exit_program(char** gb, char** db, int height)
{
    // free memory of gameboard and display board
    if(height != 0)
    {
        for(int y=0; y<height; y++)
        {
            free(gb[y]);
            free(db[y]);
        }
        free(gb);
        free(db);
    }
    endwin();
    exit(EXIT_SUCCESS);
}