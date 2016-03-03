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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TITANIC_DOOR_AUTO_SOUND_EVENT_H
#define TITANIC_DOOR_AUTO_SOUND_EVENT_H

#include "titanic/sound/auto_sound_event.h"

namespace Titanic {

class CDoorAutoSoundEvent : public CAutoSoundEvent {
public:
	CString _string1;
	CString _string2;
	int _fieldDC;
	int _fieldE0;
public:
	CDoorAutoSoundEvent::CDoorAutoSoundEvent() : CAutoSoundEvent(),
		_string1("z#44.wav"), _string2("z#43.wav"), _fieldDC(25), _fieldE0(25) {
	}

	/**
	 * Return the class name
	 */
	virtual const char *getClassName() const { return "CDoorAutoSoundEvent"; }

	/**
	 * Save the data for the class to file
	 */
	virtual void save(SimpleFile *file, int indent) const;

	/**
	 * Load the data for the class from file
	 */
	virtual void load(SimpleFile *file);
};

} // End of namespace Titanic

#endif /* TITANIC_DOOR_AUTO_SOUND_EVENT_H */
