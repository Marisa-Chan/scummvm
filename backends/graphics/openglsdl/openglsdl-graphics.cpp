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
 * $URL$
 * $Id$
 *
 */

#ifdef USE_OPENGL

#include "backends/graphics/openglsdl/openglsdl-graphics.h"
#include "backends/platform/sdl/sdl.h"

OpenGLSdlGraphicsManager::OpenGLSdlGraphicsManager()
	:
	_hwscreen(0) {

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1) {
		error("Could not initialize SDL: %s", SDL_GetError());
	}

	SDL_ShowCursor(SDL_DISABLE);
}

OpenGLSdlGraphicsManager::~OpenGLSdlGraphicsManager() {

}

#ifdef USE_RGB_COLOR

const Graphics::PixelFormat RGBList[] = {
#if defined(ENABLE_32BIT)
	Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0), // RGBA8888
	Graphics::PixelFormat(3, 8, 8, 8, 0, 16, 8, 0, 0),  // RGB888
#endif
	Graphics::PixelFormat(2, 5, 6, 5, 0, 11, 5, 0, 0),  // RGB565
	Graphics::PixelFormat(2, 5, 5, 5, 0, 10, 5, 0, 0),  // RGB555
	Graphics::PixelFormat(2, 4, 4, 4, 4, 12, 8, 4, 0),  // RGBA4444
};

Common::List<Graphics::PixelFormat> OpenGLSdlGraphicsManager::getSupportedFormats() const {
	static Common::List<Graphics::PixelFormat> list;
	static bool inited = false;

	if (inited)
		return list;

	int listLength = ARRAYSIZE(RGBList);

	Graphics::PixelFormat format = Graphics::PixelFormat::createFormatCLUT8();
	if (_hwscreen) {
		// Get our currently set hardware format
		format = Graphics::PixelFormat(_hwscreen->format->BytesPerPixel,
			8 - _hwscreen->format->Rloss, 8 - _hwscreen->format->Gloss,
			8 - _hwscreen->format->Bloss, 8 - _hwscreen->format->Aloss,
			_hwscreen->format->Rshift, _hwscreen->format->Gshift,
			_hwscreen->format->Bshift, _hwscreen->format->Ashift);

		// Workaround to MacOSX SDL not providing an accurate Aloss value.
		if (_hwscreen->format->Amask == 0)
			format.aLoss = 8;

		// Push it first, as the prefered format.
		for (int i = 0; i < listLength; i++) {
			if (RGBList[i] == format) {
				list.push_back(format);
				break;
			}
		}

		// Mark that we don't need to do this any more.
		inited = true;
	}

	for (int i = 0; i < listLength; i++) {
		if (inited && (RGBList[i].bytesPerPixel > format.bytesPerPixel))
			continue;
		if (RGBList[i] != format)
			list.push_back(RGBList[i]);
	}
	//list.push_back(Graphics::PixelFormat::createFormatCLUT8());
	return list;
}

#endif

void OpenGLSdlGraphicsManager::warpMouse(int x, int y) {
	if (_cursorState.x != x || _cursorState.y != y) {
		int y1 = y;

		/*if (_videoMode.aspectRatioCorrection && !_overlayVisible)
			y1 = real2Aspect(y);*/

		if (!_overlayVisible)
			SDL_WarpMouse(x * _videoMode.scaleFactor, y1 * _videoMode.scaleFactor);
		else
			SDL_WarpMouse(x, y1);

		setMousePos(x, y);
	}
}


//
// Intern
//

bool OpenGLSdlGraphicsManager::loadGFXMode() {
	_videoMode.overlayWidth = _videoMode.screenWidth * _videoMode.scaleFactor;
	_videoMode.overlayHeight = _videoMode.screenHeight * _videoMode.scaleFactor;
	_videoMode.hardwareWidth = _videoMode.screenWidth * _videoMode.scaleFactor;
	_videoMode.hardwareHeight = _videoMode.screenHeight * _videoMode.scaleFactor;

	// Setup OpenGL attributes for SDL
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	_hwscreen = SDL_SetVideoMode(_videoMode.hardwareWidth, _videoMode.hardwareHeight, 32,
		_videoMode.fullscreen ? (SDL_FULLSCREEN | SDL_OPENGL) : (SDL_OPENGL | SDL_RESIZABLE)
	);
	if (_hwscreen == NULL) {
		// DON'T use error(), as this tries to bring up the debug
		// console, which WON'T WORK now that _hwscreen is hosed.

		if (!_oldVideoMode.setup) {
			warning("SDL_SetVideoMode says we can't switch to that mode (%s)", SDL_GetError());
			g_system->quit();
		} else {
			return false;
		}
	}

	return OpenGLGraphicsManager::loadGFXMode();
}

void OpenGLSdlGraphicsManager::unloadGFXMode() {
	if (_hwscreen) {
		SDL_FreeSurface(_hwscreen);
		_hwscreen = NULL;
	}
}

bool OpenGLSdlGraphicsManager::hotswapGFXMode() {
	return false;
}

void OpenGLSdlGraphicsManager::internUpdateScreen() {
	OpenGLGraphicsManager::internUpdateScreen();

	SDL_GL_SwapBuffers(); 
}

bool OpenGLSdlGraphicsManager::handleScalerHotkeys(Common::KeyCode key) {

	// Ctrl-Alt-a toggles aspect ratio correction
	/*if (key == 'a') {
		beginGFXTransaction();
			setFeatureState(OSystem::kFeatureAspectRatioCorrection, !_videoMode.aspectRatioCorrection);
		endGFXTransaction();
#ifdef USE_OSD
		char buffer[128];
		if (_videoMode.aspectRatioCorrection)
			sprintf(buffer, "Enabled aspect ratio correction\n%d x %d -> %d x %d",
				_videoMode.screenWidth, _videoMode.screenHeight,
				_hwscreen->w, _hwscreen->h
				);
		else
			sprintf(buffer, "Disabled aspect ratio correction\n%d x %d -> %d x %d",
				_videoMode.screenWidth, _videoMode.screenHeight,
				_hwscreen->w, _hwscreen->h
				);
		displayMessageOnOSD(buffer);
#endif
		internUpdateScreen();
		return true;
	}*/

	SDLKey sdlKey = (SDLKey)key;

	// Increase/decrease the scale factor
	if (sdlKey == SDLK_EQUALS || sdlKey == SDLK_PLUS || sdlKey == SDLK_MINUS ||
		sdlKey == SDLK_KP_PLUS || sdlKey == SDLK_KP_MINUS) {
		int factor = _videoMode.scaleFactor;
		factor += (sdlKey == SDLK_MINUS || sdlKey == SDLK_KP_MINUS) ? -1 : +1;
		if (0 < factor && factor < 4) {
			beginGFXTransaction();
				setScale(factor);
			endGFXTransaction();
		}	
	}
	return false;
}

void OpenGLSdlGraphicsManager::setFullscreenMode(bool enable) {

}

bool OpenGLSdlGraphicsManager::isScalerHotkey(const Common::Event &event) {
	if ((event.kbd.flags & (Common::KBD_CTRL|Common::KBD_ALT)) == (Common::KBD_CTRL|Common::KBD_ALT)) {
		const bool isScaleKey = (event.kbd.keycode == Common::KEYCODE_EQUALS || event.kbd.keycode == Common::KEYCODE_PLUS || event.kbd.keycode == Common::KEYCODE_MINUS ||
			event.kbd.keycode == Common::KEYCODE_KP_PLUS || event.kbd.keycode == Common::KEYCODE_KP_MINUS);

		return (isScaleKey || event.kbd.keycode == 'a');
	}
	return false;
}

void OpenGLSdlGraphicsManager::toggleFullScreen() {
	beginGFXTransaction();
		setFullscreenMode(!_videoMode.fullscreen);
	endGFXTransaction();
#ifdef USE_OSD
	if (_videoMode.fullscreen)
		displayMessageOnOSD("Fullscreen mode");
	else
		displayMessageOnOSD("Windowed mode");
#endif
}

bool OpenGLSdlGraphicsManager::notifyEvent(const Common::Event &event) {
	switch ((int)event.type) {
	case Common::EVENT_KEYDOWN:
		// Alt-Return and Alt-Enter toggle full screen mode
		if (event.kbd.hasFlags(Common::KBD_ALT) &&
			(event.kbd.keycode == Common::KEYCODE_RETURN ||
			event.kbd.keycode == (Common::KeyCode)SDLK_KP_ENTER)) {
			toggleFullScreen();
			return true;
		}

		// Alt-S: Create a screenshot
		if (event.kbd.hasFlags(Common::KBD_ALT) && event.kbd.keycode == 's') {
			char filename[20];

			for (int n = 0;; n++) {
				SDL_RWops *file;

				sprintf(filename, "scummvm%05d.bmp", n);
				file = SDL_RWFromFile(filename, "r");
				if (!file)
					break;
				SDL_RWclose(file);
			}
			if (saveScreenshot(filename))
				printf("Saved '%s'\n", filename);
			else
				printf("Could not save screenshot!\n");
			return true;
		}

		// Ctrl-Alt-<key> will change the GFX mode
		if ((event.kbd.flags & (Common::KBD_CTRL|Common::KBD_ALT)) == (Common::KBD_CTRL|Common::KBD_ALT)) {
			if (handleScalerHotkeys(event.kbd.keycode))
				return true;
		}
	case Common::EVENT_KEYUP:
		return isScalerHotkey(event);
	/*case OSystem_SDL::kSdlEventExpose:
		break;*/
	// HACK: Handle special SDL event
	case OSystem_SDL::kSdlEventResize:
		_videoMode.hardwareWidth = event.mouse.x;
		_videoMode.hardwareHeight = event.mouse.y;
		initGL();
		return true;

	default:
		break;
	}

	return OpenGLGraphicsManager::notifyEvent(event);
}

#endif
