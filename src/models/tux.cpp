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

#include "tux.h"

#include "../water.h"

#include "../graphics.h"
#include "../model_tex.h"
#include "../path.h"

extern water Water;
extern graphics Graphics;
extern path *Path;

vec tux_tc[6]={
vec( 96.0, 337.0 ),vec( 118.0, 35.0 ),vec( 186.0, 36.0 ),
vec( 218.0, 336.0 ),vec( 8.0, 308.0 ),vec( 278.0, 293.0 ) };


int tux_tx[4][3]={ {0,1,4},{0,1,3},{1,2,3},{2,3,5} };

int tux_w[4][3]={ {0,1,4},{0,1,3},{1,2,3},{2,3,5} };


tux::tux(Dbl cx, Dbl cy, Dbl angle, Dbl scale, char *tex) : model(1500),
a_bend(0), a_stretch(0), a_strive(0), striven(false) { // hardness
	if (!loadmodel( "models/tux.mdl" , cx, cy, angle, scale, hardness, -40.0) ) {
		fprintf (stderr, "Couldn't interpret \"%s\"! Critical!\n", Path->data("models/tux.mdl") );
		exit(1);
	}

	type = ENEMY_TUX;

	minhardness=800;
	maxhardness=800;

	t = new model_tex[4];
	texID = Graphics.maketexture( Path->data(tex) );

	for (int i=0; i<4; i++) {
		t[i] = model_tex(texID, tux_tc[tux_tx[i][0]]/512.0, tux_tc[tux_tx[i][1]]/512.0,
					tux_tc[tux_tx[i][2]]/512.0, &p[tux_tx[i][0]], &p[tux_tx[i][1]], &p[tux_tx[i][2]] );
	}

	numtriangles = 4;

	radius = 2.5;

	alive=600;

	momentum(angle, 0.01);

	ugly_r = 1.0;
	ugly_g = 1.0;
	ugly_b = 1.0;


	inwater=false;
	collided=false;

}


tux::~tux() {
	Graphics.deletetexture(texID);
}

void tux::bendleft(Dbl dt) {
}

void tux::bendright(Dbl dt) {
}

void tux::stretchup(Dbl dt) {
}

void tux::nobend(Dbl dt) {
}

void tux::stretchdown(Dbl dt) {
}

void tux::strive(Dbl dt) {
}

void tux::nostretch(Dbl dt) {
}


void tux::update(Dbl dt) {
	// update hardness
	if (hardness < hardness_p - 0.001)
		hardness *= 1+40*dt;
	else if (hardness > hardness_p + 0.001) hardness /= 1+40*dt;

   for (int i=0; i<6; i++) {
      if (p[i].p.y < Water.w(p[i].p.x) ) {
         p[i].v /= 1+(2*dt);
         vec n = vec(-1*Water.dw(p[i].p.x), 1);
         n = n/n.abs();
         p[i].v += n*(40*dt); // 60
      }
   }

	if (alive > 0 && collided) alive-=2;
}

float tux::getdamage() {
	return 0.0;
}

float tux::getwaterdamage() {
	float dam = 0;

	if (!inwater) {
		for (int i=0; i<numparticles; i++) {
			if (p[i].p.y <= Water.w(p[i].p.x) ) {
				dam += p[i].v.abs();
			}
		}
	}

	if (dam > 0) inwater=true;

	return dam;
}

void tux::updatewalls() {
	if (alive >= 300) {
		for (int i=0; i<4; i++) {
			walls[i] = wall( 0, p[tux_w[i][0]].p, p[tux_w[i][1]].p, p[tux_w[i][2]].p );
		}
	}
}


bool tux::checkwalls(vec mp) {
	// checks if mp is inside at least one of the walls
//	return true;
	if (midpoint().y >= 100.0 || collided) return false; // bah, useless that high :)

	for (int i=0; i<4; i++) {

		if ( walls[i].alive && (walls[i].mp-mp).abs2() <= walls[i].r2+1 ) {
			wall *w = &walls[i];
			vec ap = mp - w->oa;
			vec ab = w->ab;
			vec ac = w->ac;

			Dbl n = ap.cross(ac)/w->ab_x_ac;
			Dbl m = ab.cross(ap)/w->ab_x_ac;

			if (n<0 || m<0 || n+m>1) continue;
			// Oh no! We're hitting!
			collided=true;
			return true;
		}
	}

	return false; // no collision
}


void tux::display() {
	if ( midpoint().y < 100.0 && alive > 0) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (alive==600) model::display();
		else {
	
			float k=alive/600.0;
	
			glColor4f(1.0, 1.0, 1.0, k);
			model::display();
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}
	}

}

void tux::springs_act(Dbl dt) {
	if (alive) {
		for (int i = 0; i < numsprings; i++) {
			s[i].act(dt);
		}
	}
}

void tux::particles_act(Dbl dt, wall *walls, int numwalls, Dbl t) {
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

