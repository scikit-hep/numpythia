#!/usr/bin/env python

import sys

# Check Python version
if sys.version_info < (2, 7):
    sys.exit("numpythia only supports python 2.7 and above")

if sys.version_info[0] < 3:
    import __builtin__ as builtins
else:
    import builtins

from setuptools import setup, Extension, find_packages
from pkg_resources import parse_version, get_distribution

from setuptools.command.build_ext import build_ext as _build_ext
from setuptools.command.install import install as _install

import os
import fnmatch
import platform
import subprocess

# Prevent setup from trying to create hard links
# which are not allowed on AFS between directories.
# This is a hack to force copying.
try:
    del os.link
except AttributeError:
    pass

local_path = os.path.dirname(os.path.abspath(__file__))
# setup.py can be called from outside the source directory
os.chdir(local_path)
sys.path.insert(0, local_path)

def recursive_glob(path, pattern):
    matches = []
    for root, dirnames, filenames in os.walk(path):
        for filename in fnmatch.filter(filenames, pattern):
            matches.append(os.path.join(root, filename))
    return matches

libnumpythia = Extension(
    'numpythia._libnumpythia',
    sources=['numpythia/src/_libnumpythia.cpp'] +
        recursive_glob('numpythia/src/extern/hepmc3.0.0/src', '*.cc') +
        recursive_glob('numpythia/src/extern/pythia8226/src', '*.cc'),
    depends=[],
    language='c++',
    include_dirs=[
        'numpythia/src',
        'numpythia/src/extern/hepmc3.0.0/include',
        'numpythia/src/extern/pythia8226/include',
    ],
    extra_compile_args=[
        '-std=c++11',  # for HepMC
        '-Wno-unused-function',
        '-Wno-write-strings',
    ],
    define_macros=[
        ('XMLDIR', '""'),
    ],
)


class build_ext(_build_ext):
    user_options = _build_ext.user_options + [
        ('external-fastjet', None, None),
    ]

    def initialize_options(self):
        _build_ext.initialize_options(self)
        self.external_fastjet = False

    def finalize_options(self):
        global libnumpythia
        #global external_fastjet
        _build_ext.finalize_options(self)
        # Prevent numpy from thinking it is still in its setup process
        try:
            del builtins.__NUMPY_SETUP__
        except AttributeError:
            pass
        import numpy
        libnumpythia.include_dirs.append(numpy.get_include())

    def build_extensions(self):
        _build_ext.build_extensions(self)


class install(_install):
    user_options = _install.user_options + [
        ('external-fastjet', None, None),
    ]

    def initialize_options(self):
        _install.initialize_options(self)
        self.external_fastjet = False

    def finalize_options(self):
        global external_fastjet
        if self.external_fastjet:
            external_fastjet = True
        _install.finalize_options(self)



setup(
    name='numpythia',
    version='0.3.3',
    description='The interface between PYTHIA and NumPy',
    long_description=''.join(open('README.rst').readlines()),
    maintainer='Noel Dawe',
    maintainer_email='noel@dawe.me',
    license='GPLv3',
    url='http://github.com/ndawe/numpythia',
    packages=[
        'numpythia',
        'numpythia.testcmnd',
    ],
    package_data={
        'numpythia': [
            'testcmnd/*.cmnd',
            'src/extern/pythia8226/share/*',
        ],
    },
    ext_modules=[libnumpythia],
    cmdclass={
        'build_ext': build_ext,
        'install': install,
    },
    classifiers=[
        'Intended Audience :: Science/Research',
        'Intended Audience :: Developers',
        'Topic :: Software Development',
        'Topic :: Scientific/Engineering',
        'Operating System :: POSIX',
        'Operating System :: Unix',
        'Operating System :: MacOS',
        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
        'Programming Language :: Cython',
        'Development Status :: 4 - Beta',
    ],
    install_requires=['numpy', 'six'],
    tests_require=['pytest'],
    zip_safe=False,
)
