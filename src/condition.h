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

/*
#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <stdarg.h>

#include "SDL.h"

const int COMMANDS = 6;

enum varlist {
	VAR_LIST,
	NO_VAR_LIST
};

enum argtype {
	END_OF_LIST,
	T_CHAR,
	T_UINT8,
	T_UINT16,
	T_UINT32,
	T_FLOAT,
	T_DOUBLE,
	T_KEY,
	T_STRING200
};

const int as[9] = {
	0,1,1,2,4,4,8,10,200
};

enum command {
	JOIN,
	TRACKER,
	ACK,
	SENDFILE,
	CONNECT,
	CONN_ACC
};

const int commandsize[COMMANDS] = {
	// Join: id-code, key nick, key pass
	as[T_UINT32] + as[T_KEY] + as[T_KEY],
	// Tracker: ip, port
	as[T_UINT32] + as[T_UINT16],
	// Ack: id
	as[T_UINT16],
	// Sendfile: special
	0,
	// Connect: key nick, key pass
	as[T_KEY] + as[T_KEY],
	// Connection accepted: uint32 sec_id
	as[T_UINT32]
};


enum reqlist {
	NO_REQ,
	REQ
};

class condition {
public:
	condition(Uint8 t, Uint8 r, enum varlist vl, ... );
	condition();

	condition(condition &c);

	~condition();

	void rewind();

	void add_char(char c);
	void add_uint8(Uint8 u);
	void add_uint16(Uint16 u);
	void add_uint32(Uint32 u);
	void add_float(float u);
	void add_double(Dbl u);
	void add_key(char *k);
	void add_string(char *s);

	char get_char();
	Uint8 get_uint8();
	Uint16 get_uint16();
	Uint32 get_uint32();
	float get_float();
	Dbl get_double();
	char *get_key();
	char *get_string();


	bool is_error();
	void set_error(char *str);

	const char *get_error();


	Uint8 *get_data();
	Uint32 get_size();
	Uint32 get_id();
	Uint8 get_req();
	Uint8 get_type();

	void set_id(Uint32 i);
	void set_type(Uint8 t);
	void set_req(Uint8 r);
	void set_data(void *d, int s);

	Uint32 wait;
	Uint32 sent;

	int channel;
	Uint32 cursor; // used for reading

	Uint8 data[4096];

private:
	Uint32 size;
	Uint8 req;

	char error_str[512];
	bool error;

	Uint16 id;
	Uint8 type;




};




#endif
*/
