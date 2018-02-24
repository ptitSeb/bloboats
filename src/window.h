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

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "SDL.h"
#ifdef HAVE_GLES
#include "SDL_opengles.h"
#include "eglport.h"
#else
#include "SDL_opengl.h"
#endif

#include <math.h>
#include "vec.h"

class window
{
	public:
		window();
		~window();

		SDL_Surface * OpenWindow(int width, int height, int bpp, int flags);
		void SetTitle(char *text, char *icon);
		bool Iconify();

		void ToggleFullscreen();

		void Center(Dbl cx, Dbl cy);
		void Viewarea(Dbl x, Dbl y);

		int width;
		int height;
		int bpp;

		Dbl centerx, centery;
		Dbl showx, showy;

		Dbl r2;
		float r;

		int xlast, ylast;

		int oldflags;

	private:
		SDL_Surface *screen;



};

#endif

