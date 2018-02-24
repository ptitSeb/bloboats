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

#include "path.h"

#include <sys/stat.h>
#include <sys/types.h>

#include "compiling_settings.h"

#include <iostream>

#include <dirent.h>

using namespace std;



path::path() {
	const char *cfgfile;

	cfgfile = DEFAULT_CONFIGFILE;
	FILE *fp = fopen(cfgfile, "r");

	if (!fp) {
		cfgfile = "bloboats.dirs";
		fp = fopen("bloboats.dirs", "r");
	}

	datadir=0;
	privdir=0;

	if (!fp) {
		fprintf (stderr, "Couldn't open file \"%s\".\n", DEFAULT_CONFIGFILE);
		fprintf (stderr, "Couldn't open file \"%s\".\n", "bloboats.dirs");
		fprintf (stderr, "Exiting. Try doing 'make config'.\n");
		exit(1);	

	} else {

		// parse the file

#if DEBUG == 1
		fprintf (stderr, "Config file parsed.\n");
#endif

		char s[1024];
		char d1[24];
		char d2[1020];

		string p;

		int linenum=0;

		while (fgets(s, 1024, fp)) {
			int type=0;
	
			if ( sscanf(s, "%s %s", d1, d2 ) == 2) {
				if (strncmp(d1, "datadir", 7) == 0) {
					type=1;
				}
				if (strncmp(d1, "privatedir", 10) == 0) {
					type=2;
				}

				string envname;
				bool done=false;
				p="";

				for (int i=0; !done; i++) {
					if (d2[i]=='$') { // read variable
						envname="";

						while (1) {
							i++;

							if ( (d2[i]>='a' && d2[i]<='z') || (d2[i]>='A' && d2[i]<='Z') || d2[i]=='_') {
								envname += d2[i];
							} else {
								p+=getenv(envname.c_str());
								if (d2[i]=='\0') done=true;
								else p+=d2[i];
								break;
							}
						}

					} else if (d2[i]=='~') p+=getenv("HOME");
					else if (d2[i]=='\0') done=true;
					else p+=d2[i];
				}


			} else {
				fprintf (stderr, "Erroneous line: \"%s\":%d\n", cfgfile, linenum);
			}
	
			if (type>0) {
				if (d2) {
					char *ps = new char[p.size()+1];
					strncpy (ps, p.c_str(), p.size()+1);
		
					if (type==1) datadir = ps;
					if (type==2) privdir = ps;

#if DEBUG == 1
					fprintf (stderr, "Type = %d. String: \"%s\"\n", type, ps);
#endif

				} else {
					fprintf (stderr, "Error in config file. (%s)\n", cfgfile);
					exit(1);
				}
			}

			linenum++;
	
		} // while

		fclose(fp);

	} // else

	if (!datadir) {
#if DEBUG == 1
		fprintf (stderr, "No datadir set. Assuming \"data\"\n");
#endif
		datadir = new char[5];
		sprintf (datadir, "data");
	}

	if (!privdir) {
#if DEBUG == 1
		fprintf (stderr, "No privatedir set. Assuming \"private\"\n");
#endif
		privdir = new char[8];
		sprintf (privdir, "private");
	}

}

path::~path() {
	delete[] datadir;
	delete[] privdir;
}

char *path::data(char *s) {
	if ( strlen(s) >= 1024 ) {
		fprintf (stderr, "Error: In path::data(char *s), strlen(s) must be < 1024.\n");
		exit(1);
	}

	sprintf (strtemp, "%s/%s", datadir, s);
	return strtemp;
}

char *path::priv(char *s) {
	if ( strlen(s) >= 1024 ) {
		fprintf (stderr, "Error: In path::priv(char *s), strlen(s) must be < 1024.\n");
		exit(1);
	}

	sprintf (strtemp, "%s/%s", privdir, s);
	return strtemp;
}


bool path::copydir( char *from, char *to, int depth ) {

#if DEBUG == 1
	fprintf (stderr, "Recursing to directory \"%s\".\n", to);
#endif

	DIR *dir = opendir (from);
	if (!dir) {
#if DEBUG == 1
		fprintf (stderr, "Couldn't open directory \"%s\".\n");
#endif
		return false;
	}

	if (depth >= 100) {
#if DEBUG == 1
		fprintf (stderr, "Recursing too deep. Exiting for safety reasons.\n");
		fprintf (stderr, "Copying \"%s\" to \"%s\" failed.\n");
#endif
		closedir(dir);
		return false;
	}

	while (1) {
      dirent *p = readdir(dir);
      if (p == NULL) break;

 		if (strcmp(p->d_name, ".")!=0 && strcmp(p->d_name, "..")!=0 ) {

#if DEBUG == 1
			fprintf (stderr, "Read file: \"%s\" 1\n", p->d_name);
#endif

			char *frompath = new char[ strlen(from) + strlen(p->d_name) + 2 ];
			char *topath = new char[ strlen(to) + strlen(p->d_name) + 2 ];

			sprintf (frompath, "%s/%s", from, p->d_name);
			sprintf (topath, "%s/%s", to, p->d_name);

#if DEBUG == 1
			fprintf (stderr, "frompath: \"%s\"\n", frompath);
			fprintf (stderr, "topath: \"%s\"\n", topath);
#endif

			DIR *d = opendir( frompath ); // hax

			if (d != NULL) {

				// it's a directory
				closedir(d);

				// create directory to to-path


#ifdef WIN32
				if ( mkdir( topath ) != 0 ) {
#else
				if ( mkdir( topath, 00775 ) != 0 ) {
#endif
					fprintf (stderr, "Couldn't create directory \"%s\". Critical.\n", topath);

					delete[] frompath;
					delete[] topath;
					return false;
				}
			
				// and copy the rest of the from-directory
				copydir( frompath, topath, depth+1 );
					
			} else {
				copyfile( frompath, topath );
#if DEBUG == 1
				fprintf (stderr, "Copied \"%s\" to \"%s\".\n", frompath, topath);
#endif
			}


			delete[] topath;
			delete[] frompath;
		}

	}

	closedir(dir);

	return true;
}

void path::copyfile(char *from, char *to) {
   FILE *infile = fopen(from, "rb");
   FILE *outfile = fopen(to, "wb");

   if (!infile) {
      fprintf (stderr, "Couldn't open \"%s\" for reading! Critical!\n", from);
      exit(1);
   }
   if (!outfile) {
      fprintf (stderr, "Couldn't open \"%s\" for writing! Critical!\n", to);
      exit(1);
   }

   // copy file

   while(1) {
      int q = fgetc(infile);
      if (q==EOF) break;
      fputc(q, outfile);
   }

   fclose(outfile);
   fclose(infile);
}
