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
#include <stdlib.h>
#include <stdio.h>

#include "handler.h"

#include "compat.h"

#if NETWORK == 1

handler::handler() {
}

handler::~handler() {
	std::list<condition *>::iterator it;

	for (it = reqs.begin(); it!=reqs.end(); it++) {
		delete *it;
	}
	for (it = stack.begin(); it!=stack.end(); it++) {
		delete *it;
	}
	for (it = received.begin(); it!=received.end(); it++) {
		delete *it;
	}
	reqs.clear();
	stack.clear();
	received.clear();

}

void handler::start() {
	if (SDLNet_Init() != 0) {
		fprintf (stderr, "Couldn't init SDL_net: %s\n", SDLNet_GetError() );
		exit(1);
	}
}

void handler::udp_open(Uint16 port) {
	p = SDLNet_AllocPacket(P_SIZE);
	if (!p) {
		fprintf (stderr, "Couldn't get packet.\n");
		exit(1);
	}

	sock = SDLNet_UDP_Open(port);
	if (!sock) {
		fprintf (stderr, "Couldn't open UDP socket: \"%s\"\n",
		SDLNet_GetError() );
		exit(1);
	}
}

void handler::udp_close() {
	SDLNet_FreePacket(p);

	SDLNet_UDP_Close(sock);
}

void handler::stop() {
	SDLNet_Quit();
}

void handler::packet_ready(int channel) {
	p->channel = channel;

	*(Uint32*)(p->data) = SWAP32(P_BLOBOATS);
	p->data[4] = P_VERSION;

	// get id-code
	Uint32 idcode=0;

	p->len = 5;

	std::list<connection*>::iterator id;
	for (id=conns.begin(); id!=conns.end() && !idcode; id++) {
		if ( (*id)->channel == channel ) {
			idcode = (*id)->idcode;
			*(Uint32*)&(p->data[5]) = idcode; // no need to swap bytes
			p->len += 4;
//			fprintf (stderr, "Handler: Added a security id.\n");
		}
	}

}


int handler::packet_send() {
	fprintf (stderr, "\n");
	return SDLNet_UDP_Send(sock, p->channel, p);
}

void handler::stack_add(condition *c, int channel) {
	// add to stack

	condition *b = new condition(*c);
	b->channel = channel;
	stack.push_back(b);

}

int handler::send(int channel) {
	std::list<condition*>::iterator it;


	packet_ready(channel);

	bool mustsend=false;

	// attach conditions from stack to packages
	for (it = stack.begin(); it != stack.end(); it++) {
		if ( (*it)->channel == channel) { // right channel

			if (p->len + (*it)->get_size() + 2 > 2048) { // no space in package
				if ( packet_send() != 0) {
					fprintf (stderr, "Error A.\n");
					return -1;
				}
				packet_ready(channel);
			}

			// add id
			Uint32 id;
			if (package_num == 0xFFFF) id = package_num = 0;
			else id = package_num++;

			(*it)->set_id(id);

			*(Uint16*)&p->data[p->len] = SWAP16(id);
			p->len+=2;

			// add in end of the package
			Uint32 s_size = (*it)->get_size();
			for (int i=0; i<s_size; i++) {
				p->data[p->len+i] = (*it)->data[i];
			}

			// if request answer, add to req-stack
			if ( (*it)->get_req() ) {
				(*it)->wait = SDL_GetTicks();
				(*it)->sent++;
				reqs.push_back( (*it) ); // to req-stack
			} else {
				// delete condition
				delete *it;
				*it = 0;
			}

			reqs.erase(it++);


			// move cursor and force sending
			p->len += s_size;
			mustsend = true;
		} // if

	} // for

	// attach conditions from req-stack to packages
	for (it = reqs.begin(); it != reqs.end(); it++) {
		if ( (*it)->channel == channel &&
			SDL_GetTicks() - (*it)->wait >= P_WAIT )
		{ // right channel

			// if too old, delete, warn
			if ( ++(*it)->sent >= P_SENT ) {
				// delete
				reqs.erase(it++); // sent -> free from stack
				fprintf (stderr, "Too old a package in reqs.\n");
				return -1;
			} else {
				(*it)->wait += P_WAIT;

				if (p->len + (*it)->get_size() + 2 > 2048) { // no space in package
					if ( packet_send() != 0) {
						fprintf (stderr, "Error B.\n");
						return -1;
					}
					packet_ready(channel);
				}

				// add id
				*(Uint16*)&p->data[p->len] = (*it)->get_id();
				p->len += 2;

				// add in end of the package
				Uint32 s_size = (*it)->get_size();
				for (int i=0; i<s_size; i++) {
					p->data[p->len+i] = (*it)->data[i];
				}

				// move cursor and force sending
				p->len += s_size;
				mustsend = true;
			} // if (too old)

		} // if

	} // for


	if (mustsend) {
		if ( packet_send() <= 0) {
			fprintf (stderr, "Error C.\n");
			return -1;
		}
	}

	return 0;

}

