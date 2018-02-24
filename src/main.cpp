/*
    Bloboats - a boat racing game by Blobtrox
    Copyright (C) 2006, 2010  Markus "MakeGho" Kettunen

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

#include "SDL.h"
#ifdef HAVE_GLES
#include "SDL_opengles.h"
#else
#include "SDL_opengl.h"
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __WIN32__
#include <windows.h>
#endif

#include "keyboard.h"
#include "window.h"
#include "graphics.h"
#include "mouse.h"
#include "font.h"

#include "menu.h"
#include "config.h"
#include "water.h"
#include "sound.h"
#include "path.h"
#include "media.h"


mouse Mouse;
graphics Graphics;
window Window;
keyboard Keyboard;
config Config;
path *Path;
media Media;
water Water;


// this system uses metric system, positive y means upwards, positive x means right


font Font;
font Font_bl; // no outline

#ifdef HAVE_GLES
Uint32 flags = SDL_SWSURFACE | SDL_FULLSCREEN;

void CleanShutDown()
{
	EGL_Close();
	SDL_Quit();
}
#else
Uint32 flags = SDL_OPENGL;
#endif

int main(int argc, char *argv[]) {

	printf("Bloboats version 1.0.2, Copyright (C) 2006, 2010 Markus Kettunen\n");
	printf("Bloboats comes with ABSOLUTELY NO WARRANTY; for details check\n");
	printf("file \"copying.txt\".\n");
	printf("This is free software, and you are welcome to redistribute it\n");
	printf("under certain conditions; check file \"copying.txt\" for details.\n");
	printf("\n");

	if (SDL_Init(SDL_INIT_VIDEO)==-1) {
		fprintf (stderr, "Couldn't init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	#ifdef HAVE_GLES
	atexit(CleanShutDown);
	#else
	atexit(SDL_Quit);
	#endif

/*	// On Windows, chdir to binary dir
#ifdef __WIN32__
	char binbuf[200];
	if (!GetModuleFileName( NULL, binbuf, 200 ) ) {
		fprintf (stderr, "GetModuleFileName failed (%d). \n", (int)GetLastError() );
	}
	// cut until get the last '\' :-P
	for (int i = strlen(binbuf)-1; i>=0; i--) {
		if (binbuf[i]=='\\' || binbuf[i]=='/') break;
		binbuf[i] = '\0';
	}
	chdir(binbuf);
#endif
*/


	// config directories
	Path = new path();


	Config.load();

	char *replay = 0;

	int reswidth=0, resheight=0;
	bool manualres=false;

	if (argc >= 2) {
		for (int i=1; i<argc; i++) {
			if (strcmp(argv[i], "--nosound")==0) {
				Config.sounds=0;
			} else
			if (strcmp(argv[i], "--sound")==0) {
				Config.sounds=1;
			} else
			if (strcmp(argv[i], "--fullscreen")==0 || strcmp(argv[i], "-f")==0) {
				Config.fullscreen = 1;
			} else
			if (strcmp(argv[i], "--windowed")==0 || strcmp(argv[i], "-w")==0) {
				Config.fullscreen = 0;
			} else
			if (strcmp(argv[i], "--nolimit")==0) {
				Config.nolimit=true;
			} else
			if (strncmp(argv[i], "--resolution=", 13)==0) {
				if ( sscanf(argv[i], "--resolution=%dx%d", &reswidth, &resheight)
					!= 2 )
				{
					printf ("Resolution flag takes two integer parameters: --resolutiuon=NxM\n");
					printf ("Example: --resolution=1280x800\n");
					exit(1);
				}
				if (reswidth <= 0 || resheight <= 0)
				{
					printf ("Incorrect resolution.\n");
					exit(1);
				}
				manualres=true;

			} else
			if (strcmp(argv[i], "--help")==0 || strcmp(argv[i], "-h")==0 ||
				strcmp(argv[i], "-?")==0 || strcmp(argv[i], "?")==0 ||
				strcmp(argv[i], "/?")==0 || strcmp(argv[i], "/h")==0)
			{
				printf("Bloboats\n");
				printf("By Markus Kettunen <makegho@blobtrox.net>\n");
				printf("\n");
				printf("See file \"copying.txt\" for license.\n");
				printf("\n");
				printf("Possible flags:\n");
				printf("  --nosound disables sounds\n");
				printf("  --sound enables sounds\n");
				printf("  --windowed or -w enables windowed mode\n");
				printf("  --fullscreen or -f enables fullscreen mode\n");
				printf("  --resolution=NxM changes resolution to NxM\n");
				printf("  --nolimit disables some speed limitations\n");
				printf("  --help prints this help\n");

				return 0;
			} else {
				// Test existence
				FILE *fp = fopen(argv[i], "rb");
				if (!fp) {
					fprintf (stderr, "Couldn't open file \"%s\". Critical.\n", argv[i]);
					exit(1);
				}
				fclose(fp);

				delete[] replay;
				replay = new char[strlen(argv[i])+1];
				strcpy(replay, argv[i]);

			}
		}
	}

	// Start sound system
#if SOUND == 1
	if (Config.sounds) {
		sound::Start();
		Media.LoadStuff();
		sound::SetChannels(16);
	}
#endif


	if (Config.fullscreen) flags |= SDL_FULLSCREEN;

	SDL_Surface *s;

	#if defined( PANDORA ) && defined( HAVE_GLES )
	reswidth=800; resheight=480;
	s = Window.OpenWindow(800, 480, 32, SDL_SWSURFACE|SDL_FULLSCREEN);
	#else
	if (!manualres) {
	    s = Window.OpenWindow(Config.reswidth, Config.resheight, 32, flags);
	} else {
	    s = Window.OpenWindow(reswidth, resheight, 32, flags);
	}
	#endif

	if (!s) {
		fprintf (stderr, "Couldn't set screen mode: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_Surface *icon = Graphics.LoadPicture( Path->data("images/icon.png") );
	SDL_WM_SetIcon(icon, NULL);


	#ifndef HAVE_GLES
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	int doublebuffer;
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doublebuffer);
	if (doublebuffer != 1) {
		fprintf (stderr, "No double buffer support!\n");
		exit(1);
	}
	#endif


	// done graphics initialization

	SDL_EnableUNICODE(1);

	// fonts
	Font.Initialize(FONT_BIG);
	Font_bl.Initialize(FONT_BL);

	Keyboard.ResetAll();

	Window.SetTitle("Bloboats", "Bloboats");

	if (replay) { // replay
		menu Menu;
		Menu.loadtimes_public(); // parses level names
		Menu.simulate(0, replay);
	} else {
		menu Menu;
		Menu.mainmenu();
		Config.save();
	}

#if SOUND == 1
	if (Config.sounds) {
		Media.FreeStuff();
		sound::Stop();
	}
#endif

	SDL_FreeSurface (icon);

	delete Path;
	delete[] replay;

}
