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

#ifndef _MODEL_H_
#define _MODEL_H_

#include <SDL.h>
#include <math.h>
#include <assert.h>

#include "graphics.h"

#include "vec.h"
#include "particle.h"
#include "spring.h"
#include "wall.h"
#include "model_tex.h"

#define ENEMY_TENTACLE 0
#define ENEMY_INVADER 1
#define ENEMY_BOULDER 2
#define ENEMY_TUX 3

class model {
public:
	model(Dbl cx, Dbl cy, Dbl angle, Dbl scale);
	model(Dbl hardness);
	model();
	virtual ~model();

	bool loadmodel(char *filename, Dbl cx, Dbl cy, Dbl angle, Dbl scale, Dbl hardness, Dbl damp);

	bool flip(wall *walls, int numwalls);

	virtual void springs_act(Dbl dt);
	virtual void particles_act(Dbl dt, wall *walls, int numwalls, Dbl t);

	virtual void display ();

	vec midpoint();
	vec midv();

	void momentum(Dbl M, Dbl dt);

	virtual void bendleft(Dbl dt);
	virtual void bendright(Dbl dt);

	virtual void stretchup(Dbl dt);
	virtual void stretchdown(Dbl dt);

	virtual void strive(Dbl dt);
	virtual void duck(Dbl dt);

	virtual void update(Dbl dt);


	virtual void stick(bool onoff);

	virtual void settime(Dbl t);

	virtual bool checkgoal(vec goal, float goalr);

	virtual float getdamage();
	virtual float getwaterdamage();

	virtual void updatewalls();
	virtual bool checkwalls(vec mp);
	
	void precision();

	particle *p;
	spring *s;
	model_tex *t;

	int numparticles;
	int numsprings;
	int numtriangles;


	Dbl hardness;
	Dbl hardness_p;

	Dbl minhardness;
	Dbl maxhardness;

	Dbl time;

	float radius;

	float ugly_r, ugly_g, ugly_b;

	GLuint texID;


	int inwater;


	int type;

private:
	void corrupt(char *filename);

};

#endif
