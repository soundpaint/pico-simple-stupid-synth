cmake_minimum_required(VERSION 3.18)

# pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# we also need PICO EXTRAS
include(pico_extras_import.cmake)

project(simple-stupid-synth)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
add_compile_options(-Wall -Wextra -Wnull-dereference)

# initialize the Pico SDK
pico_sdk_init()

add_executable(simple-stupid-synth
  src/simple-stupid-synth.cpp
  src/midi-state-machine.cpp
  src/audio-target.cpp
  src/i2s-audio-target.cpp
  src/pwm-audio-target.cpp
  src/usb_descriptors.c)

target_compile_definitions(simple-stupid-synth PRIVATE
  PICO_AUDIO_I2S_MONO_OUTPUT=0
  PICO_AUDIO_I2S_MONO_INPUT=0)

target_include_directories(simple-stupid-synth PRIVATE
  src
  ../pico-sdk/lib/tinyusb/src
  ../pico-sdk/lib/tinyusb/hw)

# enable serial output via UART
#pico_enable_stdio_uart(usb_sound_card 1)

# pull in common dependencies
target_link_libraries(simple-stupid-synth
  pico_stdlib
  pico_multicore
  tinyusb_device
  tinyusb_board
  pico_audio_i2s
  pico_audio_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(simple-stupid-synth)

# Local variables:
#   mode: cmake
#   coding: utf-8
# End:
