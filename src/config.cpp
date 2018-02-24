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

#include "SDL.h"

#include "config.h"

#include "path.h"
extern path *Path;

#include "compiling_settings.h"

config::config() {
}

void config::load() {
	reswidth = SDL_GetVideoInfo()->current_w;
	resheight = SDL_GetVideoInfo()->current_h;

	decoration=2;
	currentplayer=-1;
	showghost=true;
	sounds=true;

	nolimit=false;

	fullscreen = 1;

	music=0;
	fps=0;

	musicvol = 6;
	soundvol = 10;

	newtimes = 0;

	tux = 0;

	zoomlevel=1.0;

	#ifdef PANDORA
	keys[KEY_THRUST]=SDLK_PAGEDOWN;
	keys[KEY_REVERSE]=SDLK_PAGEUP;
	keys[KEY_CCW]=SDLK_LEFT;
	keys[KEY_CW]=SDLK_RIGHT;
	keys[KEY_JUMP]=SDLK_END;
	keys[KEY_TURN]=SDLK_HOME;
	keys[KEY_ZOOMIN]=SDLK_RSHIFT;
	keys[KEY_ZOOMOUT]=SDLK_RCTRL;
	keys[KEY_PAUSE]=SDLK_LALT;
	#else
	keys[KEY_THRUST]=SDLK_UP;
	keys[KEY_REVERSE]=SDLK_DOWN;
	keys[KEY_CCW]=SDLK_LEFT;
	keys[KEY_CW]=SDLK_RIGHT;
	keys[KEY_JUMP]=SDLK_c;
	keys[KEY_TURN]=SDLK_SPACE;
	keys[KEY_ZOOMIN]=SDLK_f;
	keys[KEY_ZOOMOUT]=SDLK_r;
	keys[KEY_PAUSE]=SDLK_p;
	#endif

#if DEBUG == 1
	fprintf(stderr, "Loading config.\n");
#endif

	FILE *fp = fopen( Path->priv("config.dat"), "r");
	if (fp) {
	    unsigned int temp;
		fscanf(fp, "resolution %d\n", &temp);
		if ( temp <= 65535 ) {
		    reswidth = SDL_GetVideoInfo()->current_w;
		    resheight = SDL_GetVideoInfo()->current_h;
		} else {
            reswidth = (temp & 0xFFFF0000) >> 16;
            resheight = temp & 0x0000FFFF;
        }

		int tmp=0;

		if ( fscanf(fp, "decoration %d\n", &decoration) == 1) {
			if (decoration < 0 || decoration > 2) decoration = 2;
		}

		if ( fscanf(fp, "currentplayer %d\n", &currentplayer) == 1) {
			if (currentplayer < 0 || currentplayer > 5) currentplayer=0;
		}

		if ( fscanf(fp, "showghost %d\n", &tmp) == 1) {
			if (tmp==1) showghost=true;
			else showghost=false;
		}


		if ( fscanf(fp, "sounds %d\n", &tmp) == 1) {
			if (tmp==1) sounds=true;
			else sounds=false;
		}

		for (int i=0; i<9; i++) {
			if ( fscanf(fp, "key %d\n", &tmp) == 1) {
				keys[i]=tmp;
#if DEBUG == 1
				fprintf(stderr, "Read: key%d %d\n", i, tmp);
#endif
			}
		}

		if (fscanf(fp, "zoomlevel %f\n", &zoomlevel) != 1) {
			zoomlevel = 1.0;
		}

		if (fscanf(fp, "newtimes %d\n", &newtimes) != 1) {
			newtimes = 0;
		}

		if (fscanf(fp, "music %d\n", &music) != 1) {
			music = 0;
		}

		if (fscanf(fp, "musicvol %d\n", &musicvol) != 1) {
			musicvol = 10;
		}

		if (fscanf(fp, "soundvol %d\n", &soundvol) != 1) {
			soundvol = 10;
		}

		if (fscanf(fp, "fullscreen %d\n", &tmp) != 1) {
			fullscreen = 1;
		} else {
			fullscreen = (bool)tmp;
		}

		if (fscanf(fp, "fps %d\n", &fps) != 1) {
			fps = 0;
		}

		fclose(fp);

#if DEBUG == 1
		fprintf(stderr, "Config loaded.\n");
#endif

	} else {

#if DEBUG == 1
		fprintf(stderr, "Couldn't load config.\n");
#endif

	}

}


void config::save() {

#if DEBUG == 1
	fprintf(stderr, "Saving config.\n");
#endif

	FILE *fp = fopen( Path->priv("config.dat"), "w+");
	if (fp) {
		fprintf (fp, "resolution %d\n", (reswidth << 16) | resheight);
		fprintf (fp, "decoration %d\n", decoration);
		fprintf (fp, "currentplayer %d\n", currentplayer);
		fprintf (fp, "showghost %d\n", showghost);
		fprintf (fp, "sounds %d\n", sounds);

		for (int i=0; i<9; i++) {
			fprintf (fp, "key %d\n", keys[i]);
		}

		fprintf (fp, "zoomlevel %f\n", zoomlevel);
		fprintf (fp, "newtimes %d\n", newtimes);
		fprintf (fp, "music %d\n", music);

		fprintf (fp, "musicvol %d\n", musicvol);
		fprintf (fp, "soundvol %d\n", soundvol);

		fprintf (fp, "fullscreen %d\n", fullscreen);
		fprintf (fp, "fps %d\n", fps);

		fclose(fp);

#if DEBUG == 1
		fprintf(stderr, "Config saved.\n");
#endif
	} else {
#if DEBUG == 1
		fprintf (stderr, "Couldn't save config.\n");
#endif
	}

}
