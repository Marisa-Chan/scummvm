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

#ifndef PINK_SOUND_H
#define PINK_SOUND_H

#include <common/stream.h>
#include <audio/mixer.h>

namespace Pink {

enum class AudioFormat{kWAV, kVOX};

/*TODO
  from disasm foreground 100 %, background 80 %
  dont know how to properly do it
  may be use ConfMan
*/

class Sound {
public:
    Sound(Audio::Mixer *mixer, AudioFormat format, Common::SeekableReadStream *stream);
    ~Sound();

    bool load(AudioFormat format, Common::SeekableReadStream *stream);
    void play(Audio::Mixer::SoundType type, int volume, bool isLoop);

    bool isLoaded();
    bool isPlaying();

    void pause();
    void resume();
    void stop();

    void setBalance(int8 balance);

private:
    Audio::Mixer *_mixer;
    Audio::AudioStream *_stream;
    Audio::SoundHandle _handle;
};

} // End of namespace Pink

#endif