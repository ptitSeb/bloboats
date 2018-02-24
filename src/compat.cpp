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

#include "compat.h"

Dbl compat::scraptail(Dbl p) {
	unsigned char *q = (unsigned char*)&p;
	*q = (*q) & 0xF0;
	return *(Dbl*)q;
}

int compat::Str2Int(char *str, int from) {
	int luku=0;
	int len=strlen(str);
	bool found=false;
	int minus=0;

	for (int i=from; (i<len && !found); i++) {
		if (str[i]=='-') minus++;
		if ( str[i] >='0' && str[i] <= '9' ) {
			from=i;
			found=true;
		}
	}

	if (found==false) return -1;
//	found=false;

	for (int i=from; i<len; i++) {
		char ch=str[i];
		if (ch <'0' || ch >'9') break;
		luku=luku*10+(str[i]-'0');
	}

	if ((minus%2)==1) return -luku;

	return luku;
}

Uint32 compat::Str2IP(char *str, int from) {
	int i=from;
	int len = strlen(str);
	int ip[4]={0};
	int cip=0;

	for (i=from; (i<len && cip<4); i++) {
		if (str[i]=='.') cip++;
		if (str[i]>='0' || str[i]<='9') ip[cip]=ip[cip]*10+str[i]-'0';
	}
	return (ip[3] <<24) | (ip[2]<<16) | (ip[1]<<8) | ip[0];

}

char * compat::IP2Str(Uint32 ip) { // remember to delete!
	char *str = new char[16];
	sprintf (str, "%d.%d.%d.%d", (Uint8)ip, (Uint8)(ip>>8), (Uint8)(ip>>16), (Uint8)(ip>>24) );
	return str;
}

bool compat::Str_CheckLastChars (char *filepath, char *extension) { // Checks, for example, the extension of a file name.
	if ( strcmp ( &filepath[strlen(filepath)-strlen(extension)], extension ) == 0 ) return 1;
	return 0;
}


bool compat::file_exists(char *f) { // a quick hack :-)
	FILE *fp = fopen(f, "rb");
	if (fp) {
		fclose (fp);
		return true;
	}
	return false;
}

void compat::ToFile_Float(FILE *stream, float f) {
	fwrite (&f, sizeof(float), 1, stream);
}

void compat::FromFile_Float(FILE *stream, float *f) {
	if ( fread (f, sizeof(float), 1, stream) != 1 ) {
		fprintf (stderr, "Corrupted file! Critical!\n");
		exit(1);
	}
}

void compat::ToFile_Int(FILE *stream, Uint32 data) {
	fwrite (&data, sizeof(Uint32), 1, stream);
}
void compat::ToFile_SInt(FILE *stream, int data) {
	fwrite (&data, sizeof(int), 1, stream);
}

void compat::FromFile_Int(FILE *stream, Uint32 *data) {
	if ( fread (data, sizeof(Uint32), 1, stream) != 1 ) {
		fprintf (stderr, "Corrupted file! Critical!\n");
		exit(1);
	}

	*data = SWAP32(*data);
}

void compat::FromFile_SInt(FILE *stream, int *data) {
	if ( fread (data, sizeof(int), 1, stream) != 1 ) {
		fprintf (stderr, "Corrupted file! Critical!\n");
		exit(1);
	}

	*data = SWAP32(*data);
}

void compat::ToFile_ShortInt(FILE *stream, Uint16 data) {
	fwrite (&data, sizeof(Uint16), 1, stream);
	data = SWAP16(data);
}

void compat::FromFile_ShortInt(FILE *stream, Uint16 *data) {
	if ( fread (data, sizeof(Uint16), 1, stream) != 1) {
		fprintf (stderr, "Corrupted file! Critical!\n");
		exit(1);
	}
}

int compat::FromFile_ShortInt_EOF(FILE *stream, Uint16 *data) {
	return fread (data, sizeof(Uint16), 1, stream);
}

char *compat::str_split(char *str, char ch, int num) {
	int i=0;
	int len=strlen(str);
	int n=0;

	// search to the beginning of the num'th piece

	for (i=0; n<num; i++) {
		if (i>=len) return 0;

		if (str[i]==ch) {
			n++;
		}
	}
	if (n<num) return 0;

	int start=i;

	// find out how long it is

	while (1) {
		if (str[i]==ch) break;
		if (i>=len) break;
		i++;
	}

	// Now length is i-start. Create the new array and mark it to end at the right place.

	// there's no information?
	if (i-start==0) return 0;

	char *parameter = new char[i-start+1];
	strncpy(parameter, &(str[start]), (i-start));
	parameter[i-start]='\0';

	return parameter;
}

bool compat::str_splitcompare(char *str, int num, char *str2) {
	bool ret;
	char *c = str_split(str, ' ', num);
	if (c) {
		if (strcmp(c, str2)==0) ret=true;
		else ret=false;
		delete[] c;
	} else return false;

	return ret;
}

int compat::str_splitgetint(char *str, char ch, int num) {
	int r=-1;
	char *c = str_split(str, ch, num);
	if (c) {
		r = Str2Int(c, 0);
		delete[] c;
		return r;
	}
	else return -1;

}

char *compat::str_fullsplit(char *str, char ch, int num) {
	int i=0;
	int len=strlen(str);
	int n=0;

	// search to the beginning of the num'th piece

	for (i=0; n<num; i++) {
		if (i>=len) return 0;

		if (str[i]==ch) {
			n++;
		}
	}
	if (n<num) return 0;

	int start=i;

	i = len;

	// Now length is i-start. Create the new array and mark it to end at the right place.

	// there's no information?
	if (i-start==0) return 0;

	char *parameter = new char[i-start+1];
	strncpy(parameter, &(str[start]), (i-start));
	parameter[i-start]='\0';

	return parameter;
}


char * compat::getstring(FILE *fp) {
	char *str = new char[200];

	int i=0;
	while (i<200) {
		int q = fgetc(fp);
		if (q==EOF || q=='\0') {
			str[i] = '\0';
			return str;
		}
		str[i]=(char)q;
		i++;
	}

	delete[] str;

	fprintf(stderr, "Corrupted file! Critical!\n");
	exit(1);
}


void compat::putstring(FILE *fp, char *str) {
	int i=0;
	while(1) {
		fputc(str[i], fp);
		if (str[i] == '\0') break;
		i++;
	}
}
