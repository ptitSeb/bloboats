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

#ifndef _MODEL_TEX_H_
#define _MODEL_TEX_H_

#ifdef HAVE_GLES
#include "SDL_opengles.h"
#else
#include "SDL_opengl.h"
#endif

#include "particle.h"
#include "window.h"

class model_tex {
public:
	model_tex();
	model_tex(GLuint texID, vec t1, vec t2, vec t3, particle *p1, particle *p2, particle *p3);

	void display();
	void display_solid();
	void display_lines();

	GLuint texID;

	vec t1, t2, t3;
	particle *p1, *p2, *p3;

};

#endif
