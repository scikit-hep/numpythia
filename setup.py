#!/usr/bin/env python

import sys

# Check Python version
if sys.version_info < (2, 6):
    sys.exit("numpythia only supports python 2.6 and above")

if sys.version_info[0] < 3:
    import __builtin__ as builtins
else:
    import builtins

try:
    # Try to use setuptools if installed
    from setuptools import setup, Extension
    from pkg_resources import parse_version, get_distribution

    if get_distribution('setuptools').parsed_version < parse_version('0.7'):
        # setuptools is too old (before merge with distribute)
        raise ImportError

    from setuptools.command.build_ext import build_ext as _build_ext
    from setuptools.command.install import install as _install
    use_setuptools = True

except ImportError:
    # Use distutils instead
    from distutils.core import setup, Extension
    from distutils.command.build_ext import build_ext as _build_ext
    from distutils.command.install import install as _install
    use_setuptools = False

import os
import fnmatch
import platform
import subprocess
from distutils.sysconfig import customize_compiler

# monkey-patch distutils for parallel compilation
def parallelCCompile(self, sources, output_dir=None, macros=None,
                     include_dirs=None, debug=0, extra_preargs=None,
                     extra_postargs=None, depends=None):
    macros, objects, extra_postargs, pp_opts, build = self._setup_compile(
        output_dir, macros, include_dirs, sources, depends, extra_postargs)
    cc_args = self._get_cc_args(pp_opts, debug, extra_preargs)
    import multiprocessing
    import multiprocessing.pool
    N = multiprocessing.cpu_count()
    def _single_compile(obj):
        try:
            src, ext = build[obj]
        except KeyError:
            return
        self._compile(obj, src, ext, cc_args, extra_postargs, pp_opts)
    # convert to list, imap is evaluated on-demand
    pool = multiprocessing.pool.ThreadPool(N)
    try:
        list(pool.imap(_single_compile, objects))
    except:
        pool.close()
        pool.join()
        raise
    return objects

import distutils.ccompiler
distutils.ccompiler.CCompiler.compile = parallelCCompile

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
        recursive_glob('numpythia/src/hepmc3.0.0/src', '*.cc') +
        recursive_glob('numpythia/src/pythia8226/src', '*.cc'),
    depends=[],
    language='c++',
    include_dirs=[
        'numpythia/src',
        'numpythia/src/hepmc3.0.0/include',
        'numpythia/src/pythia8226/include',
    ],
    extra_compile_args=[
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
        # Remove the "-Wstrict-prototypes" compiler option, which isn't valid
        # for C++.
        customize_compiler(self.compiler)
        try:
            self.compiler.compiler_so.remove('-Wstrict-prototypes')
        except (AttributeError, ValueError):
            pass
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


# Only add numpy to *_requires lists if not already installed to prevent
# pip from trying to upgrade an existing numpy and failing.
try:
    import numpy
except ImportError:
    build_requires = ['numpy']
else:
    build_requires = []

if use_setuptools:
    setuptools_options = dict(
        setup_requires=build_requires,
        install_requires=build_requires,
        extras_require={
            'with-numpy': ('numpy',),
        },
        zip_safe=False,
    )
else:
    setuptools_options = dict()

setup(
    name='numpythia',
    version='0.1.0',
    description='The interface between PYTHIA and NumPy',
    long_description=''.join(open('README.rst').readlines()),
    maintainer='Noel Dawe',
    maintainer_email='noel@dawe.me',
    license='GPLv3',
    url='http://github.com/ndawe/numpythia',
    packages=[
        'numpythia',
        'numpythia.extern',
        'numpythia.tests',
        'numpythia.testcmnd',
    ],
    package_data={
        'numpythia': [
            'testcmnd/*.cmnd',
            'src/pythia8226/share/*',
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
        'License :: OSI Approved :: BSD License',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
        'Programming Language :: Cython',
        'Development Status :: 3 - Alpha',
    ],
    **setuptools_options
)
