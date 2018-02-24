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

#include "wall.h"

#include "config.h"

extern graphics Graphics;
extern window Window;
extern config Config;

wall::wall(){
}

wall::wall(GLuint texID, vec oa, vec ob, vec oc) {
	this->texID = texID;

	this->oa=oa;
	this->ob=ob;
	this->oc=oc;

	ab = ob-oa;
	ac = oc-oa;
	bc = oc-ob;

	centerab = oa + ab/2;
	centerac = oa + ac/2;
	centerbc = ob + bc/2;

	ab_hat = ab/ab.abs();
	ac_hat = ac/ac.abs();
	bc_hat = bc/bc.abs();

	ab_x_ac = ab.cross(ac);
	ab_x_ac_inv = 1.0/ab_x_ac;

	ab_len2 = ab.abs2();
	ac_len2 = ac.abs2();
	bc_len2 = bc.abs2();

	ab_lenp2_2 = ab_len2/4;
	ac_lenp2_2 = ac_len2/4;
	bc_lenp2_2 = bc_len2/4;

	ab_lenp2 = sqrt(ab_lenp2_2);
	ac_lenp2 = sqrt(ac_lenp2_2);
	bc_lenp2 = sqrt(bc_lenp2_2);

	ab_len = 2*ab_lenp2;
	ac_len = 2*ac_lenp2;
	bc_len = 2*bc_lenp2;

	type_ab = 0; // edge type
	type_ac = 0;
	type_bc = 0;
	f_ab = 0.0; // edge alignment
	f_ac = 0.0;
	f_bc = 0.0;

// the smallest circle the triangle can be put in:
// either it's midpoint is in the halfway of the longest edge or then in the crossing point of the
// midnormals:

// n = ( AC dot AB - |AC|^2 ) / AB x AC
// x: (oa.x+oc.x)/2 - n*ac.y
// y: (oa.y+oc.y)/2 + n*ac.x

	if (!max_midedge(mp, r)) {
		if (!ab_x_ac) {
			fprintf (stderr, "A wall with no volume!\n");
			alive=false;
			return;
		}

		vec q = vec(-ab.y,ab.x);
		Dbl n = (ac.abs2() - ab.dot(ac) )/(2*ab_x_ac);
		mp = oa + ab/2 + q*n;
		r = (oa-mp).abs();
	}

	r2 = r*r;
	alive=true;
}

void wall::seticetexture(GLuint tex) {
	icetex=true;
	iceID=tex;
}

void wall::setgroundtexture(GLuint tex) {
	groundtex=true;
	groundID=tex;
}

void wall::displayground() {
	float dx = mp.x-Window.centerx;
	float dy = mp.y-Window.centery;

	if ( dx*dx+dy*dy <= (this->r+Window.r)*(this->r+Window.r) ) {
		int x1 = Graphics.projectx(oa.x);
		int y1 = Graphics.projecty(oa.y);
		int x2 = Graphics.projectx(ob.x);
		int y2 = Graphics.projecty(ob.y);
		int x3 = Graphics.projectx(oc.x);
		int y3 = Graphics.projecty(oc.y);

		Graphics.DrawTextured(x1, y1, x2, y2, x3, y3, oa.x/10, oa.y/10, ob.x/10, ob.y/10, oc.x/10, oc.y/10);
	}
}

void wall::displayground_solid() {
	int x1 = Graphics.projectx(oa.x);
	int y1 = Graphics.projecty(oa.y);
	int x2 = Graphics.projectx(ob.x);
	int y2 = Graphics.projecty(ob.y);
	int x3 = Graphics.projectx(oc.x);
	int y3 = Graphics.projecty(oc.y);

	float dx = mp.x-Window.centerx;
	float dy = mp.y-Window.centery;

	if ( dx*dx+dy*dy <= (this->r+Window.r)*(this->r+Window.r) ) {
		Graphics.DrawSolid(x1, y1, x2, y2, x3, y3);
	}
}


void wall::displayedges() {
	vec a = mp-vec(Window.centerx, Window.centery);

	if (a.abs2() <= (this->r +Window.r)*(this->r+Window.r) ) {
		switch(type_ab) {
			case WALL_GROUND:
				if (groundtex) drawedge(groundID, oa, ab, ab_len, f_ab);
			break;
			case WALL_ICE:
				if (icetex) drawedge(iceID, oa, ab, ab_len, f_ab);
			break;
			default:
			break;
		}
		switch(type_ac) {
			case WALL_GROUND:
				if (groundtex) drawedge(groundID, oa, ac, ac_len, f_ac);
			break;
			case WALL_ICE:
				if (icetex) drawedge(iceID, oa, ac, ac_len, f_ac);
			break;
			default:
			break;
		}
		switch(type_bc) {
			case WALL_GROUND:
				if (groundtex) drawedge(groundID, ob, bc, bc_len, f_bc);
			break;
			case WALL_ICE:
				if (icetex) drawedge(iceID, ob, bc, bc_len, f_bc);
			break;
			default:
			break;
		}

	}

}

