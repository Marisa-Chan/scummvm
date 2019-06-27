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

#ifndef GLK_ALAN3_GLKIO
#define GLK_ALAN3_GLKIO

/* Header file for Glk output for Alan interpreter */

#include "glk/alan3/alan3.h"
#include "glk/windows.h"

namespace Glk {
namespace Alan3 {

extern winid_t glkMainWin;
extern winid_t glkStatusWin;

/* NB: this header must be included in any file which calls printf() */

#undef printf
#define printf glkio_printf
extern void glkio_printf(const char *, ...);

#ifdef MAP_STDIO_TO_GLK
#define fgetc(stream) glk_get_char_stream(stream)
#define rewind(stream) glk_stream_set_position(stream, 0, seekmode_Start);
#define fwrite(buf, elementSize, count, stream) glk_put_buffer_stream(stream, buf, elementSize*count);
#define fread(buf, elementSize, count, stream) glk_get_buffer_stream(stream, buf, elementSize*count);
#define fclose(stream) glk_stream_close(stream, NULL)
#define fgets(buff, len, stream) glk_get_line_stream(stream, buff, len)
#endif

} // End of namespace Alan3
} // End of namespace Glk

#endif