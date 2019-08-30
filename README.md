[![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/chrisstaite/lameenc?svg=true&branch=master)](https://ci.appveyor.com/project/chrisstaite/lameenc)
[![Build Status](https://travis-ci.org/chrisstaite/lameenc.svg?branch=master)](https://travis-ci.org/chrisstaite/lameenc)

lameenc
=======

The purpose of this library is to wrap around LAME for Python.
There are many ways of doing this currently, but none have a
binary distribution on pypi.  The idea is to automatically build
and push to pypi for Linux, macOS and Windows.  This is possible
as of April 2017 due to the patents on MP3 having expired.

Build
-----

```
mkdir build
cd build
cmake ..
make
```

Usage
-----

To use the library you need to have raw PCM data in 16-bit
interleaved format.  Among the options for this encoder is
mono or stereo, i.e. 1 or 2 channels.  You can set the sample
rate of input PCM data.

Usage is fairly simple in Python:

```
import lameenc
encoder = lameenc.Encoder()
encoder.set_bit_rate(128)
encoder.set_in_sample_rate(16000)
encoder.set_channels(2)
encoder.set_quality(2)  # 2-highest, 7-fastest
# Can call this in a loop
mp3_data = encoder.encode(interleaved_pcm_data)
# Flush when finished encoding the entire stream
mp3_data += encoder.flush()
```

If LAME is being chatty to stdout and you don't like
that, then you can call `encoder.silence()` and it
should shut up.

