#define _XOPEN_SOURCE   600
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>
#include "cubes.h"

Atom wm_delete_window;
bool needsRedraw;


// Function to calculate the time difference in nanoseconds
long time_diff_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
}

void handle_pause_key(char *keyBuffer, GameState *currentState) {
    if (keyBuffer[0] == 'p' || keyBuffer[0] == 'P') {
        *currentState = (*currentState == STATE_GAME) ? STATE_PAUSE : STATE_GAME;
    }
}

int main(void) {
	GameState currentState;
	Window parentWindow; // The root window of the screen
	XWindow mainWindow;
	XIM xim; // Input method
	XIC xic; // Input context
	XftFont *fontText; // Font for normal display text
	XftFont *fontHeadlines; // Font for the headlines in the game
	char keyBuffer[32]; // This Buffer will store all important events (UTF-8 keys, arrow keys or mousclick)
	U32 mousePos[2];

	// Initial window position and size
	U32 posX = 1;
	U32 posY = 1;
	
	U64 bgColor; // background color
	U64 bdColor; // border color

	// delta time related
	long deltaTimeNs;
	struct timespec previousTime;
	struct timespec currentTime;
	struct timespec sleepTime;

	bool gameInit = 0;
	U64 highscore = 0;
	GameBoard board;
	Tetromino *currentTetromino = NULL;

	if ((mainWindow.display = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "Error: could not open connection to X Server (i.e. default display)\n");
		return -1;
	}

	// Get the default root window (typically the entire screen)
	parentWindow = XDefaultRootWindow(mainWindow.display);
	mainWindow.screenNumber = XDefaultScreen(mainWindow.display);

// TODO: Integate reversed stream in Draw text
#if REVERSED_STREAM
	bgColor = XBlackPixel(mainWindow.display, mainWindow.screenNumber);
	bdColor = XWhitePixel(mainWindow.display, mainWindow.screenNumber);
#else
	bgColor = XWhitePixel(mainWindow.display, mainWindow.screenNumber);
	bdColor = XBlackPixel(mainWindow.display, mainWindow.screenNumber);
#endif

	// Create a simple window as a child of the root window
	mainWindow.window = XCreateSimpleWindow(
		mainWindow.display, parentWindow, posX, posY, 
		WINDOW_WIDTH, WINDOW_HEIGHT, 0, bdColor, bgColor
	);
	
	// Initialize the window with required properties and mappings
	if (init_main_window(mainWindow.display, mainWindow.window, &xim, &xic) != 0) {
		return -1;
	}

	init_graphics(&mainWindow); 
	
	// Load font
	fontText = init_font(&mainWindow, "Nimbus Sans L-12");
	fontHeadlines = init_font(&mainWindow, "Nimbus Sans L-62");
	if(fontText == NULL || fontHeadlines == NULL) {
		fprintf(stderr, "Ensure your Fonts are installed correctly\n");
		return -1;
	}

	// load_score(); // TODO

	currentState = STATE_START;
	clock_gettime(CLOCK_MONOTONIC, &previousTime);
	while(true) {
		// Main event loop (not game loop)
		memset(keyBuffer, 0, 32); // Reset the buffer after handling the key press
		
		// Process events and check if the user wants to exit
		if (recv_events(mainWindow.display, xic, keyBuffer, mousePos)) {
			break;
		}
		
		// Calculate the delta time for this frame
		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		deltaTimeNs = time_diff_ns(previousTime, currentTime);
		previousTime = currentTime;

		switch(currentState) {
			
			case STATE_START:
				if(needsRedraw) {
					draw_start_screen(&mainWindow, fontText, fontHeadlines);
					needsRedraw = 0;
				}

				if (keyBuffer[0] != '\0') {
					// the user pressed any key to start
					XClearWindow(mainWindow.display, mainWindow.window);
					currentState = STATE_GAME;
					needsRedraw = 1;
				}
				break;

			case STATE_GAME:
				if(!gameInit) {
					init_game(&board);
					board.score = 0;
					board.level = 1;
					board.highscore = highscore;
					gameInit = 1;
				}

				handle_pause_key(keyBuffer, &currentState);

				if(currentTetromino == NULL) {
					currentTetromino = get_tetromino();
					// only redraw the board after the gamboard changed (i.e. a block was placed)
					XClearWindow(mainWindow.display, mainWindow.window);
					draw_board(&mainWindow, &board, fontText);
				
				} else {
					if(move_tetromino(&mainWindow, &board, currentTetromino, keyBuffer)) {
						free_tetromino(&currentTetromino);
						currentState = remove_full_row(&board); // this function checks if the user is gameover
					}

					update_game(&mainWindow, &currentTetromino); // draw the elements on the screen
				}
				break;

			case STATE_PAUSE:
				handle_pause_key(keyBuffer, &currentState);
				break;
			
			case STATE_GAME_OVER:
				if(needsRedraw) {
					draw_end_screen(&mainWindow, fontText, fontHeadlines);
					needsRedraw = 0;
				}

				if(keyBuffer[0] != '\0') {
					// the user pressed any key to start again
					XClearWindow(mainWindow.display, mainWindow.window);
					currentState = STATE_START;
					needsRedraw = 1;
					highscore = board.highscore;
					free_game(&board);
					gameInit = 0;
					break;
				}

			}


		// Sleep to maintain approximately 60 FPS
		const long frameTimeNs = 1000000000L / 60; // 60 FPS
		if(deltaTimeNs < frameTimeNs) {
			
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = frameTimeNs - deltaTimeNs;
			nanosleep(&sleepTime, NULL);
		}
		
	}
	
	// Cleanup
	if(gameInit) {
		free_game(&board);
	}

	XftFontClose(mainWindow.display, fontText);
	XftFontClose(mainWindow.display, fontHeadlines);
	XFreeGC(mainWindow.display, mainWindow.gc);
	XDestroyIC(xic);
	XCloseIM(xim);
	XCloseDisplay(mainWindow.display);

	// save_score(); // TODO
   
	return 0;
}
