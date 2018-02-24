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

#include "menutext.h"

extern window Window;
extern graphics Graphics;
extern font Font;

menutext::menutext(char *txt) {
	if (strlen(txt) >= 100) {
		fprintf (stderr, "menutext::menutext: strlen(txt) too big!\n");
		exit(1);
	}

 	text = string(txt);

	sel=0.0;
	target=0.0;
	movex=0.0;
}

menutext::menutext(string txt) {
	if (txt.length() >= 100) {
		fprintf (stderr, "menutext::menutext: txt.length() too big!\n");
		exit(1);
	}

 	text = txt;

	sel=0.0;
	target=0.0;
	movex=0.0;
}

menutext::menutext() {
	sel=0.0;
	target=0.0;
	movex=0.0;
}

void menutext::settext(char *txt) {
	if (strlen(txt) >= 100) {
		fprintf (stderr, "menutext::menutext: strlen(txt) too big!\n");
		exit(1);
	}

 	text = string(txt);
}


menutext::~menutext() {
}

void menutext::act() {
	if (sel<target) sel+=0.02;
	if (sel>target) sel-=0.02;
}

void menutext::display(float y) {

	float f = fade(sel);
	float size = 28+14*f*f;

	int len = text.length();
	int b = 0;

	float x;
	if (text[0]=='*') {
		size+=15;
		x = Window.width/2 - size*(len-2)/2;
		b = 1;
		y-=15;
	} else {
		x = Window.width/2 - size*(len-1)/2;
	}

	for (int j=b; j<len; j++) {

		float cx = x+f*wavex(j-b)-size/2+movex;
		float cy = y+f*wavey(j-b)-size/2;
		
		Font.WriteChar(text[j], cx, cy, size+f*wavez(j-b) );
		x+=size;
	}

}

float menutext::fade(float s) {
	return cos(pipertwo*(1-s));
}

float menutext::wavex(float x) {
	return sin(SDL_GetTicks()/300.0 - 40*x/3)*3;
}
float menutext::wavey(float x) {
	return cos(SDL_GetTicks()/300.0 - 40*x/3)*7;
}
float menutext::wavez(float x) {
	return cos(SDL_GetTicks()/800.0 - 40*x/3)*5;
}

