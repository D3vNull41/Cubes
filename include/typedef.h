#ifndef __TYPEDEF_H
#define __TYPEDEF_H
#include <stdint.h>
#include <X11/Xlib.h>

#define BLOCKSIZE 25

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef float F16;
typedef double F32;

typedef _Bool bool;
#define false 0
#define true 1

// demensions of everything

#define WINDOW_WIDTH 750
#define WINDOW_HEIGHT 800

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 24
#define BOARD_WIDTH_PX BOARD_WIDTH * BLOCKSIZE // 250px
#define BOARD_HEIGHT_PX BOARD_HEIGHT * BLOCKSIZE // 550px
#define BOARD_OFFSET_LEFT_B ((WINDOW_WIDTH / 2)/BLOCKSIZE) - (BOARD_WIDTH/2)
#define BOARD_OFFSET_LEFT (WINDOW_WIDTH / 2) - (BOARD_WIDTH_PX / 2)
#define BOARD_OFFSET_RIGHT (WINDOW_WIDTH / 2) + (BOARD_WIDTH_PX / 2)
#define BOARD_OFFSET_BOTTOM WINDOW_HEIGHT - ((WINDOW_HEIGHT) - BOARD_HEIGHT_PX)
#define BOARD_OFFSET_TOP 100

// Enum for game state
typedef enum {
	STATE_START = 0,
	STATE_GAME,
	STATE_PAUSE,
	STATE_GAME_OVER
} GameState;

/* Game visuals */
typedef struct {
	U8 **state;
	U32 level;
	U64 score;
	U64 highscore;
} GameBoard;

typedef struct {
	U8 rotationState;
	U16 rotations[4];
	U16 X;
	U16 Y;
	U32 color;
} Tetromino;

/* movement */

typedef enum {
	KEY_NOMOVE = -1,
	KEY_CTRL,
	KEY_SPACE,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT
} KeyAction;

/* XServer related structs */
/**
 * @brief Struct representing an X11 window and its associated graphical context.
 */
typedef struct {
    Display *display;    ///< Pointer to the Display structure, representing the connection to the X server.
    Window window;       ///< The X11 Window ID for this window.
    GC gc;               ///< The graphical context associated with this window.
    int width;           ///< The width of the window.
    int height;          ///< The height of the window.
	U32 screenNumber;
} XWindow;


#endif // __TYPEDEF_H
