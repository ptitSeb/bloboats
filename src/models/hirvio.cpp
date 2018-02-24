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

#include "hirvio.h"

#include "../water.h"

#include "../graphics.h"
#include "../model_tex.h"
#include "../path.h"

extern graphics Graphics;
extern path *Path;
extern water Water;

vec hirvio_tc[40]={
	vec(263,284), vec(74,284), vec(46,319), vec(58, 379), vec(52,444), vec(64,476), vec(79,445),
	vec(75,376), vec(93,319), vec(111,380), vec(103,446), vec(114,473), vec(131,446), vec(124,379),
	vec(142,319), vec(163,379), vec(154,446), vec(166,468), vec(181,446), vec(176,381), vec(187,317),
	vec(204,381), vec(199,441), vec(211,464), vec(227,442), vec(218,383), vec(241,317), vec(250,382),
	vec(243,443), vec(259,464), vec(272,443), vec(263,381),vec(282,319), vec(294,203), vec(275,122),
	vec(215,59), vec(126,58), vec(65,116), vec(47,203), vec(169,191)
};

int hirvio_tx[39][3]={
{1,2,8},{2,3,8},{3,4,6},{4,5,6},{6,7,3},{7,3,8},{8,14,9},{9,10,12},{10,11,12},{12,13,9},
{13,14,9},{14,15,20},{15,16,18},{16,17,18},{18,19,15},{19,20,15},{20,21,26},{21,22,24},
{22,23,24},{24,25,21},{25,26,21},{26,32,27},{27,28,30},{28,29,30},{27,30,31},{31,32,27},
{32,0,26},{0,39,26},{26,39,20},{39,20,14},{14,39,8},{8,39,1},{1,39,38},{38,39,37},{37,39,36},
{36,39,35},{35,39,34},{34,39,33},{33,39,0}
};

int hirvio_w[15][3]={
{39,0,33},{39,33,34},{39,34,35},{39,35,36},{39,36,37},
{39,37,38},{39,38,1},{39,1,0},{2,5,8},{8,11,14},
{14,17,20},{20,23,26},{26,29,32},{1,2,32},{1,0,32} };

hirvio::hirvio(Dbl cx, Dbl cy, Dbl angle, Dbl scale, char *tex) : model(1500),
a_bend(0), a_stretch(0), a_strive(0), striven(false) { // hardness
	if (!loadmodel( "models/hirvio.mdl" , cx, cy, angle, scale, hardness, -40.0) ) {
		fprintf (stderr, "Couldn't interpret \"%s\"! Critical!\n", Path->data("models/hirvio.mdl") );
		exit(1);
	}

	type = ENEMY_TENTACLE;

	minhardness=800;
	maxhardness=800;

	t = new model_tex[39];
	texID = Graphics.maketexture( Path->data(tex) );

	for (int i=0; i<39; i++) {
		t[i] = model_tex(texID, hirvio_tc[hirvio_tx[i][0]]/512.0, hirvio_tc[hirvio_tx[i][1]]/512.0,
					hirvio_tc[hirvio_tx[i][2]]/512.0, &p[hirvio_tx[i][0]], &p[hirvio_tx[i][1]], &p[hirvio_tx[i][2]] );
	}

	numtriangles = 39;


	ugly_r = 0.1;
	ugly_g = 0.3;
	ugly_b = 0.1;

/*
	ugly_r = 0.3;
	ugly_g = 0.7;
	ugly_b = 0.3;
*/

	radius = 5.0;

}

hirvio::~hirvio() {
	Graphics.deletetexture(texID);
}

void hirvio::bendleft(Dbl dt) {
//	a_bend-=0.5*dt;
	if (a_bend < -1) a_bend = -1;

	vec mp = midpoint();

	float pM = 0;

	for (int i=0; i<numparticles; i++) {
		pM += (p[i].p-mp).cross(p[i].v);
}
	pM/=numparticles;

	float maxM = 40;
	momentum((maxM-pM)/2, dt);



//	momentum(10, dt);

//	for (int i=4; i<=13; i++) p[i].v -= vec( 7.0*dt, 0 );

}

void hirvio::bendright(Dbl dt) {
//	a_bend+=0.5*dt;

if (a_bend < -1) a_bend = -1;

vec mp = midpoint();

float pM = 0;

for (int i=0; i<numparticles; i++) {
pM += (p[i].p-mp).cross(p[i].v);
}
pM/=numparticles;

float minM = 40;
momentum((-pM-minM)/2, dt);


//momentum(10, dt);



//	if (a_bend > 1) a_bend = 1;
//	momentum(-10, dt);
//	for (int i=4; i<=13; i++) p[i].v += vec( 7.0*dt, 0 );

}

void hirvio::nobend(Dbl dt) {
//	if (a_bend>0) a_bend -= dt*0.4;
//	if (a_bend<0) a_bend += dt*0.4;
}

