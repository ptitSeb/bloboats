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

#include "boulder.h"

#include "../water.h"

#include "../graphics.h"
#include "../model_tex.h"
#include "../path.h"

extern water Water;
extern graphics Graphics;
extern path *Path;

vec boulder_tc[7]={
vec( 212.999998, 165.135391 ),
vec( 134.473879,221.999992 ),
vec( 26.555019,197.868254 ),
vec( 221.341561,77.344397 ),
vec( 137.404687,12 ),
vec( 13,103.093911 ),
vec( 49.395655,37.594452 ) };



int boulder_tx[5][3]={ {0,1,2},{0,2,5},{0,5,3},{3,5,6},{3,4,6} };

int boulder_w[5][3]={ {0,1,2},{0,2,5},{0,5,3},{3,5,6},{3,4,6} };


boulder::boulder(Dbl cx, Dbl cy, Dbl angle, Dbl scale, char *tex) : model(1500),
a_bend(0), a_stretch(0), a_strive(0), striven(false) { // hardness
	if (!loadmodel( "models/boulder.mdl" , cx, cy, angle, scale, hardness, -40.0) ) {
		fprintf (stderr, "Couldn't interpret \"%s\"! Critical!\n", Path->data("models/boulder.mdl") );
		exit(1);
	}

	type = ENEMY_BOULDER;

	minhardness=800;
	maxhardness=800;

	fprintf (stderr, "5 new model_tex\n");
	t = new model_tex[5];
	texID = Graphics.maketexture( Path->data(tex) );

	for (int i=0; i<5; i++) {
		t[i] = model_tex(texID, boulder_tc[boulder_tx[i][0]]/512.0, boulder_tc[boulder_tx[i][1]]/512.0,
					boulder_tc[boulder_tx[i][2]]/512.0, &p[boulder_tx[i][0]], &p[boulder_tx[i][1]], &p[boulder_tx[i][2]] );
	}

	numtriangles = 5;

	radius = 2.5;

	alive=600;

	momentum(angle, 0.01);

	ugly_r = 0.1;
	ugly_g = 0.1;
	ugly_b = 0.1;
/*
	ugly_r = 0.6;
	ugly_g = 0.6;
	ugly_b = 0.6;
*/
	inwater=false;
	collided=false;

}


boulder::~boulder() {
	Graphics.deletetexture(texID);
}

void boulder::bendleft(Dbl dt) {
}

void boulder::bendright(Dbl dt) {
}

void boulder::stretchup(Dbl dt) {
}

void boulder::nobend(Dbl dt) {
}

void boulder::stretchdown(Dbl dt) {
}

void boulder::strive(Dbl dt) {
}

void boulder::nostretch(Dbl dt) {
}


void boulder::update(Dbl dt) {
	// update hardness
	if (hardness < hardness_p - 0.001)
		hardness *= 1+40*dt;
	else if (hardness > hardness_p + 0.001) hardness /= 1+40*dt;



	if (alive > 0 && alive<600) alive--;
}

float boulder::getdamage() {
	float dam = 0;
	if (!collided) {

		for (int i=0; i<numparticles; i++) {
			dam += p[i].damage*20.0;
			p[i].damage=0;
		}
	}

	if (dam > 0.0 && alive) {
		collided=true;
		alive--;
	}
	return dam;
}

float boulder::getwaterdamage() {
	float dam = 0;

	if (alive > 0 && !inwater) {
		for (int i=0; i<numparticles; i++) {
			if (p[i].p.y <= Water.w(p[i].p.x) ) {
				dam += p[i].v.abs();
			}
		}
	}

	if (dam > 0) inwater=true;

	return dam;
}

void boulder::updatewalls() {
	for (int i=0; i<5; i++) {
		walls[i] = wall( 0, p[boulder_w[i][0]].p, p[boulder_w[i][1]].p, p[boulder_w[i][2]].p );
	}
}


bool boulder::checkwalls(vec mp) {
	// checks if mp is inside at least one of the walls
//	return true;

	if (midpoint().y >= 100.0 || alive < 300) return false; // bah, useless that high :)

	for (int i=0; i<5; i++) {

		if ( walls[i].alive && (walls[i].mp-mp).abs2() <= walls[i].r2+1 ) {
			wall *w = &walls[i];
			vec ap = mp - w->oa;
			vec ab = w->ab;
			vec ac = w->ac;

			Dbl n = ap.cross(ac)/w->ab_x_ac;
			Dbl m = ab.cross(ap)/w->ab_x_ac;

			if (n<0 || m<0 || n+m>1) continue;
			return true; // Oh no! We're hitting!
		}
	}

	return false; // no collision
}


void boulder::display() {
	if ( alive > 0 && midpoint().y < 200.0 ) {
		if (alive==600) model::display();
		else {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
			float k=alive/600.0;
	
			glColor4f(1.0, 1.0, 1.0, k);
			model::display();
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}
	}

}

void boulder::springs_act(Dbl dt) {
	if (alive) {
		for (int i = 0; i < numsprings; i++) {
			s[i].act(dt);
		}
	}
}

void boulder::particles_act(Dbl dt, wall *walls, int numwalls, Dbl t) {
	if (alive) {
		// copy meaningful walls to another array and check only them.

		wall **checktable = new wall*[numwalls];
		int nearbywalls = 0;
	
		// make optimization table
		for (int i=0; i<numwalls; i++) {
			float r = walls[i].r+radius;
			if ( (walls[i].mp - midpoint()).abs2() <= r*r+1.0 ) {
				checktable[nearbywalls++] = &walls[i];
			}
		}

		// particles act
		for (int j=0; j<numparticles; j++) {
			p[j].act(dt, checktable, nearbywalls);
		}

		delete[] checktable;

	}

}

