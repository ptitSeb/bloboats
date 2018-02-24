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

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "vec.h"
#include "wall.h"

#include "graphics.h"
#include "window.h"

#define BOUNCE_EPSILON 0.01  // m/s

class particle {
public:
	particle();
	particle(vec p, vec v, Dbl mass, Dbl g, Dbl bfactor); // location, velocity, mass, gravitation, bounce factor

	void set(vec p, vec v);

	void act(Dbl dt, wall **walls, int numwalls); // delta time

	void display(Dbl r, Dbl g, Dbl b);

	void detectcollision (Dbl dt, vec &p, vec &v, wall **walls, int numwalls, int *nearestwall);
	int detectcollision (Dbl dt, vec &p, vec &v, wall *walls);
	int detectcollision_nov (vec &p, wall *walls); // no velocity

	bool is_onwall(vec &centerab, vec &ab, vec &ab_hat, Dbl lenp2); 

	Dbl t_to_edge(vec AP, vec V, vec AB, Dbl ABxAP, Dbl VxAB);


	vec  p, v;
	Dbl mass, g, bfactor;
	int onwall;

	bool alive;

	bool sticky;
	bool immobile;

	float damage;
	bool inwater;

	bool cheatmode;

private:


};

#endif
