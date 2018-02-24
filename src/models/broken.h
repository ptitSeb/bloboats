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

#ifndef _BROKEN_H_
#define _BROKEN_H_

#include "../model.h"


#define BROKEN_SPLASH 2
#define BROKEN_TOUCHPOINTS 20

//const int BROKEN_splash[BROKEN_SPLASH]={12, 26};

//const int BROKEN_tp[BROKEN_TOUCHPOINTS]={0,1,2,4,6,12,15,18,19,20,24,28,27,26,25,21,11,10,9,8};



class broken : public model {
public:
	broken(model *paat);
	~broken();

	void bendleft(Dbl dt);
	void bendright(Dbl dt);

	void stretchup(Dbl dt);
	void stretchdown(Dbl dt);

	void strive(Dbl dt);

	void update(Dbl dt);


	float getdamage();
	float getwaterdamage();

	Dbl a_bend;
	Dbl a_stretch;

	Dbl a_strive;
	bool striven;

	GLuint texID;

private:
};

#endif
