#!/bin/sh

git clone https://github.com/raspberrypi/tools $HOME/rpi-tools
export PATH=$PATH:$HOME/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
export ARCH=arm
export CCPREFIX=$HOME/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-
export CC=arm-linux-gnueabihf-gcc

/bin/sh ./.travis-ci.sh
