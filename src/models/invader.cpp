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

#include "invader.h"

#include "../water.h"

#include "../graphics.h"
#include "../model_tex.h"
#include "../path.h"

extern graphics Graphics;
extern path *Path;

vec invader_tc[8]={ vec(331,80),vec(457,148),vec(309,0),vec(149,0),vec(128,83),
	vec(3,146),vec(77,191),vec(382,191)};



int invader_tx[6][3]={{0,1,7},{0,2,3},{0,3,4},{4,5,6},{4,6,0},{0,6,7}};

int invader_w[6][3]={{0,1,7},{0,2,3},{0,3,4},{4,5,6},{4,6,0},{0,6,7}};

invader::invader(Dbl cx, Dbl cy, Dbl minx, Dbl maxx, char *tex) : model(1500),
a_bend(0), a_stretch(0), a_strive(0), striven(false), minx(minx), maxx(maxx) { // hardness
	if (!loadmodel( "models/invader.mdl" , cx, cy, 0.0, 0.6, hardness, -40.0) ) {
		fprintf (stderr, "Couldn't interpret \"%s\"! Critical!\n", Path->data("models/invader.mdl") );
		exit(1);
	}

	type = ENEMY_INVADER;

	for (int i=0; i<numparticles; i++) p[i].g=0;

	minhardness=800;
	maxhardness=800;

	dir = 1;

	t = new model_tex[6];
	texID = Graphics.maketexture( Path->data(tex) );

	for (int i=0; i<6; i++) {
		t[i] = model_tex(texID, invader_tc[invader_tx[i][0]]/512.0, invader_tc[invader_tx[i][1]]/512.0,
					invader_tc[invader_tx[i][2]]/512.0, &p[invader_tx[i][0]], &p[invader_tx[i][1]], &p[invader_tx[i][2]] );
	}

	ugly_r = 0.9;
	ugly_g = 0.9;
	ugly_b = 0.5;

/*
	ugly_r = 0.7;
	ugly_g = 0.5;
	ugly_b = 0.5;
*/

	numtriangles = 6;

	radius = 4.5;

}

invader::~invader() {
	Graphics.deletetexture(texID);
}


void invader::bendleft(Dbl dt) {
}

void invader::bendright(Dbl dt) {
}

void invader::stretchup(Dbl dt) {
}


void invader::stretchdown(Dbl dt) {
}

void invader::strive(Dbl dt) {

//	if (a_strive<=0.01) {
//		for (int i=0; i<=20; i++) {
//			if (p[i].p.y <= w(p[i].p.x, time) ) p[i].v += vec(0,18); // 10
//		}
//		a_strive+=1;
//	}

}


void invader::update(Dbl dt) {
	// strive-jump:
	if (a_strive > 0) a_strive -= 1*dt;

	// update hardness
	if (hardness < hardness_p - 0.001)
		hardness *= 1+40*dt;
	else if (hardness > hardness_p + 0.001) hardness /= 1+40*dt;


	for (int i=0; i<numparticles; i++) {
		if (p[i].p.x > maxx) dir=-1;
		if (p[i].p.x < minx) dir=1;
	}

	for (int i=0; i<numparticles; i++) {
		p[i].v *= 0.95;
		p[i].v += vec(dir*50.0*dt, 0);
	}

}

float invader::getdamage() {
	float dam = 0;
	for (int i=0; i<numparticles; i++) {
		dam += p[i].damage;
		p[i].damage=0;
	}
	return dam;
}

float invader::getwaterdamage() {
	return 0.0;
}

void invader::updatewalls() {
	for (int i=0; i<6; i++) {
		walls[i] = wall( 0, p[invader_w[i][0]].p, p[invader_w[i][1]].p, p[invader_w[i][2]].p );
	}
}


bool invader::checkwalls(vec mp) {
	// checks if mp is inside at least one of the walls

	for (int i=0; i<6; i++) {

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


void invader::display() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	model::display();

}

