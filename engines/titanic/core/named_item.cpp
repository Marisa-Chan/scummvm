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

#include "titanic/core/named_item.h"

namespace Titanic {

void CNamedItem::save(SimpleFile *file, int indent) const {
	file->writeNumberLine(0, indent);
	file->writeQuotedLine(_name, indent);

	CTreeItem::save(file, indent);
}

void CNamedItem::load(SimpleFile *file) {
	int val = file->readNumber();
	if (!val)
		_name = file->readString();

	CTreeItem::load(file);
}

int CNamedItem::compareTo(const CString &name, int maxLen) const {
	if (maxLen) {
		return getName().left(maxLen).compareToIgnoreCase(name);
	} else {
		return getName().compareToIgnoreCase(name);
	}
}

} // End of namespace Titanic
