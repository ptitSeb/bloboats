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

#include "keyboard.h"

#include "config.h"
extern config Config;

#include "SDL_keyboard.h"

keyboard::keyboard() {
	setkeynames();
}

keyboard::~keyboard() {
}

void keyboard::GotQuit() {
	fprintf (stderr, "Quitting..\n");
	exit(1);
}

bool keyboard::Hold(Uint16 key){
	keys=SDL_GetKeyState(NULL);
	if (keys[key]) {
		keydown[key]=true;
		return true;
	}
	else {
		keydown[key]=false;
		return false;
	}

}

void keyboard::HandleInterrupts() {
	keys=SDL_GetKeyState(NULL);
	while (SDL_PollEvent(&Event)) {
		if (Event.type == SDL_QUIT) {
			GotQuit();
		}

		if (Event.type == SDL_VIDEOEXPOSE) {
			if (!Window.OpenWindow(Window.width, Window.height, Window.bpp, Window.oldflags) ) {
				fprintf (stderr, "Window corrupted and can't reopen!\n");
				exit(1);
			}
			Graphics.reloadtextures();
		}

		if (keys[SDLK_ESCAPE] && keys[SDLK_F10]) {
			GotQuit();
		}
		if ( keys[SDLK_ESCAPE] && keys[SDLK_LCTRL] ) {
			Window.Iconify();
		}
		if ( ( Hold(SDLK_LALT) || Hold(SDLK_RALT)) && Pressed(SDLK_RETURN) ) {
			Window.ToggleFullscreen();
			Graphics.reloadtextures();
			Config.fullscreen = !Config.fullscreen;
		}
	}
}

void keyboard::ResetEvents() {
	SDL_PumpEvents();
	HandleInterrupts();
}

bool keyboard::KeyPressed() {
	while (SDL_PollEvent(&Event)) {
		if (Event.type == SDL_KEYDOWN) return 1;
	}
	return 0;
}

void keyboard::WaitForKeyPress() {
	keys=SDL_GetKeyState(NULL);
	while(1) {
		while (SDL_PollEvent(&Event)) {
			if (Event.type == SDL_QUIT) {
				GotQuit();
			}
			if (keys[SDLK_ESCAPE] && keys[SDLK_F10]) {
				GotQuit();
			} else
			if ( keys[SDLK_ESCAPE] && keys[SDLK_LCTRL] ) {
				Window.Iconify();
			} else
			if ( Pressed(SDLK_F12) ) {
				Window.ToggleFullscreen();
			} else
			if (Event.type == SDL_KEYDOWN) return;
		}
	}
}

void keyboard::ResetReleased() {
	keys=SDL_GetKeyState(NULL);
	for (int i=0; i<SDLK_LAST; i++) {
		keydown[i]=false;
	}
}

void keyboard::ResetDouble() {
	for (int i=0; i<SDLK_LAST; i++) {
		keydouble[i]=0;
	}
}

void keyboard::ResetAll() {
	ResetReleased();
	ResetDouble();
}

int keyboard::GetAscii() {

	SDL_PumpEvents();

	while (SDL_PollEvent(&Event)) {
		if (Event.type == SDL_QUIT) {
			GotQuit();
		}

		if (keys[SDLK_ESCAPE] && keys[SDLK_F10]) {
			GotQuit();
		}
		if ( keys[SDLK_ESCAPE] && keys[SDLK_LCTRL] ) {
			Window.Iconify();
		}
		if ( Pressed(SDLK_F12) ) {
			Window.ToggleFullscreen();
		}

		if (Event.type == SDL_KEYDOWN) {
			if ( (Event.key.keysym.unicode & 0xFF80) == 0 ) { // 1111111110000000
			 	return Event.key.keysym.unicode & 0x7F; // 0000000001111111
			}
			else {
				switch (Event.key.keysym.unicode) {
					case 0xC5:/* Å */
						return 127;
					case 0xC4:/* Ä */
						return 128;
					case 0xD6:/* Ö */
						return 129;
					case 0xE5:/* å */
						return 130;
					case 0xE4:/* ä */
						return 131;
					case 0xF6:/* ö */
						return 132;
					default:
						return 0;

				}


			}
		}
	}

	return 0;
}


char keyboard::Released(Uint16 key) {
	if (keys[key]) {					// if not pressed, but WAS pressed -> click
		if (!keydown[key]) {			// if pressed first time, report it with -1.
			keydown[key]=true;
			return -1;
		}
	} else {
		if (keydown[key]==true) {
			keydown[key]=false;
			return 1;
		}
	}
	return 0;
}

int keyboard::Pressed(Uint16 key) {
	SDL_PumpEvents();

	if (key == SDLK_NUMLOCK || key == SDLK_CAPSLOCK || key == SDLK_SCROLLOCK) return 0;

	if (keys[key]) {						// if pressed, but was NOT pressed -> click
		if (keydown[key]==false) {
			keydown[key]=true;
			return 1;
		}

		/* To make it repeat if pressed for a long time */
		else keydown[key]++;

		if (keydown[key]>=REPEATDELAY1) {
			keydown[key]-=REPEATDELAY2;
			return 2;
		}
	} else {
		keydown[key]=false;
	}
	return 0;
}

bool keyboard::Double(Uint16 key) {
	if (Pressed(key)) keydouble[key]++;
	if (keydouble[key]==2) {
		keydouble[key]=0;
		return 1;
	}
	return 0;
}

char *keyboard::getkeyname(Uint16 key) {
	const char *kn;

	if (key < SDLK_LAST) kn = keynames[key];
	else kn = "unknown key";

	return (char*)kn;
}

void keyboard::setkeynames() {
	// mostly from SDL_keyboard.h

	keynames[SDLK_BACKSPACE] = "backspace";
	keynames[SDLK_TAB] = "tab";
	keynames[SDLK_CLEAR] = "clear";
	keynames[SDLK_RETURN] = "return";
	keynames[SDLK_PAUSE] = "pause";
	keynames[SDLK_ESCAPE] = "escape";
	keynames[SDLK_SPACE] = "space";
	keynames[SDLK_EXCLAIM]  = "exclamation mark";
	keynames[SDLK_QUOTEDBL]  = "\"";
	keynames[SDLK_HASH]  = "hash";
	keynames[SDLK_DOLLAR]  = "dollar";
	keynames[SDLK_AMPERSAND]  = "et";
	keynames[SDLK_QUOTE] = "'";
	keynames[SDLK_LEFTPAREN] = "(";
	keynames[SDLK_RIGHTPAREN] = ")";
	keynames[SDLK_ASTERISK] = "asterisk";
	keynames[SDLK_PLUS] = "plus";
	keynames[SDLK_COMMA] = ",";
	keynames[SDLK_MINUS] = "minus";
	keynames[SDLK_PERIOD] = ".";
	keynames[SDLK_SLASH] = "/";
	keynames[SDLK_0] = "0";
	keynames[SDLK_1] = "1";
	keynames[SDLK_2] = "2";
	keynames[SDLK_3] = "3";
	keynames[SDLK_4] = "4";
	keynames[SDLK_5] = "5";
	keynames[SDLK_6] = "6";
	keynames[SDLK_7] = "7";
	keynames[SDLK_8] = "8";
	keynames[SDLK_9] = "9";
	keynames[SDLK_COLON] = ":";
	keynames[SDLK_SEMICOLON] = ";";
	keynames[SDLK_LESS] = "less than";
	keynames[SDLK_EQUALS] = "=";
	keynames[SDLK_GREATER] = "greater than";
	keynames[SDLK_QUESTION] = "?";
	keynames[SDLK_AT] = "@";
	keynames[SDLK_LEFTBRACKET] = "left bracket";
	keynames[SDLK_BACKSLASH] = "backslash";
	keynames[SDLK_RIGHTBRACKET] = "right bracket";
	keynames[SDLK_CARET] = "caret";
	keynames[SDLK_UNDERSCORE] = "underscore";
	keynames[SDLK_BACKQUOTE] = "backquote";
	keynames[SDLK_a] = "a";
	keynames[SDLK_b] = "b";
	keynames[SDLK_c] = "c";
	keynames[SDLK_d] = "d";
	keynames[SDLK_e] = "e";
	keynames[SDLK_f] = "f";
	keynames[SDLK_g] = "g";
	keynames[SDLK_h] = "h";
	keynames[SDLK_i] = "i";
	keynames[SDLK_j] = "j";
	keynames[SDLK_k] = "k";
	keynames[SDLK_l] = "l";
	keynames[SDLK_m] = "m";
	keynames[SDLK_n] = "n";
	keynames[SDLK_o] = "o";
	keynames[SDLK_p] = "p";
	keynames[SDLK_q] = "q";
	keynames[SDLK_r] = "r";
	keynames[SDLK_s] = "s";
	keynames[SDLK_t] = "t";
	keynames[SDLK_u] = "u";
	keynames[SDLK_v] = "v";
	keynames[SDLK_w] = "w";
	keynames[SDLK_x] = "x";
	keynames[SDLK_y] = "y";
	keynames[SDLK_z] = "z";
	keynames[SDLK_DELETE] = "delete";

	keynames[SDLK_WORLD_0] = "world 0";
	keynames[SDLK_WORLD_1] = "world 1";
	keynames[SDLK_WORLD_2] = "world 2";
	keynames[SDLK_WORLD_3] = "world 3";
	keynames[SDLK_WORLD_4] = "world 4";
	keynames[SDLK_WORLD_5] = "world 5";
	keynames[SDLK_WORLD_6] = "world 6";
	keynames[SDLK_WORLD_7] = "world 7";
	keynames[SDLK_WORLD_8] = "world 8";
	keynames[SDLK_WORLD_9] = "world 9";
	keynames[SDLK_WORLD_10] = "world 10";
	keynames[SDLK_WORLD_11] = "world 11";
	keynames[SDLK_WORLD_12] = "world 12";
	keynames[SDLK_WORLD_13] = "world 13";
	keynames[SDLK_WORLD_14] = "world 14";
	keynames[SDLK_WORLD_15] = "world 15";
	keynames[SDLK_WORLD_16] = "world 16";
	keynames[SDLK_WORLD_17] = "world 17";
	keynames[SDLK_WORLD_18] = "world 18";
	keynames[SDLK_WORLD_19] = "world 19";
	keynames[SDLK_WORLD_20] = "world 20";
	keynames[SDLK_WORLD_21] = "world 21";
	keynames[SDLK_WORLD_22] = "world 22";
	keynames[SDLK_WORLD_23] = "world 23";
	keynames[SDLK_WORLD_24] = "world 24";
	keynames[SDLK_WORLD_25] = "world 25";
	keynames[SDLK_WORLD_26] = "world 26";
	keynames[SDLK_WORLD_27] = "world 27";
	keynames[SDLK_WORLD_28] = "world 28";
	keynames[SDLK_WORLD_29] = "world 29";
	keynames[SDLK_WORLD_30] = "world 30";
	keynames[SDLK_WORLD_31] = "world 31";
	keynames[SDLK_WORLD_32] = "world 32";
	keynames[SDLK_WORLD_33] = "world 33";
	keynames[SDLK_WORLD_34] = "world 34";
	keynames[SDLK_WORLD_35] = "world 35";
	keynames[SDLK_WORLD_36] = "world 36";
	keynames[SDLK_WORLD_37] = "world 37";
	keynames[SDLK_WORLD_38] = "world 38";
	keynames[SDLK_WORLD_39] = "world 39";
	keynames[SDLK_WORLD_40] = "world 40";
	keynames[SDLK_WORLD_41] = "world 41";
	keynames[SDLK_WORLD_42] = "world 42";
	keynames[SDLK_WORLD_43] = "world 43";
	keynames[SDLK_WORLD_44] = "world 44";
	keynames[SDLK_WORLD_45] = "world 45";
	keynames[SDLK_WORLD_46] = "world 46";
	keynames[SDLK_WORLD_47] = "world 47";
	keynames[SDLK_WORLD_48] = "world 48";
	keynames[SDLK_WORLD_49] = "world 49";
	keynames[SDLK_WORLD_50] = "world 50";
	keynames[SDLK_WORLD_51] = "world 51";
	keynames[SDLK_WORLD_52] = "world 52";
	keynames[SDLK_WORLD_53] = "world 53";
	keynames[SDLK_WORLD_54] = "world 54";
	keynames[SDLK_WORLD_55] = "world 55";
	keynames[SDLK_WORLD_56] = "world 56";
	keynames[SDLK_WORLD_57] = "world 57";
	keynames[SDLK_WORLD_58] = "world 58";
	keynames[SDLK_WORLD_59] = "world 59";
	keynames[SDLK_WORLD_60] = "world 60";
	keynames[SDLK_WORLD_61] = "world 61";
	keynames[SDLK_WORLD_62] = "world 62";
	keynames[SDLK_WORLD_63] = "world 63";
	keynames[SDLK_WORLD_64] = "world 64";
	keynames[SDLK_WORLD_65] = "world 65";
	keynames[SDLK_WORLD_66] = "world 66";
	keynames[SDLK_WORLD_67] = "world 67";
	keynames[SDLK_WORLD_68] = "world 68";
	keynames[SDLK_WORLD_69] = "world 69";
	keynames[SDLK_WORLD_70] = "world 70";
	keynames[SDLK_WORLD_71] = "world 71";
	keynames[SDLK_WORLD_72] = "world 72";
	keynames[SDLK_WORLD_73] = "world 73";
	keynames[SDLK_WORLD_74] = "world 74";
	keynames[SDLK_WORLD_75] = "world 75";
	keynames[SDLK_WORLD_76] = "world 76";
	keynames[SDLK_WORLD_77] = "world 77";
	keynames[SDLK_WORLD_78] = "world 78";
	keynames[SDLK_WORLD_79] = "world 79";
	keynames[SDLK_WORLD_80] = "world 80";
	keynames[SDLK_WORLD_81] = "world 81";
	keynames[SDLK_WORLD_82] = "world 82";
	keynames[SDLK_WORLD_83] = "world 83";
	keynames[SDLK_WORLD_84] = "world 84";
	keynames[SDLK_WORLD_85] = "world 85";
	keynames[SDLK_WORLD_86] = "world 86";
	keynames[SDLK_WORLD_87] = "world 87";
	keynames[SDLK_WORLD_88] = "world 88";
	keynames[SDLK_WORLD_89] = "world 89";
	keynames[SDLK_WORLD_90] = "world 90";
	keynames[SDLK_WORLD_91] = "world 91";
	keynames[SDLK_WORLD_92] = "world 92";
	keynames[SDLK_WORLD_93] = "world 93";
	keynames[SDLK_WORLD_94] = "world 94";
	keynames[SDLK_WORLD_95] = "world 95";

	keynames[SDLK_KP0] = "keypad 0";
	keynames[SDLK_KP1] = "keypad 1";
	keynames[SDLK_KP2] = "keypad 2";
	keynames[SDLK_KP3] = "keypad 3";
	keynames[SDLK_KP4] = "keypad 4";
	keynames[SDLK_KP5] = "keypad 5";
	keynames[SDLK_KP6] = "keypad 6";
	keynames[SDLK_KP7] = "keypad 7";
	keynames[SDLK_KP8] = "keypad 8";
	keynames[SDLK_KP9] = "keypad 9";
	keynames[SDLK_KP_PERIOD] = "keypad .";
	keynames[SDLK_KP_DIVIDE] = "keypad divide";
	keynames[SDLK_KP_MULTIPLY] = "keypad multiply";
	keynames[SDLK_KP_MINUS] = "keypad minus";
	keynames[SDLK_KP_PLUS] = "keypad plus";
	keynames[SDLK_KP_ENTER] = "keypad enter";
	keynames[SDLK_KP_EQUALS] = "keypad equals";

	keynames[SDLK_UP] = "up";
	keynames[SDLK_DOWN] = "down";
	keynames[SDLK_RIGHT] = "right";
	keynames[SDLK_LEFT] = "left";
	keynames[SDLK_DOWN] = "down";
	keynames[SDLK_INSERT] = "insert";
	#ifdef PANDORA
	keynames[SDLK_HOME] = "(A)";
	keynames[SDLK_END] = "(B)";
	keynames[SDLK_PAGEUP] = "(Y)";
	keynames[SDLK_PAGEDOWN] = "(X)";
	#else
	keynames[SDLK_HOME] = "home";
	keynames[SDLK_END] = "end";
	keynames[SDLK_PAGEUP] = "page up";
	keynames[SDLK_PAGEDOWN] = "page down";
	#endif

	keynames[SDLK_F1] = "f1";
	keynames[SDLK_F2] = "f2";
	keynames[SDLK_F3] = "f3";
	keynames[SDLK_F4] = "f4";
	keynames[SDLK_F5] = "f5";
	keynames[SDLK_F6] = "f6";
	keynames[SDLK_F7] = "f7";
	keynames[SDLK_F8] = "f8";
	keynames[SDLK_F9] = "f9";
	keynames[SDLK_F10] = "f10";
	keynames[SDLK_F11] = "f11";
	keynames[SDLK_F12] = "f12";
	keynames[SDLK_F13] = "f13";
	keynames[SDLK_F14] = "f14";
	keynames[SDLK_F15] = "f15";

	keynames[SDLK_NUMLOCK] = "numlock";
	keynames[SDLK_CAPSLOCK] = "caps lock";
	keynames[SDLK_SCROLLOCK] = "scroll lock";
	#ifdef PANDORA
	keynames[SDLK_RSHIFT] = "(left trigger)";
	keynames[SDLK_RCTRL] = "(right trigger)";
	#else
	keynames[SDLK_RSHIFT] = "right shift";
	keynames[SDLK_RCTRL] = "right ctrl";
	#endif
	keynames[SDLK_LSHIFT] = "left shift";
	keynames[SDLK_RALT] = "right alt";
	#ifdef PANDORA
	keynames[SDLK_LALT] = "(start)";
	keynames[SDLK_LCTRL] = "(select)";
	#else
	keynames[SDLK_LALT] = "left alt";
	keynames[SDLK_LCTRL] = "left ctrl";
	#endif
	keynames[SDLK_RMETA] = "right meta";
	keynames[SDLK_LMETA] = "left meta";
	keynames[SDLK_LSUPER] = "left super";	/* "Windows" keys */
	keynames[SDLK_RSUPER] = "right super";	
	keynames[SDLK_MODE] = "alt gr";
	keynames[SDLK_COMPOSE] = "compose";

	keynames[SDLK_HELP] = "help";
	keynames[SDLK_PRINT] = "print screen";
	keynames[SDLK_SYSREQ] = "sys req";
	keynames[SDLK_BREAK] = "break";
	keynames[SDLK_MENU] = "menu";
	keynames[SDLK_POWER] = "power";
	keynames[SDLK_EURO] = "euro";
	keynames[SDLK_UNDO] = "undo";

}
