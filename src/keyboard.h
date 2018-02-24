/*
    Bloboats - a boat racing game by Blobtrox
    Copyright (C) 2006  Markus "MakeGho" Kettunen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdlib.h>
#include "SDL.h"

#include "window.h"
#include "graphics.h"

static const char* keynames[SDLK_LAST];

extern window Window;
extern graphics Graphics;

#define REPEATDELAY1 55
#define REPEATDELAY2 9

class keyboard
{
	public:
		keyboard();
		~keyboard();

		void GotQuit();

		bool Hold(Uint16 key);

		void HandleInterrupts();
		bool KeyPressed();
		void WaitForKeyPress();

		void ResetReleased();
		void ResetDouble();
		void ResetAll();

		int GetAscii();

		void ResetEvents();

		char Released(Uint16 key);
		int Pressed(Uint16 key);
		bool Double(Uint16 key);

		char *getkeyname(Uint16 key);
		void setkeynames();
 

	private:
		SDL_Event Event;
		Uint8 *keys;

		char keydown[SDLK_LAST];
		char keydouble[SDLK_LAST];

};

#endif
