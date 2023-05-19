#!/bin/bash
mkdir build
cd build
export PICO_SDK_PATH=../../pico-sdk
cmake -Wdev ..
make
