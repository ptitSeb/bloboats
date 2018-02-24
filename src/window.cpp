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

#include "window.h"

window::window(){
	oldflags=0;
	width=0;
	height=0;
	centerx=0;
	centery=0;
	showx=1;
	showy=1;
	screen=0;
	xlast=width-1;
	ylast=width-1;
}

window::~window(){}



SDL_Surface * window::OpenWindow(int w, int h, int b, int flags) {
	if (!flags) {
		flags = oldflags^SDL_FULLSCREEN;
	}

	oldflags = flags;

	width=w;
	xlast=width-1;

	height=h;
	ylast=height-1;

	bpp=b;


	screen = SDL_SetVideoMode(width, height, bpp, flags);
	#ifdef HAVE_GLES
	EGL_Open(width, height);
	#endif
	SDL_EventState(SDL_VIDEOEXPOSE, SDL_ENABLE);

	// Set viewport
	glViewport(0, 0, width, height);

	// set projection
//	float ratio = 1.0f * width / height;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	// Set the correct perspective.
	#if defined(HAVE_GLES)
	glOrthof(0, width, 0, height, -1, 1);
	#else
	gluOrtho2D(0, width, 0, height);
	#endif
	glMatrixMode(GL_MODELVIEW);

	glEnable (GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);

//	glEnable(GL_COLOR_MATERIAL);

	return screen;
}

void window::ToggleFullscreen() {
	if (! OpenWindow(width, height, bpp, 0) ) {
		fprintf (stderr, "Couldn't change screen mode: %s\n", SDL_GetError() );
	}
	SDL_Delay(1500);
}

void window::SetTitle(char *text, char *icon) {
	SDL_WM_SetCaption (text, icon);
}

bool window::Iconify() {
	return SDL_WM_IconifyWindow();
}

void window::Center(Dbl cx, Dbl cy) {
	centerx = cx;
	centery = cy;
}

void window::Viewarea(Dbl x, Dbl y) {
	showx = x;
	showy = y;

	r2 = (x*x+y*y)/4;
	r = sqrt(r2);
}


