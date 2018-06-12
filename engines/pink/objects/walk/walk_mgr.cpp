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

#include "pink/archive.h"
#include "pink/cel_decoder.h"
#include "pink/objects/actions/walk_action.h"
#include "pink/objects/actors/lead_actor.h"
#include "pink/objects/walk/walk_mgr.h"
#include "pink/objects/walk/walk_location.h"

namespace Pink {

WalkMgr::WalkMgr()
	: _isWalking(false), _leadActor(nullptr) {}

WalkMgr::~WalkMgr() {
	for (uint i = 0; i < _locations.size(); ++i) {
		delete _locations[i];
	}
}

void WalkMgr::deserialize(Pink::Archive &archive) {
	_leadActor = static_cast<LeadActor *>(archive.readObject());
	_locations.deserialize(archive);
}

WalkLocation *WalkMgr::findLocation(const Common::String &name) {
	for (uint i = 0; i < _locations.size(); ++i) {
		if (_locations[i]->getName() == name)
			return 	_locations[i];
	}
	return nullptr;
}

void WalkMgr::toConsole() {
	debug("WalkMgr:");
	for (uint i = 0; i < _locations.size(); ++i) {
		_locations[i]->toConsole();
	}
}

void WalkMgr::start(WalkLocation *destination) {
	if (_isWalking)
		return;

	if (_current.name.empty()) {
		_current.name = _locations[0]->getName();
		_current.coord = getLocationCoordinates(_locations[0]->getName());
	}

	_destination = destination;

	if (_current.name == _destination->getName()) {
		end();
	} else {
		_isWalking = true;
		WalkLocation *currentLocation = findLocation(_current.name);
		WalkShortestPath path(this);
		WalkLocation *nextLocation = path.next(currentLocation, _destination);
		initNextWayPoint(nextLocation);
		_leadActor->setAction(getWalkAction());
	}
}

void WalkMgr::initNextWayPoint(WalkLocation *location) {
	_next.name = location->getName();
	_next.coord = getLocationCoordinates(location->getName());
}

WalkAction *WalkMgr::getWalkAction() {
	Common::String walkActionName;
	if (_current.coord.z == _next.coord.z) {
		if (_next.coord.x > _current.coord.x) {
			walkActionName = Common::String::format("%dRight", _current.coord.z);
		} else
			walkActionName = Common::String::format("%dLeft", _next.coord.z);
	} else
		walkActionName = Common::String::format("%dTo%d", _current.coord.z, _next.coord.z);

	Action *action = _leadActor->findAction(walkActionName);

	return static_cast<WalkAction*>(action);
}

double WalkMgr::getLengthBetweenLocations(WalkLocation *first, WalkLocation *second) {
	Coordinates firstCoord = getLocationCoordinates(first->getName());
	Coordinates secondCoord = getLocationCoordinates(second->getName());
	return sqrt((secondCoord.x - firstCoord.x) * (secondCoord.x - firstCoord.x) +
				(secondCoord.y - firstCoord.y) * (secondCoord.y - firstCoord.y));
}

Coordinates WalkMgr::getLocationCoordinates(const Common::String &locationName) {
	Action *action  = _leadActor->findAction(locationName);
	return action->getCoordinates();
}

void WalkMgr::setCurrentWayPoint(WalkLocation *location) {
	_current.name = location->getName();
	_current.coord = getLocationCoordinates(_current.name);
}

void WalkMgr::update() {
	if (_leadActor->isPlaying())
		return;

	WalkShortestPath path(this);
	_current = _next;
	WalkLocation *next = path.next(findLocation(_current.name), _destination);
	if (next) {
		initNextWayPoint(next);
		_leadActor->setAction(getWalkAction());
	} else
		end();

}

void WalkMgr::end() {
	_isWalking = false;
	_leadActor->onWalkEnd(_destination->getName());
}

void WalkMgr::loadState(Archive &archive) {
	_isWalking = archive.readByte();
	_current.name = archive.readString();
	if (!_current.name.empty()) {
		_current.coord = getLocationCoordinates(_current.name);
	}
	if (_isWalking) {
		_next.name = archive.readString();
		_destination = findLocation(archive.readString());
		_next.coord = getLocationCoordinates(_next.name);
	}
}

void WalkMgr::saveState(Archive &archive) {
	archive.writeByte(_isWalking);
	archive.writeString(_current.name);
	if (_isWalking) {
		archive.writeString(_next.name);
		archive.writeString(_destination->getName());
	}
}

void WalkMgr::skip() {
	initNextWayPoint(_destination);
	_current = _next;
	end();
}

} // End of namespace Pink