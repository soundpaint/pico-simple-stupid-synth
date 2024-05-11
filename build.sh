#!/bin/bash
cd "$(dirname "$0")"
mkdir -p build
cd build
export PICO_SDK_PATH=../../pico-sdk
cmake -Wdev ..
make
