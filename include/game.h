#ifndef __GAME_H
#define __GAME_H
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include "typedef.h"
#include "window.h" // for window width and height
#include "graphics.h"
#include "cubes.h"
#include "bbs.h"

// precomputed tetrominos with there spective rotation values
extern Tetromino tetrominos[];
extern U32 seed;

I8 init_game(GameBoard *board);
Tetromino *get_tetromino();
bool move_tetromino(XWindow *xw, GameBoard *board, Tetromino *tetromino, const char *keyBuf);
void update_game(XWindow *xw, Tetromino **currentTetromino);
GameState remove_full_row(GameBoard *board);
void free_game(GameBoard *board);
void free_tetromino(Tetromino **tetromino);

#endif // __GAME_H
