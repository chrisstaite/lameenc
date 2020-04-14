#!/bin/sh

git clone --depth 1 https://github.com/raspberrypi/tools $HOME/rpi-tools
export PATH=$PATH:$HOME/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
export ARCH=arm
export CCPREFIX=$HOME/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-
export CC=arm-linux-gnueabihf-gcc
export CXX=arm-linux-gnueabihf-g++

sudo apt-get install -y dpkg-cross curl
curl -o python3.5.deb https://archive.raspbian.org/raspbian/pool/main/p/python3.5/python3.5_3.5.4-4_armhf.deb

python3.5 -m crossenv `which python3.5` $HOME/venv
source $HOME/venv/bin/activate
cross-pip install --upgrade pip
/bin/sh ./.travis-ci.sh
