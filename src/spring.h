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

#ifndef _SPRING_H_
#define _SPRING_H_

#include "graphics.h"

#include "vec.h"
#include "particle.h"


class spring {
public:

	spring();
	spring(particle *p1, particle *p2, Dbl nlen, Dbl maxdiff, Dbl a, Dbl damp);

	void act(Dbl dt);

	void display(float r, float g, float b);

	void normlen_reset();
	void normlen_modify(Dbl k);


	bool alive; // if false, we don't operate anymore

	particle *p1;
	particle *p2;
	Dbl nlen; // normal length
	Dbl maxdiff; // spring breaks if len>nlen*maxdiff or len<nlen/maxdiff;
	Dbl a; // spring constant

	Dbl origlen; // original normal length used to alter nlen.
	Dbl damp;

	Dbl edamp;

};


#endif