void wall::drawedge(GLuint tex, vec OA, vec AB, Dbl AB_LEN, Dbl F_AB) {
	float dx = mp.x-Window.centerx;
	float dy = mp.y-Window.centery;

	if (Config.decoration != DEC_UGLY) {

		if ( dx*dx+dy*dy <= (this->r+Window.r)*(this->r+Window.r)+2 ) {
			vec N = vec(-AB.y, AB.x);
			N /= N.abs();
	
			vec p1 = OA-N*(0.1*F_AB);
			vec p2 = OA+AB-N*(0.1*F_AB);
			vec p3 = OA+N*(0.6*F_AB)-AB*(0.3/AB_LEN);
			vec p4 = OA+AB+N*(0.6*F_AB)+AB*(0.3/AB_LEN);
	
			int x1 = Graphics.projectx(p1.x); int y1 = Graphics.projecty(p1.y);
			int x2 = Graphics.projectx(p2.x); int y2 = Graphics.projecty(p2.y);
			int x3 = Graphics.projectx(p3.x); int y3 = Graphics.projecty(p3.y);
			int x4 = Graphics.projectx(p4.x); int y4 = Graphics.projecty(p4.y);
	
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBindTexture(GL_TEXTURE_2D, tex);
			#ifdef HAVE_GLES
			GLfloat vtx1[] = {
				x1, y1,
				x2, y2,
				x4, y4,
				x3, y3
			};
			GLfloat tex1[] = {
				r/20.0, 0,
				(AB_LEN+r)/20.0, 0,
				(AB_LEN+r)/20.0, 1/16.0,
				r/20.0, 1/16.0
			};
		 
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		 
			glVertexPointer(3, GL_FLOAT, 0, vtx1);
			glTexCoordPointer(2, GL_FLOAT, 0, tex1);
			glDrawArrays(GL_TRIANGLE_FAN,0,4);
		 
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			#else
			glBegin(GL_QUADS);
	
			glTexCoord2f(r/20.0, 0);
			glVertex2f(x1, y1);
	
			glTexCoord2f((AB_LEN+r)/20.0, 0);
			glVertex2f(x2, y2);
	
			glTexCoord2f( (AB_LEN+r)/20.0, 1/16.0 );
			glVertex2f(x4, y4);
	
			glTexCoord2f( r/20.0, 1/16.0 );
			glVertex2f(x3, y3);
	
			glEnd();
			#endif

		}
	} else { // ugly
		vec p1 = OA;
		vec p2 = OA+AB;
	
		int x1 = Graphics.projectx(p1.x); int y1 = Graphics.projecty(p1.y);
		int x2 = Graphics.projectx(p2.x); int y2 = Graphics.projecty(p2.y);
	
		if (groundtex && tex == groundID) {
			glColor3f(1.0, 0.8, 0.4);
		} else if (icetex && tex == iceID) {
			glColor3f(0.7, 0.8, 1.0);
		}
	
		#ifdef HAVE_GLES
		Graphics.drawLine(x1, y1, x2, y2);
		#else
		glBegin(GL_LINES);
		glVertex2d(x1, y1);
		glVertex2d(x2, y2);
		glEnd();
		#endif

		glColor3f(1.0, 1.0, 1.0);
	}

}

bool wall::max_midedge(vec &mp, float &r) {
	int longest = 0;

	if (ab_lenp2 >= ac_lenp2 && ab_lenp2 >= bc_lenp2) longest = 1;
	else if (ac_lenp2 >= ab_lenp2 && ac_lenp2 >= bc_lenp2) longest = 2;
	else if (bc_lenp2 >= ab_lenp2 && bc_lenp2 >= ac_lenp2) longest = 3;

	switch(longest) {
		case 1:
			mp = (oa+ob)/2;
			r = ab_lenp2;
			if ( (mp-oc).abs2() >= ab_lenp2_2) return false;
		break;
		case 2:
			mp = (oa+oc)/2;
			r = ac_lenp2;
			if ( (mp-ob).abs2() >= ac_lenp2_2) return false;
		break;
		case 3:
			mp = (ob+oc)/2;
			r = bc_lenp2;
			if ( (mp-oa).abs2() >= bc_lenp2_2) return false;
		break;
		default:
		break;
	}
	return true;
}
