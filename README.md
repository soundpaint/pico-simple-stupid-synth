# pico-simple-stupid-synth

This repository contains the code for a Raspberry Pico RP2040 based
very simple polyphonic synthesizer.  The Pico will appear as a
USB-MIDI device on any USB host computer that supports MIDI over USB.

## Software Features

The synthesizer only supports a clean square shape as sole wave form,
since this is the simplest and fastest wave form to synthesize.  But
it can do this for roughly 32 polyphonic voices in parallel.
Actually, the exact limit of voices varies with environment properties
such as the overall traffic on the MIDI lines and other.  Also, lots
of high pitches will produce slightly more MCU load than low pitches.

## Connecting to a USB Host

MIDI data is transferred via USB.  That is, just connect the Pico with
your PC or notebook via USB, and the Pico, when programmed as
synthesizer, will appear as a USB-MIDI device named _Pico Simple
Stupid Synth_.  You can play any MIDI files, using your favorite MIDI
player, just by selecting this MIDI device.

## Connecting to an Audio Output Device

The synthesizer outputs its generated audio as digital stereo signal
via an I²S interface (not to be confused with I²C) with 16 bits sample
size and a sample frequency of 48kHz.  Specifically,

* the I²S BLCK clock signal is provided at GPIO 26 (pin 31),
* the I²S LRCLK word signal is provided at GPIO 27 (pin 32), and
* the I²S data signal is provided at GPIO 28 (pin 34).

To hear the sound, you will need to connect an I²S audio device to
these pins.  Check out for some I²S audio amplifier breakout or board
or any device that features an I²S input.

## Compiling

It is highly recommended to compile the project on a Raspberry Pi
computer.  Compiling _should_ also work on any other Linux machine or
maybe even under Windows, but I have not verified that.

The build files assume that the files of this repository are located
side by side with the Pico SDK as provided by the Raspberry
foundation.  See <a
href="https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html">The
C/C++ SDK</a> for instruction how to install the SDK.  The probably
easiest way to install the SDK on a Raspberry Pi is to use the setup
script that is mentioned in Section _SDK Setup_.

Assuming you have installed the Pico SDK in your home directory
<code>~/pico</code>, the directory layout structure should look as
follows:

```
├── ~/pico
|   ├── openocd
|   ├── pico-examples
|   ├── pico-extras
|   ├── pico-playground
|   ├── picoprobe
|   ├── pico-sdk
|   ├── pico-simple-stupid-synth
```

Ensure you have version 1.5 of the Pico SDK or higher.  Earlier
releases of the Pico SDK include a version of the TinyUSB library that
is likely not to work.

For compiling, run the script <code>build.sh</code> from within the
<code>pico-simple-stupid-synth</code> directory.

## Deploying

After successful compiling, you should find the file
<code>~/pico/pico-simple-stupid-synth/build/simple-stupid-synth.elf</code>.
While pressing the <code>BOOTSEL</code> button on your pico, connect
it via USB to your computer.  You should now see a new (pseudo) file
system, usually called <code>RPI-RP2</code>.  If the file system does
not automatically appear, you may need to manually mount it (it will
usually be accessible under the <code>/dev/sda1</code> device file, of
course unless you have already some other device that has already
grabbed this name).

To deploy, just copy the compiled file
<code>~/pico/pico-simple-stupid-synth/build/simple-stupid-synth.elf</code>
onto the Pico file system.  The file system should then automatically
disappear again, and the synthesizer immediately starts.

## Running

The Pico should now appear as a MIDI device named _Pico Simple Stupid
Synth_ (if not, try disconnecting and reconnecting your USB cable).
You can play any MIDI files, using your favorite MIDI player, just by
selecting this MIDI device as MIDI ouput device in your MIDI player.

## License &amp; Third-Party Code

The source code of the Pico Simple Stupid Synth is available under the
conditions of the GNU General Public Licence Version 2.  Note that
this repository additionally contains files <code>tusb_config.h</code>
and <code>usb_descriptors.c</code> based on third-party code,
copyrighted by Ha Thach (tinyusb.org), licensed under the MIT License.
The files <code>pico_extras_import.cmake</code> and
<code>pico_sdk_import.cmake</code> are copied from the Pico SDK and
are copyrighted by Raspberry Pi (Trading) Ltd.
