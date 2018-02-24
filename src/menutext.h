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

#ifndef _MENUTEXT_H_
#define _MENUTEXT_H_

#define EPSILON 0.001

#include <string>
#include <iostream>

using std::string;

const float pipertwo = 1.570796326;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "SDL.h"

#include "window.h"
#include "graphics.h"
#include "font.h"

class menutext {
public:
	menutext(char *txt);
	menutext(string txt);
	menutext();
	~menutext();

	void settext(char *txt);

	void display(float y);
	void act();

	float sel;
	float target;

	string text;

	float fade(float s);
	float wavex(float x);
	float wavey(float x);
	float wavez(float x);

	float movex;

};


#endif
