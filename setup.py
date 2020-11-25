#!/usr/bin/env python

import sys
import os
import fnmatch
import numpy # Either pre-existing or required by PEP 517/518 style build (pyproject.toml, pip 10+)

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext as _build_ext
from setuptools.command.install import install as _install

# Use -j N or set the environment variable NPY_NUM_BUILD_JOBS
# from numpy.distutils.ccompiler import CCompiler_compile
# import distutils.ccompiler
# distutils.ccompiler.CCompiler.compile = CCompiler_compile

local_path = os.path.dirname(os.path.abspath(__file__))

def recursive_glob(path, pattern):
    matches = []
    for root, dirnames, filenames in os.walk(path):
        for filename in fnmatch.filter(filenames, pattern):
            matches.append(os.path.join(root, filename))
    return matches

libnumpythia = Extension(
    'numpythia._libnumpythia',
    sources=['numpythia/src/_libnumpythia.pyx'] +
        recursive_glob('numpythia/src/extern/hepmc3.0.0/src', '*.cc') +
        recursive_glob('numpythia/src/extern/pythia8244/src', '*.cc'),
    depends=[],
    language='c++',
    include_dirs=[
        'numpythia/src',
        'numpythia/src/extern/hepmc3.0.0/include',
        'numpythia/src/extern/pythia8244/include',
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
        _build_ext.finalize_options(self)
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


extras_require = {"dev": ["pytest"], "test": ["pytest"]}

setup(
    name='numpythia',
    version='1.2.0',
    description='The interface between PYTHIA and NumPy',
    long_description=''.join(open('README.rst').readlines()),
    maintainer='the Scikit-HEP admins',
    maintainer_email='scikit-hep-admins@googlegroups.com',
    license='GPLv3',
    url='http://github.com/scikit-hep/numpythia',
    packages=find_packages(exclude='tests'),
    package_data={
        'numpythia': [
            'testcmnd/*.cmnd',
            'src/extern/pythia8244/share/Pythia8/xmldoc/*',
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
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: C++',
        'Programming Language :: Cython',
        'Development Status :: 5 - Production/Stable',
    ],
    tests_require=extras_require["dev"],
    extras_require=extras_require,
    install_requires=['numpy', 'six'],
    python_requires=">=2.7, !=3.0.*, !=3.1.*, !=3.2.*, !=3.3.*, !=3.4.*",
    zip_safe=False,
)
