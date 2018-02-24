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

#ifndef _WALL_H_
#define _WALL_H_

#include "SDL.h"
#ifdef HAVE_GLES
#include "SDL_opengles.h"
#else
#include "SDL_opengl.h"
#endif

#include "vec.h"
#include "graphics.h"
#include "window.h"


#define WALL_ONWALL_DIST 0.05

#define WALL_TYPES 3

#define WALL_DEFAULT 0
#define WALL_GROUND 1
#define WALL_ICE 2

class wall {
public:
	wall();
	wall(GLuint texID, vec oa, vec ob, vec oc);

	void seticetexture(GLuint tex);
	void setgroundtexture(GLuint tex);

	void displayground();
	void displayground_solid();

	void displayedges();

	void drawedge(GLuint tex, vec OA, vec AB, Dbl AB_LEN, Dbl F_AB);

	bool max_midedge(vec &mp, float &r);

	vec ab, ac, bc, centerab, centerac, centerbc, ab_hat, ac_hat, bc_hat;
	vec oa, ob, oc;

	Dbl ab_x_ac;
	Dbl ab_x_ac_inv;


	Dbl ab_len2, ac_len2, bc_len2;

	Dbl ab_lenp2, ac_lenp2, bc_lenp2;
	Dbl ab_lenp2_2, ac_lenp2_2, bc_lenp2_2;

	Dbl ab_len, ac_len, bc_len;


	vec mp;
	Dbl r2;
	float r;

	GLuint texID;
	bool tex;

	GLuint groundID;
	bool groundtex;

	GLuint iceID;
	bool icetex;

	int type_ab, type_ac, type_bc;
	Dbl f_ab, f_ac, f_bc;

	bool alive;
};

#endif
