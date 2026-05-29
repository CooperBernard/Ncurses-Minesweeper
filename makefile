all: minesweeper

minesweeper: main.c
	gcc main.c -o minesweeper -lncurses
