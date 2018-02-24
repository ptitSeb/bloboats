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

#include "model.h"
#include "config.h"

#include "compat.h"

#include "path.h"

extern path *Path;

extern graphics Graphics;
extern config Config;

const float gravity = -9.81*1.5;

model::model(Dbl cx, Dbl cy, Dbl angle, Dbl scale) {
//	if (!loadmodel(filename, cx, cy, angle, scale)) {
//		fprintf (stderr, "Couldn't interpret \"%s\"! Critical!\n", filename);
//		exit(1);
//	}
}

model::model(Dbl hardness): hardness(hardness), hardness_p(hardness) {
}

model::model() {
}

model::~model() {
	delete[] t;
	delete[] p;
	delete[] s;
}


bool model::loadmodel(char *filename, Dbl cx, Dbl cy, Dbl angle, Dbl scale, Dbl hardness, Dbl damp) {
	// loads the model (surprise!)

	FILE *fp = fopen( Path->data(filename), "r");

	if (!fp) {
		fprintf(stderr, "No such file!\n");
		return false;
	}

	if ( fscanf(fp, "%d %d\n", &numparticles, &numsprings) != 2) {
		fprintf(stderr, "Corrupted file! Number of particles or springs.\n");
		return false;
	}
	p = new particle[numparticles];
	s = new spring[numsprings];

	if (!p) {
		fprintf(stderr, "Couldn't get memory!\n");
		return false;
	}
	if (!s) {
		fprintf(stderr, "Couldn't get memory!\n");
		return false;
	}

	Dbl cosx = cos(angle);
	Dbl sinx = sin(angle);

	for (int i=0; i<numparticles; i++) {
		p[i] = particle( vec(0,0), vec(0,0), 1.0 /* kg */, gravity, 0.05);

		float x, y, m;

		if (fscanf(fp, "%f %f\n", &x, &y)!=2) {
			fprintf (stderr, "Corrupted file! (x, y)\n");
			return false;
		}

		p[i].p.x = x;
		p[i].p.y = y;
		p[i].mass = 1.0;
//		if (m>0.9) p[i].mass = m;
//		else p[i].mass = 0.95;


		p[i].p.rotate(cosx, sinx);
		p[i].p*=scale;
		p[i].p += vec(cx, cy);
	}

	for (int i=0; i<numsprings; i++) {
		int a, b;
		if (fscanf(fp, "%d %d\n", &a, &b)!=2) {
			fprintf (stderr, "Corrupted file!\n");
			return false;
		}
		if (a>=0 && b>=0 && a<numparticles && b<numparticles) {
			vec d = p[a].p - p[b].p;
			s[i] = spring( &p[a], &p[b], d.abs(), 0.0, hardness, damp);
		} else {
			fprintf(stderr, "Corrupted file!\n");
			return false;
		}
	}

	// reset texture
	t = 0;
	numtriangles = 0;

	return true;
}

void model::springs_act(Dbl dt) {
	for (int i = 0; i < numsprings; i++) {
		s[i].act(dt);
	}
}

bool model::flip(wall *walls, int numwalls) {
	vec mp = midpoint();
	vec akseli = p[28].p-p[15].p; // whoops.. only for paatti-model.. Who cares ;)
	akseli = vec(-akseli.y, akseli.x);
	akseli = akseli/akseli.abs();


	bool inwall=false;

	for (int i=0; i<numparticles; i++) {
		vec n = mp-p[i].p;
		vec p2 = p[i].p + akseli*(akseli.dot(n)*2);

		for (int j=0; (j<numwalls && !inwall); j++) {
			inwall = p[i].detectcollision_nov(p2, &walls[j]);
		}

		if (inwall) {
			return false;
		}
	}

	for (int i=0; i<numparticles; i++) {
		vec n = mp-p[i].p;
		p[i].p += akseli*(akseli.dot(n)*2);

	}
	return true;
}

void model::particles_act(Dbl dt, wall *walls, int numwalls, Dbl t) {
// copy meaningful walls to another array and check only them.

	wall **checktable = new wall*[numwalls];
	int nearbywalls = 0;

	// make optimization table
	if (radius > 0) {
		for (int i=0; i<numwalls; i++) {
			float r = walls[i].r+radius;
			if ( (walls[i].mp - midpoint()).abs2() <= r*r+1.0 ) {
				checktable[nearbywalls++] = &walls[i];
			}
		}
	} else {
		for (int i=0; i<numwalls; i++) {
			checktable[i] = &walls[i];
		}
		nearbywalls = numwalls;
	}

	// particles act
	for (int j=0; j<numparticles; j++) {
		p[j].act(dt, checktable, nearbywalls);
	}

	delete[] checktable; // hox?

/*
	for (int i = 0; i < numparticles; i++) {
		p[i].act(dt, walls, numwalls);
	}
*/
}

