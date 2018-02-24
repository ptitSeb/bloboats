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

#include "particle.h"

#define sign(x) ((x>0)-(x<0))

extern graphics Graphics;
extern window Window;

particle::particle(){
	alive=false;
	onwall=0;
	sticky=false;
	immobile=false;
	damage=0;
	inwater=false;
	cheatmode=false;

}
particle::particle(vec p, vec v, Dbl mass, Dbl g, Dbl bfactor):p(p),v(v),mass(mass),g(g),bfactor(bfactor) {
	alive=true;
	onwall=0;
	sticky=false;
	immobile=false;
	damage=0;
	inwater=false;
	cheatmode=false;
}

void particle::set(vec p, vec v) {
	this->p=p;
	this->v=v;
}

void particle::act(Dbl dt, wall **walls, int numwalls) {
	// if we're close enough to a wall, set onwall even if we're not going towards the wall
	if (!alive) return;

	onwall = 0;

	Dbl min_dist=100000;

/*
	wall **checktable = new wall*[numwalls]; // mark if we really won't hit.
	int nearbywalls = 0;

	for (int i=0; i<numwalls; i++) {
		if ( (walls[i]->mp-p).abs2() <= walls[i]->r2+1 ) {
			checktable[nearbywalls++] = walls[i];
		}
	}
*/

	wall **checktable = walls;
	int nearbywalls = numwalls;


	// check if we're gonna hit a wall

	int nearestwall;

	detectcollision (dt, p, v, checktable, nearbywalls, &nearestwall);

	if ( nearestwall < 0 ) {
		/* no collision */
		// delta_location = delta_time * speed
		if (!immobile) {
			p+=v*dt;
			onwall=false;
		}
		else v=vec(0,0);

	} else {
		// collided to wall nearestwall.
		wall *w = checktable[nearestwall];

		// Is the wall special?
		if ( (w->type_ab != WALL_DEFAULT && w->type_ab != WALL_GROUND) ||
				(w->type_ac != WALL_DEFAULT && w->type_ac != WALL_GROUND) ||
				(w->type_bc != WALL_DEFAULT && w->type_bc != WALL_GROUND) )
			{
			// Uh-oh. Need to know which side it collided.

			vec AP = p - w->oa;
			vec BP = p - w->oa-w->ab;

			Dbl VxAB = v.cross(w->ab);
			Dbl VxAC = v.cross(w->ac);
			Dbl VxBC = v.cross(w->bc);

			Dbl VxAP = v.cross(AP);
			Dbl VxBP = v.cross(BP);

			Dbl ABxAP = w->ab.cross(AP);
			Dbl ACxAP = w->ac.cross(AP);
			Dbl BCxBP = w->bc.cross(BP);


			// calculate the closest edge of the face

			Dbl tAB=t_to_edge(AP, v, w->ab, ABxAP, VxAB);
			Dbl tAC=t_to_edge(AP, v, w->ac, ACxAP, VxAC);
			Dbl tBC=t_to_edge(BP, v, w->bc, BCxBP, VxBC);

			vec nAP, nAB, nNORM;
			Dbl nVxAB, nABxAP;
			int ntype=0;
			int test=0;

			if (tAB <= tAC && tAB <= tBC) {
				ntype = w->type_ab;
				nAP = AP;
				nAB = w->ab;
				nNORM = vec(-nAB.y, nAB.x);
				nNORM /= nNORM.abs();
				nVxAB = VxAB;
				nABxAP=ABxAP;
			} else if (tAC <= tAB && tAC <= tBC) {
				ntype = w->type_ac;
				nAP = AP;
				nAB = w->ac;
				nNORM = vec(-nAB.y, nAB.x);
				nNORM /= nNORM.abs();
				nVxAB = VxAC;
				nABxAP=ACxAP;
			} else if (tBC <= tAB && tBC <= tAC) {
				ntype = w->type_bc;
				nAP = BP;
				nAB = w->bc;
				nNORM = vec(-nAB.y, nAB.x);
				nNORM /= nNORM.abs();
				nVxAB = VxBC;
				nABxAP=BCxBP;
			} else {
				fprintf (stderr, "particle.cpp::act: fault in axioms of reals.\n");
				return;
			}

			// ok, now we know the edge. Now we must collide with the wall.

			vec oldv = v;

			switch (ntype) {
				case WALL_DEFAULT:
				case WALL_GROUND:
					if (!onwall) damage += v.abs();
					v=vec(0,0);
					onwall=true;
				break;
				case WALL_ICE:
					v = nAB*(v.dot(nAB)/nAB.abs2());
					if (!onwall) {				
						damage += (oldv-v).abs();
					}
					detectcollision (dt, p, v, checktable, nearbywalls, &test);
					if (test<0) p+=v*dt;
					onwall=true;
				break;

				default:
					fprintf (stderr, "particle::act: erroneous ntype\n");
					exit(1);
				break;
			}

		} else {
			// All normal wall. Mark damage.
			if (!onwall) damage += v.abs();
			v=vec(0,0);
			onwall=true;
		}

	}

	// gravitation: delta_v =  delta_t * g;
	v += vec(0,g)*dt;

//	delete[] checktable; hox

}