int handler::recv() {
	int result=1;

	SDLNet_FreePacket(p);
	p = SDLNet_AllocPacket(P_SIZE);


	while ( (result = SDLNet_UDP_Recv(sock, p)) == 1 ) {

		// check that the packet has a header
		if (p->len < 5) {
			fprintf (stderr, "Got a packet with no header!\n");
			continue;
		}

		Uint32 blob = *(Uint32*)&p->data[0];
		Uint8 ver = *(Uint8*)&p->data[4];

		Uint32 cursor = 5;

		// check bloboats-signature bytes
		if (SWAP32(blob) != P_BLOBOATS) {
			fprintf (stderr, "Got a non-bloboats packet: code %x\n", SWAP32(blob));
			continue;
		}

		// check version
		if (ver != P_VERSION) {
			fprintf (stderr, "Got a bloboats-packet of wrong version (%d)\n", ver);
			continue;
		}

		std::list<connection *>::iterator c = conns.begin();

		// identify sender
		if (p->channel == -1) { // unknown
			// huh, someone we don't know has sent us a package!
		} else {
			// check security code
			Uint32 idcode = *(Uint32*)&p->data[cursor];
			cursor += 4;

			// identify
			std::list<connection*>::iterator c;
			bool found=false;

			for (; c != conns.end() && !found; c++) {
				if ( (*c)->channel == p->channel ) {
					if ( (*c)->idcode == idcode ) {
						fprintf (stderr, "Got a package from channel %d.\n",
							p->channel);
						found = true;
					}
				}
			} // for

			if (!found) {
				fprintf (stderr, "Got an ip-spoofed packet!\n");
				continue;
			}

		} // if

		// ok, now it's either unknown (channel=-1) or known (with channel)

		// parse condition

		while (cursor+4 < p->len) {
			Uint16 id = *(Uint16*)&p->data[cursor];
			id = SWAP16(id);
			cursor += 2;

//			printf ("Id(%d): %02x\n",cursor-2, id);

			Uint8 type = *(Uint8*)&p->data[cursor++];
			Uint8 req = *(Uint8*)&p->data[cursor++];

//			printf ("Type: %d\n", type);
//			printf ("Req: %d\n", req);

			// packet of correct type?
			if (type >= COMMANDS) {
				// broken condition type, whole packet is corrupted!
				fprintf (stderr, "Broken condition type %x, corrupted packet!\n",
					type);
				break; // end of this packet
			}

			// check if the packet is large enough

			Uint16 packlen = 0;

			if ( type == SENDFILE ) {
				Uint16 k = *(Uint16*)&p->data[cursor]; // file len
				cursor += 2;
				packlen = SWAP32(k);
			} else {
				packlen = commandsize[type];
			}

			if ( cursor + packlen > p->len ) {
				// nope... corrupted packet
				fprintf (stderr, "Broken packet/condition length, corrupted!\n");
				break; // end of this packet
			}

			// If it's ack, kill the req packet
			if (type == ACK) {
				Uint16 k = *(Uint16*)&p->data[cursor];
				cursor+=2;

				bool foundreq = false;
				std::list<condition *>::iterator rit;

				for (rit = reqs.begin(); !foundreq && rit != reqs.end(); rit++) {
					if ( (*rit)->get_id() == k ) {
						foundreq = true;
						delete *rit;
						reqs.erase(rit);
					}
				}
				fprintf (stderr, "Got ack.\n");
				continue;
			}

			// Here we go. If it requests for answer and we know him, do ack to it
			if (req && p->channel >= 0) {
				// automatically send an ack-packet
				condition ackpacket(ACK, 0, VAR_LIST,
					T_UINT16, id, END_OF_LIST);
				stack_add(&ackpacket, p->channel);

				// todo: ping
			}

			// after that, copy to received-stack

			condition *newcond = new condition;

			newcond->set_id(id);
			newcond->set_type(type);
			newcond->set_req(req);
			newcond->set_data( &p->data[cursor], packlen );
			cursor += packlen;

			newcond->rewind();

			newcond->channel = p->channel;

			received.push_back(newcond);

//			printf ("Received a packet.\n");


		} // while

	} // while

	return result;
}

UDPsocket handler::get_sock() {
	return sock;
}

#else



handler::handler() {
}
handler::~handler() {
}

void handler::start() {
}
void handler::stop() {
}

void handler::udp_open(Uint16 port) {
}
void handler::udp_close() {
}

void handler::packet_ready(int channel) {
}

int handler::packet_send() {
}

void handler::stack_add(condition *c, int channel) {
}

int handler::send(int channel) {
	return 0;
}
int handler::recv() {
	return 0;
}

UDPsocket handler::get_sock() {
}

#endif

*/
