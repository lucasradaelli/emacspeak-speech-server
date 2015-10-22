'''
pyibmtts: Python wrapper for IBM TTS runtime

@author: Peter Parente
@organization: IBM Corporation
@copyright: Copyright (c) 2006 IBM Corporation
@license: Berkeley Software Distribution (BSD) License

All rights reserved. This program and the accompanying materials are made 
available under the terms of the Berkeley Software Distribution (BSD) license 
which accompanies this distribution, and is available at
U{http://www.opensource.org/licenses/bsd-license.php}
'''

from distutils.core import setup
from distutils.extension import Extension
from Pyrex.Distutils import build_ext
import sys, os

SRC = ['src/pyibmtts.pyx']
LIBS = ['ibmeci']

if sys.platform.startswith('linux'):
  INC_DIRS = [os.path.join('../'),
              os.path.join('/opt/IBM/ibmtts', 'inc'), 
              os.path.join('/opt/IBM/ibmtts-devel', 'inc')]
  LIB_DIRS = [os.path.join('../'),
              os.path.join('/opt/IBM/ibmtts', 'lib'),
              os.path.join('/opt/IBM/ibmtts-devel', 'lib')]
elif sys.platform == 'win32':
  INC_DIRS = ['C:\\Program Files\\ViaVoice TTS 6.70 SDK\\include']
  LIB_DIRS = ['C:\\Program Files\\ViaVoice TTS 6.70 SDK\\lib']
else:
  raise ValueError('Modify setup.py to support your OS')

ext = Extension('pyibmtts', SRC,
                libraries=LIBS,
                include_dirs=INC_DIRS,
                library_dirs=LIB_DIRS)

description = __doc__.split('\n')[1]

setup(
  name='pyibmtts',
  version='6.7.4',
  author='IBM Corporation',
  maintainer='Peter Parente',
  maintainer_email='pparent@us.ibm.com',
  license='BSD',
  description = description,
  long_description = description,
  url='http://ibmtts-sdk.sf.net',
  download_url='http://www.sf.net/projects/ibmtts-sdk',
  ext_modules=[ext],
  cmdclass = {'build_ext': build_ext}
)
