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

#ifndef _FONT_H_
#define _FONT_H_

#include <string.h>

#include "SDL.h"
#ifdef HAVE_GLES
#include "SDL_opengles.h"
#else
#include "SDL_opengl.h"
#endif

#define FONT_NORMAL		0x000000
#define FONT_XCENTERED	0x000010
#define FONT_YCENTERED	0x000100
#define FONT_CENTERED	0x000110
#define FONT_RIGHTENED	0x001000

#define FONT_BIG 0
#define FONT_BL 1

const char big_ch[]="\"'(),-./0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ`";
const int big_num = strlen(big_ch);


class font {
	public:
		font();
		~font();


		void Initialize(int fonttype);
		void Deinitialize(int fonttype);
		void WriteString(char *str, float cx, float cy, float size);
		void WriteChar(char str, float cx, float cy, float size);

	private:

		int type;
		GLuint texID;

};

#endif
