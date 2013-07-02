/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This code is based on the original source code of Lord Avalot d'Argent version 1.3.
 * Copyright (c) 1994-1995 Mike, Mark and Thomas Thurman.
 */

/* CLOSING		The closing screen and error handler. */

#ifndef CLOSING2_H
#define CLOSING2_H

#include "common/scummsys.h"
#include "common/str.h"

namespace Avalanche {
class AvalancheEngine;

class Closing {
public:
	static const int16 scr_bugalert = 1;
	static const int16 scr_ramcram = 2;
	static const int16 scr_nagscreen = 3;
	static const int16 scr_twocopies = 5;

	Closing();

	void setParent(AvalancheEngine *vm);

	void quit_with(byte which, byte errorlev);

	void end_of_program();

private:
	AvalancheEngine *_vm;

	typedef Common::String scrtype;

	scrtype q /*absolute $B8FA:0*/; /* Nobody's using the graphics memory now. */
	//file<scrtype> f;
	void *exitsave;

	void get_screen(byte which);

	void show_screen();

	void put_in(Common::String x, uint16 where);

	void bug_handler();
};

} // End of namespace Avalanche.

#endif // CLOSING2_H
