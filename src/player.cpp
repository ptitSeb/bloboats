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

#include "player.h"

player::player() {

	tuxfound = 0;

	// reset high scores

	for (int i=0; i<MAXLEVELS; i++) {
		for (int j=0; j<10; j++) {
			times[i][j]=0.0;
		}
	}

	inuse=false;

	name=0;

}

player::~player() {
	delete[] name;
}

bool player::setnick(char *nick) {
	name = new char[strlen(nick)+1];
	strcpy(name, nick);
	return true;
}
