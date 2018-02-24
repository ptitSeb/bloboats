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

#include "media.h"

#include "config.h"

#include "path.h"

extern path *Path;


media::media() {
	menuselect=0;
	menuenter=0;
	splash=0;
	clonk=0;
	finish=0;
	vroom=0;
	roarr=0;
	bump=0;

	for (int i=0; i<NUM_MUSIC; i++) music[i] = 0;
}

media::~media() {
//	FreeStuff();
}

void media::FreeStuff() {
   if (vroom!=0) sound::FreeChunk(vroom);
   if (finish!=0) sound::FreeChunk(finish);
   if (splash!=0) sound::FreeChunk(splash);
   if (menuselect!=0) sound::FreeChunk(menuselect);
   if (menuenter!=0) sound::FreeChunk(menuenter);
	if (clonk!=0) sound::FreeChunk(clonk);
	if (roarr!=0) sound::FreeChunk(roarr);
	if (explosion!=0) sound::FreeChunk(explosion);
	if (bump!=0) sound::FreeChunk(bump);

	for (int i=0; i<NUM_MUSIC; i++) {
		if (music[i]) sound::FreeMusic( music[i] );
	}

}

void media::LoadStuff() {

	vroom = sound::LoadWAV( Path->data("sounds/vroom.wav") );
	finish = sound::LoadWAV( Path->data("sounds/finish.wav") );
	splash = sound::LoadWAV( Path->data("sounds/splash.wav") );
	menuselect = sound::LoadWAV( Path->data("sounds/menuselect.wav") );
	menuenter = sound::LoadWAV( Path->data("sounds/menuenter.wav") );
	clonk = sound::LoadWAV( Path->data("sounds/clonk.wav") );
	roarr = sound::LoadWAV( Path->data("sounds/roarr.wav") );
	explosion = sound::LoadWAV( Path->data("sounds/explosion.wav") );
	bump = sound::LoadWAV( Path->data("sounds/bump.wav") );

#if MUSIC == 1
	music[0] = sound::LoadMusic( Path->data("sounds/music0.ogg") );
	music[1] = sound::LoadMusic( Path->data("sounds/music1.ogg") );
	music[2] = sound::LoadMusic( Path->data("sounds/music2.ogg") );
	music[3] = sound::LoadMusic( Path->data("sounds/music3.ogg") );
#else
	music[0] = 0;
	music[1] = 0;
	music[2] = 0;
	music[3] = 0;
#endif


}

