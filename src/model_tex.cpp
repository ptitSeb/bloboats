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

#include "model_tex.h"

extern window Window;
extern graphics Graphics;

model_tex::model_tex():
	texID(0),t1(),t2(),t3(),p1(0),p2(0),p3(0)
{

}

model_tex::model_tex(GLuint texID, vec t1, vec t2, vec t3, particle *p1, particle *p2, particle *p3):
	texID(texID),t1(t1),t2(t2),t3(t3),p1(p1),p2(p2),p3(p3) {
}

void model_tex::display() {
	Graphics.DrawTextured(
		Graphics.projectx(p1->p.x), Graphics.projecty(p1->p.y),
		Graphics.projectx(p2->p.x), Graphics.projecty(p2->p.y),
		Graphics.projectx(p3->p.x), Graphics.projecty(p3->p.y),
		t1.x, t1.y, t2.x, t2.y, t3.x, t3.y);
}

void model_tex::display_solid() {
	Graphics.DrawSolid(
		Graphics.projectx(p1->p.x), Graphics.projecty(p1->p.y),
		Graphics.projectx(p2->p.x), Graphics.projecty(p2->p.y),
		Graphics.projectx(p3->p.x), Graphics.projecty(p3->p.y)
	);
}

void model_tex::display_lines() {
	Graphics.OutlineTriangle(
		Graphics.projectx(p1->p.x), Graphics.projecty(p1->p.y),
		Graphics.projectx(p2->p.x), Graphics.projecty(p2->p.y),
		Graphics.projectx(p3->p.x), Graphics.projecty(p3->p.y)
	);

}
