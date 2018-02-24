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

#ifndef _CANVAS_H_
#define _CANVAS_H_


#include "../model.h"


#include <string>
using std::string;

#define CANVAS_BENDL 0
#define CANVAS_BENDR 0
#define CANVAS_STRETCH 0
#define CANVAS_UPDATE 0
#define CANVAS_STRIVE 0

//const int canvas_bendl[CANVAS_BENDL]={};
//const int canvas_bendr[CANVAS_BENDR]={};
//const int canvas_update[CANVAS_UPDATE]={};

//const int canvas_strive[CANVAS_STRIVE]={};
//const int canvas_stretch[CANVAS_STRETCH]={
//22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};


class canvas : public model {
public:
	canvas();
	canvas(vec center, float w, float h, int n, int m, char *tex);
	~canvas();

	void bendleft(Dbl dt);
	void bendright(Dbl dt);
	void nobend(Dbl dt);

	void stretchup(Dbl dt);
	void stretchdown(Dbl dt);
	void nostretch(Dbl dt);

	void strive(Dbl dt);

	void update(Dbl dt);

	void reloadtexture();
	void unloadtexture();

	Dbl a_bend;
	Dbl a_stretch;

	Dbl a_strive;
	bool striven;

	int n;
	int m;

	string texpath;

private:
};

#endif
