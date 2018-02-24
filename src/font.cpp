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

#include "font.h"

#include "window.h"
#include "graphics.h"
#include "compat.h"
#include "path.h"

extern graphics Graphics;
extern path *Path;

#include "compiling_settings.h"

font::font() {
	type=-1;
}

font::~font() {
//	fprintf (stderr, "font (%d): %d\n", type, fontpic);
}

void font::Initialize(int fonttype) {
	int i=0;

#if DEBUG == 1
	fprintf (stderr, "Inititializing font as: %d\n", fonttype);
#endif

	switch (fonttype) {
		case FONT_BIG:
			texID = Graphics.maketexture( Path->data("fonts/font.png") );
		break;
		case FONT_BL:
			texID = Graphics.maketexture( Path->data("fonts/font2.png") );
		break;

		default:
			fprintf (stderr, "No such font in font::Initialize()!\n");
			exit(0);
		break;
	}

	type=fonttype;
}

void font::Deinitialize(int fonttype) {
	Graphics.deletetexture(texID);
}

void font::WriteString(char *str, float cx, float cy, float size) {

	glBindTexture(GL_TEXTURE_2D, texID);

	float x = cx;
	float y = cy;

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	#ifndef HAVE_GLES
	glBegin(GL_QUADS);
	#endif

	for (int i=0; i<strlen(str); i++) {
//		if (str[i]==' ') cx+=spacewidth;
//		else {
			for (int j=0; j<big_num; j++) {
				if (str[i]==big_ch[j]) {
					int tx = j%8;
					int ty = j/8;

					#ifdef HAVE_GLES
					GLfloat vtx1[] = {
						x, y,
						x+size, y,
						x+size, y+size,
						x, y+size
					};
					GLfloat tex1[] = {
						tx/8.0, ty/8.0,
						tx/8.0+1.0/8.0, ty/8.0,
						tx/8.0+1.0/8.0, ty/8.0+1.0/8.0,
						tx/8.0, ty/8.0+1.0/8.0
					};
				 
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				 
					glVertexPointer(2, GL_FLOAT, 0, vtx1);
					glTexCoordPointer(2, GL_FLOAT, 0, tex1);
					glDrawArrays(GL_TRIANGLE_FAN,0,4);
				 
					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					#else
					glTexCoord2f(tx/8.0, ty/8.0);
					glVertex2f(x, y);

					glTexCoord2f(tx/8.0+1.0/8.0, ty/8.0);
					glVertex2f(x+size, y);

					glTexCoord2f(tx/8.0+1.0/8.0, ty/8.0+1.0/8.0);
					glVertex2f(x+size, y+size);

					glTexCoord2f(tx/8.0, ty/8.0+1.0/8.0);
					glVertex2f(x, y+size);
					#endif

					break;
				}
			}
//		}
		x+=size;
	}

	#ifndef HAVE_GLES
	glEnd();
	#endif

}

void font::WriteChar(char chr, float cx, float cy, float size) {
	glBindTexture(GL_TEXTURE_2D, texID);

	if (chr >= 'a' && chr <= 'z') chr += 'A'-'a';

	float x = cx;
	float y = cy;

	if (chr == '_' || chr == ' ') return;


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	#ifndef HAVE_GLES
	glBegin(GL_QUADS);
	#endif

	bool found=false;
	int j=0;

	for (j=0; j<big_num; j++) {
		if (chr==big_ch[j]) {
			found=true;
			break;
		}
	}
	if (!found) j = big_num-1;

	int tx = j%8;
	int ty = j/8;
	
	#ifdef HAVE_GLES
	GLfloat vtx1[] = {
		x, y,
		x+size, y,
		x+size, y+size,
		x, y+size
	};
	GLfloat tex1[] = {
		tx/8.0, ty/8.0,
		tx/8.0+1.0/8.0, ty/8.0,
		tx/8.0+1.0/8.0, ty/8.0+1.0/8.0,
		tx/8.0, ty/8.0+1.0/8.0
	};
 
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 
	glVertexPointer(2, GL_FLOAT, 0, vtx1);
	glTexCoordPointer(2, GL_FLOAT, 0, tex1);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
 
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	#else
	glTexCoord2f(tx/8.0, ty/8.0);
	glVertex2f(x, y);

	glTexCoord2f(tx/8.0+1.0/8.0, ty/8.0);
	glVertex2f(x+size, y);

	glTexCoord2f(tx/8.0+1.0/8.0, ty/8.0+1.0/8.0);
	glVertex2f(x+size, y+size);

	glTexCoord2f(tx/8.0, ty/8.0+1.0/8.0);
	glVertex2f(x, y+size);



	glEnd();
	#endif

}
