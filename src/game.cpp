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

#include "game.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "keyboard.h"
#include "window.h"
#include "graphics.h"
#include "mouse.h"
#include "font.h"
#include "config.h"
#include "media.h"
#include "water.h"

#include "compiling_settings.h"

#include "path.h"
extern path *Path;

extern mouse Mouse;
extern graphics Graphics;
extern window Window;
extern keyboard Keyboard;
extern font Font;
extern font Font_bl;
extern config Config;
extern media Media;
extern water Water;

game::game() {
	malli=0;
	taso=0;
	goal=0;

	ghostboat=0;

	record=0;
	ghost=0;
	player=0;

	mallidead=0;
	ghostdead=0;

	tuxfound = false;

}

game::~game() {
	delete malli;
	delete ghostboat;
	delete goal;

	delete taso;

	delete mallidead;
	delete ghostdead;

	std::vector<model*>::iterator it;
	for (it = enemies.begin(); it != enemies.end(); it++) {
		delete *it;
	}

	enemies.clear();
}

void game::setrecord(element *first) {
	record = 1;
	recelement = first;
}

void game::setghost(element *first, float alpha) {
	ghost=true;
	ghostalpha=alpha;

	if (!first) {
		ghost=false;

#if DEBUG == 1
		fprintf (stderr, "Error: no ghost record data elements.\n");
#endif

	}
	ghostelement = first;

}

void game::makeship(float x, float y, float vx, float vy, float angle, char *tex) {
	delete malli;

	malli = new paatti(x, y, angle, 0.4, tex );
	for (int i=0; i<malli->numparticles; i++) {
		malli->p[i].v = vec(vx,vy);
	}

	player=1;
}

void game::maketentacle(float x, float y, float angle, float scale, char *tex) {
	enemies.push_back(new hirvio(x, y, angle, 0.2*scale, tex) );
#if DEBUG == 1
	fprintf (stderr, "Tentacle monster created.\n");
#endif
}

void game::makeinvader(float x, float y, float minx, float maxx, char *tex) {
	enemies.push_back(new invader(x, y, minx, maxx, tex) );
#if DEBUG == 1
	fprintf (stderr, "Space invader created.\n");
#endif
}

void game::makeboulder(float x, float y, float angle, float scale, char *tex) {
	enemies.push_back(new boulder(x, y, angle, scale, tex) );
#if DEBUG == 1
	fprintf (stderr, "Boulder created.\n");
#endif
}

void game::maketux(float x, float y, float angle, float scale, char *tex) {
	enemies.push_back(new tux(x, y, angle, scale, tex) );
}

void game::makeghost(float x, float y, float vx, float vy, float angle, char *tex) {
	delete ghostboat;

	ghostboat = new paatti(x, y, angle, 0.4, tex );

	for (int i=0; i<ghostboat->numparticles; i++) {
		ghostboat->p[i].v = vec(vx,vy);
	}
}

void game::makelevel(char *p, float x, float y, float angle, float zoom, char *tex) {
	delete taso;
	taso = new level(p, x, y, angle, zoom, tex );
}

void game::makelevel(FILE *fp, char *tex) {
	delete taso;
	taso = new level(fp, tex);
}


void game::focus() {
	Window.Viewarea(50 * Window.width / Window.height, 50); // m
	Window.Center(0, 0); // m
}

void game::setgoal(float x, float y, float r, char *tex) {
	goal = new paatti(x, y, 3.14159, 0.4, tex);
	goalr=r;
}

void game::setice(int i, int side, GLuint icetex, float f) {
	if (i>=0 && i<taso->numwalls) {
		taso->w[i].seticetexture(icetex);

		if (side==0) {
			taso->w[i].type_ab=WALL_ICE;
			taso->w[i].f_ab=f;
		}
		if (side==1) {
			taso->w[i].type_ac=WALL_ICE;
			taso->w[i].f_ac=f;
		}
		if (side==2) {
			taso->w[i].type_bc=WALL_ICE;
			taso->w[i].f_bc=f;
		}
	}
}

