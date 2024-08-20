#include "game.h"
#include <X11/Xlib.h>
#include <stdio.h>

U32 seed = 0;

/**
 * @brief Initializes the game board by allocating memory for its state.
 * 
 * Allocates a 2D array for the game board state. If memory allocation fails, 
 * it cleans up any allocated memory and returns an error code.
 * 
 * @param board Pointer to the GameBoard structure to be initialized.
 * @return `0` on success, `-1` on memory allocation failure.
 */
I8 init_game(GameBoard *board) {
	if((board->state = (U8**)calloc(BOARD_HEIGHT, sizeof(U8*))) == NULL) {
		fprintf(stderr, "Error: faild to allocate mem for game board\n");
		return -1;
	}
	
	for(I8 i=0;i<BOARD_HEIGHT;i++) {
		if((board->state[i] = (U8*)calloc(BOARD_WIDTH, sizeof(U8))) == NULL) {
			fprintf(stderr, "Error: faild to allocate mem for game board\n");
			// clean up what we just allocated
			for(I8 j=0;j<i;j++) {
				free(board->state[j]);
			}
			free(board->state);
			return -1;
		}
	}

	return 0;
}


/**
 * @brief Frees the allocated memory for the game board.
 * 
 * Releases all memory allocated for the game board state to prevent memory leaks.
 * 
 * @param board Pointer to the GameBoard structure to be freed.
 */
void free_game(GameBoard *board) {
	for(I8 i=0;i<BOARD_HEIGHT;i++) {
		free(board->state[i]);
	}

	free(board->state);
	return;
}

/**
 * @brief Removes full rows from the game board and updates score and level.
 * 
 * Checks each row of the game board for fullness, removes full rows, shifts 
 * rows down, and updates the score and level accordingly. Also checks for game over condition.
 * 
 * @param board Pointer to the GameBoard structure being modified.
 * @return `STATE_GAME_OVER` if the game is over, otherwise `STATE_GAME`.
 */
GameState remove_full_row(GameBoard *board) {
    bool full;
	U8 rowsCleared = 0;

    // Check the top row for any blocks
    for (U8 j = 0; j < BOARD_WIDTH; j++) {
        if (board->state[j][2] != 0) {
            return STATE_GAME_OVER;
        }
    }

    for (U8 i = 0; i < BOARD_HEIGHT; i++) {
        full = true;
        for (U8 j = 0; j < BOARD_WIDTH; j++) {
            if (board->state[j][i] == 0) {
                full = false;
                break;
            }
        }

        if (full) {
            // Move all rows above this one down
            for (U8 k = i; k > 0; k--) {
                for (U8 j = 0; j < BOARD_WIDTH; j++) {
                    board->state[j][k] = board->state[j][k-1];
                }
            }

            // Clear the top row
            for (U8 j = 0; j < BOARD_WIDTH; j++) {
                board->state[j][0] = 0;
            }

			rowsCleared++;

			// the user gets 100 points
			board->score += 100 * rowsCleared;
            // After removing a row, check the same row index again
            i--;
        }
    }

    // Check if the score has crossed a 1000-point boundary
    if (board->score >= board->level * 1000) {
        board->level++;
    }


	if(board->score > board->highscore) {
		board->highscore = board->score;
	}

	return STATE_GAME;
}

/**
 * @brief Allocates and returns a new Tetromino with random type and initial state.
 * 
 * This function selects a random Tetromino from a predefined set, allocs memory for it,
 * and initializes its position and rotation state. If memory allocation fails, it returns NULL.
 * Selection based on BBS (Blum Blum Shub) PRNG
 * 
 * @return Pointer to the newly allocated Tetromino structure, or `NULL` on memory allocation failure.
 */
Tetromino *get_tetromino() {
	// precomputed tetrominos with there spective rotation values (positions are set to 0 as default)
	U32 randVal;
	U16 randomIndex;
	Tetromino tetrominos[] = {
        	{0, {0x0F00, 0x2222, 0x00F0, 0x4444}, 0, 0, 0x00ffff},  // "I"
        	{0, {0x6600, 0x6600, 0x6600, 0x6600}, 0, 0, 0xffff00},  // "O"
        	{0, {0x4e00, 0x2320, 0x7200, 0x04c4}, 0, 0, 0x800080},  // "T"
        	{0, {0x3600, 0x0231, 0x006c, 0x8c40}, 0, 0, 0x00ff00},  // "S"
        	{0, {0xc600, 0x1320, 0x0063, 0x04c8}, 0, 0, 0xff0000},  // "Z"
        	{0, {0x8e00, 0x3220, 0x0071, 0x044c}, 0, 0, 0x0000ff},  // "J"
        	{0, {0x2e00, 0x2230, 0x0074, 0x0c44}, 0, 0, 0xff7f00}   // "L"
	};

	Tetromino *newTetromino;

	if((newTetromino = calloc(1, sizeof(Tetromino))) == NULL) {
		fprintf(stderr, "Error: failed to allocate space for new tetromino\n");
		return NULL;
	}

	if(seed == 0) {
		seed = get_seed();
	}

	randVal = random_U32(&seed);
	randomIndex = randVal % (sizeof(tetrominos) / sizeof(Tetromino));
	*newTetromino = tetrominos[randomIndex];

	// Set the initial position and rotation state
	newTetromino->X = (BOARD_OFFSET_LEFT) + BLOCKSIZE * 4;
	newTetromino->Y = BOARD_OFFSET_TOP;
	newTetromino->rotationState = 0;	
	return newTetromino;
}


