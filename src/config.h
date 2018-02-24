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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "compat.h"

#include "SDL.h"

enum KEY { KEY_THRUST, KEY_REVERSE, KEY_CCW, KEY_CW, KEY_JUMP, KEY_TURN, KEY_ZOOMIN, KEY_ZOOMOUT, KEY_PAUSE };
//#define KEY_THRUST 0
//#define KEY_REVERSE 1
//#define KEY_CCW 2
//#define KEY_CW 3
//#define KEY_JUMP 4
//#define KEY_TURN 5
//#define KEY_ZOOMIN 6
//#define KEY_ZOOMOUT 7

const Uint32 ALL_TUX = (1<<3)|(1<<7)|(1<<11)|(1<<13)|(1<<16)|(1<<17);

const int DEC_FULL = 2;
const int DEC_LOW = 1;
const int DEC_UGLY = 0;

class config {
public:
	config();
	void load();
	void save();

	unsigned int reswidth;
	unsigned int resheight;

	int decoration;
	bool showghost;

	int currentplayer;
	bool sounds;
	int music;

	Uint16 keys[9];

	float zoomlevel;
	Uint32 newtimes;

	Uint32 tux;

	int soundvol;
	int musicvol;

	bool fullscreen;

	int fps;

	bool nolimit;

};



#endif
