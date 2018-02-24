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

#include "vec.h"

#include <stdio.h>

vec::vec():x(0),y(0){
}

vec::vec(Dbl x, Dbl y):x(x),y(y){
}

vec::vec(const vec &b) {
	x=b.x;
	y=b.y;
}

Dbl vec::abscross(vec& b) {
	return fabs(x*b.y-y*b.x);
}
Dbl vec::cross(vec& b) {
	return x*b.y-y*b.x;
}

Dbl vec::dot(vec &b) {
	return x*b.x+y*b.y;
}

Dbl vec::abs() {
	return sqrt(x*x+y*y);
}

Dbl vec::abs2() {
	return x*x+y*y;
}

void vec::rotate(Dbl cosx, Dbl sinx) {
	Dbl nx = x*cosx - y*sinx;
	y = x*sinx + y*cosx;
	x = nx;
}


void vec::display() {
	printf ("Vector at %s: x:%f y:%f\n", this, x, y);
}



vec &vec::operator=(const vec& b) {
	x=b.x;
	y=b.y;
	return *this;
}

vec vec::operator+(const vec& b) {
	vec t;
	t.x = x+b.x;
	t.y = y+b.y;
	return t;
}
vec &vec::operator+=(const vec& b) {
	x+=b.x;
	y+=b.y;
	return *this;
}

vec vec::operator-(const vec& b) {
	vec t;
	t.x = x-b.x;
	t.y = y-b.y;
	return t;
}
vec &vec::operator-=(const vec& b) {
	x-=b.x;
	y-=b.y;
	return *this;
}

vec vec::operator*(Dbl b) {
	vec c;
	c.x=x*b;
	c.y=y*b;
	return c;
}
vec &vec::operator*=(Dbl b) {
	x*=b;
	y*=b;
	return *this;
}


vec vec::operator/(Dbl b) {
	if (b==0) {
		printf ("Division by zero! Mayday!\n");
		return *this;
	} else {
		vec c;
		c.x=x/b;
		c.y=y/b;
		return c;
	}
}
vec &vec::operator/=(Dbl b) {
	if (b==0) {
		printf ("Division by zero! Mayday!\n");
		return *this;
	} else {
		x/=b;
		y/=b;
		return *this;
	}
}

