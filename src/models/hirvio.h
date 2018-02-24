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

#ifndef _HIRVIO_H_
#define _HIRVIO_H_

#include "../model.h"
#include "../wall.h"

#define HIRVIO_SPLASH 2

const int hirvio_splash[HIRVIO_SPLASH]={12, 26};
//const int hirvio_bendr[HIRVIO_BENDR]={};
//const int hirvio_update[HIRVIO_UPDATE]={};

//const int hirvio_strive[HIRVIO_STRIVE]={};
//const int hirvio_stretch[HIRVIO_STRETCH]={
//22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};


class hirvio : public model {
public:
	hirvio(Dbl cx, Dbl cy, Dbl angle, Dbl scale, char *tex);
	~hirvio();
	
	void bendleft(Dbl dt);
	void bendright(Dbl dt);
	void nobend(Dbl dt);

	void stretchup(Dbl dt);
	void stretchdown(Dbl dt);
	void nostretch(Dbl dt);

	void strive(Dbl dt);

	void update(Dbl dt);


	Dbl gethardness();

	void changehardness_kbd(Dbl dt);
	void changehardness_mouse(int dy);

	float getdamage();
	float getwaterdamage();

	void updatewalls();
	bool checkwalls(vec mp);


	Dbl a_bend;
	Dbl a_stretch;

	Dbl a_strive;
	bool striven;

	wall walls[15];


private:

};

#endif
