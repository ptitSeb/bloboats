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

#include "water.h"

#include <math.h>


float water::w(float x) {
	return sin(0.75*t + 0.05*x)*wt;
}


float water::dw(float x) {
	return cos(0.75*t + 0.05*x)*dt;
}

void water::set_t(float new_t) {
	t = new_t;
	wt = cos(0.07*t)*6;
	dt = wt*0.05;
}