/**
 * @brief Frees the memory allocated for a Tetromino and sets its pointer to NULL.
 * 
 * This function releases the memory associated with a Tetromino structure and
 * ensures the pointer is set to NULL to avoid dangling references.
 * 
 * @param tetromino Double pointer to the Tetromino structure to be freed.
 */
void free_tetromino(Tetromino **tetromino) {
	free(*tetromino);
	*tetromino = NULL;
}

/**
 * @brief Places a Tetromino on the game board by updating the board state.
 * 
 * Converts the Tetromino's pixel coordinates to board coordinates and updates
 * the game board state to reflect the Tetromino's current shape and position.
 * 
 * @param board Pointer to the GameBoard structure where the Tetromino will be placed.
 * @param tetromino Pointer to the Tetromino structure to be placed on the board.
 */
void place_tetromino(GameBoard *board, Tetromino *tetromino) {
    I16 x, y;
    U16 shape = tetromino->rotations[tetromino->rotationState];

    // Convert pixel coordinates to board coordinates
    x = (tetromino->X / BLOCKSIZE) - 10;
    y = (tetromino->Y-BOARD_OFFSET_TOP) / BLOCKSIZE;
	
    for (I8 i = 0; i < 4; i++) {
        for (I8 j = 0; j < 4; j++) {
            if ((shape & (1 << (i * 4 + j))) != 0) {
                board->state[x + j][y + i] = 1;
            }
        }
    }
}

/**
 * @brief Checks if the Tetromino's new position and rotation would cause a collision.
 *
 * This function verifies whether a Tetromino's new position or rotation would cause a collision
 * with the boundaries of the game board or with existing blocks. It returns a status code indicating
 * the type of collision, if any.
 *
 * @param board Pointer to the GameBoard structure.
 * @param tetromino Pointer to the Tetromino structure to be checked.
 * @param newX The new X coordinate in pixels.
 * @param newY The new Y coordinate in pixels.
 * @param newRotationState The new rotation state index.
 * @return `0` if no collision, `1` if colliding with the bottom or another block, `2` if colliding with the sides.
 */
U8 check_bounds(GameBoard *board, Tetromino *tetromino, U16 newX, U16 newY, U8 newRotationState) {
    // The shape of the Tetromino in its new rotation state
	U16 shapeCurrent = tetromino->rotations[tetromino->rotationState];
    U16 shapeNew = tetromino->rotations[newRotationState];
    I16 newXB, newYB;
	bool collisionWithBlock = false;

    // Calculate board position in terms of blocks (not pixels)
    newXB = (newX / BLOCKSIZE) - 10;
    newYB = (newY - BOARD_OFFSET_TOP) / BLOCKSIZE;

    // Ensure alignment to block grid if not perfectly aligned
    if (newY % BLOCKSIZE != 0) {
        newYB += 1;
    }
	if (newX % BLOCKSIZE != 0) {
		newXB += 1;
	}

    // Iterate over the Tetromino's 4x4 grid representation
    for (I8 i = 0; i < 4; i++) {
        for (I8 j = 0; j < 4; j++) {
            // Check if the current block of the Tetromino is filled in the new rotation
            if ((shapeNew & (1 << (i * 4 + j))) != 0) {
                // Check boundary collisions
                if (newX + ((j + 1) * BLOCKSIZE) > BOARD_OFFSET_RIGHT || newX + (j * BLOCKSIZE) < BOARD_OFFSET_LEFT) {
                    return 2; // Collision with side, move block
                }

                // Check if the Tetromino is colliding with the bottom or another block
                if (newY + ((i + 1) * BLOCKSIZE) > (BOARD_HEIGHT_PX + BOARD_OFFSET_TOP) || board->state[newXB + j][newYB + i] == 1) {

                    // If the rotation causes a collision with another block
                    if (board->state[newXB + j][newYB + i] == 1) {
                        collisionWithBlock = true;
                    }

                    // If colliding with the bottom of the board
                    if (newY + ((i + 1) * BLOCKSIZE) >= (BOARD_HEIGHT_PX + BOARD_OFFSET_TOP)) {
                        return 1; // Collision at the bottom, block should be placed
                    }
                }
            }
        }
    }

	if(collisionWithBlock) {
		for (I8 i = 0; i < 4; i++) {
			for (I8 j = 0; j < 4; j++) {
				// Check if the current block of the Tetromino is filled in the new rotation
				if ((shapeCurrent & (1 << (i * 4 + j))) != 0) {
						// If the rotation causes a collision with another block
						if (board->state[newXB + j][newYB + i] == 1) {
							return 1;
						}
				}
			}
		}
	}

    return 0; // No collision, allow rotation and move freely
}