void game::setground(int i, int side, GLuint groundtex, float f) {
	if (i>=0 && i<taso->numwalls) {
		taso->w[i].setgroundtexture(groundtex);

		if (side==0) {
			taso->w[i].type_ab=WALL_GROUND;
			taso->w[i].f_ab=f;
		}
		if (side==1) {
			taso->w[i].type_ac=WALL_GROUND;
			taso->w[i].f_ac=f;
		}
		if (side==2) {
			taso->w[i].type_bc=WALL_GROUND;
			taso->w[i].f_bc=f;
		}
	}
}


Dbl game::simulate() {
//	Mouse.ShowCursor(false);

	sound::PlayChunk(0, Media.vroom, -1);
	sound::ChannelVolume(0, (int)(0.40*12.8*Config.soundvol) );

	stime = SDL_GetTicks();
	nextframe = stime + ms_physframetime; // all in milliseconds

	t = 0; // current time elapsed in seconds

	omx = 0, omy = 0; // reset mouse coordinates
	mx = 0, my = 0;

	Keyboard.ResetAll();

	bool infomode = false;
	int frames=0;

	int paused = 2;
	if (!player) paused=0; // no pause if we have no player playing

	bool drawn = false;

	bool cheatmode=false;

	float fwd=11.35;
	float targetfwd=fwd;

	Uint8 recdat=0;
	Uint8 oldrecdat=0;
	Uint16 nullframes=0;

	Uint8 ghostdat=0;
	Uint8 newghostdat=0;
	Uint16 gnull=0;

	bool ghostready = false;

	Dbl fps=0;
	Dbl fpsbegin=SDL_GetTicks();
	int fpsframes=0;

	vec mp; // the one we're following

	Uint32 clonktime = 0;
	Uint32 splashtime = 0;

	bool tentacledeath = false;
	bool dead = false;
	bool gdead = false;

	Uint32 deadstop=0;

	Window.Viewarea(Window.showx*Config.zoomlevel, Window.showy*Config.zoomlevel);
	fwd *= Config.zoomlevel;
	targetfwd *= Config.zoomlevel;


	// update water
	Water.set_t(t);

	// time to wait after death
	Uint32 deadwait = 3000; // ms
	Uint32 physframe = 0;

	SDL_Event Event;

	while (!Keyboard.Pressed(SDLK_ESCAPE) && (deadstop==0 || SDL_GetTicks() < deadstop) ) {

		// events
		if (! (SDL_GetAppState()&SDL_APPINPUTFOCUS) ) paused=2;
		while (SDL_PollEvent(&Event)) {
			if (Event.type == SDL_QUIT ||
				(Keyboard.Hold(SDLK_ESCAPE) && Keyboard.Hold(SDLK_F10)) )
			{
				Keyboard.GotQuit();
			}
			if (Event.type == SDL_VIDEOEXPOSE) {
         	if (!Window.OpenWindow(Window.width, Window.height, Window.bpp, Window.oldflags) ) {
      	      fprintf (stderr, "Window corrupted and can't reopen!\n");
   	         exit(1);
	         }
				Graphics.reloadtextures();
			}
			if (Keyboard.Hold(SDLK_ESCAPE) && Keyboard.Hold(SDLK_LCTRL) ) {
				Window.Iconify();
			}
			if ( (Keyboard.Hold(SDLK_RALT) || Keyboard.Hold(SDLK_LALT)) &&
					Keyboard.Pressed(SDLK_RETURN) )
			{
				Window.ToggleFullscreen();
				Graphics.reloadtextures();
				Config.fullscreen = !Config.fullscreen;
			}
		}


		if (!Config.nolimit) SDL_Delay(1);

		while (SDL_GetTicks() > nextframe) {
			// the screen must be redrawn
			drawn=false;

			if (!Config.nolimit) nextframe += ms_physframetime;

			// run one physframe

			if (paused==1) { // setting pause off
				paused=0;
			}

			recdat = 0; // data to be saved into the record file
			ghostdat = 0; // ghost controls


			// start of control section

			if (Keyboard.Hold(Config.keys[KEY_PAUSE]) ) paused=2; // pause

			if (paused == 2) {
				if (Keyboard.Hold( Config.keys[KEY_CCW] ) || Keyboard.Hold(Config.keys[KEY_CW]) ||
						Keyboard.Hold(Config.keys[KEY_THRUST]) || Keyboard.Hold(Config.keys[KEY_REVERSE]) ||
						Keyboard.Hold(Config.keys[KEY_JUMP]) || Keyboard.Hold(Config.keys[KEY_TURN]) ) paused = 1;
			} else { // !paused

				if (ghost && !ghostready && !gdead) {
					if (!ghostelement) {
						ghostready = true;

#if DEBUG == 1
						fprintf (stderr, "Ghost got EOF, time: %f\n", t);
#endif

						if (!player) {
							sound::HaltChannel(0);
							sound::ChannelVolume(0, (int)(12.8*Config.soundvol) );

							Mouse.ShowCursor(false);
							return 0.0;
						}
					} else { // ghostelement exists
						ghostelement = ghostelement->get(&ghostdat);
					}
				}


				if (player && !dead) { // player playing
					if (Keyboard.Hold(Config.keys[KEY_CCW])) {
						malli->bendleft(dt);
						if (record) recdat |= RECORD_LEFT;
					}
					if (Keyboard.Hold(Config.keys[KEY_CW])) {
						malli->bendright(dt);
						if (record) recdat |= RECORD_RIGHT;
					}

					if (Keyboard.Hold(Config.keys[KEY_JUMP])) {
						malli->strive(dt);
						if (record) recdat |= RECORD_C;
					}

					if (Keyboard.Pressed(Config.keys[KEY_TURN])==1) {
						if( malli->flip(taso->w, taso->numwalls) ) {
							if (record) recdat |= RECORD_SPACE;
							targetfwd *= -1;
							fwd /= 1.01;
						}
					}

					bool engines=false;
					if (Keyboard.Hold(Config.keys[KEY_THRUST]) && !Keyboard.Hold(Config.keys[KEY_REVERSE])) {
						malli->stretchup(dt);
						if (record) recdat |= RECORD_UP;
						engines=true;
					}
					if (Keyboard.Hold(Config.keys[KEY_REVERSE]) && !Keyboard.Hold(Config.keys[KEY_THRUST])) {
						malli->stretchdown(dt);
						if (record) recdat |= RECORD_DOWN;
						engines=true;
					}

					if (engines) sound::ChannelVolume(0, (int)(0.80*12.8*Config.soundvol) );
					else sound::ChannelVolume(0, (int)(0.40*12.8*Config.soundvol) );

				}


				if (ghost && !gdead) { // showing replay

					vec p = ghostboat->midpoint();
					vec v = ghostboat->midv();

					if ( ghostdat & RECORD_LEFT ) {
						ghostboat->bendleft(dt);
					}
					if ( ghostdat & RECORD_RIGHT ) {
						ghostboat->bendright(dt);
					}

					if ( ghostdat & RECORD_C ) {
						ghostboat->strive(dt);
					}

					if ( ghostdat & RECORD_SPACE ) {
						ghostboat->flip(taso->w, taso->numwalls);
						if (!player) {
							targetfwd *= -1;
							fwd /= 1.01;
						}
					}

					bool engines=false;

					if ( ghostdat & RECORD_UP ) {
						ghostboat->stretchup(dt);
						engines=true;
					}
					if ( ghostdat & RECORD_DOWN ) {
						ghostboat->stretchdown(dt);
						engines=true;
					}

					if (ghost && !player)  {
						if (engines) sound::ChannelVolume(0, (int)(0.80*12.8*Config.soundvol) );
						else sound::ChannelVolume(0, (int)(0.40*12.8*Config.soundvol) );
					}

				}


				if (record && !dead) { // recording player's game
					recelement = recelement->add(recdat);
				}


				// end of control section


			}

			if (player) { // no special functions if it's only a recording
				if (Keyboard.Pressed(SDLK_F1)) {
					infomode=!infomode;
					Mouse.ShowCursor(infomode);
				}

				if (infomode && Keyboard.Hold(SDLK_h) && Keyboard.Hold(SDLK_4) && Keyboard.Hold(SDLK_x) && !cheatmode) {
					cheatmode=true;

					for (int i=0; i<taso->numwalls; i++) {
						taso->w[i].seticetexture(iceID);
						taso->w[i].setgroundtexture(groundID);
					}
				}

			}

			// miscellaneous controls

			if (Keyboard.Pressed(SDLK_F2)) {
				Config.fps = !Config.fps;
			}

			if (Keyboard.Hold(Config.keys[KEY_ZOOMOUT])) { // zoom
				if (Window.showx < 100.0) {
					float q = exp(1.001*dt);
					Window.Viewarea(Window.showx*q, Window.showy*q);
					fwd *= q;
					targetfwd *= q;
					Config.zoomlevel *= q;
				}

			}

			if (Keyboard.Hold(Config.keys[KEY_ZOOMIN])) {
				if (Window.showx > 10.0) {
					float q = exp(-1.001*dt);
					Window.Viewarea(Window.showx*q, Window.showy*q);
					fwd *= q;
					targetfwd *= q;
					Config.zoomlevel *= q;
				}
			}


			// end of misc controls


			// physics frame

			if (!paused) {

				// update water
				Water.set_t(t);

				if (fwd < targetfwd) { // viewing side change
					fwd += 1.5*dt*sqrt(targetfwd*targetfwd-fwd*fwd)*3.14159;
					if (fwd > targetfwd) fwd=targetfwd;
				}

				if (fwd > targetfwd) {
					fwd -= 1.5*dt*sqrt(targetfwd*targetfwd-fwd*fwd)*3.14159;
					if (fwd < targetfwd) fwd=targetfwd;
				}


				// update ship
				if (player && !dead) {
					malli->settime(t);
					malli->springs_act(dt);
					malli->update(dt);
					malli->particles_act(dt, taso->w, taso->numwalls, t);
//					malli->precision();

					float p = malli->getdamage();
					if (p > 15.0) {
						if (SDL_GetTicks() - clonktime > 1000) {
							if (p > 12.8) p=12.8;
							sound::ChunkVolume(Media.clonk, 128);
							sound::PlayChunk(-1, Media.clonk, 0);
							clonktime = SDL_GetTicks();
						}
					}

					p = malli->getwaterdamage();
					float a = (SDL_GetTicks() - splashtime)/2000.0;
					if (a > 1.0) a = 1.0;

					if (p > 5.0 && a > 0.2) {
						sound::ChunkVolume(Media.splash, (int)((p*4+30)*a));
						sound::PlayChunk(-1, Media.splash, 0);
						splashtime = SDL_GetTicks();
					}

				}

				if (ghost && !gdead) {
					ghostboat->settime(t);
					ghostboat->springs_act(dt);
					ghostboat->update(dt);
					ghostboat->particles_act(dt, taso->w, taso->numwalls, t);
//					ghostboat->precision();

					if (!player) {
						float p = ghostboat->getdamage();
						if (p > 0.0) {
							if (SDL_GetTicks() - clonktime > 1000) {
								if (p > 12.8) p=12.8;
								sound::ChunkVolume(Media.clonk, 128);
								sound::PlayChunk(-1, Media.clonk, 0);
								clonktime = SDL_GetTicks();
							}
						}

						p = ghostboat->getwaterdamage();
						float a = (SDL_GetTicks() - splashtime)/2000.0;
						if (a > 1.0) a = 1.0;

						if (p > 5.0 && a > 0.2) {
							sound::ChunkVolume(Media.splash, (int)((p*4+30)*a));
							sound::PlayChunk(-1, Media.splash, 0);
							splashtime = SDL_GetTicks();
						}

					}

				}


				if (dead) {
					mallidead->settime(t);

					mallidead->springs_act(dt);
					mallidead->update(dt);
					mallidead->particles_act(dt, taso->w, taso->numwalls, t);

				}


				if (gdead) {
					ghostdead->settime(t);
					ghostdead->springs_act(dt);
					ghostdead->update(dt);
					ghostdead->particles_act(dt, taso->w, taso->numwalls, t);
				}


				// update goal
				goal->settime(t);
				goal->springs_act(dt);
				goal->update(dt);
				goal->particles_act(dt, taso->w, taso->numwalls, t);
//				goal->precision();

				// check goal
				if (player && !dead) {
					if (malli->checkgoal(goal->midpoint(), goalr)) {

#if DEBUG == 1
						fprintf (stderr, "Goal at (%f,%f).\n", goal->midpoint().x,
							goal->midpoint().y);
						fprintf (stderr, "Arrived goal after %f s!\n", t);
#endif

						sound::PlayChunk(-1, Media.finish, 0);

						sound::HaltChannel(0); // vroom
						sound::ChannelVolume(0, (int)(12.8*Config.soundvol) );

						Mouse.ShowCursor(false);

						return t;
					}
				}

				// check goal
				if (ghost && !gdead) {
					if (ghostboat->checkgoal(goal->midpoint(), goalr)) {

#if DEBUG == 1
						fprintf (stderr, "Ghost finished, time %f.\n", t);
#endif

						if (player) ghost=false;
						else {
							sound::HaltChannel(0); // vroom
							sound::ChannelVolume(0, (int)(12.8*Config.soundvol) );

							Mouse.ShowCursor(false);
							return t;
						}
					}
				}

				// update enemies

				std::vector<model*>::iterator curen;
				for (curen = enemies.begin(); curen != enemies.end(); curen++) {
					(*curen)->settime(t);
					(*curen)->springs_act(dt);
					(*curen)->update(dt);
					(*curen)->particles_act(dt, taso->w, taso->numwalls, t);
//					(*curen)->precision();

					(*curen)->updatewalls();

				}

				// set mp to followed one's midpoint
				if (player) mp = malli->midpoint();
				else if (ghost) mp = ghostboat->midpoint();

				if (mp.x < -10000.0 || mp.x > 10000.0) {
					sound::HaltChannel(0); // vroom
					sound::ChannelVolume(0, (int)(12.8*Config.soundvol) );

					Mouse.ShowCursor(false);
					return -1.0;
				}

				// player vs. tentacles

				if (player && !dead) {
					vec pmid = malli->midpoint();

					for (curen = enemies.begin(); curen != enemies.end(); curen++) {
						vec d = pmid - (*curen)->midpoint();
						float r2 = 8.5*8.5;

						if ( !dead && d.abs2() <= r2) { // check closer

							for (int i=0; i<PAATTI_TOUCHPOINTS; i++) {
								if (!dead && (*curen)->checkwalls(malli->p[paatti_tp[i]].p) ){
									// the ship has been killed except if it was a tux

									if ( (*curen)->type != ENEMY_TUX ) {

										switch( (*curen)->type ) {
											case ENEMY_TENTACLE:
												sound::PlayChunk(-1,Media.roarr,0);
											break;
											case ENEMY_INVADER:
												sound::PlayChunk(-1,Media.roarr,0);
											break;
											default:
											break;
										}

										sound::PlayChunk(-1,Media.explosion,0);

										dead = true;
										mallidead = new broken(malli );
										if (!deadstop) deadstop = SDL_GetTicks() + deadwait;

										sound::HaltChannel(0); // vroom, engines halt
										sound::ChannelVolume(0, (int)(12.8*Config.soundvol) );

									} else { // tux
										tuxfound=true;
									} // if


								} // if

							}

						}
					}
				}

				// ghost vs. tentacles

				if (ghost && !gdead) {
					vec gmid = ghostboat->midpoint();

					for (curen = enemies.begin(); curen != enemies.end(); curen++) {

						// ghost doesn't collect tuxes :)
						if ( (*curen)->type == ENEMY_TUX && player ) continue;

						vec d = gmid - (*curen)->midpoint();
						float r2 = 8.5*8.5; // tentacle is the tallest..

						if ( !gdead && d.abs2() <= r2) { // check closer

							for (int i=0; i<PAATTI_TOUCHPOINTS; i++) {
								if (!gdead && (*curen)->checkwalls(ghostboat->p[paatti_tp[i]].p) ){
									// the old bad tentacle monster has eaten the ship

									if ( (*curen)->type != ENEMY_TUX ) {
										if (player) {
											gdead = true;
											ghostdead = new broken(ghostboat);
										} else {
											// play sound

											switch( (*curen)->type ) {
												case ENEMY_TENTACLE:
													sound::PlayChunk(-1,Media.roarr,0);
												break;
												case ENEMY_INVADER:
													sound::PlayChunk(-1,Media.roarr,0);
												break;
												default:
												break;
											}

											sound::PlayChunk(-1,Media.explosion,0);

											gdead = true;
											ghostdead = new broken(ghostboat);
											if (!deadstop) deadstop = SDL_GetTicks() + deadwait;

											sound::HaltChannel(0); // vroom, engines halt
											sound::ChannelVolume(0, (int)(12.8*Config.soundvol) );


										}

									} // if !tux

								}
							}

						}
					}
				}


				// enemy collisions

				for (curen = enemies.begin(); curen != enemies.end(); curen++) {
					vec d = mp - (*curen)->midpoint(); // distance

					float p = (*curen)->getwaterdamage();
					if (p > 5.0) {
						float r = ((*curen)->midpoint() - vec( Window.centerx, Window.centery )).abs2();
						float vol = 1270.0*p/r;
						if (vol > 127.0) vol = 127.0;
						if (vol > 50)  {
							sound::ChunkVolume(Media.splash, (int)vol );
							sound::PlayChunk(-1, Media.splash, 0);
						}
					}
					p = (*curen)->getdamage();

					if (p > 5.0) {
						float r = ((*curen)->midpoint() - vec( Window.centerx, Window.centery )).abs();
						float vol = 1270.0*p/r;
						if (vol > 127.0) vol = 127.0;
						if (vol > 50)  {
							sound::ChunkVolume( Media.bump, (int)vol );
							sound::PlayChunk(-1, Media.bump, 0);
						}
					}



				}



				t+=dt;
				physframe++;

				if (Config.nolimit) break;

			}


			// end of physics frame


		}

		if ( (!Config.nolimit && !drawn) || (Config.nolimit && (physframe%100)==0) ) { // if not drawn this frame already, draw it
			Graphics.clear();


			// draw background

			if (Config.decoration == DEC_FULL) {
				glDisable(GL_BLEND);
				glDisable(GL_TEXTURE_2D);

				float y_max = Graphics.backprojecty(0);
				if (y_max > 100) y_max = 100;
				if (y_max < 0) y_max = 0.0;
				float y_min = Graphics.backprojecty(Window.height);
				if (y_min > 100) y_min = 100.0;
				if (y_min < 0) y_min = 0.0;

				float g1 = 0.85 - 0.005*y_max;
				float g2 = 0.85 - 0.005*y_min;


				#ifdef HAVE_GLES
				GLfloat vtx1[] = {
					0,0,
					Window.width-1,0,
					Window.width-1,Window.height-1,
					0,Window.height-1
				};
				GLfloat col1[] = {
					g1, g1, 1.0, 1.0f,
					g1, g1+0.01, 1.0, 1.0f,
					g2, g2*0.7, 1.0, 1.0f,
					g2, g2*0.7+0.01, 1.0, 1.0f
				};
			 
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);
			 
				glVertexPointer(2, GL_FLOAT, 0, vtx1);
				glColorPointer(4, GL_FLOAT, 0, col1);
				glDrawArrays(GL_TRIANGLE_FAN,0,4);
			 
				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
					
				#else
				glBegin(GL_QUADS);

				glColor3f(g1, g1, 1.0);
				glVertex2f(0,0);

				glColor3f(g1, g1+0.01, 1.0);
				glVertex2f(Window.width-1,0);

				glColor3f(g2, g2*0.7, 1.0);
				glVertex2f(Window.width-1,Window.height-1);

				glColor3f(g2, g2*0.7+0.01, 1.0);
				glVertex2f(0,Window.height-1);

				glEnd();
				#endif
				glColor3f(1.0,1.0,1.0);

				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
			}

			// done

			// infomode stuff

			// mouse cursor at screen
			vec m = vec(Graphics.backprojectx(mx),
							Graphics.backprojecty(my) );

			int closestp=-1;
			int closests=-1;
			int closestw=-1;
			int closestside=-1;
			int closesttype=-1;

			if (infomode) {
				Mouse.GetCursorCoords(&mx, &my);
				// detect the closest wall face side
				Dbl closestw_d = 1000;
				for (int i=0; i<taso->numwalls; i++) {
					Dbl d1 = (taso->w[i].oa+taso->w[i].ab*0.5-m).abs();
					Dbl d2 = (taso->w[i].oa+taso->w[i].ac*0.5-m).abs();
					Dbl d3 = (taso->w[i].oa+taso->w[i].ab+taso->w[i].bc*0.5-m).abs();
					if (d1 < closestw_d) {
						closestw_d = d1;
						closestw=i;
						closestside=0;
						closesttype=taso->w[i].type_ab;
					}
					if (d2 < closestw_d) {
						closestw_d = d2;
						closestw=i;
						closestside=1;
						closesttype=taso->w[i].type_ac;
					}
					if (d3 < closestw_d) {
						closestw_d = d3;
						closestw=i;
						closestside=2;
						closesttype=taso->w[i].type_bc;
					}
				}

				if (cheatmode && infomode && Keyboard.Pressed(SDLK_F9)) {
					if (closestside==0) {
						taso->w[closestw].type_ab++;
						taso->w[closestw].f_ab=-1.0;
					}
					if (closestside==1) {
						taso->w[closestw].type_ac++;
						taso->w[closestw].f_ac=-1.0;
					}
					if (closestside==2) {
						taso->w[closestw].type_bc++;
						taso->w[closestw].f_bc=-1.0;
					}

					if (taso->w[closestw].type_ab>=WALL_TYPES) taso->w[closestw].type_ab=0;
					if (taso->w[closestw].type_ac>=WALL_TYPES) taso->w[closestw].type_ac=0;
					if (taso->w[closestw].type_bc>=WALL_TYPES) taso->w[closestw].type_bc=0;
				}

				if (cheatmode && infomode && Keyboard.Pressed(SDLK_F10)) {
					if (closestside==0) taso->w[closestw].f_ab*=-1;
					if (closestside==1) taso->w[closestw].f_ac*=-1;
					if (closestside==2) taso->w[closestw].f_bc*=-1;
				}

				if (cheatmode && infomode && Keyboard.Pressed(SDLK_F12)) {
#if DEBUG == 1
					fprintf (stderr, "Saving map information to mapinfo.txt\n");
#endif

					FILE *fp = fopen("mapinfo.txt", "w+");
					if (fp) {
						fprintf (fp, "Map information:\n");
						for (int i=0; i<taso->numwalls; i++) {
							if (taso->w[i].type_ab > 0) fprintf (fp, "%d.0=%d,%f\n", i, taso->w[i].type_ab, taso->w[i].f_ab);
							if (taso->w[i].type_ac > 0) fprintf (fp, "%d.1=%d,%f\n", i, taso->w[i].type_ac, taso->w[i].f_ac);
							if (taso->w[i].type_bc > 0) fprintf (fp, "%d.2=%d,%f\n", i, taso->w[i].type_bc, taso->w[i].f_bc);
						}
						fclose(fp);
					} else {
#if DEBUG == 1
						fprintf (stderr, "Couldn't open mapinfo.txt for writing.\n");
#endif
					}
				}


				// detect the particle closest to m
				Dbl closestp_d = 1000;
				for (int i=0; i<malli->numparticles; i++) {
					Dbl d = (malli->p[i].p - m).abs();
					if (d < closestp_d) {
						closestp_d = d;
						closestp = i;
					}
				}

				// detect the spring closest to m
				Dbl closests_d = 1000;
				for (int i=0; i<malli->numsprings; i++) {
					// compare distance from the midpoints of the line segments

					vec avg = (malli->s[i].p1->p + malli->s[i].p2->p)/2;
					Dbl d = (m - avg).abs();

					if (d < closests_d) {
						closests_d = d;
						closests = i;
					}
				}

				char text1[200];
				char text2[200];
				char text3[200];
				char text4[200];
				char text5[200];
				sprintf (text1, "NEAREST PARTICLE: %d", closestp);
				sprintf (text2, "NEAREST SPRING: %d", closests);
				sprintf (text3, "X: %f  Y: %f", m.x, m.y);
				sprintf (text4, "NEAREST WALL: %d.%d", closestw, closestside);
				sprintf (text5, "WALL TYPE: %d", closesttype);

				Font_bl.WriteString(text1, 1, 1, 16);
				Font_bl.WriteString(text2, 1, 16, 16);
				Font_bl.WriteString(text3, 1, 31, 16);
				Font_bl.WriteString(text4, 1, 47, 16);
				Font_bl.WriteString(text5, 1, 63, 16);
			}




			// set camera to follow player

			vec c;
			if (player) c = malli->midpoint();
			else c = ghostboat->midpoint();

			Window.Center(c.x+fwd, c.y);

			// draw goal
			glDisable(GL_BLEND);
			goal->display();

			if (ghost) {
				if (ghostalpha < 0.995) {
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					#ifdef HAVE_GLES
					Graphics.color4f(1.0, 1.0, 1.0, ghostalpha);
					#else
					glColor4f(1.0, 1.0, 1.0, ghostalpha);
					#endif
					if (!gdead) ghostboat->display();
					else ghostdead->display();
					#ifdef HAVE_GLES
					Graphics.color4f(1.0, 1.0, 1.0, 1.0);
					#else
					glColor4f(1.0, 1.0, 1.0, 1.0);
					#endif
					glDisable(GL_BLEND);
				} else {
					glDisable(GL_BLEND);
					if (!gdead) ghostboat->display();
					else ghostdead->display();
				}
			}

			if (player) {
				glDisable(GL_BLEND);
				if (!dead) malli->display();
				else mallidead->display();
			}

			// draw tentacles and others
			std::vector<model*>::iterator curen;
			for (curen = enemies.begin(); curen != enemies.end(); curen++) {
				(*curen)->display();
			}

			taso->display();


			char text1[200];
			int tm = (int)(100.0*t);
			int sad = tm%100;
			int sek = (tm/100)%60;
			int min = (tm/6000)%60;
			sprintf (text1, "%d:%.2d.%.2d", min, sek, sad);
			Font.WriteString(text1, Window.width-1-20*8, 1, 21);

			frames++;

			char text2[200];
			char text3[200];


			if (SDL_GetTicks() - fpsbegin >= 1000) {
				fps = (Dbl)frames/(Dbl)(SDL_GetTicks()-stime)*1000.0;
			}

			if (Config.fps) {
				if (fps <= 198) {
					sprintf (text2, "%d FPS", (int)fps );
				} else {
					sprintf (text2, "MAX FPS");
				}

				Font.WriteString(text2, Window.width-1-20*8, 21, 21);
			}

			if (paused) {
				char ptxt[]="PAUSED";
				char ptxt2[]="START PLAYING TO UNPAUSE";

				Font.WriteString (ptxt, Window.width/2-30.0*(strlen(ptxt)-1), Window.height/2-100.0, 60.0);
				Font.WriteString (ptxt2, Window.width/2-12.0*(strlen(ptxt2)-1), Window.height/2-30.0, 24.0);

			}

			// tutorial texts and so on
			vec mp; // middle point

			if (player) mp = malli->midpoint();
			else if (ghost) mp = ghostboat->midpoint();

			std::vector<text>::iterator curtext;
			for (curtext = texts.begin(); curtext != texts.end(); curtext++) {
				// find texts
				if (mp.x >= curtext->lu.x && mp.x <= curtext->rl.x &&
					mp.y <= curtext->lu.y && mp.y >= curtext->rl.y) {
					// show text

					Font_bl.WriteString( (char*)curtext->str.c_str(), 0, 14*curtext->line, 14);
				}

			}


			glFinish();
			#ifdef HAVE_GLES
			EGL_SwapBuffers();
			#else
			SDL_GL_SwapBuffers();
			#endif

			drawn=true;


			// end of draw section
		} else {
			if (!Config.nolimit) SDL_Delay(1); // save cpu power
		}

	}

	sound::HaltChannel(0); // vroom
	sound::ChannelVolume(0, (int)(12.8*Config.soundvol) );

	Mouse.ShowCursor(false);

	return -1.0;
}

void game::savelevel(FILE *fp) {
	taso->savelevel(fp);
}
