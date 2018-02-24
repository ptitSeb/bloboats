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

#ifndef _MEDIA_H_
#define _MEDIA_H_

#include "SDL.h"

#include "sound.h"

const int NUM_MUSIC = 4;

class media {
public:
	media();
	~media();

	void FreeStuff();	
	void LoadStuff();
	
#if SOUND == 1
	Mix_Chunk *menuselect;
	Mix_Chunk *menuenter;
	Mix_Chunk *splash;
	Mix_Chunk *clonk;
	Mix_Chunk *finish;
	Mix_Chunk *vroom;
	Mix_Chunk *roarr;
	Mix_Chunk *explosion;
	Mix_Chunk *bump;

	Mix_Music *music[NUM_MUSIC];


#else
	int *menuselect;
	int *menuenter;
	int *splash;
	int *clonk;
	int *finish;
	int *vroom;
	int *roarr;
	int *explosion;
	int *bump;

	int *music[NUM_MUSIC];

#endif

};



#endif
