/*
 * Audio Target of Simple Stupid Synthesizer
 *
 * Copyright (C) 2023 Jürgen Reuter
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * As a special exception to the GNU General Public License, if you
 * distribute this file as part of a program that contains a
 * configuration script generated by Autoconf, you may include it
 * under the same distribution terms that you use for the rest of that
 * program.
 *
 * For updates and more info or contacting the author, visit:
 * <https://github.com/soundpaint>
 *
 * Author's web site: www.juergen-reuter.de
 */

#ifndef AUDIO_TARGET_HPP
#define AUDIO_TARGET_HPP

#include <inttypes.h>
#include "pico/audio.h"

class Audio_target {
public:
  Audio_target(const uint32_t sample_freq);
  virtual ~Audio_target();
  uint32_t get_sample_freq() const;
  struct audio_buffer *take_audio_buffer(const bool block);
  void give_audio_buffer(audio_buffer_t *audio_buffer);
protected:
  struct audio_format _target_audio_format = {
    .sample_freq = 0,
    .format = AUDIO_BUFFER_FORMAT_PCM_S16,
    .channel_count = 2,
  };
  struct audio_buffer_format _target_audio_buffer_format = {
    .format = &_target_audio_format,
    .sample_stride = 4
  };
  struct audio_buffer_pool *_target_producer_pool;
};

#endif /* AUDIO_TARGET_HPP */

/*
 * Local variables:
 *   mode: c++
 *  coding: utf-8
 * End:
 */