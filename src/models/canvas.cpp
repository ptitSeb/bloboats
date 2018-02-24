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

#include "canvas.h"
#include "../water.h"

#include "../graphics.h"
#include "../model_tex.h"

#include "../path.h"
#include "../water.h"

extern path *Path;
extern water Water;

extern graphics Graphics;

extern const float gravity;

canvas::canvas() {
}

canvas::canvas( vec center, float w, float h, int n, int m, char *tex) : model(3000),
a_bend(0), a_stretch(0), a_strive(0), striven(false),n(n),m(m) { // hardness

	type = -1;

	texpath = (string)(Path->data(tex));

	minhardness = hardness;
	maxhardness = hardness;

	numparticles = (n+1)*(m+1)+1;
//	numsprings = (m+1)*n + (n+1)*m;
	numsprings = (m+1)*n + (n+1)*m + 2*n*m +3;

	p = new particle[numparticles];
	s = new spring[numsprings];


	// create grid

	for (int j=0; j<=m; j++) {
		for (int i=0; i<=n; i++) {
			float x = (Dbl)i/Dbl(n)-0.5;
			float y = -(Dbl)j/Dbl(m)+0.5;

			p[ j*(n+1) + i ] = particle( center +
				vec(w*x,h*y), vec(0,0), 1.0, -9.81*2, 0.05);
		}
	}

	p[(n+1)*(m+1)] = particle(center + vec(-w/1.7, 0), vec(0,0), 1.0, -9.81*2, 0.05);

	p[(n+1)*(m+1)].immobile=true;



	// springs:

// o--o--o--o
	int l=0;
	for (int j=0; j<=m; j++) {
		for (int i=0; i<n; i++) {
			int a = j*(n+1)+i;
			int b = j*(n+1)+i+1;
			s[l++] = spring( &p[a], &p[b], (p[a].p-p[b].p).abs(), 0.0, hardness, -30.0);
		}
	}

// (o--o--o--o)^T
	for (int j=0; j<m; j++) {
		for (int i=0; i<=n; i++) {
			int a = j*(n+1)+i;
			int b = (j+1)*(n+1)+i;
			s[l++] = spring( &p[a], &p[b], (p[a].p-p[b].p).abs(), 0.0, hardness, -30.0);
		}
	}

// (o\o\o\o)
	for (int j=0; j<m; j++) {
		for (int i=0; i<n; i++) {
			int a = j*(n+1)+i;
			int b = (j+1)*(n+1)+i+1;
			s[l++] = spring( &p[a], &p[b], (p[a].p-p[b].p).abs(), 0.0, hardness/40, -1000.0);
		}
	}

// (o/o/o/o)^T
	for (int j=0; j<m; j++) {
		for (int i=1; i<=n; i++) {
			int a = j*(n+1)+i;
			int b = (j+1)*(n+1)+i-1;
			s[l++] = spring( &p[a], &p[b], (p[a].p-p[b].p).abs(), 0.0, hardness/40, -1000.0);
		}
	}

//	s[l++] = spring( &p[0], &p[(n+1)*m], (p[0].p-p[(n+1)*m].p).abs()*1.5, 0.0, hardness/1000, -2000);
//	s[l++] = spring( &p[n], &p[(n+1)*m+n], (p[0].p-p[(n+1)*m].p).abs()*1.5, 0.0, hardness/1000, -2000);

	s[l++] = spring( &p[0], &p[(n+1)*(m+1)], (p[0].p-p[(n+1)*(m+1)].p).abs(), 0.0, hardness, -20);
	s[l++] = spring( &p[m*(n+1)], &p[(n+1)*(m+1)], (p[0].p-p[(n+1)*(m+1)].p).abs(), 0.0, hardness, -20);

	s[l++] = spring( &p[0], &p[(n+1)*m], (p[0].p-p[(n+1)*m].p).abs()*1.2, 0.0, hardness, -200);
//	s[l++] = spring( &p[n], &p[(n+1)*m+n], (p[n].p-p[(n+1)*m+n].p).abs()*1.2, 0.0, hardness, -200);

	t = new model_tex[n*m*2];
	texID = Graphics.maketexture(Path->data(tex));

	// texture;

	l=0;
	float dx = 1.0/Dbl(n);
	float dy = 1.0/Dbl(m)*0.43;

	for (int j=0; j<m; j++) {
		for (int i=0; i<n; i++) {
			float x = (Dbl)i/Dbl(n);
			float y = (Dbl)j/Dbl(m)*0.43;
			t[l++] = model_tex(texID, vec(x, y), vec(x+dx, y), vec(x, y+dy),
				&p[ j*(n+1)+i ], &p[ j*(n+1)+i+1 ], &p[ (j+1)*(n+1)+i] );

			t[l++] = model_tex(texID, vec(x+dx, y+dy), vec(x+dx, y), vec(x, y+dy),
				&p[ (j+1)*(n+1)+i+1 ], &p[ j*(n+1)+i+1 ], &p[ (j+1)*(n+1)+i] );

		}
	}

   numtriangles = n*m*2;


	radius=-1;


}

