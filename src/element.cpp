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

#include "element.h"

element::element() {
	controls = 255;
	duration = 0;
	next = 0;
}

element* element::add(Uint8 newc) {
/* If controls have changed, make a new element, otherwise increase duration.
   If duration goes to 2^10-1, make a new element instead because 2^10-1 is the maximum.
*/

	if (newc != controls) {
		this->next = new element();
		this->next->controls = newc; // 0011 1111 = 0x3F
		return this->next;
	} else {
		if (duration == 0x03FF) { // 0000 0011 1111 1111
			this->next = new element();
			this->next->controls = controls;
			return this->next;
		} else {
			this->duration++;
			return this;
		}
	}

}

element* element::get(Uint8 *ghostdat) {
	*ghostdat = controls;

	if (duration == 0) return next;


	duration--;
	return this;
}

// file saving and loading functions

void element::set(Uint16 p) {
	p = SWAP16(p); // CCCCCCTT TTTTTTTT
	controls = p>>10;
	duration = p&0x03FF; // 0000 0011 1111 1111

}

void element::save(FILE *fp) {
	Uint16 p = (controls<<10) | SWAP16(duration);
	compat::ToFile_ShortInt(fp, p);
}
