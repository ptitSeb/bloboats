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

#include "menu.h"
#include "sound.h"
#include "media.h"
#include "path.h"
#include "menutext.h"
#include "compat.h"
#include "handler.h"
#include "condition.h"
#include "models/canvas.h"

#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "compiling_settings.h"

#include <iostream>
#include <vector>
#include <string>

#define RECORDVERSION 4

using std::vector;

extern path *Path;
extern media Media;

Uint32 menu_level;

menu::menu() {
	blobcanvas=0;
	cheatmode=false;
}

menu::~menu() {
	delete blobcanvas;
}

void menu::mainmenu() {
	// Play music and set volumes
	if (Config.music < 0 || Config.music >= NUM_MUSIC) Config.music = 0;
	sound::FadeInMusic_select(Media.music[Config.music], -1, 500);
	sound::FadeInMusic();
	sound::MusicVolume( (int)(12.8*Config.musicvol) );
	sound::SoundVolume( (int)(12.8*Config.soundvol) );


	// hide cursor
	Mouse.ShowCursor(false);

	// background color, though only visible when in no-decoration mode
	glClearColor(0.225, 0.225, 0.625, 1.00);

	// bloboats canvas

	blobcanvas = new canvas( vec(-10.0, 25.0), 30.0, 30.0*0.43, 15, 15, "images/bloboats.png" );

	message=0;
	lastrecord=false;

	// Initialize: players

	// check for private directory

	DIR *testdir = opendir( Path->privdir );
	if (testdir) {
		closedir (testdir); // exists and is readable;
	} else {
		// doesn't exist - let's create it
#ifdef WIN32
		if (mkdir( Path->privdir ) != 0 ) {
#else
		if (mkdir( Path->privdir, 00775 ) != 0 ) {
#endif

			fprintf (stderr, "Couldn't create directory \"%s\". Critical.\n", Path->privdir);
			exit(1);
		}
		if (! Path->copydir( Path->data("defaults/private"), Path->privdir, 0) ) {
			fprintf (stderr, "Can't copy defaults to private directory.\n");
			fprintf (stderr, "Check \"%s\".\n", DEFAULT_CONFIGFILE);
			exit(1);
		}

	}

#if DEBUG == 1
	fprintf (stderr, "Loading player data.\n");
#endif

	FILE *fp = fopen( Path->priv("players.dat") , "r");
	if (fp) {

		for (int i=0; i<6; i++) {
			char nick[200];
			char str[200];
			Uint32 tuxfound=0;
			bool used=false;

			if (fgets(nick, NICK_MAXLEN+2, fp)) {
				if (nick && strlen(nick)>1 ) {
					nick[strlen(nick)-1]='\0';
					used=true;
					Player[i].setnick(nick);
					Player[i].inuse=true;

#if DEBUG == 1
					fprintf (stderr, "nick: \"%s\"\n", nick);
#endif
				} else {
					Player[i].setnick("");

					if (Config.currentplayer == i) Config.currentplayer=-1;
				}

			} else {
				Player[i].setnick("");

#if DEBUG == 1
				fprintf (stderr, "i=%d, curplr=%d\n", i, Config.currentplayer);
#endif
				if (Config.currentplayer == i) Config.currentplayer=-1;
			}

			if (fgets(str, 200, fp) ) {
				if (sscanf(str, "%d", &tuxfound)==1) {
					if (used) Player[i].tuxfound = SWAP32(tuxfound);
					else Player[i].tuxfound = 0;
				}
			}

			for (int lev=0; lev<MAXLEVELS; lev++) {
				for (int num=0; num<10; num++) {
					float f=0.0;
					if (fgets(str, 200, fp)) {
						if (sscanf(str, "%f\n", &f)==1) {
							if (used) Player[i].times[lev][num]=f;
							else Player[i].times[lev][num]=0.0;
						}

					}
				}
			}
		}

		fclose(fp);

	} else {
		Config.currentplayer = -1;
	}


	// initialize: levels

	loadtimes_public();

	// done highscores


	// Load rank times

	for (int i=0; i<levelcount; i++) {
		char levpath[1200];
		sprintf (levpath, "%s/levels/meri%d.hsc", Path->datadir, i);
		FILE *fp = fopen(levpath, "r");

		if (fp) {
			if (fscanf(fp, "%f %f", &rank_times[i][0], &rank_times[i][1]) != 2) {
				fprintf (stderr, "Couldn't get rank times from file \"%s\"\n", levpath);
				rank_times[i][0]=0.0;
				rank_times[i][1]=0.0;
			}
			fclose(fp);
		} else {
			fprintf (stderr, "Couldn't open file \"%s\".\n", levpath);
			rank_times[i][0]=0.0;
			rank_times[i][1]=0.0;
		}
	}


	// End of Load rank times

	// End of initialization


	menutext item[6]={ menutext("PLAY GAME"), menutext("SELECT PLAYER"), menutext("VIEW RECORDS"), menutext("SCOREBOARD"),
								menutext("OPTIONS"),menutext("QUIT") };

	int selection=0;

	for (int i=0; i<6; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime;

	bool ready=false;

	canvt=0;
	canvalpha=0.0;

	focuscanvas();

	bool drawn=false;

// Start of menu code
	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() >= nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();



			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 0) selection += 6;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection > 5) selection -= 6;
			}

			for (int i=0; i<6; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=0; i<6; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				switch(selection) {
					case 0: // play game
						if (Config.currentplayer<0) {
							if (selectplayer() && Config.currentplayer>=0 ) playgame();
						} else {
							playgame();
						}

						for (int i=0; i<6; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=0; i<6; i++) item[i].target = item[i].sel;

						nextframe=SDL_GetTicks()+10;
					break;
					case 1: // select player
						selectplayer();

						for (int i=0; i<6; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=0; i<6; i++) item[i].target = item[i].sel;

						nextframe=SDL_GetTicks()+10;
					break;
					case 2: // view replay
//						viewreplay();
//						loadrecord();
						viewrecordlist();

						for (int i=0; i<6; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=0; i<6; i++) item[i].target = item[i].sel;

						nextframe=SDL_GetTicks()+10;
					break;
					case 3: // scoreboard
						allhigh();

						for (int i=0; i<6; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=0; i<6; i++) item[i].target = item[i].sel;

						nextframe=SDL_GetTicks()+10;
					break;
					case 4: // options
						options();

						for (int i=0; i<6; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=0; i<6; i++) item[i].target = item[i].sel;

						nextframe=SDL_GetTicks()+10;
					break;
					case 5: // quit
						ready = quit(false);

						for (int i=0; i<6; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=0; i<6; i++) item[i].target = item[i].sel;

						nextframe=SDL_GetTicks()+10;
					break;
					default:
					break;
				}
			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				ready = quit(true);
				nextframe=SDL_GetTicks()+10;
			}

		} // if

		Graphics.clear();


//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ADD);

//		glColor4f(0.0, 0.0, 0.0, 0.1);

//		blobcanvas->display();

		if (!drawn) {
			drawn=true;

			drawcanvas();

			float y =  Window.height/2.0-60.0;
			for (int i=0; i<6; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif

		} else SDL_Delay(1);

	} // while


	// save highscores
	savetimes_player();
//	savetimes_public();

	sound::FadeOutMusic(200);
	SDL_Delay(200);

}

void menu::options() {
	const int items = 7;
	menutext item[items]={ menutext("*OPTIONS"), menutext("RESOLUTION"), menutext("SHOW GHOST"),
		menutext("DECORATION"), menutext("CONTROLS"), menutext("SOUND OPTIONS"),/* menutext("UPLOAD SCORES"), */menutext("BACK") };

	int selection=1;

	for (int i=1; i<items; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;

	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += items-1;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection >= items) selection -= items-1;
			}

			for (int i=1; i<items; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<items; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);

				int s=-1;

				switch(selection) {
					case 1: // resolution
						resolution();
						for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=1; i<items; i++) item[i].target = item[i].sel;
						nextframe=SDL_GetTicks()+10;
					break;
					case 2: // show ghost
						s = state("*SHOW GHOST?", Config.showghost);
						if (s==1) Config.showghost=true;
						else if (s==0) Config.showghost=false;

						for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=1; i<items; i++) item[i].target = item[i].sel;
						nextframe=SDL_GetTicks()+10;
					break;
					case 3: // decoration
						decoration();
						for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=1; i<items; i++) item[i].target = item[i].sel;
						nextframe=SDL_GetTicks()+10;
					break;
					case 4: // controls
						controls();
						for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=1; i<items; i++) item[i].target = item[i].sel;
						nextframe=SDL_GetTicks()+10;
					break;
					case 5: // sounds options
						soundoptions();
						for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=1; i<items; i++) item[i].target = item[i].sel;
						nextframe=SDL_GetTicks()+10;
					break;
					/*case 6: // update network scores
						netscores();
						for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=1; i<items; i++) item[i].target = item[i].sel;
						nextframe=SDL_GetTicks()+10;
					break;*/
					case 6: // back
						return;
					break;
					default:
					break;
				}
			}
			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}


		} // if


		if (!drawn) {
			drawn=true;

			Graphics.clear();

			drawcanvas();

			float y = Window.height/2.0-142.5;
			for (int i=0; i<items; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}
}



bool menu::quit(bool escquits) {
	menutext item[3]={ menutext("*QUIT?"), menutext("YES"), menutext("NO") };

	int selection=1;

	for (int i=1; i<3; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += 2;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection > 2) selection -= 2;
			}

			for (int i=1; i<3; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<3; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				switch(selection) {
					case 1: // yes
						return true;
					break;
					case 2: // no
						return false;
					break;
					default:
					break;
				}
			}

			if (Keyboard.Pressed(SDLK_y) ) {
				sound::PlayChunk(-1, Media.menuenter, 0);
				return true;
			}
			if (Keyboard.Pressed(SDLK_n) ) {
				sound::PlayChunk(-1, Media.menuenter, 0);
				return false;
			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return escquits;
			}


		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();
			drawcanvas();

			float y = Window.height/2.0-40.0;
			for (int i=0; i<3; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}

	return false;
}




