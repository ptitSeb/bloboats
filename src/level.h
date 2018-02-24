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

#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "SDL.h"
#ifdef HAVE_GLES
#include "SDL_opengles.h"
#else
#include "SDL_opengl.h"
#endif

#include <math.h>
#include <assert.h>

#include "graphics.h"
#include "window.h"
#include "config.h"

#include "vec.h"
#include "wall.h"


class level {
public:
	level(char *filename, Dbl cx, Dbl cy, Dbl angle, Dbl scale, char *tex);
	level(FILE *fp, char *tex);
	level();
	~level();

	bool loadlevel(char *filename, Dbl cx, Dbl cy, Dbl angle, Dbl scale, char *tex);
	bool loadlevel(FILE *fp, char *tex);

	void savelevel(FILE *fp);

	void display();

	wall *w;
	int numwalls;

	int waterheight;

	GLuint texID;

private:
	void corrupt(char *filename);

	int numvertices;

};

#endif
