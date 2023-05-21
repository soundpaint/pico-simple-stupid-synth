/*
 * I2S Audio Target of Simple Stupid Synthesizer
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

#ifndef I2S_AUDIO_TARGET_HPP
#define I2S_AUDIO_TARGET_HPP

#include "audio-target.hpp"
#include "pico/audio_i2s.h"

class I2S_audio_target : public Audio_target {
public:
  virtual ~I2S_audio_target();
  void init(const uint32_t sample_freq,
            const uint8_t gpio_pin_i2s_clock_base,
            const uint8_t gpio_pin_i2s_data);
  virtual uint32_t get_sample_freq() const;
private:
  struct audio_i2s_config _target_audio_config = {
    .data_pin = 255,
    .clock_pin_base = 255,
    .dma_channel = 0,
    .pio_sm = 0,
  };
};

#endif /* I2S_AUDIO_TARGET_HPP */

/*
 * Local variables:
 *   mode: c++
 *  coding: utf-8
 * End:
 */
