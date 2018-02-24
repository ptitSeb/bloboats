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

#ifndef _VEC_H_
#define _VEC_H_

#ifdef PANDORA
#define Dbl float
#else
#define Dbl double
#endif

#include <math.h>

class vec {
public:
	vec();
	vec(Dbl x, Dbl y);
	vec(const vec &b);
	Dbl abscross(vec &b);
	Dbl cross(vec &b);
	Dbl dot(vec &b);
	Dbl abs();
	Dbl abs2();

	void display();

	void rotate(Dbl cosx, Dbl sinx);

	vec &operator=(const vec& b);

	vec operator+(const vec& b);
	vec &operator+=(const vec& b);

	vec operator-(const vec& b);
	vec &operator-=(const vec& b);

	vec operator*(Dbl b);
	vec &operator*=(Dbl b);

	vec operator/(Dbl b);
	vec &operator/=(Dbl b);

	Dbl x, y;

private:


};


#endif
