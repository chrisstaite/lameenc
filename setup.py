import sys
import setuptools
import distutils.core
import os.path


print(sys.argv)
# Grab the library location from the command line
libdir = None
for argument in sys.argv:
    if argument.startswith('--libdir='):
        libdir = argument
if libdir is None:
    if 'bdist_wheel' in sys.argv:
        print('Need to specify location of liblame')
        sys.exit(1)
else:
    sys.argv.remove(libdir)
    libdir = libdir[len('--libdir='):]

# Grab the include location from the command line
incdir = None
for argument in sys.argv:
    if argument.startswith('--incdir='):
        incdir = argument
if incdir is None:
    if 'bdist_wheel' in sys.argv:
        print('Need to specify location of liblame source')
        sys.exit(1)
else:
    sys.argv.remove(incdir)
    incdir = incdir[len('--incdir='):]

# Create the extension
lameenc = distutils.core.Extension(
    'lameenc',
    include_dirs=[incdir] if incdir else [],
    libraries=['libmp3lame'] if sys.platform == 'win32' else [],
    extra_objects=
        [] if sys.platform == 'win32' or not libdir else [os.path.join(libdir, 'libmp3lame.a')],
    library_dirs=[libdir] if sys.platform == 'win32' and libdir else [],
    sources=['lameenc.c']
)

configuration = dict(
    name='lameenc',
    description='LAME encoding bindings',
    long_description='''
Python 3 bindings for the LAME encoding library.
This library makes it simple to encode PCM data into MP3 without having
to compile any binaries.

Provides binaries in PyPi for Python 3.8+ for Windows, macOS and Linux.
''',
    author='Chris Staite',
    author_email='chris@yourdreamnet.co.uk',
    url='https://github.com/chrisstaite/lameenc',
    license='LGPLv3',
    ext_modules=[lameenc],
    classifiers=[
        'Topic :: Multimedia :: Sound/Audio :: Conversion',
        'Programming Language :: Python :: 3 :: Only',
        'License :: OSI Approved :: GNU Lesser General Public License v3 (LGPLv3)',
        'Operating System :: MacOS :: MacOS X',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux'
    ]
)

# Create the package
setuptools.setup(**configuration)