int menu::state(char *text, int defstate) {
	menutext item[3]={ menutext(text), menutext("YES"), menutext("NO") };

	int selection=2-defstate;

	for (int i=1; i<3; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += 2;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection > 2) selection -= 2;
			}

			for (int i=1; i<3; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<3; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				switch(selection) {
					case 1: // yes
						return true;
					break;
					case 2: // no
						return false;
					break;
					default:
					break;
				}
			}

			if (Keyboard.Pressed(SDLK_y) ) {
				sound::PlayChunk(-1, Media.menuenter, 0);
				return true;
			}
			if (Keyboard.Pressed(SDLK_n) ) {
				sound::PlayChunk(-1, Media.menuenter, 0);
				return false;
			}


			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return -1;
			}


		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();

			drawcanvas();

			float y = Window.height/2.0-60.0;
			for (int i=0; i<3; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}

	return -1;
}



void menu::playgame() {
	Uint32 levels = levelcount;

	menutext item[MAXLEVELS+2];

	item[0].settext("*SELECT LEVEL");

	for (int i=0; i<levelcount; i++) {
		item[i+1].settext( (char*)levelname[i].c_str() );
	}

	item[levelcount+1].settext("BACK");


	int selection = 1;

	float miny = 0;
	float maxy = (levels+2)*45;

	for (int i=1; i<levels+2; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime;

	float y=Window.height/2.0-60.0;

	float mvm[50];
	for (int i=0; i<50; i++) mvm[i]=0.0;
	int mvmind=0;

	float mvmbig[50];
	for (int i=0; i<50; i++) mvmbig[i]=0.0;
	int mvmbigind=0;

	float h=45.0;
	bool drawn=false;

	while (1) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				if (selection <= 1) {
					selection += levels;
					mvmbig[mvmbigind++] = -1.0;
					y -= levels*h;
				} else {
					selection--;
					mvm[mvmind++] = 1.0;
					y += h;
				}
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				if (selection >= levels+1) {
					selection -= levels;
					mvmbig[mvmbigind++] = 1.0;
					y += levels*h;
				} else {
					selection++;
					mvm[mvmind++] = -1.0;
					y -= h;
				}
			}

			if (mvmind>=50) mvmind=0;
			if (mvmbigind>=50) mvmbigind=0;

			for (int i=1; i<levels+2; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<levels+2; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);

				if (selection >= 1 && selection <= levels) {

					// check if allowed
					int yellow=0;
					for (int i=0; i<selection; i++) {
						if (!cheatmode &&
							Player[Config.currentplayer].times[i-1][0] < 0.005)
						{
							yellow++;
						}
					}
					// end of check

					if (yellow <= 3) {
						levelstats( (Uint32)( selection-1 ) );

						selection = menu_level+1; // change selection and set counters

						y = Window.height/2.0-60.0-menu_level*h;

						for (int i=0; i<50; i++) {
							mvm[i]=0.0;
							mvmbig[i]=0.0;
						}

						for (int i=1; i<levels+2; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=1; i<levels+2; i++) item[i].target = item[i].sel;
					}

					nextframe=SDL_GetTicks()+10;
				} else {

					if ( selection == levels+1 ) {
						return;
					}

				}
			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}

			for (int i=0; i<50; i++) {
				if (mvm[i] < 0) mvm[i]+=0.02;
				if (mvm[i] > 0) mvm[i]-=0.02;
				if (mvmbig[i] < 0) mvmbig[i]+=0.01;
				if (mvmbig[i] > 0) mvmbig[i]-=0.01;
			}


		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();
			drawcanvas();

			float y2=y;

			while (y2 > Window.height/2.0-60.0+h/2.0) y2 -= h*(levels+1.0);
			while (y2 < Window.height/2.0-60.0-h*levels-h/2.0) y2 += h*(levels+1.0);

			for (int i=0; i<50; i++) {
				float x = mvm[i];
				if (x >= 0.02) y2 -= h/2.0*(1-cos(3.14159*x));
				if (x <= -0.02) y2 += h/2.0*(1-cos(3.14159*x));
				x = mvmbig[i];
				if (x >= 0.02) y2 -= levels*h/2.0*(1-cos(3.14159*x));
				if (x <= -0.02) y2 += levels*h/2.0*(1-cos(3.14159*x));
			}

			int yellow=0;
			for (int i=0; i<levels+2; i++) {
				if (i >= 1 && i <= levels) {
					if (!cheatmode &&
						Player[Config.currentplayer].times[i-1][0] < 0.005)
					{
						yellow++;
						if (yellow <= 3) {
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 0.6, 1.0);
							#else
							glColor4f(1.0, 1.0, 0.6, 1.0);
							#endif
							item[i].display(y2);
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 1.0, 1.0);
							#else
							glColor4f(1.0, 1.0, 1.0, 1.0);
							#endif
						} else {
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 0.6, 0.6, 1.0);
							#else
							glColor4f(1.0, 0.6, 0.6, 1.0);
							#endif
							item[i].display(y2);
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 1.0, 1.0);
							#else
							glColor4f(1.0, 1.0, 1.0, 1.0);
							#endif
						}
					} else {
						#ifdef HAVE_GLES
						Graphics.color4f(0.6, 1.0, 0.6, 1.0);
						#else
						glColor4f(0.6, 1.0, 0.6, 1.0);
						#endif
						item[i].display(y2);
						#ifdef HAVE_GLES
						Graphics.color4f(1.0, 1.0, 1.0, 1.0);
						#else
						glColor4f(1.0, 1.0, 1.0, 1.0);
						#endif
					}
				} else {
					item[i].display(y2);
				}
				y2+=h;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif

		} else SDL_Delay(1);
	}
}

