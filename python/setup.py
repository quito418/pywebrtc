import os
import re
import sys

from setuptools import setup, find_packages, Extension

def get_version(header_filepath):    
    with open(header_filepath, 'r') as f:
        headerfile = f.read().strip()

        try:
            version_found = re.search('\s*\#define\s*PYWEBRTC_VERSION\s*(\"(\d+\.)*\d+"?)[ \t]*$',
                                      headerfile, re.MULTILINE).group(1)
            return version_found
        except:
            raise Exception("Could not file definition of 'PYWEBRTC_VERSION' in header file: {}".format(header_filepath))


    
VERSION = get_version('../src/common.hh')

module = Extension('pywebrtc._ext.pywebrtc',
                   sources=['pywebrtc/src/pywebrtc.cc'],
                   define_macros=[],
                   include_dirs=['pywebrtc'],
                   library_dirs=[],
                   libraries=[],
                   extra_compile_args=['-std=c++14', '-Wall', '-Wextra',
                                       '-D', 'PYWEBRTC_VERSION={}'.format(VERSION)],
                   extra_link_args=[]
                   )

setup(
    name='pywebrtc',
    version=VERSION,
    description='',
    long_description='',
    url='',
    author='John R. Emmons',
    author_email='jemmons@cs.stanford.edu',
    install_requires=[],
    setup_requires=[],
    packages=find_packages(exclude=['build']),
    ext_modules=[module]
)
