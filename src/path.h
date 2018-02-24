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

#ifndef _PATH_H_
#define _PATH_H_

#include "compiling_settings.h"

#ifdef WIN32
#ifndef DEFAULT_CONFIGFILE
#define DEFAULT_CONFIGFILE "./bloboats.dirs"
#endif
#else
#ifndef DEFAULT_CONFIGFILE
#ifdef PANDORA
#define DEFAULT_CONFIGFILE "/mnt/utmp/bloboats/etc/bloboats.dirs"
#else
#define DEFAULT_CONFIGFILE "/etc/bloboats.dirs"
#endif
#endif
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXDEPTH 100


class path {
public:
	path();
	~path();

	char *data(char *s);
	char *priv(char *s);

	bool copydir(char *from, char *to, int depth);
	void copyfile(char *from, char *to);

	char strtemp[2048];

	char *datadir;
	char *privdir;

};

#endif
