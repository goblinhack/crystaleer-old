/*
 * Copyright (C) 2011-2017 goblinhack@gmail.com
 *
 * See the LICENSE file for license.
 */

#include "python.h"
#include "main.h"
#include "py_sdl.h"
#include <SDL.h>

/*
 * Borrowed from https://github.com/holtrop/pysdl2
 */
PYFUNC(SDLGetKeyState, "get a snapshot of the current keyboard state")
{
    int numkeys;
#if SDL_MAJOR_VERSION == 1 && SDL_MINOR_VERSION == 2 /* { */
    const uint8_t *keys = SDL_GetKeyState(&numkeys);
#else
    const uint8_t *keys = SDL_GetKeyboardState(&numkeys);
#endif

    PyObject *lst = PyList_New(numkeys);

    for (int i = 0; i < numkeys; i++) {
        PyList_SetItem(lst, i, Py_BuildValue("i", keys[i]));
    }

    return (lst);
}
