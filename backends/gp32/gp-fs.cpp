/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001  Ludvig Strigeus
 * Copyright (C) 2001/2004 The ScummVM project
 * Copyright (C) 2002 Ph0x - GP32 Backend
 * Copyright (C) 2003/2004 DJWillis - GP32 Backend
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. 
 *
 * $Header$
 *
 */

#if defined (__GP32__)

#include "stdafx.h"

#include "backends/gp32/gp32.h"
#include "backends/fs/fs.h"
#include <stdio.h>
#include <gpstdio.h>

/*
 * Implementation of the ScummVM file system API based on GP32 SMC File Access.
 */

class GP32FilesystemNode : public FilesystemNode {
protected:
	Common::String _displayName;
	bool _isDirectory;
	bool _isValid;
	Common::String _path;
	
public:
	GP32FilesystemNode();
	GP32FilesystemNode(const Common::String &path);
	GP32FilesystemNode(const GP32FilesystemNode *node);

	virtual Common::String displayName() const { return _displayName; }
	virtual bool isValid() const { return _isValid; }
	virtual bool isDirectory() const { return _isDirectory; }
	virtual Common::String path() const { return _path; }

	virtual FSList listDir(ListMode mode = kListDirectoriesOnly) const;
	virtual FilesystemNode *parent() const;
	virtual FilesystemNode *clone() const { return new GP32FilesystemNode(this); }
};


AbstractFilesystemNode *FilesystemNode::getRoot() {
	return new GP32FilesystemNode();
}

GP32FilesystemNode::GP32FilesystemNode() {
	_displayName = "gp:\\SCUMMVM\\GAMES\\";
	_isValid = true;
	_isDirectory = true;
	_path = "gp:\\scummvm\\games\\";
}

/*
GP32FilesystemNode::GP32FilesystemNode() {
	_displayName = "gp:\\";
	_isValid = true;
	_isDirectory = true;
	_path = "gp:\\";
}
*/

GP32FilesystemNode::GP32FilesystemNode(const Common::String &p) {
	// TODO - extract last component from path
	_displayName = p;
	// TODO - check whether it is a directory, and whether the file actually exists
	_isValid = true;
	_isDirectory = true;
	_path = p;
}

GP32FilesystemNode::GP32FilesystemNode(const GP32FilesystemNode *node) {
	_displayName = node->_displayName;
	_isValid = node->_isValid;
	_isDirectory = node->_isDirectory;
	_path = node->_path;
}

AbstractFilesystemNode *FilesystemNode::getNodeForPath(const String &path) {
	return new GP32FilesystemNode(path);
}

FSList GP32FilesystemNode::listDir(ListMode mode) const {
	assert(_isDirectory);

	GPDIRENTRY dp;
	ulong read;

	FSList myList;
	
	int start=0; // current file

	// ... loop over dir entries using readdir
	while (GpDirEnumList(_path.c_str(), start++, 1, &dp, &read)  == SM_OK) { 
		if (strcmp(dp.name,".")==0|| strcmp(dp.name,"..")==0) continue;
		GP32FilesystemNode entry;
		entry._displayName = dp.name;
		entry._path = _path;
		entry._path += dp.name;
		
		GPFILEATTR attr;
		char s[256];
		sprintf(s, "%s%s", _path.c_str(), dp.name);
		GpFileAttr(s, &attr);
		entry._isDirectory = attr.attr & (1<<4);

		// Honor the chosen mode
		if ((mode == kListFilesOnly && entry._isDirectory) ||
			(mode == kListDirectoriesOnly && !entry._isDirectory))
			continue;

		if (entry._isDirectory)
			entry._path += "\\"; //ph0x
		myList.push_back(entry);
	}	
	return myList;
}

const char *lastPathComponent(const Common::String &str) {
	const char *start = str.c_str();
	const char *cur = start + str.size() - 2;
	
	while (cur > start && *cur != '\\') { //ph0x
		--cur;
	}
	
	return cur+1;
}

FilesystemNode *GP32FilesystemNode::parent() const {
	
	GP32FilesystemNode *p = new GP32FilesystemNode();

// OLD - REMOVE ON CLEAN COMPILE
//	// Root node is its own parent. Still we can't just return this
//	// as the GUI code will call delete on the old node.
//	if (_path != "gp:\\") {  //ph0x
//		const char *start = _path.c_str();
//		const char *end = lastPathComponent(_path);
//
//		p->_path = String(start, end - start);
//		p->_isValid = true;
//		p->_isDirectory = true;
//		p->_displayName = lastPathComponent(p->_path);
//	}
//	return p;
//}

	// Root node is its own parent. Still we can't just return this
	// as the GUI code will call delete on the old node.
	if (_path != "gp:\\") {
		const char *start = _path.c_str();
		const char *end = lastPathComponent(_path);

		p->_path = Common::String(start, end - start);
		p->_displayName = lastPathComponent(p->_path);
	} else {
		p->_path = _path;
		p->_displayName = _displayName;
	}
	p->_isValid = true;
	p->_isDirectory = true;
	return p;
}

#endif /* defined(__GP32__) */
