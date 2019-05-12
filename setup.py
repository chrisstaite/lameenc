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
    version='1.0.2',
    description='LAME encoding bindings',
    author='Chris Staite',
    author_email='chris@yourdreamnet.co.uk',
    url='https://github.com/chrisstaite/lameenc',
    ext_modules=[lameenc],
    classifiers=(
        'Topic :: Multimedia :: Sound/Audio :: Conversion',
        'Programming Language :: Python :: 3 :: Only',
        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Operating System :: MacOS :: MacOS X',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux'
    )
)
