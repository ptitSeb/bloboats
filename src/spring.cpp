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

#include "spring.h"

extern graphics Graphics;

spring::spring():alive(false),p1(0),p2(0){
}
spring::spring(particle *p1, particle *p2, Dbl nlen, Dbl maxdiff, Dbl a, Dbl damp): alive(true), p1(p1),p2(p2),nlen(nlen),
maxdiff(maxdiff),a(a),origlen(nlen),damp(damp) {

	if (nlen<=0.0001) {
		fprintf (stderr, "Trying to create a zero-length spring. Critical!\n");
		exit(1);
	}

	edamp = exp(damp*0.005)/2;
}

void spring::act(Dbl dt) {

	// F = -kx
	// a = -kx/m
	// delta_v = -kx/m * dt

	vec P12 = (p2->p)-(p1->p);

	Dbl len2 = P12.abs2();

	// calculate the effects of the spring
	vec vd = P12*a*dt*(sqrt(len2)-nlen);

	p1->v += vd;
	p2->v -= vd;


	// kill some of the particles' speed towards each other

	if ( len2 != 0) {

		// give some of the momentum to 'em both.

		vec vel12 = (p1->v - p2->v );

		// velocity diff projected to p12 and scaled with damping factor

		vec d = P12 * (edamp*vel12.dot(P12) / len2);

		p1->v -= d;
		p2->v += d;

	}


}

void spring::display(float r, float g, float b) {
   int x1 = Graphics.projectx(p1->p.x);
   int y1 = Graphics.projecty(p1->p.y);
   int x2 = Graphics.projectx(p2->p.x);
   int y2 = Graphics.projecty(p2->p.y);

   Graphics.DrawLine(r, g, b, x1, y1, x2, y2);
}

void spring::normlen_reset() {
	nlen = origlen;
}

void spring::normlen_modify(Dbl k) {
	nlen += origlen*k;
}

