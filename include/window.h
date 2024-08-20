#ifndef __WINDOW_H
#define __WINDOW_H

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/Xcomposite.h>
#include "typedef.h"
#include "game.h"

extern Atom wm_delete_window;

I8 init_main_window(Display *display, Window window, XIM *xim, XIC *xic);

#endif // __WINDOW_H

