#include "window.h"

/**
 * @brief Initializes the main application window that interacts with the window manager.
 * 
 * @param display Pointer to the Display structure representing the connection to the X server.
 * @param window The main application window to be initialized.
 * @param xim Pointer to an XIM input method structure.
 * @param xic Pointer to an XIC input context structure.
 * @return I8 Returns 0 on success, or -1 on failure.
 */
I8 init_main_window(Display *display, Window window, XIM *xim, XIC *xic) {
    XSizeHints *hints;

    // Disallow resizing of the window
    if ((hints = XAllocSizeHints()) == NULL) {
        fprintf(stderr, "Error: could not allocate memory for size hints\n");
        return -1;
    }

    hints->flags = PMinSize | PMaxSize;
    hints->min_width = hints->max_width = WINDOW_WIDTH;
    hints->min_height = hints->max_height = WINDOW_HEIGHT;

    XSetWMNormalHints(display, window, hints);
    XFree(hints);   // we don't need the hints anymore as the WM knows what we want to do

    // Set up window manager protocols, e.g., close window handling
    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    // Map the window (make it visible)
    XMapWindow(display, window);

    // Select input events (keyboard, mouse, etc.)
    XSelectInput(display, window, KeyPressMask | ButtonPressMask | StructureNotifyMask | ExposureMask);

    // Initialize input method and input context
    if ((*xim = XOpenIM(display, NULL, NULL, NULL)) == NULL) {
        fprintf(stderr, "Error: could not open input method\n");
        return -1;
    }

    if ((*xic = XCreateIC(*xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, window, NULL)) == NULL) {
        fprintf(stderr, "Error: could not create input context\n");
        XCloseIM(*xim);
        return -1;
    }

    XStoreName(display, window, "Cubes"); // tell the WM our game name for the window

    return 0;
}