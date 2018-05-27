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
interleaved format.  The only options for this encoder are
mono or stereo, i.e. 1 or 2 channels.  Currently the only
supported sample rate is 44100.

Usage is fairly simple in Python:

```
import lameenc
encoder = lameenc.Encoder()
encoder.set_bit_rate(128)
encoder.set_channels(2)
encoder.set_quality(2)  # 2-highest, 7-fastest
# Can call this in a loop
mp3_data = encoder.encode(interleaved_pcm_data)
# Flush when finished encoding the entire stream
mp3_data += encoder.flush()
```

