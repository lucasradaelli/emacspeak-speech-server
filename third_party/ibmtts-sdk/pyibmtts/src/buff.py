'''
Buffered synthesis test case.

@author: Peter Parente
@organization: IBM Corporation
@copyright: Copyright (c) 2006 IBM Corporation
@license: Berkeley Software Distribution (BSD) License

All rights reserved. This program and the accompanying materials are made 
available under the terms of the Berkeley Software Distribution (BSD) license 
which accompanies this distribution, and is available at
U{http://www.opensource.org/licenses/bsd-license.php}
'''
from pyibmtts import *

tts = Engine()
b = SynthBuffer(tts)
tts.setListener(b)
tts.setOutputBuffer(2**16)
tts.addText('This is a test.')
tts.insertIndex(10)
tts.addText('Sally sells sea shells by the seashore.')
tts.synthSync()
print b.samples
print b.events
