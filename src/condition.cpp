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
#include "condition.h"
#include "compat.h"

#include <stdlib.h>


condition::condition(Uint8 t, Uint8 r, enum varlist vl, ... ) {


	va_list ap;
	int ptype;

	error = false;
	size = 0;
	wait = 0;
	sent = 0;

	id = 0;
	type = t;
	req = r;

	add_uint8(type);
	add_uint8(req);



	char c, *p;
	float f;
	Dbl d;
	Uint8 u8;
	Uint16 u16;
	Uint32 u32;

	if (vl == VAR_LIST) {
		va_start(ap, vl);
		bool done=false;

		do {
			ptype = va_arg (ap, int);

			switch (ptype) {
				case T_CHAR:
					c = va_arg(ap, int);
					add_char(c);
				break;
				case T_UINT8:
					u8 = va_arg(ap, int);
					add_uint8(u8);
				break;
				case T_UINT16:
					u16 = va_arg(ap, int);
					add_uint16(u16);
				break;
				case T_UINT32:
					u32 = va_arg(ap, int);
					add_uint32(u32);
				break;
				case T_FLOAT:
					f = va_arg(ap, Dbl);
					add_float(f);
				break;
				case T_DOUBLE:
					d = va_arg(ap, Dbl);
					add_double(d);
				break;
				case T_KEY:
					p = va_arg(ap, char*);
					add_key(p);
				break;
				case T_STRING200:
					p = va_arg(ap, char*);
					add_string(p);
				break;
				default:
					done=true;
				break;
			}

		} while(!done);

	} // if

}


condition::condition(condition &c) {
	memcpy(data, c.data, c.size);
	size=c.size;
	req=c.get_req();
	cursor=c.cursor;
	id=c.get_id();
	type=c.get_type();

	channel=c.channel;
	rewind();
}

condition::condition() {
	size = 0;
	req = 0;
	cursor = 0;
	id = 0;
	type = 0;
	channel = 0;
}


condition::~condition() {
}


void condition::rewind() {
	cursor = 0;
}


void condition::add_char(char c) {
	if (size > 4096-1) {
		fprintf (stderr, "Error: no space in condition!\n");
		exit(1);
	}

	Uint8 *p = (Uint8*)&c;

	data[size++] = *p;
}

void condition::add_uint8(Uint8 u) {
	if (size > 4096-1) {
		fprintf (stderr, "Error: no space in condition!\n");
		exit(1);
	}

	data[size++] = u;
}

void condition::add_uint16(Uint16 u) {
	if (size > 4096-2) {
		fprintf (stderr, "Error: no space in condition!\n");
		exit(1);
	}

	Uint16 *p = (Uint16*) &data[size];
	*p = SWAP16(u);

	size += 2;
}



void condition::add_uint32(Uint32 u) {
	if (size > 4096-4) {
		fprintf (stderr, "Error: no space in condition!\n");
		exit(1);
	}

	Uint32 *p = (Uint32*) &data[size];
	*p = SWAP32(u);

	size += 4;
}


void condition::add_float(float u) {
	if (size > 4096-4) {
		fprintf (stderr, "Error: no space in condition!\n");
		exit(1);
	}

	Uint32 *p = (Uint32*) &data[size];
	Uint32 *q = (Uint32*) &q;
	*p = SWAP32(*q);

	size += 4;
}


void condition::add_double(Dbl u) {
	if (size > 4096-8) {
		fprintf (stderr, "Error: no space in condition!\n");
		exit(1);
	}

	Uint32 *q = (Uint32*) &q;

#if SDL_BYTEORDER == LITTLE_ENDIAN
	Uint32 *p1 = (Uint32*) &data[size];
	Uint32 *p2 = (Uint32*) &data[size+4];

	*p2 = SWAP32(*q++);
	*p1 = SWAP32(*q);
#else
	Uint32 *p = (Uint32*) &data[size];
	*(p++) = *(q++);
	*p = *q;
#endif

	size += 8;
}