/**
 * @brief Maps a string buffer containing key input to a corresponding KeyAction enum value.
 *
 * Converts the string representation of a key input to the corresponding KeyAction enum.
 *
 * @param keyBuf Pointer to the string containing the key input.
 * @return The corresponding `KeyAction` enum value, or `KEY_NOMOVE` if no match is found.
 */
KeyAction getKeyAction(const char *keyBuf) {
	if (strcmp(keyBuf, "Up") == 0) return KEY_UP;
	if (strcmp(keyBuf, "Down") == 0) return KEY_DOWN;
	if (strcmp(keyBuf, "Left") == 0) return KEY_LEFT;
	if (strcmp(keyBuf, "Right") == 0) return KEY_RIGHT;
	if (strcmp(keyBuf, "Control") == 0) return KEY_CTRL;
	if (strcmp(keyBuf, " ") == 0) return KEY_SPACE;
	return KEY_NOMOVE;
}

/**
 * @brief Moves and rotates the Tetromino based on user input and updates its position on the board.
 *
 * This function handles the movement and rotation of a Tetromino based on user input,
 * checks for collisions using `check_bounds`, and places the Tetromino if it collides
 * with the bottom or another block.
 *
 * @param xw Pointer to the XWindow structure for rendering.
 * @param board Pointer to the GameBoard structure.
 * @param tetromino Pointer to the Tetromino structure to be moved.
 * @param keyBuf Pointer to the string containing the key input.
 * @return `true` if the Tetromino is placed on the board, `false` otherwise.
 */
bool move_tetromino(XWindow *xw, GameBoard *board, Tetromino *tetromino, const char *keyBuf) {
    U8 newRotationState, boundCheck;
    U16 speed;
    U16 shape = tetromino->rotations[tetromino->rotationState];
    U16 newX, newY, stepX, stepY;
    
    if(tetromino == NULL || keyBuf == NULL) {
        fprintf(stderr, "Invalid input to move_tetromino\n");
        return false;
    }

    KeyAction action = getKeyAction(keyBuf);
    newRotationState = tetromino->rotationState;
    newX = tetromino->X;
    newY = tetromino->Y;

    // clear the old position of the tetromino blocks
    for(I8 i=0; i<4; i++) {
        for(I8 j=0; j<4; j++) {
            if((shape & (1 << (i * 4 + j))) != 0) {
                XClearArea(xw->display, xw->window, newX+(j*BLOCKSIZE), newY+(i*BLOCKSIZE), BLOCKSIZE, BLOCKSIZE, false);
            }
        }
    }

    speed = 1;

    switch (action) {
        case KEY_UP:
            newRotationState = (tetromino->rotationState + 1) % 4; // clock wise rotation
            break;
        case KEY_DOWN:
            speed = 0xf;
            break;
		case KEY_SPACE:
			speed = BOARD_HEIGHT_PX;
			break;
        case KEY_CTRL:
            newRotationState = (tetromino->rotationState + 3) % 4; // counter clock rotation
            break;
        case KEY_LEFT:
            newX -= BLOCKSIZE;
            break;
        case KEY_RIGHT:
            newX += BLOCKSIZE;
            break;
        default:
            break;
    }

    // Iterate from the current position to the new position in steps of speed
    for (int step = 1; step <= speed; step++) {
        stepX = newX;
        stepY = newY + step;

        boundCheck = check_bounds(board, tetromino, stepX, stepY, newRotationState);
        if (boundCheck == 0) {
            tetromino->X = stepX;
            tetromino->Y = stepY;
            tetromino->rotationState = newRotationState;

		} else if(boundCheck == 1) {
			if (action == KEY_LEFT) {
				newX += 1;
			} else if (action == KEY_RIGHT) {
				newX -= 1;
			} else {
				place_tetromino(board, tetromino);
				return true; // has to be freeed
			}
		}
    }

    return false; // Tetromino not placed yet
}

/**
 * @brief Updates the game display by drawing the current Tetromino and board boundaries.
 *
 * Renders the current Tetromino on the game board and draws the boundaries of the board.
 *
 * @param xw Pointer to the XWindow structure for rendering.
 * @param currentTetromino Pointer to the pointer of the current Tetromino to be drawn.
 */
void update_game(XWindow *xw, Tetromino **currentTetromino) {
	if(*currentTetromino == NULL) {
		return;
	}

	draw_tetromino(xw->display, xw->window, xw->gc, *currentTetromino);

#if REVERSED_STREAM
	XSetForeground(xw->display, xw->gc, WhitePixel(xw->display, xw->screenNumber));
#else
	XSetForeground(xw->display, xw->gc, BlackPixel(xw->display, xw->screenNumber));
#endif

	XDrawRectangle(xw->display, xw->window, xw->gc, BOARD_OFFSET_LEFT, BOARD_OFFSET_TOP, BOARD_WIDTH_PX, BOARD_HEIGHT_PX); // TODO make more efficent method
	return;	
}
