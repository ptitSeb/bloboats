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

#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include "SDL.h"
#include "compat.h"

class element {
public:
	element();

	// getting controls:

	element* add(Uint8 newc);
	element* get(Uint8 *c);

	// file saving and loading:

	void set(Uint16 p);
	void save(FILE *fp);


	Uint8 controls;
	Uint16 duration;

	element *next;
};


#endif
