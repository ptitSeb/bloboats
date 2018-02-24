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

#ifndef _SOUND_H_
#define _SOUND_H_


#include <stdlib.h>
#include <stdio.h>

#include "compiling_settings.h"


#if (SOUND == 1)


#include "SDL_mixer.h"

static Mix_Music *fadein_music;
static int fadein_loops;
static int fadein_ms;

namespace sound {
	int Start();
	int Stop();	
	Mix_Chunk *LoadWAV(char *filename);
	void FreeChunk(Mix_Chunk *chunk);
	
	int ChunkVolume(Mix_Chunk *chunk, int volume);
	void SoundVolume(int volume);

	void SetChannels(int channels);
	int ChannelVolume(int channel, int volume);
	void PauseChannel(int channel);
	void ResumeChannel(int channel);
	void HaltChannel(int channel);
	
	int PlayChunk(int channel, Mix_Chunk *chunk, int loops);
	
	Mix_Music *LoadMusic(char *filename);
	void FreeMusic(Mix_Music *music);
	
	int PlayMusic(Mix_Music *music, int loops);


	void FadeInMusic_select(Mix_Music *music, int loops, int ms);
	void FadeInMusic();

	int MusicVolume(int volume);
	void PauseMusic();
	void ResumeMusic();
	int HaltMusic();
	
	int FadeOutMusic(int ms);
	
	int MusicPlaying();

};

#else

namespace sound {
	int Start();
	int Stop();	
	int *LoadWAV(char *filename);
	void FreeChunk(int *chunk);
	
	int ChunkVolume(int *chunk, int volume);
	void SoundVolume(int volume);

	void SetChannels(int channels);
	int ChannelVolume(int channel, int volume);
	void PauseChannel(int channel);
	void ResumeChannel(int channel);
	void HaltChannel(int channel);
	
	int PlayChunk(int channel, int *chunk, int loops);
	
	int *LoadMusic(char *filename);
	void FreeMusic(int *music);
	
	int PlayMusic(int *music, int loops);

	void FadeInMusic_select(int *music, int loops, int ms);
	void FadeInMusic();

	
	int MusicVolume(int volume);
	void PauseMusic();
	void ResumeMusic();
	int HaltMusic();
	
	int FadeOutMusic(int ms);
	
	int MusicPlaying();
};

#endif


#endif

