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
#ifndef _HANDLER_H_
#define _HANDLER_H_

#include "compiling_settings.h"

#include "SDL.h"
#include "SDL_net.h"

#include <iostream>
#include <list>

#include "condition.h"
#include "connection.h"

#define P_BLOBOATS 0xCEFAADDE
#define P_VERSION 1

#define P_SIZE 2048
#define P_SENT 5

#define P_WAIT 250

using std::list;

static int package_num = 0;

#if NETWORK != 1
#define UDPpacket int
#define UDPsocket int
#endif

class handler {
public:

	handler();
	~handler();

	void start();
	void stop();

	void udp_open(Uint16 port);
	void udp_close();

	void packet_ready(int channel);

	int packet_send();

	void stack_add(condition *c, int channel);

	int send(int channel);
	int recv();

	UDPsocket get_sock();

	list <condition *> received;
	list <connection *> conns;


private:

	list <condition *> stack;
	list <condition *> reqs;

#if NETWORK == 1
	UDPsocket sock;
	UDPpacket *p;
#endif



};




#endif
*/
