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

#ifndef _MENU_H_
#define _MENU_H_

#include "SDL.h"
#include "game.h"
#include "models/paatti.h"
#include "models/canvas.h"

#include "config.h"
#include "player.h"

#include <iostream>
#include <string>

#include "mouse.h"
#include "keyboard.h"
#include "font.h"

using std::string;

extern mouse Mouse;
extern graphics Graphics;
extern window Window;
extern keyboard Keyboard;
extern font Font;
extern font Font_bl;
extern config Config;

#define MSG_NOTHING 0
#define MSG_TIME 1
#define MSG_RECORD 2
#define MSG_TOPTEN 3
#define MSG_FAILED 4

class menu {
public:
	menu();
	~menu();

	void mainmenu();
	bool quit(bool escquits);
	void playgame();
	void simulate(Uint32 level, char *record);
	void options();
	void resolution();
	void decoration();
	bool selectplayer();
	bool entername(int playernum);
	bool eraseplayer(int playernum);
	void levelstats(Uint32 level);

	void colortime(Uint32 level, float time);

	void scoreboard(Uint32 level); 

	bool saverecord(Uint32 level);

	void loadrecord();
	void showghost();

	void savetimes_player();
	void savetimes_public();

	void allhigh();

	void viewrecordlist();

	void copylevel(char *from, char *to);

	void drawcanvas();
	void focuscanvas();
	void canvasphys();
	void reloadcanvas();
	void unloadcanvas();

	void loadtimes_public();

	int state(char *text, int defstate);

	void controls();
	void setkey( int num, char *dsc );

	void netscores();

	void selectmusic(int i);

	void soundoptions();



	player Player[6];

	string levelname[MAXLEVELS];
	float hi_times[MAXLEVELS][10];
	string hi_nicks[MAXLEVELS][10];
	float rank_times[MAXLEVELS][2];
	Uint32 levelcount;

	int message;
	float msgtime;

	bool lastrecord;
	double canvt;
	double canvalpha;

	canvas *blobcanvas;

	bool cheatmode;

};





#endif