void particle::display(Dbl r, Dbl g, Dbl b) {
/*	int x = (int)(screen->w*(p.x - centerx)/width) + screen->w/2;
	int y = (int)(screen->h*(centery - p.y)/height) + screen->h/2;

	if (x>=0 && x<screen->w && y>=0 && y<screen->h)
		*((Uint32*)(screen->pixels)+x+y*screen->w)=color;
*/

/*
	int x1 = x+(int)(screen->w*(v.x)/width);
	int y1 = y+(int)(screen->h*(-v.y)/height);

	Graphics.DrawLine(screen, 0xFF0000, x, y, x1, y1);
*/
}


void particle::detectcollision (Dbl dt, vec &p, vec &v, wall **walls, int numwalls, int *nearestwall) {
	*nearestwall = -1;
	for (int i=0; i<numwalls; i++) {
		if (detectcollision(dt, p, v, walls[i]) ) {
			*nearestwall = i;
			return;
		}
	}
}

int particle::detectcollision (Dbl dt, vec &p, vec &v, wall *w) {
	vec ap = p - w->oa + v*dt;

	Dbl n = ap.cross(w->ac)*w->ab_x_ac_inv;
	if (n<0) return 0;

	Dbl m = w->ab.cross(ap)*w->ab_x_ac_inv;
	if (m<0 || n+m>1) return 0;

	// Oh no! We're hitting!
	return true;
}

int particle::detectcollision_nov (vec &p, wall *w) {
	vec ap = p - w->oa;

	Dbl n = ap.cross(w->ac)*w->ab_x_ac_inv;
	if (n<0) return 0;

	Dbl m = w->ab.cross(ap)*w->ab_x_ac_inv;
	if (m<0 || n+m>1) return 0;

	// Oh no! We're hitting!
	return true;
}

bool particle::is_onwall(vec &centerab, vec &ab, vec &ab_hat, Dbl lenp2 ) {
	vec PT = p-centerab;
	Dbl h = PT.abscross(ab_hat);

	// 0 <= AP dot AB / AB <= AB  <=>  0 <= AP dot AB one <= AB len

	Dbl x = fabs(PT.dot( ab_hat ) );
	if (x > lenp2) return false;

	if (h < WALL_ONWALL_DIST) return true;
	return false;
}

Dbl particle::t_to_edge(vec AP, vec V, vec AB, Dbl ABxAP, Dbl VxAB) {
	if (VxAB != 0.0) {
		Dbl VxAP = v.cross(AP);
		Dbl tAB = ABxAP / VxAB;
		if (tAB <= 0.0) return 1000;
		Dbl kAB = VxAP / VxAB;
		if (kAB < 0.0 || kAB > 1.0) return 1000.0;
		return tAB;
	}
	return 1000.0;
}
