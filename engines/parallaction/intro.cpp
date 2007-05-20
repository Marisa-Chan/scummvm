/* ScummVM - Scumm Interpreter
 * Copyright (C) 2006 The ScummVM project
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
 * $URL$
 * $Id$
 *
 */

#include "common/stdafx.h"

#include "parallaction/parallaction.h"
#include "parallaction/menu.h"
#include "parallaction/sound.h"

#include "graphics/primitives.h"

namespace Parallaction {

static Animation *_rightHandAnim;

static uint16 _rightHandPositions[684] = {
	0x0064, 0x0046, 0x006c, 0x0046, 0x0074, 0x0046, 0x007c, 0x0046,
	0x0084, 0x0046, 0x008c, 0x0046, 0x0094, 0x0046, 0x009c, 0x0046,
	0x00a4, 0x0046, 0x00ac, 0x0046, 0x00b4, 0x0046, 0x00bc, 0x0046,
	0x00c4, 0x0046, 0x00cc, 0x0046, 0x00d4, 0x0046, 0x00dc, 0x0046,
	0x00e4, 0x0046, 0x00ec, 0x0046, 0x00f4, 0x0046, 0x00fc, 0x0046,
	0x0104, 0x0046, 0x00ff, 0x0042, 0x00ff, 0x004a, 0x00ff, 0x0052,
	0x00ff, 0x005a, 0x00ff, 0x0062, 0x00ff, 0x006a, 0x00ff, 0x0072,
	0x00ff, 0x007a, 0x00ff, 0x0082, 0x00ff, 0x008a, 0x00ff, 0x0092,
	0x00ff, 0x009a, 0x00ff, 0x00a2, 0x0104, 0x0097, 0x00fc, 0x0097,
	0x00f4, 0x0097, 0x00ec, 0x0097, 0x00e4, 0x0097, 0x00dc, 0x0097,
	0x00d4, 0x0097, 0x00cc, 0x0097, 0x00c4, 0x0097, 0x00bc, 0x0097,
	0x00b4, 0x0097, 0x00ac, 0x0097, 0x00a4, 0x0097, 0x009c, 0x0097,
	0x0094, 0x0097, 0x008c, 0x0097, 0x0084, 0x0097, 0x007c, 0x0097,
	0x0074, 0x0097, 0x006c, 0x0097, 0x0064, 0x0097, 0x0066, 0x0042,
	0x0066, 0x004a, 0x0066, 0x0052, 0x0066, 0x005a, 0x0066, 0x0062,
	0x0066, 0x006a, 0x0066, 0x0072, 0x0066, 0x007a, 0x0066, 0x0082,
	0x0066, 0x008a, 0x0066, 0x0092, 0x0066, 0x009a, 0x0066, 0x00a2,
	0x008c, 0x0091, 0x0099, 0x0042, 0x0099, 0x004a, 0x0099, 0x0052,
	0x0099, 0x005a, 0x0099, 0x0062, 0x0099, 0x006a, 0x0099, 0x0072,
	0x0099, 0x007a, 0x0099, 0x0082, 0x0099, 0x008a, 0x0099, 0x0092,
	0x0099, 0x009a, 0x0099, 0x00a2, 0x00a0, 0x004d, 0x00cc, 0x0042,
	0x00cc, 0x004a, 0x00cc, 0x0052, 0x00cc, 0x005a, 0x00cc, 0x0062,
	0x00cc, 0x006a, 0x00cc, 0x0072, 0x00cc, 0x007a, 0x00cc, 0x0082,
	0x00cc, 0x008a, 0x00cc, 0x0092, 0x00cc, 0x009a, 0x00cc, 0x00a2,
	0x00ca, 0x0050, 0x00b1, 0x0050, 0x0081, 0x0052, 0x007e, 0x0052,
	0x007c, 0x0055, 0x007c, 0x005c, 0x007e, 0x005e, 0x0080, 0x005e,
	0x0082, 0x005c, 0x0082, 0x0054, 0x0080, 0x0052, 0x0078, 0x0052,
	0x007c, 0x005e, 0x0077, 0x0061, 0x0074, 0x006e, 0x0074, 0x0078,
	0x0076, 0x007a, 0x0079, 0x0078, 0x0079, 0x0070, 0x0078, 0x0070,
	0x0078, 0x006b, 0x007b, 0x0066, 0x007a, 0x006f, 0x0084, 0x006f,
	0x0085, 0x0066, 0x0086, 0x0070, 0x0085, 0x0070, 0x0085, 0x0079,
	0x0088, 0x0079, 0x008a, 0x0078, 0x008a, 0x006c, 0x0087, 0x0061,
	0x0085, 0x005f, 0x0082, 0x005f, 0x0080, 0x0061, 0x007e, 0x0061,
	0x007b, 0x005f, 0x007c, 0x006f, 0x007c, 0x0071, 0x0079, 0x0074,
	0x0079, 0x0089, 0x0076, 0x008c, 0x0076, 0x008e, 0x007a, 0x008e,
	0x007f, 0x0089, 0x007f, 0x0083, 0x007e, 0x0083, 0x007e, 0x0077,
	0x0080, 0x0077, 0x0080, 0x0083, 0x0080, 0x008b, 0x0084, 0x0090,
	0x0088, 0x0090, 0x0088, 0x008e, 0x0085, 0x008b, 0x0085, 0x0074,
	0x0082, 0x0071, 0x00b2, 0x0052, 0x00b0, 0x0054, 0x00b0, 0x0056,
	0x00ae, 0x0058, 0x00af, 0x0059, 0x00af, 0x005e, 0x00b2, 0x0061,
	0x00b5, 0x0061, 0x00b8, 0x005e, 0x00b8, 0x005a, 0x00b9, 0x0059,
	0x00b9, 0x0058, 0x00b7, 0x0056, 0x00b7, 0x0054, 0x00b5, 0x0052,
	0x00b2, 0x0052, 0x00ae, 0x005a, 0x00ab, 0x005b, 0x00ab, 0x006d,
	0x00ae, 0x0072, 0x00b8, 0x0072, 0x00bc, 0x006d, 0x00bc, 0x005b,
	0x00b9, 0x005a, 0x00bc, 0x005c, 0x00be, 0x005c, 0x00c1, 0x005f,
	0x00c4, 0x0067, 0x00c4, 0x006d, 0x00c1, 0x0076, 0x00c0, 0x0077,
	0x00bd, 0x0077, 0x00bb, 0x0075, 0x00bd, 0x0073, 0x00bb, 0x0072,
	0x00be, 0x0070, 0x00be, 0x006a, 0x00a9, 0x006a, 0x00a9, 0x0070,
	0x00ac, 0x0072, 0x00aa, 0x0073, 0x00ac, 0x0075, 0x00aa, 0x0077,
	0x00a7, 0x0077, 0x00a3, 0x006d, 0x00a3, 0x0067, 0x00a6, 0x005f,
	0x00a9, 0x005c, 0x00ab, 0x005c, 0x00ac, 0x0077, 0x00ac, 0x007c,
	0x00ab, 0x007c, 0x00ab, 0x0084, 0x00ac, 0x0084, 0x00ac, 0x008b,
	0x00a9, 0x008e, 0x00a9, 0x0090, 0x00ae, 0x0090, 0x00ae, 0x008d,
	0x00b2, 0x008c, 0x00b2, 0x0087, 0x00b1, 0x0086, 0x00b1, 0x007b,
	0x00b2, 0x0079, 0x00b4, 0x0079, 0x00b4, 0x007d, 0x00b5, 0x007d,
	0x00b5, 0x0087, 0x00b4, 0x0087, 0x00b4, 0x008c, 0x00b6, 0x008c,
	0x00b9, 0x0091, 0x00b4, 0x0091, 0x00bd, 0x008f, 0x00ba, 0x008c,
	0x00ba, 0x0083, 0x00bb, 0x0082, 0x00bb, 0x0075, 0x00cc, 0x006e,
	0x00d4, 0x006c, 0x00db, 0x0069, 0x00d9, 0x0068, 0x00d9, 0x0064,
	0x00dc, 0x0064, 0x00dc, 0x0060, 0x00df, 0x0056, 0x00e5, 0x0052,
	0x00e7, 0x0052, 0x00ec, 0x0056, 0x00ef, 0x005d, 0x00f1, 0x0065,
	0x00f3, 0x0064, 0x00f3, 0x0069, 0x00f0, 0x0069, 0x00ec, 0x0065,
	0x00ec, 0x005e, 0x00e9, 0x005f, 0x00e9, 0x005a, 0x00e7, 0x0058,
	0x00e4, 0x0058, 0x00e3, 0x0054, 0x00e3, 0x0058, 0x00e1, 0x005c,
	0x00e4, 0x0061, 0x00e7, 0x0061, 0x00e9, 0x005f, 0x00eb, 0x005d,
	0x00e4, 0x0062, 0x00e0, 0x0064, 0x00e0, 0x0069, 0x00e2, 0x006b,
	0x00e0, 0x0072, 0x00e0, 0x0077, 0x00ec, 0x0077, 0x00ec, 0x0071,
	0x00ea, 0x006b, 0x00ec, 0x006a, 0x00ec, 0x0063, 0x00e7, 0x0063,
	0x00e7, 0x0065, 0x00e1, 0x0069, 0x00e3, 0x0068, 0x00e6, 0x0069,
	0x00ec, 0x005e, 0x00ea, 0x006b, 0x00e7, 0x006b, 0x00e7, 0x006a,
	0x00e5, 0x006a, 0x00e5, 0x006b, 0x00e2, 0x006b, 0x00df, 0x006c,
	0x00dc, 0x006f, 0x00dc, 0x0071, 0x00da, 0x0073, 0x00d8, 0x0073,
	0x00d8, 0x006f, 0x00dc, 0x006b, 0x00dc, 0x0069, 0x00dd, 0x0068,
	0x00ef, 0x0068, 0x00f0, 0x0069, 0x00f0, 0x006b, 0x00f4, 0x006f,
	0x00f4, 0x0072, 0x00f3, 0x0073, 0x00f2, 0x0073, 0x00f0, 0x0071,
	0x00f0, 0x006f, 0x00ec, 0x006b, 0x00ec, 0x007a, 0x00eb, 0x007b,
	0x00eb, 0x007f, 0x00ec, 0x0080, 0x00ec, 0x0084, 0x00eb, 0x0085,
	0x00eb, 0x008b, 0x00ec, 0x008c, 0x00ec, 0x008f, 0x00ed, 0x0091,
	0x00e9, 0x0091, 0x00e9, 0x008f, 0x00e7, 0x008d, 0x00e7, 0x0090,
	0x00e7, 0x0089, 0x00e8, 0x0088, 0x00e8, 0x0086, 0x00e7, 0x0085,
	0x00e7, 0x007d, 0x00e6, 0x007c, 0x00e6, 0x0078, 0x00e5, 0x007d,
	0x00e5, 0x0085, 0x00e4, 0x0086, 0x00e4, 0x0088, 0x00e5, 0x0089,
	0x00e5, 0x0090, 0x00e5, 0x008b, 0x00e3, 0x0091, 0x00df, 0x0091,
	0x00e0, 0x0090, 0x00e0, 0x008c, 0x00e2, 0x008b, 0x00e1, 0x0085,
	0x00e0, 0x0084, 0x00e0, 0x0080, 0x00e1, 0x007f, 0x00e1, 0x007c,
	0x00e0, 0x007b, 0x00e0, 0x0077
};

extern Credit _credits[];

void _c_startIntro(void *parm) {
	_rightHandAnim = _vm->findAnimation("righthand");

	if (_vm->getPlatform() == Common::kPlatformPC) {
		_vm->_soundMan->setMusicFile("intro");
		_vm->_soundMan->playMusic();
	}

	_engineFlags |= kEngineMouse;

	return;
}

void _c_endIntro(void *parm) {

	_vm->_gfx->setFont(kFontMenu);

	debugC(1, kDebugLocation, "endIntro()");

	for (uint16 _si = 0; _si < 6; _si++) {
		_vm->_gfx->displayCenteredString(80, _credits[_si]._role);
		_vm->_gfx->displayCenteredString(100, _credits[_si]._name);

		_vm->_gfx->updateScreen();

		for (uint16 v2 = 0; v2 < 100; v2++) {
			_mouseButtons = kMouseNone;
			_vm->updateInput();
			if (_mouseButtons == kMouseLeftUp)
				break;

			_vm->waitTime( 1 );
		}

		_vm->_gfx->copyScreen(Gfx::kBitBack, Gfx::kBitFront);
	}
	debugC(1, kDebugLocation, "endIntro(): done showing credits");

	if ((_vm->getFeatures() & GF_DEMO) == 0) {
		_vm->_gfx->displayCenteredString(80, "CLICK MOUSE BUTTON TO START");
		_vm->_gfx->updateScreen();

		waitUntilLeftClick();

		_engineFlags &= ~kEngineMouse;
		_vm->_menu->selectCharacter();
	} else {
		waitUntilLeftClick();
	}

	return;
}

void _c_moveSheet(void *parm) {

	static uint16 x = 319;

	if (x > 66)
		x -= 16;

	Common::Rect r;

	r.left = x;
	r.top = 47;
	r.right = (x + 32 > 319) ? 319 : (x + 32);
	r.bottom = 199;
	_vm->_gfx->floodFill(Gfx::kBitBack, r, 1);
	_vm->_gfx->floodFill(Gfx::kBit2, r, 1);

	if (x >= 104) return;

	r.left = x+215;
	r.top = 47;
	r.right = (x + 247 > 319) ? 319 : (x + 247);
	r.bottom = 199;
	_vm->_gfx->floodFill(Gfx::kBitBack, r, 12);
	_vm->_gfx->floodFill(Gfx::kBit2, r, 12);

	return;
}

void plotPixel(int x, int y, int color, void *data) {
	_vm->_gfx->plotMaskPixel(x, y, color);
}

void _c_sketch(void *parm) {

	static uint16 index = 1;

	uint16 newy = _rightHandPositions[2*index+1];
	uint16 newx = _rightHandPositions[2*index];

	uint16 oldy = _rightHandPositions[2*(index-1)+1];
	uint16 oldx = _rightHandPositions[2*(index-1)];

	Graphics::drawLine(oldx, oldy, newx, newy, 0, plotPixel, NULL);

	_rightHandAnim->_left = newx;
	_rightHandAnim->_top = newy - 20;

	index++;

	return;
}




void _c_shade(void *parm) {

	Common::Rect r(
		_rightHandAnim->_left - 36,
		_rightHandAnim->_top - 36,
		_rightHandAnim->_left,
		_rightHandAnim->_top
	);

	_vm->_gfx->fillMaskRect(r, 0);

	return;

}

void _c_projector(void*) {
#ifdef HALFBRITE
	static int dword_16032 = 0;

//	Bitmap bm;
//	InitBitMap(&bm);

	if (dword_16032 != 0) {
/*		// keep drawing spotlight in its final place
		_vm->_gfx->flatBlitCnv(&scnv, 110, 25, Gfx::kBitFront);
		BltBitMap(&bm, 0, 0, &_screen._bitMap, 110, 25, a3->??, a3->??, 0x20, 0x20);
*/		return;
	}

	_vm->_gfx->setHalfbriteMode(true);
/*
	// move spot light around the stage
	int d7, d6;
	for (d7 = 0; d7 < 150; d7++) {

		if (d7 < 100) {
			int d1 = d7;
			if (d1 < 0)
				d1++;

			d1 >>= 1;
			d6 = 50 - d1;
		} else {
			int d1 = d7 / 100;
			if (d1 < 0)
				d1++;

			d1 >>= 1;
			d6 = d1;
		}

		BltBitMap(&bm, 0, 0, &_screen._bitMap, d7+20, d6, a3->??, a3->??, 0x20, 0x20);
		sub_1590C(d6 + a3->??);
		BltBitMap(&bm, 0, 0, &_screen._bitMap, d7+20, d6, a3->??, a3->??, 0xFA, 0x20);
	}

	for (d7 = 50; d7 > -10; d7--) {
		BltBitMap(&bm, 0, 0, &_screen._bitMap, d7+120, d6, a3->??, a3->??, 0x20, 0x20);
		sub_1590C(d6 + a3->??);
		BltBitMap(&bm, 0, 0, &_screen._bitMap, d7+120, d6, a3->??, a3->??, 0xFA, 0x20);
	}

	BltBitMap(&bm, 0, 0, &_screen._bitMap, d7+120, d6, a3->??, a3->??, 0x20, 0x20);
	_vm->_gfx->flatBlitCnv(&scnv, d7+120, d6, Gfx::kBitFront);
*/

	dword_16032 = 1;
	return;
#endif
}

void _c_HBOff(void*) {
#ifdef HALFBRITE
	_vm->_gfx->setHalfbriteMode(false);
#endif
}

void _c_HBOn(void*) {
#ifdef HALFBRITE
	_vm->_gfx->setHalfbriteMode(true);
#endif
}

} // namespace Parallaction
