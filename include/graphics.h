#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include "typedef.h"
#include "game.h" // for the external score

#define REVERSED_STREAM 1 // reversal of the default color scheme

void init_graphics(XWindow *xw);
XftFont* init_font(XWindow *xw, const char* fontname);
U16 draw_text_center(Display *display, Window window, XftFont *font, const char *text, I16 yPadding, bool effect);
void draw_start_screen(XWindow *xw, XftFont *fontText, XftFont *fontHeadlines);
void draw_end_screen(XWindow *xw, XftFont *fontText, XftFont *fontHeadlines);
void draw_board(XWindow *xw, GameBoard *board, XftFont *scoreFont) ;
void draw_tetromino(Display *display, Window window, GC gc, Tetromino *tetromino);
#endif // __GRAPHICS_H

