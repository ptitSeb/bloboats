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

#include "paatti.h"

#include "../water.h"

#include "../graphics.h"
#include "../model_tex.h"
#include "../path.h"

extern graphics Graphics;
extern path *Path;
extern water Water;

vec tc[29]={
	vec(409, 295), vec(69, 56), vec(126, 26), vec(203, 20), vec(276, 39),
						vec(59, 161), vec(120, 125), vec(195, 123), vec(236, 119),
						vec(12, 168), vec(97, 172), vec(175, 175), vec(228, 174), vec(308, 178), vec(396, 168), vec(489, 153),
						vec(30, 247), vec(98, 250), vec(174, 251), vec(242, 250), vec(320, 250), vec(380, 241), vec(453, 231),
						vec(32, 299), vec(86, 324), vec(171, 328), vec(257, 327), vec(317, 322), vec(375, 310)

};

int tx[36][6]={
{1,2,5, 28,27,24}, {2,5,6, 27,24,23}, {2,3,6, 27,26,23}, {3,6,7, 26,23,22}, {3,7,8, 26,22,21}, {3,4,8, 26,25,21},
{5,9,10, 24,20,17}, {5,6,10, 24,23,17}, {6,10,11, 23,17,14}, {6,7,11, 23,22,14}, {7,11,12, 22,14,11}, {7,8,12, 22,21,11},
{9,10,16, 20,17,19}, {10,16,17, 17,19,16}, {10,11,17, 17,14,16}, {11,17,18, 14,16,13}, {11,18,19, 14,13,7},
{11,12,19, 14,11,7}, {12,13,19, 11,10,7}, {13,19,20, 10,7,5}, {13,20,21, 10,5,3}, {13,14,21, 10,9,3}, {14,21,22, 9,3,0},
{14,15,22, 9,8,0}, {16,23,24, 19,18,15}, {16,17,24, 19,16,15}, {17,24,25, 16,15,12}, {17,18,25, 16,13,12},
{18,19,25, 13,7,12}, {19,25,26, 7,12,6}, {19,26,27, 7,6,4}, {19,20,27, 7,5,4}, {20,27,28, 5,4,2}, {20,21,28, 5,3,2},
{21,28,0, 3,2,1}, {21,22,0, 3,0,1}
};

paatti::paatti(Dbl cx, Dbl cy, Dbl angle, Dbl scale, char *tex) : model(1500),
a_bend(0), a_stretch(0), a_strive(0), striven(false) { // hardness
	if (!loadmodel( "models/paatti.mdl" , cx, cy, angle, scale, hardness, -400.0) ) {
		fprintf (stderr, "Couldn't interpret \"%s\"! Critical!\n", Path->data("models/paatti.mdl") );
		exit(1);
	}

	type = -1;

	minhardness=800;
	maxhardness=800;

	t = new model_tex[36];
	texID = Graphics.maketexture( Path->data(tex) );

	for (int i=0; i<36; i++) {
		t[i] = model_tex(texID, tc[tx[i][0]]/512.0, tc[tx[i][1]]/512.0, tc[tx[i][2]]/512.0,
					&p[tx[i][3]], &p[tx[i][4]], &p[tx[i][5]] );
	}

   numtriangles = 36;

	radius = 3.0;

	ugly_r = 0.9;
	ugly_g = 0.9;
	ugly_b = 1.0;

}


paatti::~paatti() {
	Graphics.deletetexture(texID);
}


void paatti::bendleft(Dbl dt) {
	if (a_bend < -1) a_bend = -1;

	vec mp = midpoint();

	float pM = 0;

	for (int i=0; i<numparticles; i++) {
		pM += (p[i].p-mp).cross(p[i].v);
   }
	pM/=numparticles;

	float maxM = 40;
	momentum((maxM-pM)/2, dt);
}

void paatti::bendright(Dbl dt) {
   if (a_bend < -1) a_bend = -1;

   vec mp = midpoint();

   float pM = 0;

   for (int i=0; i<numparticles; i++) {
      pM += (p[i].p-mp).cross(p[i].v);
   }
   pM/=numparticles;

   float minM = 40;
   momentum((-pM-minM)/2, dt);
}


void paatti::stretchup(Dbl dt) {
	if (a_stretch > 1) a_stretch = 1;

	if (p[15].p.y < Water.w(p[15].p.x) ) {
		vec akseli = (p[3].p-p[15].p);
		akseli = akseli/akseli.abs();
		p[15].v += akseli*(600.0*dt);
	}
}

void paatti::stretchdown(Dbl dt) {
	if (a_stretch > 1) a_stretch = 1;

	if (p[15].p.y < 0) {
		vec akseli = (p[4].p-p[15].p);
		akseli = akseli/akseli.abs();
		p[15].v -= akseli*(200.0*dt);
	}
}

void paatti::strive(Dbl dt) {

	if (a_strive<=0.01) {
		for (int i=0; i<=20; i++) {
			if (p[i].p.y <= Water.w(p[i].p.x) ) p[i].v += vec(0,18); // 10
		}
		a_strive+=1;
	}

}


void paatti::update(Dbl dt) {
	// update springs

	for (int i=0; i<numparticles; i++) {
		if (p[i].p.y < Water.w(p[i].p.x) ) {
			p[i].v /= 1+(2*dt);
			vec n = vec(-1*Water.dw(p[i].p.x), 1);
			n = n/n.abs();
			p[i].v += n*(50*dt); // 60
		}
	}

	// strive-jump:
	if (a_strive > 0) a_strive -= 1*dt;

	// update hardness
	if (hardness < hardness_p - 0.001)
		hardness *= 1+40*dt;
	else if (hardness > hardness_p + 0.001) hardness /= 1+40*dt;

}

float paatti::getdamage() {
	float dam = 0;
	for (int i=0; i<numparticles; i++) {
		dam += p[i].damage;
		p[i].damage=0;
	}
	return dam;
}

float paatti::getwaterdamage() {
	float dam = 0;

	bool wasinwater = false;
	bool isinwater = false;

	// if the first particle comes to water, report.

	for (int i=0; i<numparticles; i++) {
		if (p[i].inwater) { // was in water
			wasinwater=true; // already in water
			break;
		}
	}

	for (int i=0; i<numparticles; i++) {
		if (p[i].p.y <= Water.w(p[i].p.x) ) {
			isinwater = true;
			if (!p[i].inwater) {
				float n = p[i].v.abs();
				if (n > dam) dam = n;
			}
			p[i].inwater=true;
		} else p[i].inwater=false;
	}
	
	if (!wasinwater && isinwater) return dam;
	else return 0.0;

}
