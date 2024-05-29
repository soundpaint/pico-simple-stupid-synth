/*
 * Simple Stupid Synthesizer
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

#include "simple-stupid-synth.hpp"
#include "i2s-audio-target.hpp"
#include "pwm-audio-target.hpp"
#include <math.h>
#include "pico/stdlib.h"

//#define USE_PWM_AUDIO

const uint32_t
Simple_stupid_synth::GPIO_PIN_LED = 25;

const uint32_t
Simple_stupid_synth::DEFAULT_SAMPLE_FREQ = 24000; // [HZ]

const uint8_t
Simple_stupid_synth::VOL_BITS = 8;

Simple_stupid_synth::
Simple_stupid_synth(Audio_target *const audio_target,
                    MIDI_state_machine *const midi_state_machine,
                    const uint8_t gpio_pin_activity_indicator) :
  _is_stereo(audio_target->is_stereo()),
  _audio_target(audio_target), _midi_state_machine(midi_state_machine)
{
  const uint32_t sample_freq = _audio_target->get_sample_freq();
  _midi_state_machine->init(sample_freq, gpio_pin_activity_indicator);
  sleep_ms(10);
}

inline uint8_t
Simple_stupid_synth::do_limit(const int16_t elongation, const uint16_t limit)
{
  return
    elongation >= 0 ?
    (elongation <= limit ? elongation : limit) :
    (-elongation <= limit ? -elongation : -limit);
}

void
Simple_stupid_synth::synth_task()
{
  struct audio_buffer *audio_buffer = _audio_target->take_audio_buffer(false);
  if (!audio_buffer) {
    return;
  }
  const uint32_t audio_buffer_sample_count = audio_buffer->max_sample_count;
  if (!audio_buffer_sample_count) {
    return;
  }
  audio_buffer->sample_count = audio_buffer_sample_count;
  int16_t *out = (int16_t *) audio_buffer->buffer->bytes;
  const uint16_t vol_mul = round(2.0 * (((long)1u) << VOL_BITS));
  const size_t num_osc = MIDI_state_machine::NUM_KEYS;
  const uint32_t count_inc = MIDI_state_machine::COUNT_INC;
  MIDI_state_machine::osc_status_t *osc_statuses =
    _midi_state_machine->get_osc_statuses();
  const uint32_t total_sample_count =
    audio_buffer->max_sample_count * (_is_stereo ? 2 : 1);
  const int16_t cumulated_channel_pressure =
    _midi_state_machine->get_cumulated_channel_pressure();
  for (uint32_t sample_index = 0; sample_index < total_sample_count;) {
    int64_t sample_value = 0;
    for (uint8_t osc = 0; osc < num_osc; osc++) {
      MIDI_state_machine::osc_status_t *osc_status = &osc_statuses[osc];
      uint32_t elongation = osc_status->elongation;
      //do_limit(osc_status->elongation, cumulated_channel_pressure);
      if (elongation) {
        const uint32_t count_wrap = osc_status->count_wrap;
        uint32_t count = osc_status->count;
        count += count_inc;
        if (count >= count_wrap) {
          count -= count_wrap;
          elongation = -elongation;
          osc_status->elongation = elongation;
        }
        osc_status->count = count;
        sample_value += elongation;
      }
    }
    const int16_t scaled_sample_value =
      (int16_t)((sample_value * vol_mul) >> VOL_BITS);
    if (_is_stereo) {
      out[sample_index++] = scaled_sample_value; // left channel
      out[sample_index++] = scaled_sample_value; // right channel
    } else {
      out[sample_index++] = scaled_sample_value; // mono channel
    }
  }
  _audio_target->give_audio_buffer(audio_buffer);
}

void
Simple_stupid_synth::main_loop()
{
  for (;;) {
    _midi_state_machine->tx_task();
    _midi_state_machine->rx_task();
    synth_task();
  }
}

int main()
{
  stdio_init_all();
#ifdef USE_PWM_AUDIO
  const uint8_t gpio_pin_pwm_mono = PICO_AUDIO_PWM_L_PIN; // GPIO 0 (PWM_L)
  PWM_audio_target audio_target(Simple_stupid_synth::DEFAULT_SAMPLE_FREQ,
                                gpio_pin_pwm_mono);
  audio_target.init(3);
#else
  const uint8_t gpio_pin_i2s_clock_base =
    PICO_AUDIO_I2S_CLOCK_PIN_BASE; // GPIO 26 (BLCK) + GPIO 27 (LRCLK)
  const uint8_t gpio_pin_i2s_data =
    PICO_AUDIO_I2S_DATA_PIN; // GPIO 28 (DATA)
  I2S_audio_target audio_target(Simple_stupid_synth::DEFAULT_SAMPLE_FREQ,
                                gpio_pin_i2s_clock_base, gpio_pin_i2s_data);
  audio_target.init();
#endif
  MIDI_state_machine midi_state_machine;
  const uint8_t gpio_pin_activity_indicator =
    Simple_stupid_synth::GPIO_PIN_LED; // GPIO 25 (LED)
  Simple_stupid_synth simple_stupid_synth(&audio_target,
                                          &midi_state_machine,
                                          gpio_pin_activity_indicator);
  simple_stupid_synth.main_loop();
  return 0;
}
