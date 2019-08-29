import sys
import setuptools
import distutils.core
import os.path


# Grab the library location from the command line
libdir = None
for argument in sys.argv:
    if argument.startswith('--libdir='):
        libdir = argument
if libdir is None:
    print('Need to specify location of liblame')
    sys.exit(1)
sys.argv.remove(libdir)
libdir = libdir[len('--libdir='):]

# Grab the include location from the command line
incdir = None
for argument in sys.argv:
    if argument.startswith('--incdir='):
        incdir = argument
if incdir is None:
    print('Need to specify location of liblame source')
    sys.exit(1)
sys.argv.remove(incdir)
incdir = incdir[len('--incdir='):]

# Create the extension
lameenc = distutils.core.Extension(
    'lameenc',
    include_dirs=[incdir],
    libraries=['libmp3lame'] if sys.platform == 'win32' else [],
    extra_objects=
        [] if sys.platform == 'win32' else [os.path.join(libdir, 'libmp3lame.a')],
    library_dirs=[libdir] if sys.platform == 'win32' else [],
    sources=['lameenc.c']
)

# Create the package
setuptools.setup(
    name='lameenc',
    version_config={
        'version_format': '{tag}.dev{sha}',
        'starting_version': '0.1.0'
    },
    description='LAME encoding bindings',
    long_description='''
Python 3 bindings for the LAME encoding library.
This library makes it simple to encode PCM data into MP3 without having
to compile any binaries.

Provides binaries in PyPi for Python 3.4+ for Windows, macOS and Linux.
''',
    author='Chris Staite',
    author_email='chris@yourdreamnet.co.uk',
    url='https://github.com/chrisstaite/lameenc',
    license='GPLv3',
    ext_modules=[lameenc],
    setup_requires=['better-setuptools-git-version'],
    classifiers=(
        'Topic :: Multimedia :: Sound/Audio :: Conversion',
        'Programming Language :: Python :: 3 :: Only',
        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Operating System :: MacOS :: MacOS X',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux'
    )
)