void menu::simulate(Uint32 level, char *replay) {

	// Open ghost file

	FILE *ghostfile=0;
	char ghostpath[1200];

	sprintf (ghostpath, "%s/ghost/meri%d.brc", Path->privdir, level);

	if (!replay && Config.showghost) {
		ghostfile = fopen(ghostpath, "rb");
	} else if (replay) {
		ghostfile = fopen(replay, "rb");
		if (!ghostfile) {
			fprintf (stderr, "Couldn't open file \"%s\". Critical.\n", replay);
			exit(1);
		}
	}

	bool ghost;
	bool rewriteghost=false;
	char *ghostname=0;
	element *ghostelement = 0;

	if (ghostfile) ghost=true;
	else ghost=false;

	if (ghost) {
		Uint32 recversion=0;
		compat::FromFile_Int(ghostfile, &recversion);

		if ( recversion != RECORDVERSION ) {
			fprintf (stderr, "Can't show record: wrong version.\n");
			rewriteghost=true;
			fclose(ghostfile);

			if (replay) {
				fprintf (stderr, "Broken record file \"%s\".\n", replay);
				exit(1);
			}

			ghostfile=NULL;
			ghost=false;
		} else {
			Uint32 ghostlevel = 0;
			compat::FromFile_Int(ghostfile, &ghostlevel); // read level number from ghost file
			if ( replay && ghostlevel >= levelcount ) {
				fprintf (stderr, "Corrupted record file. Critical.\n");
				exit(1);
			}
			if (replay) level = ghostlevel;

			ghostname = compat::getstring(ghostfile);
#if DEBUG == 1
			fprintf (stderr, "Ghostname: %s\n", ghostname);
#endif

			// read elements

			element *ge = 0;

			do {
				Uint16 p;

				if ( compat::FromFile_ShortInt_EOF(ghostfile, &p) == 0) break;

				if (ge != 0) ge = ge->next = new element();
				else {
					ghostelement = ge = new element();
				}

				ge->set(p);

			} while (1);

		}

	}


	// Open level file for reading
	char path1[1200];
	sprintf (path1, "%s/levels/meri%d.dat", Path->datadir, level);

	FILE *dat = fopen(path1, "r");
	if (!dat) {
		fprintf (stderr, "Can't open '%s'! Critical\n", path1);
		exit(1);
	}

	// Read level data

	game Game;
	char mdl[200], levtex[200], shiptex[200], goaltex[200], groundtex[200];
	char icetex[200], tentacletex[200], invadertex[200], bouldertex[200], tuxtex[200];

	float lx,ly,lrot,lscale, sx,sy,svx,svy,srot, gx,gy,gr;
	int mus;

	if ( fscanf(dat, "%190s\n%190s\n%190s\n%190s\n%190s\n%190s\n%190s\n%190s\n%190s\n%190s\n%d\n%f %f %f %f\n%f %f %f %f %f\n%f %f %f",
		mdl, levtex, shiptex, goaltex, groundtex, icetex, tentacletex, invadertex, bouldertex, tuxtex,
		&mus,
		&lx, &ly, &lrot, &lscale, &sx,&sy,&svx,&svy,&srot,&gx,&gy,&gr) != 23 )
	{
		fprintf (stderr, "File '%s' is corrupted! Critical!\n", path1);
		exit(1);
	}

	// play music
	selectmusic(mus);

	GLuint iceID = Graphics.maketexture( Path->data(icetex) );
	GLuint groundID = Graphics.maketexture( Path->data(groundtex) );

	Game.iceID = iceID;
	Game.groundID = groundID;

	Game.makelevel( mdl, lx, ly, lrot, lscale, levtex);
	if (ghost) {
		Game.setghost(ghostelement, 0.3); // set ghost
		Game.makeghost(sx, sy, svx, svy, srot, shiptex);

		if (replay) Game.ghostalpha=1.0;
	}

	if (!replay) Game.makeship(sx, sy, svx, svy, srot, shiptex);

	Game.focus(); // set aspect ratio
	Game.setgoal(gx,gy,gr, goaltex);

	// Edge data

	while (1) {
		int i, j, t;
		float f;

		if (fscanf(dat, "%d.%d=%d,%f\n", &i, &j, &t, &f) != 4) break;
		switch(t) {
			case WALL_GROUND:
				Game.setground(i, j, groundID, f);
			break;
			case WALL_ICE:
				Game.setice(i, j, iceID, f);
			break;
		}
	}


	// load enemies
	while (1) {
		int type;
		float x, y, a, s;
		if (fscanf(dat, "enemy %d %f %f %f %f\n", &type, &x, &y, &a, &s) != 5) break;

		switch(type) {
			case ENEMY_TENTACLE:
				Game.maketentacle(x, y, a, s, tentacletex);
			break;
			case ENEMY_INVADER:
				Game.makeinvader(x, y, a, s, invadertex);
			break;
			case ENEMY_BOULDER:
				Game.makeboulder(x, y, a, s, bouldertex);
			break;
			case ENEMY_TUX:
				Game.maketux(x, y, a, s, tuxtex);
			break;
			default:
			break;
		}
	}
	fclose(dat);

	// Finished reading level.

	// Open recording file

	FILE *recfile = 0;
	element *recelement = new element();

	if (!replay) {
		recfile = fopen( Path->priv("records/last.brc"), "wb");
		if (recfile) {
			Game.setrecord(recelement);
			compat::ToFile_Int(recfile, RECORDVERSION);
			compat::ToFile_Int(recfile, level);
			compat::putstring(recfile, Player[Config.currentplayer].name);
		}
	}

	double time = 0.0;

	// Tutorial texts, etc.
	char tpath[1200];
	sprintf (tpath, "%s%d%s", Path->data("levels/meri"), level, ".tdt");

#if DEBUG == 1
	fprintf (stderr, "Trying to open \"%s%d%s\"\n", Path->data("levels/meri"), level, ".tdt");
#endif

	FILE *tfile = fopen( tpath, "r" );
	if (tfile) {

#if DEBUG == 1
		fprintf (stderr, "Tut file opened.\n");
#endif

		// game text file exists

		char s[1024];
		char d[1024];
		int lnum=0;
		float ax=0.0, ay=0.0, bx=0.0, by=0.0;

		while (fgets(s, 1024, tfile) ) {

			if ( sscanf(s, "%d %f %f %f %f %s\n", &lnum, &ax, &ay, &bx, &by, d) != 6) break;
			// Read. Save then.

			Game.texts.push_back( text(lnum, d, vec(ax, ay), vec(bx, by) ) );
		}

		fclose(tfile);
	}


	if (replay) { // record playing screen

#if DEBUG == 1
		fprintf (stderr, "Entering replay presentation screen.\n");
#endif

		Uint32 stime = SDL_GetTicks(), nextframe = stime;
		bool ready=false;
		char tstr[200]; // here be time
		int state=0;

		glClearColor(0.0, 0.0, 0.0, 1.00); // Want black background

#if DEBUG == 1
		fprintf (stderr, "Entering while loop.\n");
#endif

		bool drawn=false;

		while (state!=2) {
			Keyboard.HandleInterrupts();
			SDL_Delay(1);

			while (SDL_GetTicks() > nextframe) {
				nextframe += 10;
				drawn=false;

				#ifdef PANDORA
				if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
				#else
				if (Keyboard.Pressed(SDLK_ESCAPE)) {
				#endif
					sound::PlayChunk(-1, Media.menuenter, 0);
					glClearColor(0.225, 0.225, 0.625, 1.00); // background color back to menu's one
					return;
				}

				#ifdef PANDORA
				if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))  || Keyboard.Pressed(SDLK_SPACE) || Config.nolimit ) {
				#else
				if (Keyboard.Pressed(SDLK_RETURN) || Keyboard.Pressed(SDLK_SPACE) || Config.nolimit ) {
				#endif
					sound::PlayChunk(-1, Media.menuenter, 0);
					if (state==1) state++;
					else {
						state=1;
#if DEBUG == 1
						fprintf (stderr, "Starting simulation.\n");
#endif

						if (Config.decoration != DEC_FULL) glClearColor(0.65, 0.58, 1.0, 1.00);
						else glClearColor(0.0, 0.0, 0.0, 1.0);

						double time = Game.simulate();
						glClearColor(0.0, 0.0, 0.0, 1.00); // Want black background

						time += 0.001; // Heh heh.

						if (time > 0.005) sprintf (tstr, "%d:%.2d.%.2d", (int)(time/60), (int)(time)%60, (int)(time*100.0)%100);
						else sprintf(tstr, "FINISHED REPLAY");
						nextframe = SDL_GetTicks()+10;

						time -= 0.001; // (Heh heh.)^2

						focuscanvas();

						if (Config.nolimit) { // display text and continue
							printf("%s", tstr);
						}

					}
				} // if pressed

			} // while

			if (!drawn) {
				drawn=true;

				Graphics.clear();

				char *lname = (char*)levelname[level].c_str();
				Font.WriteString(lname, Window.width/2-20*strlen(lname), Window.height/2-120, 40);

				Font.WriteString(ghostname, Window.width/2-15*strlen(ghostname), Window.height/2-70, 30);
				if (state==1) Font.WriteString(tstr, Window.width/2-15*strlen(tstr), Window.height/2+70, 30);

				glFinish();
				#ifdef HAVE_GLES
				EGL_SwapBuffers();
				#else
				SDL_GL_SwapBuffers();
				#endif
			} else SDL_Delay(1);

		} // while

		glClearColor(0.225, 0.225, 0.625, 1.00); // background color back to menu's one

	} else {

		if (Config.decoration != DEC_FULL) glClearColor(0.65, 0.58, 1.0, 1.00);
		else glClearColor(0.0, 0.0, 0.0, 1.0);

		// Playing
		time = Game.simulate();

		glClearColor(0.225, 0.225, 0.625, 1.00);

		// tux:
		if (Game.tuxfound) Player[Config.currentplayer].tuxfound |= (1<<level);

	}

	// free tutorial texts
	Game.texts.clear();

	// free ghost stuff
	if (ghost) {
		fclose(ghostfile);
		delete[] ghostname;

		while (ghostelement) {
			element *ge = ghostelement->next;
			delete ghostelement;
			ghostelement = ge;
		}
	}

	// free recordfile and save elements
	if (recfile) {

		if (recelement) { // don't save the first one
			element *re = recelement->next;
			delete recelement;
			recelement = re;
		}
		while (recelement) {
			recelement->save(recfile);
			element *re = recelement->next;
			delete recelement;
			recelement = re;
		}

		fclose(recfile);
	}

	if (time >= 0.01) {

		message = MSG_TIME;
		msgtime = time+0.001;

		// High scores

		// reload public highscores
		loadtimes_public();

		for (int i=0; i<10; i++) {
			if (time < Player[Config.currentplayer].times[level][i] ||
				Player[Config.currentplayer].times[level][i] < 0.009)
			{
				for (int j=9; j>=i; j--) {
					Player[Config.currentplayer].times[level][j] =
						Player[Config.currentplayer].times[level][j-1];
				}
				Player[Config.currentplayer].times[level][i] = time;

				break;
			}
		}

		for (int i=0; i<10; i++) {
			if (time < hi_times[level][i]) {
				for (int j=9; j>i; j--) {
					hi_times[level][j] = hi_times[level][j-1];
					hi_nicks[level][j] = hi_nicks[level][j-1];
				}
				hi_times[level][i] = time;
				hi_nicks[level][i] = Player[Config.currentplayer].name;

				if (i==0) {
					message=MSG_RECORD;
					rewriteghost=true;
					Config.newtimes |= (1<<level);

#if DEBUG == 1
					fprintf (stderr, "Ghost to be rewritten. Highscore.\n");
#endif

				} else {
					message=MSG_TOPTEN;
				}

				break;
			}
		}
	} else {
		message = MSG_FAILED; // failed
	}

	if (rewriteghost && time > 0) {
#if DEBUG == 1
		fprintf (stderr, "Rewriting ghost.\n");
#endif

		sprintf (ghostpath, "%s/ghost/meri%d.brc", Path->privdir, level);
		copylevel( Path->priv("records/last.brc"), ghostpath);
	}


	Graphics.deletetexture(iceID);
	Graphics.deletetexture(groundID);

	if (!replay) {
		savetimes_player();
		savetimes_public();
	}

	// add the begin position of control keys to the file

}

void menu::levelstats(Uint32 level) {
	const int items = 7;
	menu_level=level;

	char title[200];
	sprintf (title, "*%s", (char*)levelname[level].c_str());

	menutext item[items]={ menutext( title ), menutext("PLAY"), menutext("PLAY NEXT"),
		menutext("REPLAY LAST"), menutext("SAVE REPLAY"), menutext("SCOREBOARD"),
		menutext("BACK") };

	int selection=1;

	for (int i=1; i<items; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool offered=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += items-1;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection >= items) selection -= items-1;
			}

			for (int i=1; i<items; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<items; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				switch(selection) {
					case 1: // play
						message=0;

						simulate(level, 0);

						for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=1; i<items; i++) item[i].target = item[i].sel;

						nextframe=SDL_GetTicks()+10;

						lastrecord=true;

						if ( (message == MSG_TIME || message == MSG_RECORD || message == MSG_TOPTEN) &&
							!offered && level+1 < levelcount &&
							Player[Config.currentplayer].times[level+1][0] < 0.005 )
						{
							offered=true;
							selection++;
						}

						focuscanvas();
					break;
					case 2: // play next
						if (level+1 < levelcount) {
							// check if allowed
							int yellow=0;
							for (int i=0; i<=level; i++) {
								if (!cheatmode &&
									Player[Config.currentplayer].times[i][0] < 0.005)
								{
									yellow++;
								}
							}

							// end of check
							if (yellow < 3) {
								message=0;
								lastrecord = 0;
								levelstats(level+1);
								return;
							}
						}
					break;
					case 3: // replay last
						if	(lastrecord) {
							char path[1200];
							sprintf (path, "%s/records/last.brc", Path->privdir);

							simulate(0, path);

							for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
							for (int i=1; i<items; i++) item[i].target = item[i].sel;

							nextframe = SDL_GetTicks()+10;
							focuscanvas();

						}
					break;
					case 4: // save record
						if (lastrecord) {
							saverecord(level);

							for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
							for (int i=1; i<items; i++) item[i].target = item[i].sel;

							nextframe = SDL_GetTicks()+10;
						}
					break;
					case 5: // scoreboard
						scoreboard(level);

						for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
						for (int i=1; i<items; i++) item[i].target = item[i].sel;

						nextframe=SDL_GetTicks()+10;

					break;
					case 6: // back
						lastrecord=false;
						message=0;
						return;
					break;
					default:
					break;
				}
			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				message=0;
				lastrecord=false;
				return;
			}

		} // if

		// drawing section

		if (!drawn) {
			Graphics.clear();
			drawcanvas();

			float y = Window.height/2.0-125.0;

			for (int i=0; i<items; i++) {
				if (i==1) { // play
					if (!cheatmode &&
						Player[Config.currentplayer].times[level][0] < 0.005)
					{
						#ifdef HAVE_GLES
						Graphics.color4f(1.0, 1.0, 0.6, 1.0);
						#else
						glColor4f(1.0, 1.0, 0.6, 1.0);
						#endif
						item[i].display(y); // yellow play game
						#ifdef HAVE_GLES
						Graphics.color4f(1.0, 1.0, 1.0, 1.0);
						#else
						glColor4f(1.0, 1.0, 1.0, 1.0);
						#endif
					} else {
						#ifdef HAVE_GLES
						Graphics.color4f(0.6, 1.0, 0.6, 1.0);
						#else
						glColor4f(0.6, 1.0, 0.6, 1.0);
						#endif
						item[i].display(y); // green play game
						#ifdef HAVE_GLES
						Graphics.color4f(1.0, 1.0, 1.0, 1.0);
						#else
						glColor4f(1.0, 1.0, 1.0, 1.0);
						#endif
					}
				} else if (i==2) { // play next
					int yellow=0;
					if (level+1 >= levelcount) { // grey play next
						#ifdef HAVE_GLES
						Graphics.color4f(0.6, 0.6, 0.6, 1.0);
						#else
						glColor4f(0.6, 0.6, 0.6, 1.0);
						#endif
						item[i].display(y);
						#ifdef HAVE_GLES
						Graphics.color4f(1.0, 1.0, 1.0, 1.0);
						#else
						glColor4f(1.0, 1.0, 1.0, 1.0);
						#endif
					} else { // calculate yellows
						for (int j=0; j<=level; j++) {
							if (Player[Config.currentplayer].times[j][0] < 0.005) yellow++;
						}
						if (yellow >= 3 && !cheatmode) {
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 0.6, 0.6, 1.0);
							#else
							glColor4f(1.0, 0.6, 0.6, 1.0);
							#endif
							item[i].display(y); // red play next
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 1.0, 1.0);
							#else
							glColor4f(1.0, 1.0, 1.0, 1.0);
							#endif
						} else if ( cheatmode ||
										Player[Config.currentplayer].times[level+1][0] >= 0.005)
						{
							#ifdef HAVE_GLES
							Graphics.color4f(0.6, 1.0, 0.6, 1.0);
							#else
							glColor4f(0.6, 1.0, 0.6, 1.0);
							#endif
							item[i].display(y); // green
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 1.0, 1.0);
							#else
							glColor4f(1.0, 1.0, 1.0, 1.0);
							#endif
						} else {
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 0.6, 1.0);
							#else
							glColor4f(1.0, 1.0, 0.6, 1.0);
							#endif
							item[i].display(y); // yellow
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 1.0, 1.0);
							#else
							glColor4f(1.0, 1.0, 1.0, 1.0);
							#endif
						}
					}
				} else {
					if ( (i==3 || i==4) && !lastrecord ) {
						#ifdef HAVE_GLES
						Graphics.color4f(0.6, 0.6, 0.6, 1.0);
						#else
						glColor4f(0.6, 0.6, 0.6, 1.0);
						#endif
						item[i].display(y);
						#ifdef HAVE_GLES
						Graphics.color4f(1.0, 1.0, 1.0, 1.0);
						#else
						glColor4f(1.0, 1.0, 1.0, 1.0);
						#endif
					} else {
						if ( i==5 ) {
							if ( Player[Config.currentplayer].times[level][0] > 0.005 ) {
								colortime(level, Player[Config.currentplayer].times[level][0]);
							} else {
								#ifdef HAVE_GLES
								Graphics.color4f(1.0, 0.6, 0.6, 1.0);
								#else
								glColor4f(1.0, 0.6, 0.6, 1.0);
								#endif
							}
							item[i].display(y);
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 1.0, 1.0);
							#else
							glColor4f(1.0, 1.0, 1.0, 1.0);
							#endif
						} else {
							item[i].display(y);
						}
					}
				}
				y+=45.0;
			}

			if (message>0) {
				char str[200];

				if (message != MSG_FAILED) {
					colortime(level, msgtime);
				}

				if (message==MSG_RECORD) sprintf (str, "TOP SCORE: %d:%.2d.%.2d",
					(int)(msgtime/60), (int)(msgtime)%60, (int)(msgtime*100.0)%100);

				if (message==MSG_TOPTEN) sprintf (str, "HIGH SCORE: %d:%.2d.%.2d",
					(int)(msgtime/60), (int)(msgtime)%60, (int)(msgtime*100.0)%100);

				if (message==MSG_TIME) sprintf (str, "%d:%.2d.%.2d",
					(int)(msgtime/60), (int)(msgtime)%60, (int)(msgtime*100.0)%100);

				if (message==MSG_FAILED) sprintf (str, "YOU FAILED" );


				Font.WriteString(str, Window.width/2-15*strlen(str), Window.height/2+180, 30 );

				#ifdef HAVE_GLES
				Graphics.color4f(1.0, 1.0, 1.0, 1.0);
				#else
				glColor4f(1.0, 1.0, 1.0, 1.0);
				#endif
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);
	}
}


struct Resolution {
	int width, height;
	char str[64];
};
void menu::resolution() {
	// Get resolutions
	vector<Resolution> resolutions;
	SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE|SDL_OPENGL);
	if(modes > 0) {
		Uint32 bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
		for(int i=0; modes[i] && i < 10; ++i) {
			Resolution resolution;
			resolution.width = modes[i]->w;
			resolution.height = modes[i]->h;

			// Menu doesn't fit
			if(resolution.width < 800 || resolution.height < 600) continue;

			snprintf(resolution.str, 64, "%d X %d", resolution.width, resolution.height);
			resolutions.push_back(resolution);
		}
	}

	// Form menuitems
	vector<menutext> item;
	item.push_back(menutext("*RESOLUTION:"));
	for(unsigned int i=0; i<resolutions.size(); ++i) {
		item.push_back(menutext(resolutions[i].str));
	}
	item.push_back(menutext(""));
	item.push_back("BACK");
	int items = (int)item.size();

	// Set current selection
	int selection = 1;
	for(unsigned int i=0; i<resolutions.size(); ++i) {
		if(SDL_GetVideoSurface()->w == resolutions[i].width &&
			SDL_GetVideoSurface()->h == resolutions[i].height)
		{
			selection = 1 + i;
		}
	}


	for (int i=1; i<items; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);


		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += items-1;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection >= items) selection -= items-1;
			}

			for (int i=1; i<items; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<items; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif

				#ifdef HAVE_GLES
				Uint32 flags = SDL_SWSURFACE | SDL_FULLSCREEN;
				#else
				Uint32 flags = SDL_OPENGL;
				if (Config.fullscreen) flags |= SDL_FULLSCREEN;
				#endif

				sound::PlayChunk(-1, Media.menuenter, 0);

				if(selection >= 1 && selection <= resolutions.size()) { // Resolution
					int width = resolutions[selection-1].width;
					int height = resolutions[selection-1].height;
					if (!Window.OpenWindow(width, height, 32, flags)) {
						fprintf (stderr, "Couldn't set screen mode: %s\n", SDL_GetError());
						exit(1);
					}
					Config.reswidth = width;
					Config.resheight = height;
					Graphics.reloadtextures();
					glClearColor(0.225, 0.225, 0.625, 1.00);
					return;
				} else if (selection == items - 2) { // Full screen / Windowed
						if (!Window.OpenWindow(Window.width, Window.height, 32, 0) ) {
							fprintf (stderr, "Couldn't set screen mode: %s\n", SDL_GetError());
							exit(1);
						}
						Config.fullscreen = !Config.fullscreen;
						Graphics.reloadtextures();
						nextframe = SDL_GetTicks();
				} else if (selection == items - 1) { // Back
					return;
				}

			}


			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}


		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			if (Config.fullscreen) item[items-2].settext("WINDOWED");
			else item[items-2].settext("FULLSCREEN");

			Graphics.clear();
			drawcanvas();

			float y = Window.height/2.0-156;
			for (int i=0; i<items; i++) {
				item[i].display(y);
				y+=35;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}
}

void menu::decoration() {
	menutext item[4]={
		menutext("*DECORATION?"),
		menutext("YES"),
		menutext("NO"),
		menutext("NEGATIVE") };

	int selection=3-(int)Config.decoration;

	for (int i=1; i<4; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += 3;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection > 3) selection -= 3;
			}

			for (int i=1; i<4; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<4; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				switch(selection) {
					case 1: // full
						Config.decoration=2;
						return;
					break;
					case 2: // low
						Config.decoration=1;
						return;
					break;
					case 3: // no
						Config.decoration=0;
						return;
					break;
					default:
					break;
				}
			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}


		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();
			drawcanvas();

			float y = Window.height/2.0-62.5;
			for (int i=0; i<4; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}
}


void menu::showghost() {
	menutext item[3]={
		menutext("*SHOW GHOST?"),
		menutext("YES"),
		menutext("NO") };

	int selection=2-(int)Config.showghost;

	for (int i=1; i<3; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += 2;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection > 2) selection -= 2;
			}

			for (int i=1; i<3; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<3; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				switch(selection) {
					case 1: // yes
						Config.showghost=true;
						return;
					break;
					case 2: // no
						Config.showghost=false;
						return;
					break;
					default:
					break;
				}
			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}


		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();

			drawcanvas();

			float y = Window.height/2.0-40.0;
			for (int i=0; i<3; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}
}



