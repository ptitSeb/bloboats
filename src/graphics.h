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

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <stdlib.h>

#include "SDL.h"
#include "SDL_image.h"

#ifdef HAVE_GLES
#include "SDL_opengles.h"
#include "eglport.h"
#else
#include "SDL_opengl.h"
#endif

#include "window.h"
#include "texfile.h"
#include "vec.h"

#include <list>

using std::list;

class graphics
{
	public:
		graphics();
		~graphics();

		SDL_Surface * LoadPicture(char *file);
		SDL_Surface * LoadPicture_NoExit(char *file);
		void DrawIMG(SDL_Surface *destsurface, SDL_Surface *img, int x, int y);
		void DrawPartOfIMG(SDL_Surface *destsurface, SDL_Surface *img, int x, int y, int w, int h, int x2, int y2);
		void SetTransparentColor(SDL_Surface *image, Uint8 r, Uint8 g, Uint8 b);

		void RemoveTransparentColor(SDL_Surface *image, Uint8 r, Uint8 g, Uint8 b);
		Uint32 MapRGB(SDL_Surface *image, Uint8 r, Uint8 g, Uint8 b);
		void GetRGB(Uint32 pixel, SDL_PixelFormat *fmt, Uint8 *r, Uint8 *g, Uint8 *b);

		void PutPixel(SDL_Surface *image, Uint32 c, int x, int y);
		void PutPixel(SDL_Surface *image, Uint32 c, Uint32 p, int x, int y);

		void DrawLine(float r, float g, float b, int x1, int y1, int x2, int y2);
		void DrawTextured(GLuint texID, float x1, float y1, float x2, float y2, float x3, float y3,
			float tx1, float ty1, float tx2, float ty2, float tx3, float ty3);
		void DrawTextured(float x1, float y1, float x2, float y2, float x3, float y3,
			float tx1, float ty1, float tx2, float ty2, float tx3, float ty3);
		void DrawSolid(float x1, float y1, float x2, float y2, float x3, float y3);
		void OutlineTriangle(float x1, float y1, float x2, float y2, float x3, float y3);


		void freetexture(GLuint tex);

		int projectx(Dbl x);
		int projecty(Dbl y);

		Dbl backprojectx(int x);
		Dbl backprojecty(int y);

		GLuint maketexture(char *filename);
		GLuint remaketexture(char *filename);

		void reloadtextures();

		void deletetexture(GLuint tex);

		void clear();

		std::list<texfile> surfacelist;
		
		#ifdef HAVE_GLES
		GLfloat cur_r,cur_g,cur_b, cur_a;	// current color
		void color3f(GLfloat r, GLfloat g, GLfloat b) {cur_r = r; cur_g = g; cur_b = b; cur_a = 1.0f;};
		void color4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {cur_r = r; cur_g = g; cur_b = b; cur_a = a;};
		void drawLine(int x1, int y1, int x2, int y2);
		void drawQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,
					 int tx1, int ty1, int tx2, int ty2, int tx3, int ty3, int tx4, int ty4);
		#endif

	private:
};
#ifdef HAVE_GLES
#ifndef glColor3f
#define glColor3f Graphics.color3f
#endif
#endif

#endif