void model::display() {
	if (Config.decoration != DEC_UGLY) {
		if (t) {
			GLuint tex = t[0].texID;
	
			glBindTexture(GL_TEXTURE_2D, t[0].texID);
			#ifndef HAVE_GLES
			glBegin(GL_TRIANGLES);
			#endif

			t[0].display();
		
			for (int i=1; i<numtriangles; i++) {
				if (t[i].texID != tex) {
					tex = t[i].texID;
					glBindTexture(GL_TEXTURE_2D, tex);
				}
				t[i].display();
			}

			#ifndef HAVE_GLES
			glEnd();
			#endif


		} else { // no textures
			for (int i=0; i < numsprings; i++) {
				if (s[i].alive) s[i].display(1.0, 1.0, 1.0);
		 }
		
			for (int i=0; i < numparticles; i++) {
				p[i].display(1.0, 1.0, 1.0);
			}
		}
	} else {

/*		glColor3f( ugly_r, ugly_g, ugly_b );
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLES);

		for (int i=0; i<numtriangles; i++) {
			t[i].display_solid();
		}

		glEnd();
		glEnable(GL_TEXTURE_2D);
		glColor3f( 1.0, 1.0, 1.0 );

*/

		glColor3f( ugly_r, ugly_g, ugly_b );
		glDisable(GL_TEXTURE_2D);
		#ifndef HAVE_GLES
		glBegin(GL_LINES);
		#endif

		for (int i=0; i<numtriangles; i++) {
			t[i].display_lines();
		}

		#ifndef HAVE_GLES
		glEnd();
		#endif
		glEnable(GL_TEXTURE_2D);
		glColor3f( 1.0, 1.0, 1.0 );

	}
}

vec model::midpoint() {
	vec avg(p[0].p);

	int num=1;

	for (int i=1; i<numparticles; i++) {
		if (p[i].alive) {
			avg += p[i].p;
			num++;
		}
	}

	avg/=num;

	return avg;
}

vec model::midv() {
	vec avg(p[0].v);

	int num=1;

	for (int i=1; i<numparticles; i++) {
		if (p[i].alive) {
			avg += p[i].v;
			num++;
		}
	}

	avg/=num;

	return avg;
}

void model::momentum(Dbl M, Dbl dt) { // kgm^2/s^2
	// apply momentum.  M = F*r = m*a*r [kg*m/s^2*m] => M*dt/(m*r) = dv. M = 1.0 kgm^2/s^2
	vec avg = midpoint();
	for (int i=0; i<numparticles; i++) {
		vec d = p[i].p - avg;
		vec dv(-d.y, d.x);
		dv *= M*( dt/ (d.abs()*p[i].mass) );//  M kgm^2/s^2 * s / m / kg -> m/s
		p[i].v += dv;
	}
}


void model::bendleft(Dbl dt) {
}
void model::bendright(Dbl dt) {
}

void model::stretchup(Dbl dt) {
}
void model::stretchdown(Dbl dt) {
}

void model::strive(Dbl dt) {
}
void model::duck(Dbl dt) {
}

void model::update(Dbl dt) {
}


void model::stick(bool onoff) {
}

bool model::checkgoal(vec goalr, float r) {
	for (int i=0; i<numparticles; i++) {
		if ((p[i].p-goalr).abs2() < r*r) return true;
	}
	return false;
}

void model::settime(Dbl t) {
	time=t;
}

float model::getdamage() {
	return 0.0;
}

float model::getwaterdamage() {
	return 0.0;
}

void model::updatewalls() {
}

bool model::checkwalls(vec mp) {
	return false;
}

void model::precision() {
/*	for (int i=0; i<numparticles; i++) {
		p[i].p.x = compat::scraptail(p[i].p.x);
		p[i].p.y = compat::scraptail(p[i].p.y);
		p[i].v.x = compat::scraptail(p[i].v.x);
		p[i].v.y = compat::scraptail(p[i].v.y);
	}
*/
}