bool menu::selectplayer() {
	const int items = 8;
	menutext item[items];
	item[0].settext("*SELECT PLAYER:");
	for (int i=0; i<6; i++) {
		if (Player[i].inuse) item[i+1].settext(Player[i].name);
		else item[i+1].settext("ENTER NAME");
	};
	item[7].settext("BACK");

	int selection=1;
	if (Config.currentplayer >= 0) selection = Config.currentplayer+1;

	for (int i=1; i<items; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += items-1;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection >= items) selection -= items-1;
			}

			for (int i=1; i<items; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<items; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				if (selection >= 1 && selection <= 6) {
					if (!Player[selection-1].inuse) {

						if (entername(selection-1) ) {

							item[selection].settext(Player[selection-1].name);

							Player[selection-1].inuse=true;
							Player[selection-1].tuxfound=0;

							for (int u=0; u<MAXLEVELS; u++) {
								for (int v=0; v<10; v++) {
									Player[selection-1].times[u][v]=0.0;
								}
							}

						}

					} else {
						Config.currentplayer=selection-1;
						return true;
					}

					for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
					for (int i=1; i<items; i++) item[i].target = item[i].sel;

					nextframe = SDL_GetTicks()+10;

				}
				if (selection==7) return false;
			}

			if (Keyboard.Pressed(SDLK_DELETE) || Keyboard.Pressed(SDLK_d)
				|| Keyboard.Pressed(SDLK_BACKSPACE) ) {
				if (selection >= 1 && selection <= 6) {
					if (Player[selection-1].inuse) {

						if (eraseplayer(selection-1) ) {
							item[selection].settext("ENTER NAME");
							if ( Config.currentplayer == selection-1 ) Config.currentplayer=-1;
						}
					}

					for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
					for (int i=1; i<items; i++) item[i].target = item[i].sel;

					nextframe = SDL_GetTicks()+10;
				}
			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return false;
			}

		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();

			drawcanvas();

			float y = Window.height/2.0-140.0;
			for (int i=0; i<items; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}
	return true;
}


bool menu::entername(int playernum) {
	const int items = 2;
	menutext item[2];
	item[0].settext("*ENTER NAME:");
	item[1].settext("");

	char nick[NICK_MAXLEN+1];
	nick[0]=0;
	int cursor=0;

	int selection=1;

	item[1].sel = 1.0;
	item[1].target = 1.0;

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_BACKSPACE) > 0 ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				cursor--;
				if (cursor<0) cursor=0;
				nick[cursor]='\0';

				item[1].settext(nick);
			}

			char ch = Keyboard.GetAscii();

			if (ch!=0 && ch !='\n' && ch !='\r' && ch != ' ') {
				sound::PlayChunk(-1, Media.menuselect, 0);

				if (ch >= 'a' && ch <= 'z') ch -= 'a'-'A';

				for (int i=0; i<big_num; i++) {
					if (big_ch[i]==ch) {
						if (cursor < NICK_MAXLEN) {
							nick[cursor]=ch;
							cursor++;
							nick[cursor]='\0';

							item[1].settext(nick);


							break;
						}
					}
				} // for

			} // if


			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				// check that there is a name

				// cheat:
				if (strcmp(nick, "IDDQD")==0) {
					sound::PlayChunk(-1, Media.roarr, 0);
					cheatmode=true;
					return false;
				}

				if (strlen(nick) >= 1) {

					// check that the name is not in use
					bool used=false;

					for (int i=0; (i<6 && !used); i++) {
						if (i != playernum) {
							if (Player[i].inuse) {
								if ( strcmp(Player[i].name, nick) == 0) used=true;
							}
						}
					}

					if (!used) {
						Player[playernum].setnick(nick);
						Player[playernum].inuse=true;
						return true;
					}
				}
			} // if


			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return false;
			}


		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();

			drawcanvas();

			float y = Window.height/2.0-40.0;
			for (int i=0; i<items; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}

	return false;
}


bool menu::eraseplayer(int playernum) {
	menutext item[3]={
		menutext(""),
		menutext("YES"),
		menutext("NO") };

	char t[200];
	sprintf (t, "DELETE \"%s\"?", Player[playernum].name);
	item[0].settext(t);

	int selection=2;

	for (int i=1; i<3; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += 2;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection > 2) selection -= 2;
			}

			for (int i=1; i<3; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<3; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				switch(selection) {
					case 1: // yes
						Player[playernum].inuse=false;
						return true;
					break;
					case 2: // no
						return false;
					break;
					default:
					break;
				}
			}

			if (Keyboard.Pressed(SDLK_y) ) {
				sound::PlayChunk(-1, Media.menuenter, 0);
				Player[playernum].inuse=false;
				return true;
			}
			if (Keyboard.Pressed(SDLK_n) ) {
				sound::PlayChunk(-1, Media.menuenter, 0);
				return false;
			}


			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return false;
			}


			if (Keyboard.Pressed(SDLK_DELETE) || Keyboard.Pressed(SDLK_d)) {
				Player[playernum].inuse=false;
				return true;
			}

		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();
			drawcanvas();

			float y = Window.height/2.0-40.0;
			for (int i=0; i<3; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}

	return false;
}

void menu::colortime(Uint32 level, float time) {
	if (time <= rank_times[level][0]) 
		#ifdef HAVE_GLES
		Graphics.color4f(0.6, 1.0, 0.6, 1.0);
		#else
		glColor4f(0.6, 1.0, 0.6, 1.0);
		#endif
	else if (time <= rank_times[level][1]) 
		#ifdef HAVE_GLES
		Graphics.color4f(1.0, 1.0, 0.6, 1.0);
		#else
		glColor4f(1.0, 1.0, 0.6, 1.0);
		#endif
	else
		#ifdef HAVE_GLES
		Graphics.color4f(1.0, 0.6, 0.6, 1.0);
		#else
		glColor4f(1.0, 0.6, 0.6, 1.0);
		#endif
}


void menu::scoreboard(Uint32 level) {
	Uint32 levels = levelcount;

	char numbers[10][4];

	char time1[10][15];
	char time2[10][15];

	char nick1[10][15];
	char nick2[10][15];



	for (int i=0; i<10; i++) {
		float t1 = hi_times[level][i]+0.001; // rounding ;-)
		float t2 = Player[Config.currentplayer].times[level][i]+0.001;

		sprintf (numbers[i], "%2d.", i+1);

		if (t1 < (9*60.0+59.0) ) {
			sprintf (nick1[i], "%s", hi_nicks[level][i].c_str());
			sprintf (time1[i], "%d:%.2d.%.2d", (int)(t1/60), (int)(t1)%60, (int)(t1*100.0)%100);
		} else {
			sprintf (nick1[i], "-");
			sprintf (time1[i], "-");
		}
		if (t2 > 0.009) {
			sprintf (nick2[i], "%s", Player[Config.currentplayer].name);
			sprintf (time2[i], "%d:%.2d.%.2d", (int)(t2/60), (int)(t2)%60, (int)(t2*100.0)%100);
		} else {
			sprintf (nick2[i], "-");
			sprintf (time2[i], "-");
		}
	}



	menutext item[2]={menutext( (char*)levelname[level].c_str() ), menutext("BACK") };

	int selection = 1;

	item[1].sel = 1.0;
	item[1].target = 1.0;

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime;

	bool drawn=false;

	while (1) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}
			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}


			bool updatestats=false;
			if (Keyboard.Pressed(SDLK_LEFT)) {
				sound::PlayChunk(-1, Media.menuselect, 0);

				if (level==0) level=levelcount-1;
				else level--;

				updatestats=true;
#if DEBUG == 1
				fprintf (stderr, "Level: %d\n", level);
#endif
			}
			if (Keyboard.Pressed(SDLK_RIGHT)) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				level++;
				if (level >= levelcount) level -= levelcount;
				updatestats=true;
#if DEBUG == 1
				fprintf (stderr, "Level: %d\n", level);
#endif
			}

			if (updatestats) {

				item[0].settext( (char*)levelname[level].c_str() );
				for (int i=0; i<10; i++) {
					float t1 = hi_times[level][i]+0.001; // rounding ;-)
					float t2 = Player[Config.currentplayer].times[level][i]+0.001;

					if (t1 < (9*60.0+59.0) ) {
						sprintf (nick1[i], "%s", hi_nicks[level][i].c_str());
						sprintf (time1[i], "%d:%.2d.%.2d", (int)(t1/60), (int)(t1)%60, (int)(t1*100.0)%100);
					} else {
						sprintf (nick1[i], "-");
						sprintf (time1[i], "-");
					}

					if (t2 > 0.009) {
						sprintf (nick2[i], "%s", Player[Config.currentplayer].name);
						sprintf (time2[i], "%d:%.2d.%.2d", (int)(t2/60), (int)(t2)%60, (int)(t2*100.0)%100);
					} else {
						sprintf (nick2[i], "-");
						sprintf (time2[i], "-");
					}
				}

			}

		} // if

		// drawing section

		if (!drawn) {

			Graphics.clear();

			drawcanvas();

			for (int i=0; i<10; i++) {
				float y = Window.height/2-160+30*i;
				float s = 15.0;

				Font.WriteString( numbers[i], Window.width/2-312, y, s+0.5 );

				if (hi_times[level][i] < 9*60.0+59.0) {
					colortime(level, hi_times[level][i]);
				} else 
					#ifdef HAVE_GLES
					Graphics.color4f(1.0, 1.0, 1.0, 1.0);
					#else
					glColor4f(1.0, 1.0, 1.0, 1.0);
					#endif

				Font.WriteString( time1[i], Window.width/2-312+3*s, y, s+0.5 );
				Font.WriteString( nick1[i], Window.width/2-312+11*s, y, s+0.5 );

				if (Player[Config.currentplayer].times[level][i] > 0.005) {
					colortime(level, Player[Config.currentplayer].times[level][i]);
				} else
					#ifdef HAVE_GLES
					Graphics.color4f(1.0, 1.0, 1.0, 1.0);
					#else
					glColor4f(1.0, 1.0, 1.0, 1.0);
					#endif

				Font.WriteString( time2[i], Window.width/2-312+23*s, y, s+0.5 );
				Font.WriteString( nick2[i], Window.width/2-312+31*s, y, s+0.5 );

				#ifdef HAVE_GLES
				Graphics.color4f(1.0, 1.0, 1.0, 1.0);
				#else
				glColor4f(1.0, 1.0, 1.0, 1.0);
				#endif
				
			}

			item[0].display(Window.height/2-200);
			item[1].display(Window.height/2+200);

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}
}



bool menu::saverecord(Uint32 level) {
	const int items = 2;
	menutext item[2];
	item[0].settext("*FILE NAME:");
	item[1].settext("");

	char path[20+1];
	path[0]=0;
	int cursor=0;

	int selection=1;

	item[1].sel = 1.0;
	item[1].target = 1.0;

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_BACKSPACE) > 0 ) {
				cursor--;
				if (cursor<0) cursor=0;
				path[cursor]='\0';

				item[1].settext(path);
			}

			char ch = Keyboard.GetAscii();

			if (ch!=0 && ch !='\n' && ch !='\r' && ch != ' ') {

				if (ch >= 'a' && ch <= 'z') ch -= 'a'-'A';

				for (int i=0; i<big_num; i++) {
					if (big_ch[i]==ch) {
						if (cursor < 20) {
							path[cursor]=ch;
							cursor++;
							path[cursor]='\0';

							item[1].settext(path);

							break;
						}
					}
				} // for

			} // if


			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				// check that there is a name
				if (strlen(path) >= 1 && strcmp(path, "LAST") != 0 ) {

					char to[200];
					sprintf (to, Path->priv("records/%s.brc"), path);

					int ok=1;
					if (compat::file_exists(to)) {
						ok = state("*OVERWRITE?", 1);
					}
					if (ok == 1) {
						copylevel( Path->priv("records/last.brc"), to);
						return true;
					}

				}
				nextframe = SDL_GetTicks();
			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return false;
			}


		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();
			drawcanvas();

			float y = Window.height/2.0-40.0;
			for (int i=0; i<items; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}

	return false;
}



void menu::loadrecord() {
	const int items = 2;
	menutext item[2];
	item[0].settext("*FILE NAME:");
	item[1].settext("");

	char path[20+1];
	path[0]=0;
	int cursor=0;

	int selection=1;

	item[1].sel = 1.0;
	item[1].target = 1.0;

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_BACKSPACE) > 0 ) {
				cursor--;
				if (cursor<0) cursor=0;
				path[cursor]='\0';

				item[1].settext(path);
			}

			char ch = Keyboard.GetAscii();

			if (ch!=0 && ch !='\n' && ch !='\r' && ch != ' ') {

				if (ch >= 'a' && ch <= 'z') ch -= 'a'-'A';

				for (int i=0; i<big_num; i++) {
					if (big_ch[i]==ch) {
						if (cursor < 20) {
							path[cursor]=ch;
							cursor++;
							path[cursor]='\0';

							item[1].settext(path);

							break;
						}
					}
				} // for

			} // if


			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				// check that there is a name
				if (strlen(path) >= 1) {
					char buf[1200];
					sprintf (buf, "%s/records/%s.brc", Path->privdir, path);

					simulate(0, buf);
					message=0;

					focuscanvas();
					return;
				}

			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				message=0;
				return;
			}

		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();

			float y = Window.height/2.0-40.0;
			for (int i=0; i<items; i++) {
				item[i].display(y);
				y+=45;
			}

			drawcanvas();

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}

	message=0;
	return;
}


void menu::savetimes_player() {
	FILE *fp = fopen( Path->priv("players.dat"), "w");
	if (fp) {

#if DEBUG == 1
		fprintf (stderr, "Saving player data.\n");
#endif

		for (int i=0; i<6; i++) {

			if (Player[i].inuse) {
				fprintf (fp, "%s\n", Player[i].name);
				fprintf (fp, "%d\n", SWAP32(Player[i].tuxfound) );
				for (int u=0; u<MAXLEVELS; u++) {
					for (int v=0; v<10; v++) {
						fprintf (fp, "%f\n", Player[i].times[u][v]);
					}
				}


			} else {
				fprintf (fp, "\n");
				fprintf (fp, "0\n");
				for (int u=0; u<MAXLEVELS; u++) {
					for (int v=0; v<10; v++) {
						fprintf (fp, "%f\n", 0.0);
					}
				}
			}
		}
		fclose (fp);
	} else {
		fprintf (stderr, "Couldn't open \"%s/players.dat\" for writing.\n", Path->privdir);
	}

}


void menu::savetimes_public() {
	FILE *fp = fopen( Path->priv("levels.dat"), "w");

	if (fp) {

		for (int i=0; i<levelcount; i++) {
			fprintf (fp, "%s\n", levelname[i].c_str() );
			for (int j=0; j<10; j++) {
				fprintf (fp, "%s %f\n", hi_nicks[i][j].c_str(), hi_times[i][j] );
			}
		}

		fclose(fp);

	} else {
		fprintf (stderr, "Couldn't save \"%s\"\n", Path->priv("levels.dat") );
	}
}


void menu::allhigh() {
	Uint32 levels = levelcount;

	menutext item[MAXLEVELS+2];

	char time[MAXLEVELS][200];
	char nick[MAXLEVELS][200];
	char list[MAXLEVELS][200];

	item[0].settext("*SCOREBOARD");

	for (int i=0; i<levelcount; i++) {
		char str[200];
		float t1 = hi_times[i][0] + 0.001; // rounding ;-)

		sprintf (str, "%s", (char*)levelname[i].c_str() );

		item[i+1].settext( str );
	}


	for (int i=0; i<levelcount; i++) {
		float t1 = hi_times[i][0]+0.001;


		if (t1 < (9*60.0+59.0) ) {
			sprintf (list[i], "%s %d:%.2d.%.2d", (char*)hi_nicks[i][0].c_str(), (int)(t1/60), (int)(t1)%60, (int)(t1*100.0)%100);
		} else {
			sprintf (list[i], "-");
		}


	}


	item[levelcount+1].settext("BACK");


	int selection = 1;

	float miny = 0;
	float maxy = (levels+2)*45;


	for (int i=1; i<levels+2; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime;

	float y=Window.height/2.0-60.0;

	float mvm[50];
	for (int i=0; i<50; i++) mvm[i]=0.0;
	int mvmind=0;

	float mvmbig[50];
	for (int i=0; i<50; i++) mvmbig[i]=0.0;
	int mvmbigind=0;

	float h = 80.0;
	bool drawn=false;

	while (1) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				if (selection <= 1) {
					selection += levels;
					mvmbig[mvmbigind++] = -1.0;
					y -= levels*h;
				} else {
					selection--;
					mvm[mvmind++] = 1.0;
					y += h;
				}
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				if (selection >= levels+1) {
					selection -= levels;
					mvmbig[mvmbigind++] = 1.0;
					y += levels*h;
				} else {
					selection++;
					mvm[mvmind++] = -1.0;
					y -= h;
				}


			}

			if (mvmind>=50) mvmind=0;
			if (mvmbigind>=50) mvmbigind=0;

			for (int i=1; i<levels+2; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<levels+2; i++) item[i].act();

			if (Keyboard.Pressed(SDLK_RETURN)) {
				sound::PlayChunk(-1, Media.menuenter, 0);
				if (selection >= 1 && selection <= levels) {

					scoreboard(selection-1);

					for (int i=1; i<levels+2; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
					for (int i=1; i<levels+2; i++) item[i].target = item[i].sel;


					nextframe=SDL_GetTicks()+10;
				} else {

					if ( selection == levels+1 ) {
						return;
					}

				}
			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}


			for (int i=0; i<50; i++) {
				if (mvm[i] < 0) mvm[i]+=0.02;
				if (mvm[i] > 0) mvm[i]-=0.02;
				if (mvmbig[i] < 0) mvmbig[i]+=0.007;
				if (mvmbig[i] > 0) mvmbig[i]-=0.007;
			}

		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();
			drawcanvas();

			float y2=y;

			while (y2 > Window.height/2.0-60.0+h/2.0) y2 -= h*(levels+1.0);
			while (y2 < Window.height/2.0-60.0-h*levels-h/2.0) y2 += h*(levels+1.0);

			for (int i=0; i<50; i++) {
				float x = mvm[i];
				if (x >= 0.02) y2 -= h/2.0*(1-cos(3.14159*x));
				if (x <= -0.02) y2 += h/2.0*(1-cos(3.14159*x));

				x = mvmbig[i];
				if (x >= 0.02) y2 -= levels*h/2.0*(1-cos(3.14159*x));
				if (x <= -0.02) y2 += levels*h/2.0*(1-cos(3.14159*x));
			}

			int yellow=0;

			for (int i=0; i<levelcount+2; i++) {

				if (i>=1 && i<=levelcount) {
					int w = strlen(list[i-1]);

					colortime(i-1, hi_times[i-1][0]);
					Font.WriteString(list[i-1], Window.width/2-10*w, y2+20, 23);
					#ifdef HAVE_GLES
					Graphics.color4f(1.0, 1.0, 1.0, 1.0);
					#else
					glColor4f(1.0, 1.0, 1.0, 1.0);
					#endif
				}


				// colorings
				if (i >= 1 && i <= levels) {
					if (!cheatmode &&
						Player[Config.currentplayer].times[i-1][0] < 0.005)
					{
						yellow++;
						if (yellow <= 3) {
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 0.6, 1.0);
							#else
							glColor4f(1.0, 1.0, 0.6, 1.0);
							#endif
							item[i].display(y2);
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 1.0, 1.0);
							#else
							glColor4f(1.0, 1.0, 1.0, 1.0);
							#endif
						} else {
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 0.6, 0.6, 1.0);
							#else
							glColor4f(1.0, 0.6, 0.6, 1.0);
							#endif
							item[i].display(y2);
							#ifdef HAVE_GLES
							Graphics.color4f(1.0, 1.0, 1.0, 1.0);
							#else
							glColor4f(1.0, 1.0, 1.0, 1.0);
							#endif
						}
					} else {
						#ifdef HAVE_GLES
						Graphics.color4f(0.6, 1.0, 0.6, 1.0);
						#else
						glColor4f(0.6, 1.0, 0.6, 1.0);
						#endif
						item[i].display(y2);
						#ifdef HAVE_GLES
						Graphics.color4f(1.0, 1.0, 1.0, 1.0);
						#else
						glColor4f(1.0, 1.0, 1.0, 1.0);
						#endif
					}
				} else {
					item[i].display(y2);
				}
				y2+=h;

			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif

		} else SDL_Delay(1);

	}
}


