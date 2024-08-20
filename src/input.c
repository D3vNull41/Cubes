#include "input.h"

/**
 * @brief Processes all pending X11 events, handling key presses, mouse clicks, window exposure, and client messages.
 *
 * This function retrieves and processes all pending events from the X server, handling different event types such as
 * key presses, mouse clicks, window exposures, and client messages. The function also updates the mouse position, 
 * modifies the key buffer with the appropriate input, and checks for exit conditions (e.g., pressing the Escape key or closing the window).
 *
 * @param display Pointer to the X11 Display structure representing the connection to the X server.
 * @param xic Input context used for handling input methods, such as translating key events into UTF-8 strings.
 * @param keyBuf Character buffer for storing key input, expected to be at least 32 bytes in size.
 * @param mousePos Array to store the x and y coordinates of the mouse when a ButtonPress event is detected.
 *
 * @return `True` if an exit condition is met (e.g., Escape key pressed or window closed), `False` otherwise.
 */
bool recv_events(Display *display, XIC xic, char *keyBuf, U32 mousePos[2]) {
	int length;
	bool exit = False; // Indicate whether to exit the proc

	XEvent event = {0}; // Initialize the event structure
	KeySym keysym;      // Variable to store the KeySym
	Status status;      // Status variable for Xutf8LookupString

	// Process all pending events from the X server
	while (XPending(display) > 0) {
		XNextEvent(display, &event); // Retrieve the next event

		switch (event.type) {
			case KeyPress:
				// Handle key press event

				// Retrieve the key that was pressed and convert it to a UTF-8 string
				length = Xutf8LookupString(xic, &event.xkey, keyBuf, (32 - 1), &keysym, &status);

				if(keysym == XK_Escape) {
					exit = True;
				}

				if (status == XBufferOverflow) {
					// Handle the case where the buffer is too small (should be rare with 32 bytes)
					printf("Buffer overflow detected in Xutf8LookupString\n");
				
				} else if (status == XLookupChars || status == XLookupBoth) {
					// Null-terminate the string if we got a valid UTF-8 string
					keyBuf[length] = '\0';
				
				} else {
					// Handle other statuses like Up Down etc. wich are not valid UTF-8 codes on its own
					if(keysym == XK_Up || keysym == XK_Down || keysym == XK_Left || keysym == XK_Right) {
						strcpy(keyBuf, XKeysymToString(keysym));

					} else if(keysym == XK_Control_L || keysym == XK_Control_R) {
						strcpy(keyBuf, "Control"); // both control keys should do the same thing (i.e. rotate against the clock)
					}
				}

				break;

			case ButtonPress:
				// mabye do something with the mouse in the UI but this are planed for next version of this game
				if(event.xbutton.button == Button1) { // left mouse button
					
					mousePos[0] = (U32)event.xbutton.x;
					mousePos[1] = (U32)event.xbutton.y;
					strcpy(keyBuf, "MouseL");
				}
				break;

			case Expose:
				// Handle window expose (redraw) event
				if (event.xexpose.count == 0) {
					// redraw the screen
					needsRedraw = 1;
				}
				break;
			
			case ClientMessage:
				// Handle client messages from the window manager
				if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
					// If the message is a WM_DELETE_WINDOW, set exit flag to true
					printf("User closed window\n");
					exit = True;
				}
				break;

			default:
				// Handle other events if necessary
				break;
		}
	}
	
	return exit; // Return whether to exit the application
}
