/*
 * MIDI State Machine of Simple Stupid Synthesizer
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

#include "midi-state-machine.hpp"
#include <math.h>
#include "pico/stdlib.h"
#include "bsp/board.h"

const uint8_t
MIDI_state_machine::COUNT_HEADROOM_BITS = 0x8;

const uint32_t
MIDI_state_machine::COUNT_INC = ((long)1u) << COUNT_HEADROOM_BITS;

MIDI_state_machine::MIDI_state_machine()
{
}

MIDI_state_machine::~MIDI_state_machine()
{
}

void
MIDI_state_machine::init(const uint32_t sample_freq,
                         const uint8_t gpio_pin_activity_indicator)
{
  _gpio_pin_activity_indicator = gpio_pin_activity_indicator;
  gpio_init(gpio_pin_activity_indicator);
  gpio_set_dir(gpio_pin_activity_indicator, GPIO_OUT);
  const double count_inc = COUNT_INC;
  for (size_t pitch = 0; pitch < NUM_PITCHES; pitch++) {
    const double pitch_freq =
      440.0 * exp((pitch - 69.0) / 12.0 * log(2.0)); // [Hz]
    const uint32_t count_wrap =
      round(0.5 * count_inc * sample_freq / pitch_freq);
    _pitch_statuses[pitch].count_wrap = count_wrap;
    _pitch_statuses[pitch].count = 0;
    _pitch_statuses[pitch].velocity = 0;
    _pitch_statuses[pitch].amplitude = 0;
  }
  _timestamp_active_sensing = time_us_64();
  board_init();
  tusb_init();
}

MIDI_state_machine::pitch_status_t *
MIDI_state_machine::get_pitch_statuses()
{
  return &_pitch_statuses[0];
}

/*
 * For the structure of event packets, see Sect. 4, "USB-MIDI Event
 * Packets" in the "Universal Serial Bus Device Class Definition for
 * MIDI Devices" at https://usb.org/sites/default/files/midi10.pdf.
 */
void
MIDI_state_machine::consume_event_packet(const uint8_t *event_packet)
{
  const uint8_t code_index_number = event_packet[0] & 0xf;
  if (code_index_number == 0x9) {
    // note on
    const uint8_t pitch = event_packet[2] & 0x7f;
    const uint8_t velocity = event_packet[3] & 0x7f;
    _pitch_statuses[pitch].velocity = velocity;
    _pitch_statuses[pitch].amplitude = velocity;
    gpio_put(_gpio_pin_activity_indicator, 1);
  } else if (code_index_number == 0x8) {
    // note off
    const uint8_t pitch = event_packet[2] & 0x7f;
    _pitch_statuses[pitch].velocity = 0;
    _pitch_statuses[pitch].amplitude = 0;
    gpio_put(_gpio_pin_activity_indicator, 0);
  }
}

void
MIDI_state_machine::rx_task()
{
  tud_task();
  if (!tud_midi_mounted()) {
    return;
  }
  while (tud_midi_available()) {
    /*
     * For the structure of the 4-byte packets, see "USB_MIDI Event
     * Packets" in: USB device class definition
     * (usb.org/sites/default/files/midi10.pdf), page 16.
     */
    uint8_t event_packet[4];
    if (tud_midi_packet_read(event_packet)) {
      consume_event_packet(&event_packet[0]);
    }
  }
}

void
MIDI_state_machine::produce_tx_data(uint8_t *buffer,
                                    __unused const size_t max_buffer_size,
                                    size_t *const buffer_size)
{
  /* when deadline for next active sensing has expired, produce a
     packet containing a sensive acting MIDI code */
  const uint64_t timestamp_now = time_us_64();
  if (timestamp_now - _timestamp_active_sensing > 300000) {
    _timestamp_active_sensing += 300000;
    buffer[0] = 0xFE; // MIDI code for active sensing
    *buffer_size = 1;
  } else {
    *buffer_size = 0;
  }
}

void
MIDI_state_machine::tx_task()
{
  static uint8_t tx_data_buffer[3];
  size_t buffer_size;
  produce_tx_data(tx_data_buffer, sizeof(tx_data_buffer), &buffer_size);
  tud_midi_stream_write(0, tx_data_buffer, buffer_size);
}

/*
 * Local variables:
 *   mode: c++
 *  coding: utf-8
 * End:
 */
