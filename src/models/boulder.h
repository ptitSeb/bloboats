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

#ifndef _BOULDER_H_
#define _BOULDER_H_

#include "../model.h"
#include "../wall.h"

#define BOULDER_SPLASH 2

//const int BOULDER_splash[BOULDER_SPLASH]={12, 26};

class boulder : public model {
public:
	boulder(Dbl cx, Dbl cy, Dbl angle, Dbl scale, char *tex);
	~boulder();

	void bendleft(Dbl dt);
	void bendright(Dbl dt);

	void nobend(Dbl dt);

	void stretchup(Dbl dt);
	void stretchdown(Dbl dt);
	void nostretch(Dbl dt);

	void strive(Dbl dt);

	void update(Dbl dt);

	float getdamage();
	float getwaterdamage();

	void updatewalls();
	bool checkwalls(vec mp);

	void display();

	void springs_act(Dbl dt);
	void particles_act(Dbl dt, wall *walls, int numwalls, Dbl t);



	Dbl a_bend;
	Dbl a_stretch;

	Dbl a_strive;
	bool striven;

	int alive;

	wall walls[5];

	bool collided;

private:

};

#endif