void menu::viewrecordlist() {
	vector <menutext>item;
	vector <string>filename;

	item.push_back( menutext("*SELECT RECORD") );

	int recordcount=0;

	DIR *dir = opendir( Path->priv("records/") );
	if (!dir) return;

	// read files from "records/"

#if DEBUG == 1
	fprintf (stderr, "Directory opened.\n");
#endif

	while(1) {
		dirent *p = readdir(dir);
		if (p == NULL) break;

#if DEBUG == 1
		fprintf (stderr, "Read file. \"%s\"\n", p->d_name);
#endif

		if (strcmp(p->d_name, ".")!=0 && strcmp(p->d_name, "..")!=0 && strcmp(p->d_name, "last.brc")!=0 ) {
			if (compat::Str_CheckLastChars(p->d_name, ".brc")) {

#if DEBUG == 1
				fprintf (stderr, "Read file. \"%s\" 1\n", p->d_name);
#endif

				string n = string(p->d_name);
				n = n.substr(0, n.size()-4);

				item.push_back( menutext(n) );

				filename.push_back(string(n));

				recordcount++;
			}

		}

	}
	closedir(dir);

	item.push_back(menutext("BACK"));


	int selection = 1;

	float miny = 0;
	float maxy = (recordcount+2)*45;

	for (int i=1; i<recordcount+2; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime;

	float y=Window.height/2.0-60.0;

	float mvm[50];
	for (int i=0; i<50; i++) mvm[i]=0.0;
	int mvmind=0;

	float mvmbig[50];
	for (int i=0; i<50; i++) mvmbig[i]=0.0;
	int mvmbigind=0;

	float h = 45.0;
	bool drawn=false;

	while (1) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				if (selection <= 1) {
					selection += recordcount;
					mvmbig[mvmbigind++] = -1.0;
					y -= recordcount*h;
				} else {
					selection--;
					mvm[mvmind++] = 1.0;
					y += h;
				}
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				if (selection >= recordcount+1) {
					selection -= recordcount;
					mvmbig[mvmbigind++] = 1.0;
					y += recordcount*h;
				} else {
					selection++;
					mvm[mvmind++] = -1.0;
					y -= h;
				}


			}

			if (mvmind>=50) mvmind=0;
			if (mvmbigind>=50) mvmbigind=0;

			for (int i=1; i<recordcount+2; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<recordcount+2; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				if (selection >= 1 && selection <= recordcount) {
					char buf[1200];
					sprintf (buf, "%s/records/%s.brc", Path->privdir,
						(char*)(filename[selection-1].c_str()) );

					simulate(0, buf);

					for (int i=1; i<recordcount+2; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
					for (int i=1; i<recordcount+2; i++) item[i].target = item[i].sel;

					focuscanvas();

					nextframe=SDL_GetTicks()+10;
				} else {

					if ( selection == recordcount+1 ) {
						item.clear();
						filename.clear();
						return;
					}

				}
			}

			if ( Keyboard.Pressed(SDLK_DELETE) || Keyboard.Pressed(SDLK_BACKSPACE) ||
				Keyboard.Pressed(SDLK_d) )
			{
				sound::PlayChunk(-1, Media.menuenter, 0);
				if (selection >= 1 && selection <= recordcount) {
					int i = state("*DELETE RECORD?", 0);
					if (i) {
						char p[1000];
						sprintf (p, "%s/records/%s.brc", Path->privdir, filename[selection-1].c_str());
						remove( p );

#if DEBUG == 1
						fprintf (stderr, "Trying to delete \"%s\"\n", p);
#endif

						item.erase( item.begin() + selection  );
						filename.erase( filename.begin() + selection - 1);

						if ( selection  == recordcount+1 ) {
							selection--;
							y+=h;
						}
						recordcount--;

						if (selection <= 0) selection += recordcount;


					}
				}

				nextframe = SDL_GetTicks();

			}


			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);

				item.clear();
				filename.clear();
				return;
			}

			for (int i=0; i<50; i++) {
				if (mvm[i] < 0) mvm[i]+=0.02;
				if (mvm[i] > 0) mvm[i]-=0.02;
				if (mvmbig[i] < 0) mvmbig[i]+=0.01;
				if (mvmbig[i] > 0) mvmbig[i]-=0.01;
			}

		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();

			float y2=y;

			while (y2 > Window.height/2.0-60.0+h/2.0) y2 -= h*(recordcount+1.0);
			while (y2 < Window.height/2.0-60.0-h*recordcount-h/2.0) y2 += h*(recordcount+1.0);

			for (int i=0; i<50; i++) {
				float x = mvm[i];
				if (x >= 0.02) y2 -= h/2.0*(1-cos(3.14159*x));
				if (x <= -0.02) y2 += h/2.0*(1-cos(3.14159*x));

				x = mvmbig[i];
				if (x >= 0.02) y2 -= recordcount*h/2.0*(1-cos(3.14159*x));
				if (x <= -0.02) y2 += recordcount*h/2.0*(1-cos(3.14159*x));
			}

			drawcanvas();

			for (int i=0; i<item.size(); i++) {
				item[i].display(y2);
				y2+=h;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);
	}

}


void menu::copylevel(char *from, char *to) {
	FILE *infile = fopen(from, "rb");
	FILE *outfile = fopen(to, "wb");

	if (!infile) {
		fprintf (stderr, "Couldn't open \"%s\" for reading! Critical!\n", from);
		exit(1);
	}
	if (!outfile) {
		fprintf (stderr, "Couldn't open \"%s\" for writing! Critical!\n", to);
		exit(1);
	}

	// copy file

	while(1) {
		int q = fgetc(infile);
		if (q==EOF) break;
		fputc(q, outfile);
	}

	fclose(outfile);
	fclose(infile);
}

void menu::canvasphys() {
	for (int t=0; t<2; t++) {
		blobcanvas->settime(canvt);
		blobcanvas->springs_act(dt);
		blobcanvas->update(dt);
		blobcanvas->particles_act(dt, NULL, 0, canvt);

		canvt += dt;
	}

	if (canvalpha < 0.9) canvalpha += 0.0015;
}

void menu::drawcanvas() {

	if (Config.decoration == DEC_FULL) {
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);

		#ifdef HAVE_GLES
		GLfloat vtx1[] = {
			0,0,
			Window.width-1,0,
			Window.width-1,Window.height-1,
			0,Window.height-1
		};
		GLfloat col1[] = {
			0.35, 0.35, 0.75, 1.0f,
			0.35, 0.35, 0.75, 1.0f,
			0.1, 0.1, 0.5, 1.0f,
			0.1, 0.1, 0.5, 1.0f
		};
	 
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
	 
		glVertexPointer(2, GL_FLOAT, 0, vtx1);
		glColorPointer(4, GL_FLOAT, 0, col1);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
	 
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		#else
		glColor3f(0.35, 0.35, 0.75);

		glBegin(GL_QUADS);
		glVertex2f(0,0);
		glVertex2f(Window.width-1,0);

		glColor3f(0.1, 0.1, 0.5);

		glVertex2f(Window.width-1,Window.height-1);
		glVertex2f(0,Window.height-1);
		glEnd();
		#endif

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);


		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		#ifdef HAVE_GLES
		Graphics.color4f(1.0, 1.0, 1.0, canvalpha);
		#else
		glColor4f(1.0, 1.0, 1.0, canvalpha);
		#endif
		blobcanvas->display();
		#ifdef HAVE_GLES
		Graphics.color4f(1.0, 1.0, 1.0, 1.0);
		#else
		glColor4f(1.0, 1.0, 1.0, 1.0);
		#endif
	}

	char str[40];

	if (Config.currentplayer >= 0 && Player[Config.currentplayer].inuse) {
		sprintf (str, "%s", Player[Config.currentplayer].name);
		Font.WriteString(str, Window.width - strlen(str)*30-10, 10.0, 30);

		int secrets=0;
		int sec = Player[Config.currentplayer].tuxfound ^ ALL_TUX ;
		for (int i=0; i<32; i++) {
			if (sec & 0x01) secrets++;
			sec >>=1;
		}

		sprintf (str, "%d/6", 6-secrets);
		Font.WriteString(str, Window.width - strlen(str)*30-10, 40.0, 30);

	} else {
		Font.WriteString("-", Window.width - 40, 10.0, 30);
		Font.WriteString("-", Window.width - 40, 40.0, 30);
	}

}

void menu::focuscanvas() {
	Window.Viewarea(75, 50); // m
	Window.Center(0, 0); // m
}

void menu::reloadcanvas() {
	blobcanvas->reloadtexture();
}
void menu::unloadcanvas() {
	blobcanvas->unloadtexture();
}

