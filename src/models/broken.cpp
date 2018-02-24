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

#include "broken.h"

#include "../water.h"

#include "../graphics.h"
#include "../model_tex.h"
#include "../path.h"

extern graphics Graphics;
extern path *Path;
extern water Water;

vec broken_tc[29]={
	vec(409, 295), vec(69, 56), vec(126, 26), vec(203, 20), vec(276, 39),
						vec(59, 161), vec(120, 125), vec(195, 123), vec(236, 119),
						vec(12, 168), vec(97, 172), vec(175, 175), vec(228, 174), vec(308, 178), vec(396, 168), vec(489, 153),
						vec(30, 247), vec(98, 250), vec(174, 251), vec(242, 250), vec(320, 250), vec(380, 241), vec(453, 231),
						vec(32, 299), vec(86, 324), vec(171, 328), vec(257, 327), vec(317, 322), vec(375, 310)

};

int broken_corr[74]={
28,27,24,23, 27,26,23,22, // 8
26,25,22,21, 24,23,17, // 15
23,22,17,14, 21,21,14,11, // 23
20,19,17,16, 20,24,17, // 29
19,18,16,15, 16,15,13,12, // 37
17,16,14,13, 13,12,7,6, // 45
14,13,11,7,  7,5,6,4, // 53
11,10,7,5,   5,4,3,2, // 61
10,9,5,3,    3,2,1,0, // 69
9,8,3,0 };

int broken_tx[36][6]={
{0,1,2, 1,2,5},
{1,2,3, 2,5,6},
{4,5,6, 2,3,6},
{5,6,7, 3,6,7},
{8,9,10, 3,4,7},
{9,10,11, 4,7,8},
{12,13,14, 5,6,10},
{15,16,17, 6,7,10},
{16,17,18, 7,10,11},
{19,20,21, 7,8,11},
{20,21,22, 8,11,12},
{23,24,25, 9,16,10},
{24,25,26, 16,10,17},
{27,28,29, 9,5,10},
{30,31,32, 16,23,17},
{31,32,33, 23,17,24},
{34,35,36, 17,24,18},
{35,36,37, 24,18,25},
{38,39,40, 10,17,11},
{39,40,41, 17,11,18},
{42,43,44, 18,25,19},
{43,44,45, 25,19,26},
{46,47,48, 11,18,12},
{47,48,49, 18,12,19},
{50,51,52, 19,20,26},
{51,52,53, 20,26,27},
{54,55,56, 12,13,1},
{55,56,57, 13,19,20},
{58,59,60, 20,27,21},
{59,60,61, 27,21,28},
{62,63,64, 13,14,20},
{63,64,65, 14,20,21},
{66,67,68, 21,23,0},
{67,68,69, 23,0,22},
{70,71,72, 14,15,21},
{71,72,73, 15,21,22} }; 

/*
int tx[36][6]={
{1,2,5, 28,27,24}, {2,5,6, 27,24,23}, {2,3,6, 27,26,23}, {3,6,7, 26,23,22}, {3,7,8, 26,22,21}, {3,4,8, 26,25,21},
{5,9,10, 24,20,17}, {5,6,10, 24,23,17}, {6,10,11, 23,17,14}, {6,7,11, 23,22,14}, {7,11,12, 22,14,11}, {7,8,12, 22,21,11},
{9,10,16, 20,17,19}, {10,16,17, 17,19,16}, {10,11,17, 17,14,16}, {11,17,18, 14,16,13}, {11,18,19, 14,13,7},
{11,12,19, 14,11,7}, {12,13,19, 11,10,7}, {13,19,20, 10,7,5}, {13,20,21, 10,5,3}, {13,14,21, 10,9,3}, {14,21,22, 9,3,0},
{14,15,22, 9,8,0}, {16,23,24, 19,18,15}, {16,17,24, 19,16,15}, {17,24,25, 16,15,12}, {17,18,25, 16,13,12},
{18,19,25, 13,7,12}, {19,25,26, 7,12,6}, {19,26,27, 7,6,4}, {19,20,27, 7,5,4}, {20,27,28, 5,4,2}, {20,21,28, 5,3,2},
{21,28,0, 3,2,1}, {21,22,0, 3,0,1}
};
*/

broken::broken(model *paat) : model(1500),
a_bend(0), a_stretch(0), a_strive(0), striven(false) { // hardness
	if (!loadmodel( "models/broken.mdl" , 0.0, 0.0, 0.0, 1.0, 100.0, -400.0) ) {
		fprintf (stderr, "Couldn't interpret \"%s\"! Critical!\n", Path->data("models/broken.mdl") );
		exit(1);
	}


	minhardness=800;
	maxhardness=800;

	int i=1;

	vec mp = paat->midpoint();

	for (int i=0; i<74; i++) {
		vec d = paat->p[broken_corr[i]].p-mp;

		p[i].set(paat->p[broken_corr[i]].p, paat->p[broken_corr[i]].v + d*20.0 + vec(d.y, -d.x)*20.0*d.abs() );
	}


	t = new model_tex[36];


	texID = paat->texID;

	for (int i=0; i<36; i++) {
		t[i] = model_tex(texID, broken_tc[broken_tx[i][3]]/512.0, broken_tc[broken_tx[i][4]]/512.0,
					broken_tc[broken_tx[i][5]]/512.0, &p[broken_tx[i][0]], &p[broken_tx[i][1]], &p[broken_tx[i][2]] );
	}

   numtriangles = 36;


	ugly_r = 0.9;
	ugly_g = 0.9;
	ugly_b = 1.0;

	radius = 50.0;
}

broken::~broken() {
	Graphics.deletetexture(texID);
}


void broken::bendleft(Dbl dt) {
}

void broken::bendright(Dbl dt) {
}

void broken::stretchup(Dbl dt) {
}

void broken::stretchdown(Dbl dt) {
}

void broken::strive(Dbl dt) {
}


void broken::update(Dbl dt) {
	// update springs

	for (int i=0; i<numparticles; i++) {
		if (p[i].p.y < Water.w(p[i].p.x) ) {
			p[i].v /= 1+(2*dt);
			vec n = vec(-1*Water.dw(p[i].p.x), 1);
			n = n/n.abs();
			p[i].v += n*(10*dt); // 60
		}
	}


	// update hardness
	if (hardness < hardness_p - 0.001)
		hardness *= 1+40*dt;
	else if (hardness > hardness_p + 0.001) hardness /= 1+40*dt;

}


float broken::getdamage() {
	float dam = 0;
	for (int i=0; i<numparticles; i++) {
		dam += p[i].damage;
		p[i].damage=0;
	}
	return dam;
}

float broken::getwaterdamage() {
/*	float dam = 0;
	for (int j=0; j<broken_SPLASH; j++) {
		int i = broken_splash[j];

		if (p[i].p.y <= Water.w(p[i].p.x) ) {
			if (!p[i].inwater) {
				p[i].inwater=true;
				dam += p[i].v.abs();
			}
		} else p[i].inwater=false;

	}

	return dam;
*/
	return 0.0;
}
