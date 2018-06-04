#!/bin/bash --norc

# root: ${CTNG_HOME}/armv8-rpi3-linux-gnueabihf

PATH="external/ctng/armv8-rpi3-linux-gnueabihf/bin:${PATH}" \
  exec \
  armv8-rpi3-linux-gnueabihf-gcc
  "$@"
