#include "graphics.h"

/**
 * @brief Initializes the graphical context for the XWindow.
 *
 * This function sets up the graphics context (GC) with predefined line attributes,
 * fill style, and foreground/background colors. The colors are determined by the
 * REVERSED_STREAM macro.
 *
 * @param xw A pointer to the XWindow structure containing display and window info.
 */
void init_graphics(XWindow *xw) {
    unsigned long valuemask = 0; // GC creation mask
    XGCValues values;            // Initial values for GC
    unsigned int line_width = 2; // Line width
    int line_style = LineSolid;  // Line style
    int cap_style = CapButt;     // Cap style
    int join_style = JoinBevel;  // Join style

    xw->gc = XCreateGC(xw->display, xw->window, valuemask, &values);
    if (xw->gc == NULL) {
        fprintf(stderr, "XCreateGC: \n");
    }

    // Set colors based on stream direction
#if REVERSED_STREAM
    XSetForeground(xw->display, xw->gc, WhitePixel(xw->display, xw->screenNumber));
    XSetBackground(xw->display, xw->gc, BlackPixel(xw->display, xw->screenNumber));
#else
    XSetForeground(xw->display, xw->gc, BlackPixel(xw->display, xw->screenNumber));
    XSetBackground(xw->display, xw->gc, WhitePixel(xw->display, xw->screenNumber));
#endif

    XSetLineAttributes(xw->display, xw->gc, line_width, line_style, cap_style, join_style);
    XSetFillStyle(xw->display, xw->gc, FillSolid); // Solid fill
}

/**
 * @brief Initializes the font for rendering text in the XWindow.
 *
 * Load a font by name using Xft (X FreeType Interface) and returns a pointer
 * to the XftFont structure.
 *
 * @param xw A pointer to the XWindow structure containing display and window info.
 * @param fontname The name of the font to be loaded.
 * @return XftFont* A pointer to the loaded font, or NULL if loading fails.
 */
XftFont* init_font(XWindow *xw, const char* fontname) {
    XftFont *font = XftFontOpenName(xw->display, xw->screenNumber, fontname);
    if (!font) {
        fprintf(stderr, "Unable to load font %s\n", fontname);
        return NULL;
    }
    return font;
}

/**
 * @brief Draws a string of text at specified coordinates with an optional effect.
 *
 * This function draws UTF-8 encoded text using the specified font at the given
 * coordinates. Optionally, it can apply a glow effect around the text.
 *
 * @param display The X display.
 * @param window The target window for drawing.
 * @param font The font to use for drawing the text.
 * @param x The x-coordinate for the text.
 * @param y The y-coordinate for the text.
 * @param text The text string to draw.
 * @param effect If true, applies a glow effect around the text.
 */
void draw_characters(Display *display, Window window, XftFont *font, U16 x, U16 y, const char *text, bool effect) {
    XftDraw *xftDraw = XftDrawCreate(display, window, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)));

    if (effect) {
        // Glow effect
#if REVERSED_STREAM
        XRenderColor glowColor = {0xffff, 0xffff, 0xffff, 0x6000}; 
#else
        XRenderColor glowColor = {0x0000, 0x0000, 0x0000, 0x6000}; 
#endif
        XftColor glowXftColor;
        XftColorAllocValue(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &glowColor, &glowXftColor);

        I8 offsets[] = {-2, -1, 1, 2}; 
        U8 numOffsets = sizeof(offsets) / sizeof(offsets[0]);

        for (I8 ox = 0; ox < numOffsets; ++ox) {
            for (I8 oy = 0; oy < numOffsets; ++oy) {
                XftDrawString8(xftDraw, &glowXftColor, font, x + offsets[ox], y + font->ascent + offsets[oy], (FcChar8 *)text, strlen(text));
            }
        }

        XftColorFree(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &glowXftColor);
    }

    // Render text
#if REVERSED_STREAM
    XRenderColor renderColor = {0xffff, 0xffff, 0xffff, 0xf000};
#else
    XRenderColor renderColor = {0x0000, 0x0000, 0x0000, 0xf000};
#endif
    XftColor color;
    XftColorAllocValue(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &renderColor, &color);

    XftDrawStringUtf8(xftDraw, &color, font, x, y + font->ascent, (FcChar8 *)text, strlen(text));

    XftDrawDestroy(xftDraw);
    XftColorFree(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &color);
}

/**
 * @brief Draws a T-shaped cube at the specified y-coordinate.
 *
 * This function draws a T-shaped cube in the center of the window, with optional shadow effects.
 * (used for start screen could be also used for future settings menu)
 *
 * @param display The X display.
 * @param window The target window for drawing.
 * @param gc The graphical context used for drawing.
 * @param win_attr Pointer to the window attributes.
 * @param size The size of the T-cube.
 * @param y The y-coordinate for the top of the T-cube.
 */
void draw_T_cube(Display *display, Window window, GC gc, XWindowAttributes *win_attr, int size, U16 y) {
    U16 x;

    // Centered T-cube
    U16 blockSize = size; 
    x = (win_attr->width - blockSize * 3) / 2;

    XPoint points[] = {
        {x + blockSize, y},
        {x + 3 * blockSize, y},
        {x + 3 * blockSize, y + blockSize},
        {x + 2 * blockSize, y + blockSize},
        {x + 2 * blockSize, y + 2 * blockSize},
        {x + blockSize, y + 2 * blockSize},
        {x + blockSize, y + blockSize},
        {x, y + blockSize},
        {x, y},
        {x + blockSize, y}
    };

    // Shadow
#if REVERSED_STREAM
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
#else
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
#endif
    for (int i = 0; i < 10; ++i) {
        points[i].x += 4;
        points[i].y += 4;
    }
    XDrawLines(display, window, gc, points, 10, CoordModeOrigin);

    // T shape
    for (int i = 0; i < 10; ++i) {
        points[i].x -= 4;
        points[i].y -= 4;
    }
    XSetLineAttributes(display, gc, 2, LineSolid, CapButt, JoinMiter);
    XDrawLines(display, window, gc, points, 10, CoordModeOrigin);
}


/**
 * @brief Draws text centered on the screen with optional vertical padding.
 *
 * This function calculates the center of the window and draws text centered
 * horizontally. The vertical position can be adjusted with padding.
 *
 * @param display The X display.
 * @param window The target window for drawing.
 * @param font The font to use for drawing the text.
 * @param text The text string to draw.
 * @param yPadding Vertical padding as a percentage of the window height.
 * @param effect If true, applies a glow effect around the text.
 * @return U16 The y-coordinate where the text was drawn.
 */
U16 draw_text_center(Display *display, Window window, XftFont *font, const char *text, I16 yPadding, bool effect) {
    U16 x, y;
    XGlyphInfo extents;

    XftTextExtentsUtf8(display, font, (FcChar8 *)text, strlen(text), &extents);

    yPadding = yPadding % 100;
    x = (WINDOW_WIDTH - extents.width) / 2;
    y = (WINDOW_HEIGHT / 2 + (extents.height / 2)) + (yPadding * WINDOW_HEIGHT / 100);
    
    draw_characters(display, window, font, x, y, text, effect);
    return y;
}

/**
 * @brief Draws the end screen with a "Game Over" message and a prompt for the user.
 * 
 * This function clears the current window content and then displays the start screen.
 * 
 * @param xw Pointer to the XWindow structure containing display and window info.
 * @param fontText A pointer to the XftFont structure used for rendering the user message text.
 * @param fontHeadlines A pointer to the XftFont structure used for rendering the "Game Over" headline.
 */
void draw_start_screen(XWindow *xw, XftFont *fontText, XftFont *fontHeadlines) {
	char *startMessage = "Press any key to start";
	char *title = "Cubes";
    U16 y;

	XWindowAttributes win_attr;
	XGetWindowAttributes(xw->display, xw->window, &win_attr);

	XClearWindow(xw->display, xw->window); // clear old state
	(void)draw_text_center(xw->display, xw->window, fontText, startMessage, 20, false);

	// align the T behind the title
	y = draw_text_center(xw->display, xw->window, fontHeadlines, title, -29, true);
	draw_T_cube(xw->display, xw->window, xw->gc, &win_attr, 100, y);
}

/**
 * @brief Draws the end screen with a "Game Over" message and a prompt for the user.
 * 
 * This function clears the current window content and then displays the "Game over" screen.
 * 
 * @param xw Pointer to the XWindow structure containing display and window info.
 * @param fontText A pointer to the XftFont structure used for rendering the user message text.
 * @param fontHeadlines A pointer to the XftFont structure used for rendering the "Game Over" headline.
 */
void draw_end_screen(XWindow *xw, XftFont *fontText, XftFont *fontHeadlines) {
	char *endMessage = "Game Over";
	char *userMessage = "Press any key to play again";

	XClearWindow(xw->display, xw->window); // clear old state
	(void)draw_text_center(xw->display, xw->window, fontText, userMessage, 20, false);
	(void)draw_text_center(xw->display, xw->window, fontHeadlines, endMessage, -20, true);
}

/**
 * @brief Draws the game board with blocks and score information.
 *
 * This function draws the game board based on the given board.
 * It renders the blocks, score, high score, and level text on the screen.
 *
 * @param board Pointer to the Board structure containing game state information.
 * @param xw Pointer to the XWindow structure containing display and window info.
 * @param scoreFont Pointer to the font used for rendering the score.
 * @param scoreText The text displaying the current score.
 * @param highscoreText The text displaying the high score.
 * @param levelText The text displaying the current level.
 */
void draw_board(XWindow *xw, GameBoard *board, XftFont *scoreFont) {
	char scoreText[28];
	char highscoreText[32];
	char levelText[24];

	// Format the scores
    snprintf(scoreText, sizeof(scoreText), "score: %lu", board->score);
	snprintf(highscoreText, sizeof(highscoreText), "highscore: %lu", board->highscore); 
	snprintf(levelText, sizeof(levelText), "level: %u", board->level);

	// Render all cubes placed on the board
	XSetForeground(xw->display, xw->gc, 0xc0c0c0);
	for(U8 i=0;i<BOARD_HEIGHT;i++) {
		for(U8 j=0;j<BOARD_WIDTH;j++) {
			if(board->state[j][i] == 1) {
				XFillRectangle(xw->display, xw->window, xw->gc, j*BLOCKSIZE + BOARD_OFFSET_LEFT, i*BLOCKSIZE + BOARD_OFFSET_TOP, BLOCKSIZE-1, BLOCKSIZE-1);
			}
		}
	}

	// Render text
#if REVERSED_STREAM
	XSetForeground(xw->display, xw->gc, WhitePixel(xw->display, xw->screenNumber));
#else
	XSetForeground(xw->display, xw->gc, BlackPixel(xw->display, xw->screenNumber));
#endif

	XDrawRectangle(xw->display, xw->window, xw->gc, BOARD_OFFSET_LEFT, BOARD_OFFSET_TOP, BOARD_WIDTH_PX, BOARD_HEIGHT_PX);
	draw_characters(xw->display, xw->window, scoreFont, BOARD_OFFSET_RIGHT + BLOCKSIZE, BLOCKSIZE + BOARD_OFFSET_TOP, scoreText, false);
	draw_characters(xw->display, xw->window, scoreFont, BOARD_OFFSET_RIGHT + BLOCKSIZE, BLOCKSIZE*2 + BOARD_OFFSET_TOP, highscoreText, false);
	draw_characters(xw->display, xw->window, scoreFont, BOARD_OFFSET_RIGHT + BLOCKSIZE, BLOCKSIZE*3 + BOARD_OFFSET_TOP, levelText, false);
}

void draw_tetromino(Display *display, Window window, GC gc, Tetromino *tetromino) {
	I16 x, y;
	x = tetromino->X;
	y = tetromino->Y;

	U16 shape = tetromino->rotations[tetromino->rotationState];

	XSetForeground(display, gc, tetromino->color);

	for(U8 i=0;i<4;i++) {
		for(U8 j=0;j<4;j++) {
			if((shape & (1 << (i * 4 + j))) != 0) {
				XFillRectangle(display, window, gc, tetromino->X, tetromino->Y, BLOCKSIZE-1, BLOCKSIZE-1);
			}
			
			tetromino->X = (tetromino->X + BLOCKSIZE);	
		}

		tetromino->X = x;
		tetromino->Y = (tetromino->Y + BLOCKSIZE);	
	}

	tetromino->Y = y;
}

