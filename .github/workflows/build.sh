#!/bin/bash

set -e -x

# Change to the source directory
cd "${0%/*}/../../"

mkdir build
cd build

curl -L https://sourceforge.net/projects/lame/files/lame/3.100/lame-3.100.tar.gz/download | tar -xzf -
lame-*/configure --prefix=`pwd`
cp ../fixed-libmp3lame.sym lame-*/include/libmp3lame.sym
make CFLAGS=-fPIC LDFLAGS=-fPIC
make install
cd ..
for PYBIN in /opt/python/*/bin; do
    if [[ $PYBIN = *"cp27"* ]]; then
        continue
    fi
    "${PYBIN}/python" setup.py bdist_wheel -b build/build_tmp -d build --libdir=build/lib --incdir=build/include/lame
done
for whl in build/*.whl; do
    auditwheel repair "$whl" -w build/
    rm "$whl"
done
