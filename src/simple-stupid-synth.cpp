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

const uint32_t
Simple_stupid_synth::GPIO_PIN_LED = 25;

const uint32_t
Simple_stupid_synth::DEFAULT_SAMPLE_FREQ = 48000; // [HZ]

const uint8_t
Simple_stupid_synth::VOL_BITS = 8;

Simple_stupid_synth::
Simple_stupid_synth(Audio_target *const audio_target,
                    MIDI_state_machine *const midi_state_machine,
                    const uint8_t gpio_pin_activity_indicator) :
  _audio_target(audio_target), _midi_state_machine(midi_state_machine)
{
  const uint32_t sample_freq = _audio_target->get_sample_freq();
  _midi_state_machine->init(sample_freq, gpio_pin_activity_indicator);
  sleep_ms(10);
}

void
Simple_stupid_synth::synth_task()
{
  struct audio_buffer *audio_buffer = _audio_target->take_audio_buffer(false);
  if (!audio_buffer) {
    return;
  }
  const uint32_t audio_buffer_sample_count = audio_buffer->max_sample_count;
  audio_buffer->sample_count = audio_buffer_sample_count;
  int16_t *out = (int16_t *) audio_buffer->buffer->bytes;
  const uint16_t vol_mul = round(2.0 * (((long)1u) << VOL_BITS));
  const size_t num_pitches = MIDI_state_machine::NUM_PITCHES;
  const uint32_t count_inc = MIDI_state_machine::COUNT_INC;
  MIDI_state_machine::pitch_status_t *pitch_statuses =
    _midi_state_machine->get_pitch_statuses();
  for (uint32_t i = 0; i < audio_buffer_sample_count * 2;) {
    int64_t sample = 0;
    for (size_t pitch = 0; pitch < num_pitches; pitch++) {
      MIDI_state_machine::pitch_status_t *pitch_status = &pitch_statuses[pitch];
      uint32_t amplitude = pitch_status->amplitude;
      if (amplitude) {
        const uint32_t count_wrap = pitch_status->count_wrap;
        uint32_t count = pitch_status->count;
        count += count_inc;
        if (count >= count_wrap) {
          count -= count_wrap;
          amplitude = -amplitude;
          pitch_status->amplitude = amplitude;
        }
        pitch_status->count = count;
        sample += amplitude;
      }
    }
    const int16_t scaled_sample = (int16_t)((sample * vol_mul) >> VOL_BITS);
    out[i++] = scaled_sample; // left channel
    out[i++] = scaled_sample; // right channel
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
  const uint8_t gpio_pin_pwm_left = PICO_AUDIO_PWM_L_PIN; // GPIO 0 (PWM_L)
  const uint8_t gpio_pin_pwm_right = PICO_AUDIO_PWM_R_PIN; // GPIO 1 (PWM_R)
  PWM_audio_target audio_target;
  audio_target.init(Simple_stupid_synth::DEFAULT_SAMPLE_FREQ,
                    gpio_pin_pwm_left, gpio_pin_pwm_right);
#else
  const uint8_t gpio_pin_i2s_clock_base =
    PICO_AUDIO_I2S_CLOCK_PIN_BASE; // GPIO 26 (BLCK) + GPIO 27 (LRCLK)
  const uint8_t gpio_pin_i2s_data =
    PICO_AUDIO_I2S_DATA_PIN; // GPIO 28 (DATA)
  I2S_audio_target audio_target;
  audio_target.init(Simple_stupid_synth::DEFAULT_SAMPLE_FREQ,
                    gpio_pin_i2s_clock_base, gpio_pin_i2s_data);
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
