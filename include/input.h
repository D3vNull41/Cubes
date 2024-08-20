#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "typedef.h"
#include "cubes.h"

bool recv_events(Display *display, XIC xic, char *keyBuf, U32 mousePos[2]);

#endif // INPUT_H