canvas::~canvas() {
	Graphics.deletetexture(texID);
}


void canvas::unloadtexture() {
	Graphics.deletetexture(texID);
}

void canvas::reloadtexture() {
	texID = Graphics.maketexture((char*)texpath.c_str());

	int l=0;
	float dx = 1.0/Dbl(n);
	float dy = 1.0/Dbl(m)*0.43;

	for (int j=0; j<m; j++) {
		for (int i=0; i<n; i++) {
			t[l++].texID = texID;
		}
	}

}

void canvas::bendleft(Dbl dt) {
//	a_bend-=0.5*dt;
	if (a_bend < -1) a_bend = -1;

	vec mp = midpoint();

	float pM = 0;

	for (int i=0; i<numparticles; i++) {
		pM += (p[i].p-mp).cross(p[i].v);
   }
	pM/=numparticles;

	float maxM = 30;
	momentum((maxM-pM)/2, dt);


//	momentum(10, dt);

//	for (int i=4; i<=13; i++) p[i].v -= vec( 7.0*dt, 0 );

}

void canvas::bendright(Dbl dt) {
//	a_bend+=0.5*dt;

   if (a_bend < -1) a_bend = -1;

   vec mp = midpoint();

   float pM = 0;

   for (int i=0; i<numparticles; i++) {
      pM += (p[i].p-mp).cross(p[i].v);
   }
   pM/=numparticles;

   float minM = 30;
   momentum((-pM-minM)/2, dt);


//   momentum(10, dt);



//	if (a_bend > 1) a_bend = 1;
//	momentum(-10, dt);
//	for (int i=4; i<=13; i++) p[i].v += vec( 7.0*dt, 0 );

}

void canvas::stretchup(Dbl dt) {
//	a_stretch+=dt*15;

	if (a_stretch > 1) a_stretch = 1;

	if (p[15].p.y < Water.w(p[15].p.x) ) {
		vec akseli = (p[4].p-p[15].p);
		akseli = akseli/akseli.abs();
		p[15].v += akseli*(400.0*dt);
	}

}

void canvas::stretchdown(Dbl dt) {
//	a_stretch-=dt*15;


	if (a_stretch > 1) a_stretch = 1;

	if (p[15].p.y < 0) {
		vec akseli = (p[4].p-p[15].p);
		akseli = akseli/akseli.abs();
		p[15].v -= akseli*(200.0*dt);
	}

}

void canvas::strive(Dbl dt) {

	if (a_strive<=0.01) {
		for (int i=0; i<=20; i++) {
			if (p[i].p.y <= Water.w(p[i].p.x) ) p[i].v += vec(0,13); // 10
		}
		a_strive+=1;
	}

}


void canvas::update(Dbl dt) {
	// update springs

	for (int i=0; i<=n; i++) {
		float r = rand()/(RAND_MAX+1.0);
		p[i].v += vec(dt*100.0*r,0.0);


	}

	// update hardness
	if (hardness < hardness_p - 0.001)
		hardness *= 1+40*dt;
	else if (hardness > hardness_p + 0.001) hardness /= 1+40*dt;

}