void condition::add_key(char *k) {
	// adds a 10-byte 'key' to data

	if (size > 4096-10) {
		fprintf (stderr, "Error: no space in condition!\n");
		exit(1);
	}

	int p;
	for (p=0; p<10; p++) {
		if (k[p]=='\0') break;
		add_char(k[p]);
	}
	for (; p<10; p++) {
		add_char('\0');
	}

}

void condition::add_string(char *s) {
	// adds a 200-byte string to data

	if (size > 4096-200) {
		fprintf (stderr, "Error: no space in condition!\n");
		exit(1);
	}

	int p;
	for (p=0; p<200; p++) {
		if (s[p]=='\0') break;
		add_char(s[p]);
	}
	for (; p<200; p++) {
		add_char('\0');
	}
}

char condition::get_char() {
	if (cursor >= size) {
		fprintf (stderr, "Error.\n");
		set_error("Error: no more data in condition!");
		return 0;
	}


	char *p = (char*)&data[cursor++];

	return *p;
}

Uint8 condition::get_uint8() {
	if (cursor >= size) {
		set_error("Error: no more data in condition!");
		return 0;
	}

	return data[cursor++];
}

Uint16 condition::get_uint16() {
	if (cursor >= 1023) {
		set_error("Error: no more data in condition!");
		return 0;
	}

	Uint16 *p = (Uint16*)&data[cursor];
	cursor += 2;

	return SWAP16(*p);
}

Uint32 condition::get_uint32() {
	if (cursor >= size-3) {
		set_error("Error: no more data in condition!");
		return 0;
	}

	Uint32 *p = (Uint32*)&data[cursor];
	cursor += 4;

	return SWAP32(*p);
}

float condition::get_float() {
	if (cursor >= size-3) {
		set_error("Error: no more data in condition!");
		return 0;
	}

	Uint32 *p = (Uint32*)&data[cursor];
	float *f = (float*)SWAP32(*p);

	cursor += 4;

	return *f;
}

Dbl condition::get_double() {
	if (cursor >= size-8) {
		set_error("Error: no more data in condition!");
		return 0;
	}

	Dbl d;

	Uint32 *q = (Uint32*) &data[cursor];

#if SDL_BYTEORDER == BIG_ENDIAN

	Uint32 *p1 = (Uint32*) &d;
	Uint32 *p2 = p1+1;

	*p2 = SWAP32(*q++);
	*p1 = SWAP32(*q);
#else
	Uint32 *p = (Uint32*)&d;
	*(p++) = *(q++);
	*p = *q;
#endif

	cursor += 8;
}

char *condition::get_key() {
	// reads a 10-byte 'key' from data

	if (cursor >= size-10) {
		set_error("Error: no more data in condition!\n");
		return 0;
	}

	char *str = new char[11];

	for (int p=0; p<10; p++) str[p] = get_char();
	str[10]='\0';

	return str;
}

char *condition::get_string() {
	// reads a 200-byte string from data

	if (cursor >= size-200) {
		set_error("Error: no more data in condition!\n");
		return 0;
	}

	char *str = new char[201];

	for (int p=0; p<200; p++) str[p] = get_char();
	str[200]='\0';

	return str;
}


bool condition::is_error() {
	return error;
}

void condition::set_error(char *str) {
	int len = strlen(str);
	if (len > 1023) {
		fprintf (stderr, "Condition::set_error: error message too long!\n");
		len=1023;
	}

	strncpy (error_str, str, len);
	error_str[1023] = '\0';

	error = true;
}

const char *condition::get_error() {
	return (const char*)error_str;
}

Uint8 *condition::get_data() {
	return data;
}

Uint8 condition::get_req() {
	return req;
}

Uint32 condition::get_size() {
	return size;
}

Uint32 condition::get_id() {
	return id;
}

Uint8 condition::get_type() {
	return type;
}

void condition::set_id(Uint32 i) {
	id = i;
}

void condition::set_type(Uint8 t) {
	type = t;
}

void condition::set_req(Uint8 r) {
	req = r;
}

void condition::set_data(void *d, int s) {
	memcpy( data, d, s);
	size = s;
}

*/
