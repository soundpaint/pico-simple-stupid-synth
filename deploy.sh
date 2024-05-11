#!/bin/bash
#
# Run this script (without any argument) for deploying the latest
# build on a Raspberry Pico that is connected to this host computer.
# This script assumes that a single Pico has been connected while
# pressing the BOOTSEL button.  It tries to access the expected
# directory path /media/${USER}/RPI-RP2, or, if not yet available, to
# first mount the Pico under this directory path.  If the path is
# found, this script copies the latest compiled image to this
# directory.  After copying, the Pico should itself disconnect and
# immediately run the uploaded image.
#
# NOTE: This script has been written and tested under a Debian /
# Ubuntu-like distribution.  It should run on any such system
# (including the Debian-based Raspberry Pi OS), but may not work for
# other distributions.

cd "$(dirname "$0")"
mounted_path=/media/${USER}/RPI-RP2
if [ ! -f ${mounted_path} ] ; then
    serial_number=$(udisksctl dump | grep RPI-RP2- | head -n 1 |
                        rev | cut -d- -f1 | rev)
    if [ -z "${serial_number}" ] ; then
        printf "no connected RPI-RP2 found, aborting...\n"
        exit 255
    fi
    device=$(udisksctl dump | grep -E " Device:| IdLabel: *RPI-RP2" |
                 grep -B1 IdLabel | head -n 1 | rev | cut -d" " -f1 | rev)
    printf "mounting RP2 Pico with serial number %s on device %s...\n" \
           ${serial_number} ${device}
    udisksctl mount -b ${device}
fi
mounted_device=$(df | grep ${mounted_path} | cut -d" " -f1)
if [ -z "${mounted_device}" ] ; then
    printf "mount failed (are you sure you pressed the BOOTSEL \
button while plugging in?), aborting...\n"
    exit 255
fi
printf "deploying binary to RP2 Pico device %s mounted at %s...\n" \
       ${mounted_device} ${mounted_path}
pv --buffer-size 1K build/simple-stupid-synth.uf2 \
   > ${mounted_path}/simple-stupid-synth.uf2

# Local variables:
#   mode: bash
#  coding: utf-8
# End:
