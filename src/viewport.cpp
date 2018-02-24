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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#ifdef HAVE_GLES
#include "SDL_opengles.h"
#include "eglport.h"
#else
#include "SDL_opengl.h"
#endif

#include "compiling_settings.h"

const float W = 60.0/2.0;
const float H = W * 320.0/490.0f*2.0;

const float IWp2 = 60.0/2.0;
const float IHp2 = IWp2 * 195.0/458.0f;

float time;

void warn(char *str);
SDL_Surface *LoadPicture(char *file);
GLuint maketexture(SDL_Surface *s);
float w(float x);
float dw(float x);

class invader {
public:
	invader(float x, float y) {
		p[0]=x;
		p[1]=y;
		p0[0]=x;
		p0[1]=y;
		next=0;
		prev=0;
	}
	~invader() {
	}

	invader *prev;
	invader *next;

	float p[2];
	float p0[2];
};

int main(int argc, char*argv[]) {
	char datapath[200], path[300];
	int doublebuffer;
	SDL_Surface *s, *t;
	GLuint alusid, invaderid, laserid;
	Mix_Chunk *vroom, *finish, *explosion, *roarr;
	int dir, num, lives;
	bool dead, gamestops;

	Uint8 *keys;
	SDL_Event Event;

	if (SDL_Init(SDL_INIT_VIDEO)==-1) {
		fprintf (stderr, "Couldn't init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	FILE *fp = fopen(DEFAULT_CONFIGFILE, "r"); // Load path config
	if (!fp)  warn("Couldn't open configfile.\n");
	if (fscanf(fp, "datadir %s\n", datapath) != 1) warn("Couldn't parse configfile.\n");
	fclose(fp);

	#ifdef PANDORA
	#define screen_height 480
	#else
	#define screen_height 600
	#endif
	#ifdef HAVE_GLES
	if (!SDL_SetVideoMode(800, screen_height, 32, SDL_SWSURFACE | SDL_FULLSCREEN) ) warn("Couldn't open SDL screen.\n");
	EGL_Init();
	#else
	if (!SDL_SetVideoMode(800, screen_height, 32, SDL_OPENGL) ) warn("Couldn't open OpenGL screen.\n");
	#endif
	glViewport(0, 0, 800,screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, screen_height);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable (GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	#ifndef HAVE_GLES
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doublebuffer);
	if (doublebuffer != 1) warn("No double buffer support.\n");
	#else
	doublebuffer=0;
	#endif

	SDL_WM_SetCaption ("Bloboats Invaders - brought to you by MakeGho", NULL);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) warn("Couldn't initialize sound.\n");

	glClearColor(0.625, 0.625, 1.0, 1.0);

	keys = SDL_GetKeyState(NULL);

	sprintf (path, "%s/images/alus.png", datapath);
	t = LoadPicture(path);
	alusid = maketexture(t);
	sprintf (path, "%s/images/invader.png", datapath);
	t = LoadPicture(path);
	invaderid = maketexture(t);
	sprintf (path, "%s/sounds/vroom.wav", datapath);
	if (!(vroom = Mix_LoadWAV(path)) ) warn("Couldn't load sound.\n");
	sprintf (path, "%s/sounds/finish.wav", datapath);
	if (!(finish = Mix_LoadWAV(path)) ) warn("Couldn't load sound.\n");
	sprintf (path, "%s/sounds/explosion.wav", datapath);
	if (!(explosion = Mix_LoadWAV(path)) ) warn("Couldn't load sound.\n");
	sprintf (path, "%s/sounds/roarr.wav", datapath);
	if (!(roarr = Mix_LoadWAV(path)) ) warn("Couldn't load sound.\n");

	Mix_Volume(0, 50);
	Mix_PlayChannel(0, vroom, -1);

	Uint32 stime = SDL_GetTicks();
	Uint32 nextframe = stime;

	invader *first, *last;

	for (int i=0; i<10; i++) {
		for (int j=0; j<5; j++) {
			if (i==0 && j==0) {
				first = last = new invader(100.0f, 550.0f);
			} else {
				last->next = new invader(100.0 + i*60.0f, 550.0f - j*40.0f );
				last->next->prev = last;
				last = last->next;
			}
		}
	}

	float pommit[20][3];
	float laserit[20][4];

	for (int i=0; i<20; i++) {
		for (int j=0; j<3; j++) {
			pommit[i][j]=-1;
		}
		for (int j=0; j<4; j++) {
			laserit[i][j]=-1;
		}

	}

	dir = 1;
	num = 10*5;
	lives = 3;
	dead = false;
	gamestops = false;

	float v = 0, p = 400, goaly = 0, sink = 0, sinkv = 0;
	float invav = 2.5;
	int laserloaded = 0;

	bool ready=false;

	while(!ready) {

		while (SDL_PollEvent(&Event)) { // Window closing
			if (Event.type == SDL_QUIT) ready=true;
		}

		while (SDL_GetTicks() >= nextframe && dead) {
			nextframe += 5;

			float in_v = 2.5/(float)num*(float)dir;


			// invader movement

			if (last->p[0] > 780) {
				dir=-1;
				if (goaly >= 0) goaly -= 30;
			} 
			else if(first->p[0] < 20) {
				dir=1;
				if (goaly >= 0) goaly -= 30;
			}

			if (goaly < 0) {
				invader *current = first;
				float k = fabs(in_v);
				while (current) {
					current->p[1] -= k;
					current = current->next;	
				}
				goaly += k;
			}

			if (goaly >= 0) {
				invader *current = first;
				while (current) {
					current->p[0] += in_v;
					current = current->next;	
				}
			}

			sink += sinkv;
			sinkv -= 0.01;

			p += v;
			v /= 1.03;
			sinkv /= 1.03;

			if (sink + w(p) < -100) { // new game
				sink = 0;
				sinkv = 0;
				dead = false;
				p=400;
				laserloaded=0;
				goaly=0;

				if (gamestops || lives <= 0) {
					gamestops = false;

					invav = 2.5;
					dir = 1;

					invader *current = first; // delete all
					while (current) {
						if (current->next) {
							current = current->next;
							delete current->prev;
						} else {
							delete current;
							break;
						}
					}

					for (int i=0; i<10; i++) { // create new ones
						for (int j=0; j<5; j++) {
							if (i==0 && j==0) {
								first = last = new invader(100.0f, 550.0f);
							} else {
								last->next = new invader(100.0 + i*60.0f, 550.0f - j*40.0f );
								last->next->prev = last;
								last = last->next;
							}
						}
					}
					num = 50;

					lives = 3;

				}

			}

		}

		while (SDL_GetTicks() >= nextframe && !dead) {
			nextframe += 5;

			float in_v = invav/(float)num*(float)dir;
			float prob = 0.005/num;

			// Controls

			if (keys[SDLK_LEFT]) v-=0.01;
			if (keys[SDLK_RIGHT]) v+=0.01;

			if (keys[SDLK_SPACE] && laserloaded <= 0) {
				for (int i=0; i<20; i++) {
					if (laserit[i][0] < 0) {
						laserloaded += 200;
						fprintf (stderr, "Laser created.\n");
	
						laserit[i][0]=p;
						laserit[i][1]=w(p);
	
						float k = dw(p);
						if (k != 0) {
							laserit[i][3] = 1/sqrt(k*k+1);
							laserit[i][2] = -laserit[i][3]*k;
						} else {
							laserit[i][2]=0;
							laserit[i][3]=1;
						}
						break;
					}
				}
			} // if

			// End of Controls


			// Invader bombing

			invader *current = first;
			while (current) {
				if (rand()/(RAND_MAX+1.0) <= prob) {
					for (int i=0; i<20; i++) {
						if (pommit[i][0] < 0) {
							pommit[i][0] = current->p[0];
							pommit[i][1] = current->p[1];
							pommit[i][2] = -0.05;
							break;
						}
					}
				}
				current = current->next;
			}

			// End of Invader bombing
				

			// Moving Lasers

			for (int i=0; i<20; i++) {
				if (laserit[i][0] < 0 || laserit[i][0] > 799 || 
					laserit[i][1] > 600
				) { // Out of Area, Kill
					laserit[i][0] = -1.0;
				} else { // Move
					laserit[i][0] += laserit[i][2];
					laserit[i][1] += laserit[i][3];

					invader *current = first;
					while (current != 0) {
						if (laserit[i][0] > current->p[0] - IWp2 &&
							laserit[i][0] < current->p[0] + IWp2 &&
							laserit[i][1] < current->p[1] + IHp2 &&
							laserit[i][1] > current->p[1] - IHp2
						) {
							laserit[i][0]=-1;

							if (current->prev) current->prev->next = current->next;
							if (current->next) current->next->prev = current->prev;
							if (first == current) first = current->next;
							if (last == current) last = current->prev;

							delete current;

							Mix_VolumeChunk(explosion, 50);
							Mix_PlayChannel(-1, explosion, 0);

							num--;
							if (num==0) {
								first=0;
								Mix_PlayChannel(-1, finish, 0);
								lives++;
							}

							break;
						}

						current = current->next;
					}

				}
			}

			// End of Moving Lasers


			// Moving Bombs

			for (int i=0; i<20; i++) {
				if (pommit[i][1] <= 0) pommit[i][0]=-1; // In Area?
				else if (pommit[i][0] >= 0) { // Move
					pommit[i][1] += pommit[i][2];
					pommit[i][2] -= 0.01;

					if (pommit[i][0] >= p-W && pommit[i][0] <= p+W && pommit[i][1] >= w(p) && pommit[i][1] <= w(p)+H) {
						lives--;

						dead = true;			

						Mix_VolumeChunk(explosion, 127);
						Mix_PlayChannel(-1, explosion, 0);

						for (int i=0; i<20; i++) {
							laserit[i][0]=-1;
							pommit[i][0]=-1;
						}

						break;
					}

				}
			}

			// End of Moving Bombs

			// Limits and other Movements

			v /= 1.01;

			p += v/sqrt(1+dw(p)*dw(p));

			if (p < 20) {
				p=20;
				v=0;
			}
			if (p > 780) {
				p=780;
				v=0;
			}

			if (num > 0) {
				if (last->p[0] > 780) {
					dir=-1;
					if (goaly >= 0) goaly -= 30;
				} 
				else if(first->p[0] < 20) {
					dir=1;
					if (goaly >= 0) goaly -= 30;
				}
			}


			if (goaly < 0) {
				invader *current = first;
				float k = fabs(in_v);

				while (current) {
					current->p[1] -= k;

					if (current->p[1] + goaly < w(p)+H+IHp2-30 ) {
						gamestops=true;
					}


					current = current->next;	

				}

				goaly += k;
			}

			if (goaly >= 0) {
				invader *current = first;
				while (current) {
					current->p[0] += in_v;
					current = current->next;	
				}
			}

			// End of Limits and other Movements

			// Counters

			if (laserloaded > 0) laserloaded--;

			time += 0.005;		

			// End of Counters

			if (gamestops) {
				Mix_PlayChannel(-1, roarr, 0);
				dead = true;			
				for (int i=0; i<20; i++) {
					laserit[i][0]=-1;
					pommit[i][0]=-1;
				}
			}

			// Game finishing
			if (num == 0) { // new game
				for (int i=0; i<20; i++) {
					laserit[i][0]=-1;
					pommit[i][0]=-1;
				}

				for (int i=0; i<10; i++) { // create new ones
					for (int j=0; j<5; j++) {
						if (i==0 && j==0) {
							first = last = new invader(100.0f, 550.0f);
						} else {
							last->next = new invader(100.0 + i*60.0f, 550.0f - j*40.0f );
							last->next->prev = last;
							last = last->next;
						}
					}
				}
				num = 50;
				invav *= 1.3;
			}


			if (keys[SDLK_ESCAPE]) ready=true;	

		}

		// Draw

		float k = dw(p);
		float ca = 1/sqrt(1+k*k), sa = k*ca;
		float a = W*sa, b = W*ca, c = H*sa, d = H*ca;
		float y = w(p);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glLoadIdentity();
		glTranslatef(0, 0, 0);
		glScalef(1, 1, 1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glBindTexture(GL_TEXTURE_2D, alusid);
		
		#ifdef HAVE_GLES
		Graphics.drawQuad(p-b-c,y-a+d+sink, p+b-c,y+a+d+sink, p+b,y+a+sink, p-b,y-a+sink,
			6.0f/512.0f, 14.0f/512.0f, 497.0f/512.0f, 14.0f/512.0f, 497.0f/512.0f, 338.0f/512.0f, 6.0f/512.0f, 338.0f/512.0f);
		#else
		glBegin(GL_QUADS);


		glTexCoord2f(6.0f/512.0f, 14.0f/512.0f);
		glVertex2f(p-b-c,y-a+d+sink);
		glTexCoord2f(497.0f/512.0f, 14.0f/512.0f);
		glVertex2f(p+b-c,y+a+d+sink);
		glTexCoord2f(497.0f/512.0f, 338.0f/512.0f);
		glVertex2f(p+b,y+a+sink);
		glTexCoord2f(6.0f/512.0f, 338.0f/512.0f);
		glVertex2f(p-b,y-a+sink);
		#endif

		for (int i=0; i<lives-1; i++) {
			float x = 40*i;
			float y = 579;
			#ifdef HAVE_GLES
			Graphics.drawQuad(x, y+H*0.5, x+W, y+H*0.5, x+W, y, x, y,
				6.0f/512.0f, 14.0f/512.0f, 497.0f/512.0f, 14.0f/512.0f, 497.0f/512.0f, 338.0f/512.0f, 6.0f/512.0f, 338.0f/512.0f);
			#else
			glTexCoord2f(6.0f/512.0f, 14.0f/512.0f);
			glVertex2f(x, y+H*0.5);
			glTexCoord2f(497.0f/512.0f, 14.0f/512.0f);
			glVertex2f(x+W, y+H*0.5);
			glTexCoord2f(497.0f/512.0f, 338.0f/512.0f);
			glVertex2f(x+W, y);
			glTexCoord2f(6.0f/512.0f, 338.0f/512.0f);
			glVertex2f(x, y);
			#endif
		}

		#ifndef HAVE_GLES
		glEnd();
		#endif

		glBindTexture(GL_TEXTURE_2D, invaderid);

		#ifndef HAVE_GLES
		glBegin(GL_QUADS);
		#endif


		invader *current = first;
		while (current) {
			#ifdef HAVE_GLES
			Graphics.drawQuad(current->p[0]-IWp2, current->p[1]-IHp2, current->p[0]+IWp2, current->p[1]-IHp2, current->p[0]+IWp2, current->p[1]+IHp2, current->p[0]-IWp2, current->p[1]+IHp2,
				0.0f/512.0f, 195.0f/512.0f, 458.0f/512.0f, 195.0f/512.0f, 458.0f/512.0f, 0.0f/512.0f, 0.0f/512.0f, 0.0f/512.0f);
			#else
			glTexCoord2f(0.0f/512.0f, 195.0f/512.0f);
			glVertex2f(current->p[0]-IWp2, current->p[1]-IHp2);
			glTexCoord2f(458.0f/512.0f, 195.0f/512.0f);
			glVertex2f(current->p[0]+IWp2, current->p[1]-IHp2);
			glTexCoord2f(458.0f/512.0f, 0.0f/512.0f);
			glVertex2f(current->p[0]+IWp2, current->p[1]+IHp2);
			glTexCoord2f(0.0f/512.0f, 0.0f/512.0f);
			glVertex2f(current->p[0]-IWp2, current->p[1]+IHp2);
			#endif
			current = current->next;
		}
		#ifdef HAVE_GLES
		glEnd();
		#endif

		glDisable(GL_TEXTURE_2D);
		
		#ifdef HAVE_GLES
		GLfloat vtx[(20+20+800)*2*2];
		GLfloat col[(20+20+800)*4*2];
		int idx = 0;
		#else
		glBegin(GL_LINES);
		#endif

		for (int i=0; i<20; i++) {
			if (laserit[i][0] >= 0) {
				#ifdef HAVE_GLES
				vtx[idx*2]=laserit[i][0]; vtx[idx*2+1]=laserit[i][1];
				col[idx*4]=1.0f; col[idx*4+1]=1.0f; col[idx*4+2]=0.7f; col[idx*4+3]=1.0f;
				idx++;
				vtx[idx*2]=laserit[i][0]-50.0*laserit[i][2]; vtx[idx*2+1]=laserit[i][1]-50.0*laserit[i][3];
				col[idx*4]=1.0f; col[idx*4+1]=0.0f; col[idx*4+2]=0.0f; col[idx*4+3]=0.0f;
				idx++;
				#else
				glColor4f(1.0, 1.0, 0.7, 1.0);
				glVertex2f(laserit[i][0], laserit[i][1]);
				glColor4f(1.0, 0.0, 0.0, 0.0);
				glVertex2f(laserit[i][0]-50.0*laserit[i][2], laserit[i][1]-50.0*laserit[i][3]);
				#endif
			}

		}
		for (int i=0; i<20; i++) {
			if (pommit[i] >= 0) {
				#ifdef HAVE_GLES
				vtx[idx*2]=pommit[i][0]; vtx[idx*2+1]=pommit[i][1];
				col[idx*4]=1.0f; col[idx*4+1]=1.0f; col[idx*4+2]=0.7f; col[idx*4+3]=1.0f;
				idx++;
				vtx[idx*2]=pommit[i][0]; vtx[idx*2+1]=pommit[i][1] + 20.0;
				col[idx*4]=0.0f; col[idx*4+1]=1.0f; col[idx*4+2]=0.0f; col[idx*4+3]=0.0f;
				idx++;
				#else
				glColor4f(1.0, 1.0, 0.7, 1.0);
				glVertex2f(pommit[i][0], pommit[i][1]);
				glColor4f(0.0, 1.0, 0.0, 0.0);
				glVertex2f(pommit[i][0], pommit[i][1] + 20.0);
				#endif
			}
		}


		glColor4f(1.0, 1.0, 1.0, 1.0);


		// water

		glColor4f(0.4, 0.4, 1.0, 0.8);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (int x=0; x<800; x++) {
			#ifdef HAVE_GLES
			vtx[idx*2]=x; vtx[idx*2+1]=w(x);
			col[idx*4]=0.4f; col[idx*4+1]=0.4f; col[idx*4+2]=1.0f; col[idx*4+3]=0.8f;
			idx++;
			vtx[idx*2]=x; vtx[idx*2+1]=0;
			col[idx*4]=0.4f; col[idx*4+1]=0.4f; col[idx*4+2]=1.0f; col[idx*4+3]=0.8f;
			idx++;
			#else
			glVertex2f(x, w(x));
			glVertex2f(x, 0);
			#endif
		}

		#ifdef HAVE_GLES
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
	 
		glVertexPointer(2, GL_FLOAT, 0, vtx);
		glColorPointer(4, GL_FLOAT, 0, col);
		glDrawArrays(GL_LINES,0,idx);
	 
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		#else
		glEnd();
		#endif
		glColor4f(1.0,1.0,1.0, 1.0);
		glEnable(GL_TEXTURE_2D);


		
		glFinish();
		#ifdef HAVE_GLES
		EGL_SwapBuffers();
		#else
		SDL_GL_SwapBuffers();
		#endif
					
	}

	invader *current = first;
	while (current) {
		if (current->next) {
			current = current->next;
			delete current->prev;
		} else {
			delete current;
			break;
		}
	}

	Mix_HaltChannel(0);
	Mix_CloseAudio();
	
}

void warn(char *str) {
	fprintf (stderr, "%s", str);
	exit(1);
}

SDL_Surface *LoadPicture(char *file) {
	SDL_Surface *picture=IMG_Load(file);
	if (!picture) warn("Couldn't load picture");
	SDL_Surface *newpic = SDL_DisplayFormatAlpha(picture);
	if (newpic==NULL) warn("Out of memory!\n");
	SDL_FreeSurface(picture);
	return newpic;
}

GLuint maketexture(SDL_Surface *s) {
#ifdef __BIG_ENDIAN__
	for (Uint32 *p = (Uint32*)(s->pixels); p < (Uint32*)(s->pixels)+s->w*s->h; p++) {
		*p = ((*p&0xff000000)>>24)|((*p&0x00ff0000)>>8)|((*p&0x0000ff00)<<8)|((*p&0x000000ff)<<24);
	}
#endif
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, s->w, s->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, s->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	return texID;
}

float w(float x) {
	return 100+sin(0.5*time+0.005*x)*cos(0.4*time)*30;
}

float dw(float x) {
	return cos(0.5*time+0.005*x)*cos(0.4*time)*30*0.005;
}
