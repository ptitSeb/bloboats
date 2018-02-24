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

#ifndef _GAME_H_
#define _GAME_H_

#include <SDL.h>
#ifdef HAVE_GLES
#include "SDL_opengles.h"
#else
#include "SDL_opengl.h"
#endif


#include "vec.h"
#include "particle.h"
#include "spring.h"
#include "level.h"

#include "element.h"

#include "model.h"
#include "models/paatti.h"
#include "models/hirvio.h"
#include "models/invader.h"
#include "models/broken.h"
#include "models/boulder.h"
#include "models/tux.h"

#include "text.h"

#include <vector>


const Uint32 ms_physframetime = 5; // 1 frame / 10 ms => 100 frames / s
const Dbl dt = 0.001*ms_physframetime; // 0.005 seconds per frame = delta time = dt
// dt is now fixed to be 0.005 -- can't change anymore easily! optimizing

#define RECORD_RIGHT 1
#define RECORD_UP 2
#define RECORD_LEFT 4
#define RECORD_DOWN 8
#define RECORD_SPACE 16
#define RECORD_C 32

class game {
public:
	game();
	~game();

	void makeship(float x, float y, float vx, float vy, float angle, char *tex);
	void makeghost(float x, float y, float vx, float vy, float angle, char *tex);

	void makelevel(char *p, float x, float y, float angle, float zoom, char *tex);
	void makelevel(FILE *fp, char *tex);

	void focus();

	void setice(int i, int side, GLuint icetex, float f);
	void setground(int i, int side, GLuint groundtex, float f);

	void setgoal(float x, float y, float r, char *tex);

	void setrecord(element *first);
	void setghost(element *first, float alpha);

	void savelevel(FILE *fp);

	Dbl simulate();

	void maketentacle(float x, float y, float angle, float scale, char *tex);
	void makeinvader(float x, float y, float minx, float maxx, char *tex);
	void makeboulder(float x, float y, float angle, float scale, char *tex);
	void maketux(float x, float y, float angle, float scale, char *tex);

 
	Uint32 stime;
	Uint32 nextframe;
	Dbl t;	

	int omx, omy, mx, my; // mouse coordinates

	GLuint iceID;
	GLuint groundID;

	model *malli;
	level *taso;

	model *goal;
	float goalr;

	Uint8 record;
	element *recelement;

	Uint8 player;

	Uint8 ghost;
	model *ghostboat;
	float ghostalpha;

	element *ghostelement;

	model *ghostdead;
	model *mallidead;

	std::vector <text> texts;
	std::vector <model*> enemies;

	bool tuxfound;

};

#endif
