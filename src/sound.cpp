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

#include "sound.h"

#include "compiling_settings.h"

#include "config.h"
extern config Config;

static int _channels=0;


#if (SOUND == 1)


int sound::Start() {
#if DEBUG == 1
	fprintf (stderr, "Calling Mix_OpenAudio...\n");
#endif
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2 /* stereo */, 4096)==-1) {
		fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}

#if DEBUG == 1
	fprintf (stderr, "Started sound system.\n");
#endif

	return 1;
}

int sound::Stop() {
	Mix_CloseAudio();
#if DEBUG == 1
	fprintf (stderr, "Stopped sound system.\n");
#endif
	return 1;
}

Mix_Chunk *sound::LoadWAV(char *filename) {
	if (Config.sounds) {
		Mix_Chunk *sample;
		sample=Mix_LoadWAV(filename);
		if(!sample) {
			fprintf(stderr, "Couldn't load sample \"%s\": Mix_LoadWAV: %s\n", filename, Mix_GetError() );
			exit(1);
		}
#if DEBUG == 1
		fprintf (stderr, "Sample \"%s\" loaded.\n", filename);
#endif
		return sample;
	}
}

void sound::FreeChunk(Mix_Chunk *chunk) {
	if (Config.sounds) {
		Mix_FreeChunk(chunk);
		chunk=0;
	}
}

int sound::ChunkVolume(Mix_Chunk *chunk, int volume) {
	if (Config.sounds) return Mix_VolumeChunk(chunk, volume);
	return 0;
}


void sound::SoundVolume(int volume) {
	if (Config.sounds) {
		ChannelVolume(-1, volume);
	}
}


void sound::SetChannels(int channels) {
	if (Config.sounds) {
		Mix_AllocateChannels(channels);
		_channels=channels;
	}
}

int sound::ChannelVolume(int channel, int volume) {
	if (Config.sounds) return Mix_Volume(channel, volume);
	return 127;
}

void sound::PauseChannel(int channel) {
	if (Config.sounds) Mix_Pause(channel);
}
void sound::ResumeChannel(int channel) {
	if (Config.sounds) Mix_Resume(channel);
}
void sound::HaltChannel(int channel) {
	if (Config.sounds) Mix_HaltChannel(channel);
}

int sound::PlayChunk(int channel, Mix_Chunk *chunk, int loops) {
	if (Config.sounds) return Mix_PlayChannel(channel, chunk, loops);
	return 0;
}

Mix_Music *sound::LoadMusic(char *filename) {
	if (Config.sounds) {
		Mix_Music *music;
		music = Mix_LoadMUS(filename);
		if (!music) {
			fprintf (stderr, "Mix_LoadMUS(\"%s\"): %s\n", filename, Mix_GetError());
			return 0;
		}
		return music;
	}
	return 0;
}

void sound::FreeMusic(Mix_Music *music) {
	if (Config.sounds && music) {
		Mix_FreeMusic(music);
		music=NULL;
	}
}

int sound::PlayMusic(Mix_Music *music, int loops) {
	if (Config.sounds && music) {
		if (Mix_PlayMusic(music, loops)==-1) {
			fprintf(stderr, "Mix_PlayMusic: %s\n", Mix_GetError());
			return 0;
	 	}
		return 1;
	}
	return 1;
}

void sound::FadeInMusic_select(Mix_Music *music, int loops, int ms) {
	fadein_music = music;
	fadein_loops = loops;
	fadein_ms = ms;
}

void sound::FadeInMusic() {
	Mix_FadeInMusic(fadein_music, fadein_loops, fadein_ms);
}

int sound::MusicVolume(int volume) {
	if (Config.sounds) return Mix_VolumeMusic(volume);
	return 127;
}

void sound::PauseMusic() {
	if (Config.sounds) Mix_PauseMusic();
}

void sound::ResumeMusic() {
	if (Config.sounds) Mix_ResumeMusic();
}

int sound::HaltMusic() {
	if (Config.sounds) return Mix_HaltMusic();
	return 0;
}

int sound::FadeOutMusic(int ms) {
	if (Config.sounds) return Mix_FadeOutMusic(ms);
	return 0;
}

int sound::MusicPlaying() {
	if (Config.sounds) return Mix_PlayingMusic();
	return 0;
}



#else



int sound::Start() {
	return 1;
}

int sound::Stop() {
	return 1;
}

int *sound::LoadWAV(char *filename) {
	return 0;
}

void sound::FreeChunk(int *chunk) {
}

int sound::ChunkVolume(int *chunk, int volume) {
	return 1;
}


void sound::SoundVolume(int volume) {
}


void sound::SetChannels(int channels) {
}

int sound::ChannelVolume(int channel, int volume) {
	return 127;
}

void sound::PauseChannel(int channel) {
}
void sound::ResumeChannel(int channel) {
}
void sound::HaltChannel(int channel) {
}

int sound::PlayChunk(int channel, int *chunk, int loops) {
	return 1;
}

int *sound::LoadMusic(char *filename) {
	return 0;
}

void sound::FreeMusic(int *music) {
}

int sound::PlayMusic(int *music, int loops) {
	return 1;
}

void sound::FadeInMusic_select(int *music, int loops, int ms) {
}

void sound::FadeInMusic() {
}

int sound::MusicVolume(int volume) {
	return 127;
}

void sound::PauseMusic() {
}

void sound::ResumeMusic() {
}

int sound::HaltMusic() {
	return 1;
}

int sound::FadeOutMusic(int ms) {
	return 1;
}

int sound::MusicPlaying() {
	return 1;
}

#endif

