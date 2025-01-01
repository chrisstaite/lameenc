#!/bin/bash

set -e -x

# Change to the source directory
cd "${0%/*}/../../"

mkdir build
cd build

curl -L https://sourceforge.net/projects/lame/files/lame/3.100/lame-3.100.tar.gz/download | tar -xzf -
sed -i 's/0-9\*0-9\*/\[0-9]\*\[0-9]\*/g' lame-*/configure
lame-*/configure "CFLAGS=-fPIC -fno-fast-math" LDFLAGS=-fPIC --prefix=`pwd` --enable-expopt --enable-nasm --disable-frontend \
 --disable-decoder --disable-analyzer-hooks --disable-debug --disable-dependency-tracking
cp ../fixed-libmp3lame.sym lame-*/include/libmp3lame.sym
make
make install
cd ..
for PYBIN in /opt/python/*/bin; do
    if [[ $PYBIN = *"cp27"* ]] || [[ $PYBIN = *"cp36"* ]] || [[ $PYBIN = *"cp37"* ]]; then
        continue
    fi
    "${PYBIN}/python" -m build -w -o build -C="--build-option=--libdir=build/lib" -C="--build-option=--incdir=build/include/lame"
done
for whl in build/*.whl; do
    auditwheel repair "$whl" -w build/
    rm "$whl"
done
