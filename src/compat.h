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

#ifndef _COMPAT_H_
#define _COMPAT_H_

#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "SDL_endian.h"

#include "vec.h"

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SWAP16(X)    (X)
#define SWAP32(X)    (X)
#else
#define SWAP16(X)    SDL_Swap16(X)
#define SWAP32(X)    SDL_Swap32(X)
#endif

namespace compat {

		Dbl scraptail(Dbl p);

		// type conversions

		int Str2Int(char *str, int from);

		// net tools

		Uint32 Str2IP(char *str, int from); /* This is useless because of SDLNet_ResolveHost */
		char * IP2Str(Uint32 ip); /* This is useless because of SDLNet_ResolveIP */

		// string checkers

		bool Str_CheckLastChars (char *filepath, char *extension); // Checks, for example, the extension of a file name.

		// file functions

		bool file_exists(char *f);

		char *getstring(FILE *fp);
		void putstring(FILE *fp, char *str);

		void ToFile_Float(FILE *stream, float f);
		void FromFile_Float(FILE *stream, float *f);

		void ToFile_Int(FILE *stream, Uint32 data);
		void FromFile_Int(FILE *stream, Uint32 *data);

		void ToFile_SInt(FILE *stream, int data); // Signed int
		void FromFile_SInt(FILE *stream, int *data);

		void ToFile_ShortInt(FILE *stream, Uint16 data);
		void FromFile_ShortInt(FILE *stream, Uint16 *data);

		int FromFile_ShortInt_EOF(FILE *stream, Uint16 *data);

		char *str_split(char *str, char ch, int num); // remember to delete the new char-array!
		int str_splitgetint(char *str, char ch, int num);
		bool str_splitcompare(char *str, int num, char *str2);
		char *str_fullsplit(char *str, char ch, int num); // remember to delete the new char-array!

};




#endif