void hirvio::stretchup(Dbl dt) {
//	a_stretch+=dt*15;

	if (a_stretch > 1) a_stretch = 1;

	if (p[39].p.y < Water.w(p[39].p.x) ) {
		vec akseli = (p[39].p-(p[2].p+p[32].p)/2.0);
		akseli = akseli/akseli.abs();
		p[15].v += akseli*(600.0*dt);
	}

}

void hirvio::stretchdown(Dbl dt) {
//	a_stretch-=dt*15;


	if (a_stretch > 1) a_stretch = 1;

	if (p[39].p.y < Water.w(p[39].p.x) ) {
		vec akseli = (p[39].p-(p[2].p+p[32].p)/2.0);
		akseli = akseli/akseli.abs();
		p[15].v -= akseli*(200.0*dt);
	}

}

void hirvio::strive(Dbl dt) {

	if (a_strive<=0.01) {
		for (int i=0; i<=20; i++) {
			if (p[i].p.y <= Water.w(p[i].p.x) ) p[i].v += vec(0,18); // 10
		}
		a_strive+=1;
	}

}

void hirvio::nostretch(Dbl dt) {
	if (a_stretch>0) a_stretch -= dt*12;
	if (a_stretch<0) a_stretch += dt*12;
}


void hirvio::update(Dbl dt) {
	// update springs
/*	for (int i=0; i<RCCAR_UPDATE; i++) s[ hirvio_update[i] ].normlen_reset();

	for (int i=0; i<RCCAR_BENDL; i++) s[ hirvio_bendl[i] ].normlen_modify(a_bend/20.0 + a_stretch/4.0 );
	for (int i=0; i<RCCAR_BENDR; i++) s[ hirvio_bendr[i] ].normlen_modify(-a_bend/20.0 + a_stretch/4.0 );
*/

	for (int i=2; i<=32; i++) {
		if (p[i].p.y < Water.w(p[i].p.x) ) {
			p[i].v /= 1+(2*dt);
			vec n = vec(-1*Water.dw(p[i].p.x), 1);
			n = n/n.abs();
			p[i].v += n*(5*dt); // 60
		}
	}

	for (int i=0; i<1; i++) {
		if (p[i].p.y < Water.w(p[i].p.x) ) {
			p[i].v /= 1+(2*dt);
			vec n = vec(-1*Water.dw(p[i].p.x), 1);
			n = n/n.abs();
			p[i].v += n*(100*dt); // 60
		}
	}

	for (int i=33; i<numparticles; i++) {
		if (p[i].p.y < Water.w(p[i].p.x) ) {
			p[i].v /= 1+(2*dt);
			vec n = vec(-1*Water.dw(p[i].p.x), 1);
			n = n/n.abs();
			p[i].v += n*(100*dt); // 60
		}
	}

//	a_bend/=exp(0.2*dt);

	// strive-jump:
	if (a_strive > 0) a_strive -= 1*dt;

	// update hardness
	if (hardness < hardness_p - 0.001)
		hardness *= 1+40*dt;
	else if (hardness > hardness_p + 0.001) hardness /= 1+40*dt;


//	for (int i=0; i<numsprings; i++) s[i].a=hardness+500*a_strive;

//	for (int i=130; i<=141; i++) s[i].a*=0.05;

}

void hirvio::changehardness_kbd(Dbl dt) {
	hardness_p*=exp(dt);
	if (hardness_p<minhardness) hardness_p=minhardness;
	if (hardness_p>maxhardness) hardness_p=maxhardness;
}

void hirvio::changehardness_mouse(int dy) {
	hardness_p*=exp(dy*0.007);
	if (hardness_p<minhardness) hardness_p=minhardness;
	if (hardness_p>maxhardness) hardness_p=maxhardness;
}

Dbl hirvio::gethardness() {
//	return hardness+a_strive*10000;
}

float hirvio::getdamage() {
	float dam = 0;
/*	for (int i=0; i<numparticles; i++) {
		dam += p[i].damage;
		p[i].damage=0;
	}*/
	return dam;
}

float hirvio::getwaterdamage() {
	float dam = 0;

	bool wasinwater=false;

	for (int j=0; j<HIRVIO_SPLASH; j++) {
		int i = hirvio_splash[j];
		if (p[i].inwater) wasinwater=true;
	}

	for (int j=0; j<HIRVIO_SPLASH; j++) {
		int i = hirvio_splash[j];

		if (p[i].p.y <= Water.w(p[i].p.x) ) {
			if (!p[i].inwater) {
				p[i].inwater=true;
				dam += p[i].v.abs();
			}
		} else p[i].inwater=false;

	}
	if (!wasinwater) return dam;
	return 0;
}

void hirvio::updatewalls() {
	for (int i=0; i<15; i++) {
		walls[i] = wall( 0, p[hirvio_w[i][0]].p, p[hirvio_w[i][1]].p, p[hirvio_w[i][2]].p );
	}
}


bool hirvio::checkwalls(vec mp) {
	// checks if mp is inside at least one of the walls

	for (int i=0; i<15; i++) {

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