void menu::loadtimes_public() {
	FILE *fp = fopen(Path->priv("levels.dat"), "r");
	if (!fp) {
		// copy levels.dat to private/
		string origpath = Path->data("defaults/private/levels.dat");
		string newpath = Path->priv("levels.dat");

		Path->copyfile( (char*)origpath.c_str(), (char*)newpath.c_str() );
#if DEBUG == 1
		fprintf (stderr, "Copied \"%s\" to \"%s\".\n", origpath.c_str(), newpath.c_str() );
#endif

		fp = fopen(Path->priv("levels.dat"), "r");
	}

	if (fp) {
		char str[200];
		char str2[200];
		float time=0.0;

		char *u=0;
		int s=0;

		levelcount = 0;
		while (levelcount < MAXLEVELS) {

			u = fgets(str, 40, fp);
			if (u == str) {
				s = sscanf(str, "%s\n", str2);
				if (s != 1) { // seems that we're having all the levels already then.
					break;
				}

				levelname[levelcount] = str2;
			} else {
				break;
			}

			for (int j=0; j<10; j++) {
				u = fgets(str, 40, fp);
				if (u == str) {
					s = sscanf(str, "%s %f\n", str2, &time);
					if (s != 2) {
						fprintf (stderr, "Error in \"%s\". Critical!\n", Path->priv("levels.dat") );
						exit(1);
					}
					hi_times[levelcount][j] = time;
					hi_nicks[levelcount][j] = str2;
				}
			}

			levelcount++;


		}

		fclose(fp);

	} else {
		fprintf (stderr, "Couldn't open \"%s/level.dat\". Critical!\n", Path->privdir);
		exit(1);
	}
}

void menu::controls() {
	const int items = 11;
	menutext item[items]={ menutext("*CONTROLS"), menutext("THRUST"), menutext("REVERSE"),
		menutext("C-CWISE"), menutext("CWISE"), menutext("JUMP"), menutext("TURN"),
		menutext("ZOOM IN"), menutext("ZOOM OUT"), menutext("PAUSE"), menutext("BACK") };

	int selection=1;

	for (int i=1; i<items; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += items-1;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection >= items) selection -= items-1;
			}

			for (int i=1; i<items; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<items; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);

				switch(selection) {
					case 1: // thrust
						setkey( KEY_THRUST, "*THRUST KEY?" );
					break;
					case 2: // reverse
						setkey( KEY_REVERSE, "*REVERSE KEY?" );
					break;
					case 3: // couner-clockwise
						setkey( KEY_CCW, "*C-CWISE KEY?" );
					break;
					case 4: // clockwise
						setkey( KEY_CW, "*CWISE KEY?" );
					break;
					case 5: // jump
						setkey( KEY_JUMP, "*JUMP KEY?" );
					break;
					case 6: // turn
						setkey( KEY_TURN, "*TURN KEY?" );
					break;
					case 7: // zoom in
						setkey( KEY_ZOOMIN, "*ZOOM IN KEY?" );
					break;
					case 8: // zoom out
						setkey( KEY_ZOOMOUT, "*ZOOM OUT KEY?" );
					break;
					case 9: // pause
						setkey( KEY_PAUSE, "*PAUSE KEY?" );
					break;
					case 10: // back
						return;
					break;
					default:
						return;
				}

				for (int i=1; i<items; i++) item[i].sel = exp(-2.5*(float)abs(selection-i));
				for (int i=1; i<items; i++) item[i].target = item[i].sel;

				nextframe=SDL_GetTicks()+10;

			}

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}


		} // if

		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();
			drawcanvas();

			float y = Window.height/2.0-156.0;
			for (int i=0; i<items; i++) {
				item[i].display(y);
				y+=33;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}
}


void menu::setkey( int num, char *dsc ){
	menutext item[2]={ menutext(dsc), menutext( Keyboard.getkeyname( Config.keys[num] ) ) };

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);


		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			for (Uint16 i=0; i<SDLK_LAST; i++) {
				int p = Keyboard.Pressed(i);
				if (p) {
					sound::PlayChunk(-1, Media.menuenter, 0);

					#ifdef PANDORA
					if ((i==SDLK_ESCAPE) || (i == SDLK_LALT)) return;
					#else
					if (i==SDLK_ESCAPE) return;
					#endif
					Config.keys[num] = i;

					return;
				}
			}

		}


		// drawing section

		if (!drawn) {
			drawn=true;

			Graphics.clear();

			drawcanvas();

			float y = Window.height/2.0-40.0;
			for (int i=0; i<2; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}

	return;
}
/*
void menu::netscores() {

#if NETWORK == 1

	// Ask about watching
	int watch = state("*MAKE PUBLIC?", 1);
	if (watch == -1) return;


	// Open network interface:
	handler Handler;
	Handler.start();
	Handler.udp_open(0);

	// resolve server address
	IPaddress servaddress;
	if ( SDLNet_ResolveHost(&servaddress, "bloboats.dy.fi", 4012) != 0) {
		fprintf (stderr, "Couldn't resolve host.\n");
		Handler.udp_close();
		Handler.stop();

		return;
	}

	SDLNet_UDP_Bind( Handler.get_sock(), 0, &servaddress);

	// send levels

	Uint32 c = Config.newtimes;

	for (int i=0; i<levelcount; i++, c>>=1) {

#if DEBUG == 1
		fprintf (stderr, "Testing whether to send the best time of level %d.\n");
#endif

		if (c & 1) { // not sent yet
			float time = hi_times[i][0];

			if ( time <= 3*rank_times[i][0] ) { // < 3 x green time, send!
#if DEBUG == 1
				fprintf (stderr, "Sending the best time of level %d.\n", i);
#endif

				char path[500];
				sprintf(path, "%s/ghost/meri%d.brc", Path->privdir, i);

				FILE *fp = fopen(path, "rb");
				if (!fp) continue;

				// read the file to a buffer
				Uint8 f[2048];
				Uint32 size=0;
				int p=0;
				for (; size<2048, (p = fgetc(fp)) != EOF; size++) {
					f[size] = (Uint8) p;
				}

				// make condition
				condition sfile (SENDFILE, NO_REQ, NO_VAR_LIST);

				sfile.add_uint16(size+1);

				sfile.add_uint8( (Uint8)watch );

				for (int k=0; k<size; k++) {
					sfile.add_uint8(f[k]);
				}

				Handler.stack_add(&sfile, 0);


			}

		}
	}

	Config.newtimes = 0;

	if (Handler.send(0) < 0) {
		fprintf (stderr, "Error: %s\n", SDLNet_GetError() );
		exit(1);
	}

	SDLNet_UDP_Unbind(Handler.get_sock(), 0);

	Handler.udp_close();
	Handler.stop();
#endif
}
*/
void menu::selectmusic(int i) {

#if DEBUG == 1
	fprintf (stderr, "Selecting music from %d to %d.\n", Config.music, i);
#endif

	if (i<0 || i >= NUM_MUSIC) return;
	if (Config.music == i) {
		return;
	} else { // changing music
		sound::FadeOutMusic(500);

		sound::FadeInMusic_select(Media.music[i], -1, 500);

#if SOUND == 1
		Mix_HookMusicFinished(sound::FadeInMusic);
#endif

		Config.music = i;

	}
}

void menu::soundoptions() {
	const int items = 4;
	menutext item[items]={ menutext("*SOUND OPTIONS:"), menutext(""), menutext(""), menutext("BACK") };

	int selection=1;

	for (int i=1; i<items; i++) {
		item[i].sel = exp(-2.5*(float)abs(selection-i));
		item[i].target = item[i].sel;
	}

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime + 10;

	bool ready=false;
	bool drawn=false;

	while (!ready) {
		Keyboard.HandleInterrupts();
		SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			nextframe += 10;
			drawn=false;

			canvasphys();

			if (Keyboard.Pressed(SDLK_UP) && !Keyboard.Pressed(SDLK_DOWN) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection--;
				if (selection < 1) selection += items-1;
			}
			if (Keyboard.Pressed(SDLK_DOWN) && !Keyboard.Pressed(SDLK_UP) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				selection++;
				if (selection >= items) selection -= items-1;
			}

			for (int i=1; i<items; i++) item[i].target = exp(-2.5*(float)abs(selection-i));
			for (int i=1; i<items; i++) item[i].act();

			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_RETURN)) || (Keyboard.Pressed(SDLK_PAGEDOWN)) || (Keyboard.Pressed(SDLK_END))) {
			#else
			if (Keyboard.Pressed(SDLK_RETURN)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				if (selection == 3) return;
			}

			if (Keyboard.Pressed(SDLK_LEFT) && !Keyboard.Pressed(SDLK_RIGHT) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				if (selection == 1) { // less music volume
					Config.musicvol -= 1;
					if (Config.musicvol < 0) Config.musicvol = 0;
					sound::MusicVolume( (int)(12.8*Config.musicvol) );
				}
				if (selection == 2) { // less sound volume
					Config.soundvol -= 1;
					if (Config.soundvol < 0) Config.soundvol = 0;
					sound::SoundVolume( (int)(12.8*Config.soundvol) );
				}
			}
			if (Keyboard.Pressed(SDLK_RIGHT) && !Keyboard.Pressed(SDLK_LEFT) ) {
				sound::PlayChunk(-1, Media.menuselect, 0);
				if (selection == 1) { // less music volume
					Config.musicvol += 1;
					if (Config.musicvol >= 10) Config.musicvol = 10;
					sound::MusicVolume( (int)(12.8*Config.musicvol) );
				}
				if (selection == 2) { // less sound volume
					Config.soundvol += 1;
					if (Config.soundvol >= 10) Config.soundvol = 10;
					sound::SoundVolume( (int)(12.8*Config.soundvol) );
				}
			}


			#ifdef PANDORA
			if ((Keyboard.Pressed(SDLK_ESCAPE))|| (Keyboard.Pressed(SDLK_LALT))) {
			#else
			if (Keyboard.Pressed(SDLK_ESCAPE)) {
			#endif
				sound::PlayChunk(-1, Media.menuenter, 0);
				return;
			}


		} // if

		if (!drawn) {
			drawn=true;

			Graphics.clear();

			drawcanvas();

			// volume displays
			char str[20];
			if (Config.musicvol == 10) sprintf (str, "MUSICVOL 10");
			else sprintf (str, "MUSICVOL %d ", Config.musicvol);
			item[1].settext(str);
			if (Config.soundvol == 10) sprintf (str, "SOUNDVOL 10");
			else sprintf (str, "SOUNDVOL %d ", Config.soundvol);
			item[2].settext(str);

			float y = Window.height/2.0-40;
			for (int i=0; i<items; i++) {
				item[i].display(y);
				y+=45;
			}

			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif
		} else SDL_Delay(1);

	}
}
