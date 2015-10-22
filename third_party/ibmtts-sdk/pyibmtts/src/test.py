'''
Simple test case for pyibmtts.

@author: Peter Parente
@organization: IBM Corporation
@copyright: Copyright (c) 2006 IBM Corporation
@license: Berkeley Software Distribution (BSD) License

All rights reserved. This program and the accompanying materials are made 
available under the terms of the Berkeley Software Distribution (BSD) license 
which accompanies this distribution, and is available at
U{http://www.opensource.org/licenses/bsd-license.php}
'''
import pyibmtts

# define a test string to speak
test_string = 'IBM TTS runtime version %s' % pyibmtts.getVersion()

# make a listener class that will receive callbacks
class MyListener(pyibmtts.EngineListener):
  def __init__(self):
    self.last_offset = 0

  def onIndex(self, param):
    print test_string[self.last_offset:param]
    self.last_offset = param
    # return True to indicate we processed this event
    return True

# initialize the engine (you can give it a language constant)
tts = pyibmtts.Engine()
# register our listener
tts.setListener(MyListener())
# set some engine parameters
tts.realWorldUnits = True
tts.synthMode = 1
tts.inputType = 1

# make the second predefined voice the active one
v = tts.getVoice(2)
v = tts.setActiveVoice(v)
# modify the active voice a bit
v.speed = 200
#v.breathiness = 100

# buffer some text to speak along with index markers
i = 0
for word in test_string.split(' '):
  i += len(word)+1
  tts.insertIndex(i)
  tts.addText(word)
  
# synth and wait
tts.synthSync()
