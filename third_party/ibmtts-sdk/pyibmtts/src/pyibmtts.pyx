'''
Pyrex source code for the pyibmtts wrapper.

@todo: PP: EngineFilter is untested
@todo: PP: EngineDictionary with Asian languages is untested

@author: Peter Parente
@organization: IBM Corporation
@copyright: Copyright (c) 2006 IBM Corporation
@license: Berkeley Software Distribution (BSD) License

All rights reserved. This program and the accompanying materials are made 
available under the terms of the Berkeley Software Distribution (BSD) license 
which accompanies this distribution, and is available at
U{http://www.opensource.org/licenses/bsd-license.php}
'''

# include all declarations
include "support.pxi"
include "eci.pxi"
include "constants.pxi"

cdef ECICallbackReturn _eci_callback(ECIHand heci, ECIMessage msg, long lparam,
                                    void* data):
  '''
  Handles callbacks from the speech engine by dispatching them to the 
  EngineListener instance registered with the Engine instance that owns this
  callback. The listener instance is passed using the void* parameter. All
  parameters are converted to appropriate Python data types before notifying
  the listener. The listener may return True to indicate the event was 
  processed, False to indicate it was not processed and should be refired, or
  None to indicate the operation that caused the event to fire should be 
  aborted.
  
  This is turned into a C function and is not exposed via the Python wrapper.
  
  @param heci: Opaque handle to the ECI instance
  @param msg: Message indicating the type of event that occurred
  @param lparam: Parameter containing more information about the event. This
    parameter is a pointer to a ECIMouthData structure when the message is a
    WordIndexReply or PhonemeIndexReply.
  @param data: EventListener instance provided to IBM TTS when the callback
    was set
  '''
  cdef object listener
  cdef object param
  cdef ECIMouthData* md
  
  # cast the void data as a listener
  listener = <object>data
  if msg == eciWordIndexReply or msg == eciPhonemeIndexReply:
    # cast to a ECIMouthData struct when its a word or phoneme index reply
    md = <ECIMouthData *>lparam
    # wrap all struct data in a Python class
    #@todo: PP: what about wsz from the union?
    param = EngineMouthData(phoneme=<char *>md.phoneme.sz,
                      lang=md.eciLanguageDialect, 
                      mouthHeight = md.mouthHeight,
                      mouthWidth = md.mouthWidth, 
                      mouthUpturn = md.mouthUpturn,
                      jawOpen = md.jawOpen,
                      teethUpperVisible = md.teethUpperVisible,
                      teethLowerVisible = md.teethLowerVisible,
                      tonguePosn = md.tonguePosn,
                      lipTension = md.lipTension)
  elif msg == eciStringIndexReply or msg == eciAudioIndexReply:
    # cast to a null terminated string
    param = <char *>lparam
  else:
    # otherwise, treat the lparam as a meaningful number
    param = lparam
  try:
    # try to inform the listener
    rv = listener.onMessage(msg, param)
  except KeyboardInterrupt:
    # abort gracefully if an interrupt occurs during the callback
    return eciDataAbort
  if rv  == True:
    # indicate data processed
    return eciDataProcessed
  elif rv == False:
    # indicate data not processed and the callback should be repeated
    return eciDataNotProcessed
  else:
    # indicate the current operation should be aborted
    return eciDataAbort

class ECIException(Exception): 
  '''Base class for all pyibmtts exceptions.'''
  pass
class ECIDictException(ECIException): 
  '''Exception class for dictionary related exceptions.'''
  pass
class ECIFilterException(ECIException): 
  '''Exception class for filter related exceptions.'''
  pass
      
cdef eciError(ECIHand heci):
  '''
  Raises a Python ECIException containing the message and code associated with
  the last error. Resets the error flags.
  
  This is turned into a C function and is not exposed via the Python wrapper.
  
  @param heci: Opaque handle to the ECI instance
  @raise ECIException: Always
  '''
  # constant array size taken from IBM TTS manual recommendation
  cdef char msg[100]
  cdef int code
  code = eciProgStatus(heci)
  eciErrorMessage(heci, <void *>msg)
  eciClearErrors(heci)
  raise ECIException(code, msg)
  
cdef eciDictError(ECIDictError code):
  '''
  Raises a Python ECIDictException containing an informative message when the
  given code is not DictNoError. Otherwise, returns immediately.
  
  This is turned into a C function and is not exposed via the Python wrapper.  
  
  @param code: Code indicating a dictionary error
  @raise ECIDictException: When the given code is not DictNoError
  '''
  if code == DictNoError:
    return
  elif code == DictNoEntry:
    msg = 'Dictionary is empty or there are no more entries'
  elif code == DictFileNotFound:
    msg = 'Specified file could not be found'
  elif code == DictOutOfMemory:
    msg = 'Ran out of heap space'
  elif code == DictInternalError:
    msg = 'Error occurred in the synthesis engine'
  elif code == DictErrLookUpKey:
    msg = 'Error occurred when looking up the key'
  elif code == DictAccessError:
    msg = 'Error occurred when claiming operating system specific resources'
  elif code == DictInvalidVolume:
    msg = 'Dictionary volume not supported by current language'
  raise ECIDictException(code, msg)
  
cdef eciFilterError(ECIFilterError code):
  '''
  Raises a Python ECIFiltException containing an informative message when the
  given code is not FilterNoError. Otherwise, returns immediately.
  
  This is turned into a C function and is not exposed via the Python wrapper.  
  
  @param code: Code indicating a filter error
  @raise ECIFilterException: When the given code is not FilterNoError
  '''
  if code == FilterNoError:
    return
  elif code == FilterFileNotFound:
    msg = 'Specified file could not be found'
  elif code == FilterOutOfMemory:
    msg = 'Ran out of heap space'
  elif code == FilterInternalError:
    msg = 'Error occurred in the synthesis engine'
  elif code == FilterAccessError:
    msg = 'Error occurred when claiming operating system specific resources'
  raise ECIFilterException(code, msg)
  
def getVersion():
  '''
  Gets the version number for the IBM TTS runtime.
  
  @note: Constant array size taken from IBM TTS manual recommendation
  @return: Version number
  @rtype: string
  '''
  cdef char version[20]
  eciVersion(version)
  return version
  
def getDefaultParam(int param):
  '''
  Gets the the current default value for the given engine parameter.
  
  @param param: Engine parameter value to retrieve
  @return: Value of the given parameter
  @rtype: integer
  '''
  cdef int val
  val = eciGetDefaultParam(<ECIParam>param)
  if val < 0:
    raise ECIException(0, 'Param is out of range')
  return val
  
def setDefaultParam(int param, int value):
  '''
  Sets the the current default value for the given engine parameter.
  
  @param param: Engine parameter to modify
  @param value: New value to set
  @return: Previous value of the given parameter
  @rtype: integer
  '''
  cdef int prev
  prev = eciSetDefaultParam(<ECIParam>param, value)
  if prev < 0:
    raise ECIException(0, 'Param is out of range')
  return prev
  
def speakText(char* text, Boolean annotations=0, lang=None):
  '''
  Speaks the given text. Creates a separate engine instance, uses it to speak
  the text, and then destroys that instance.
  
  @param text: Text to speak
  @param annotations: Does the text contain eci annotations?
  @param lang: Runtime of the language to use
  '''
  if lang is None:
    if not eciSpeakText(text, annotations):
      raise ECIException(0, 'speakText failed')
  else:
    if not eciSpeakTextEx(text, annotations, lang):
      raise ECIException(0, 'speakTextEx failed')

cdef class Engine:
  '''  
  Speech engine. Provides methods for speaking text, configuring engine
  settings, configuring voice settings, configuring output buffers, setting
  callbacks, configuring dictionaries, and configuring filters.
  
  @ivar heci: Opaque handle to the ECI instance
  @ivar listener: Reference to an engine event listener
  @type listener: EngineListener
  @ivar buf: Buffer to store audio data when synthesizing to a buffer
  @ivar buf_size: Size of the waveform buffer
  @ivar ph_buf: Buffer to store phonemes when synthesizing only phonemes
  @ivar ph_buf_size: Size of the phoneme buffer
  @ivar dict: Active dictionary
  @type dict: EngineDictionary
  @ivar filters: List of all active engine filters
  @type filters: list of EngineFilter
  '''
  cdef ECIHand heci
  cdef object listener
  cdef short* buf
  cdef char* ph_buf
  cdef object dict
  cdef object filters
  cdef int buf_size
  cdef int ph_buf_size
  
  def __new__(self, lang=None):
    '''
    Initializes a new speech engine instance. If lang is None, uses the default
    language.
    
    @param lang: Language of the engine to initialize
    @type lang: integer
    '''
    if lang is None:
      self.heci = eciNew()
    else:
      self.heci = eciNewEx(lang)
    if self.heci is NULL:
      raise ECIException(0, 'Unrecoverable error occurred')
    self.listener = None
    self.buf = NULL
    self.buf_size = 0
    self.ph_buf = NULL
    self.ph_buf_size = 0
    self.dict = None
    self.filters = []

  def __dealloc__(self):
    '''
    Deallocates the engine instance, removes all filters, removes the active
    dictionary, and free the output buffer if it's allocated.
    '''
    self.filters = None
    self.dict = None
    if self.buf is not NULL:
      free(self.buf)
    eciDelete(self.heci)
    
  def reset(self):
    '''
    Rests the engine to its default settings.
    
    @raise ECIException: When the engine cannot be reset
    '''
    if not eciReset(self.heci):
      eciError(self.heci)
      
  def testPhrase(self):
    '''
    Speaks a test phrase.
    
    @raise ECIException: When speaking fails
    '''
    if not eciTestPhrase(self.heci):
      eciError(self.heci)

  def addText(self, char* text):
    '''
    Adds text to be spoken in a later call to synthesize.
    
    @param text: Text to buffer for later speech
    @raise ECIException: When text cannot be added
    '''
    if not eciAddText(self.heci, <ECIInputText *>text):
      eciError(self.heci)
      
  def clearInput(self):
    '''
    Clears buffered text.
    
    @raise ECIException: When buffered text cannot be cleared
    '''
    if not eciClearInput(self.heci):
      eciError(self.heci)
    
  def getIndex(self):
    '''   
    @return: Gets the last index encountered
    @rtype: integer    
    '''
    return eciGetIndex(self.heci)
    
  def insertIndex(self, int index):
    '''
    Inserts an index marker.
    
    @param: Inserts an index marker into the buffered text
    @raise ECIException: When a marker cannot be inserted
    '''
    if not eciInsertIndex(self.heci, index):
      eciError(self.heci)
      
  def pause(self, Boolean pause):
    '''
    Pauses or unpauses current speech.
    
    @param: Pause speech (True) or not (False)?
    @raise ECIException: When speech cannot be paused
    '''
    if not eciPause(self.heci, pause):
      eciError(self.heci)
      
  def stop(self):
    '''
    Stops current speech.
    
    @raise ECIException: When speech cannot be stopped
    '''
    if not eciStop(self.heci):
      eciError(self.heci)
           
  def speaking(self):
    '''
    @return: Is the engine currently speaking?
    @rtype: integer
    '''
    return eciSpeaking(self.heci)

  def synthesize(self, filename=None):
    '''
    Triggers the synthesis of buffered text. If a filename is given, speech
    waveform data is synthesized to that file.
    
    @param filename: Filename to which the waveform data is written
    @type filename: string
    @raise ECIException: When synthesis cannot begin
    '''
    cdef char *fn
    if filename is None:
      if not eciSynthesize(self.heci):
        eciError(self.heci)
    else:
      fn = filename
      if not eciSynthesizeFile(self.heci, <void *>fn):
        eciError(self.heci)

  def synchronize(self):
    '''
    Blocks until the current synthesis operation completes.
    
    @raise ECIException: When synchronization cannot occur
    '''
    if not eciSynchronize(self.heci):
      eciError(self.heci)
      
  def synthSync(self, filename=None):
    '''
    Begins synthesis and blocks until it is complete. Convenience method that
    calls synthesize followed by synchronize.
    
    @param filename: Filename to which the waveform data is written
    @type filename: string
    @raise ECIException: When synth or sync cannot occur
    '''
    self.synthesize(filename)
    self.synchronize()
    
  def generatePhonemes(self, int size):
    '''
    Generates buffers of characters representing phonemes. The registered
    listener will be notified on phoneme creation and will have an opportunity
    to read the generated phonemes from the buffer before they are overwritten.
    Blocks until all phonemes in the buffered text are generated.
    
    @param size: Size of the phoneme buffer array
    @raise ECIException: When generation fails
    '''
    self.ph_buf = <char *>malloc(sizeof(char) * size)
    if self.ph_buf is NULL:
      raise MemoryError()
    self.ph_buf_size = size
    if not eciGeneratePhonemes(self.heci, size, self.ph_buf):
      eciError(self.heci)
    free(self.ph_buf)
    
  def getPhonemeBuffer(self, end=None):
    '''
    Gets a copy of the current contents of the phoneme buffer as a string. If 
    end is not None, retrieves the contents of the buffer up to index end.
    
    @param end: Fetch up to this index:
    @type end: integer
    @return: Contents of the buffer
    @rtype: string
    @raise IndexError: When end is greater than the size of the buffer
    '''
    if end is None:
      # snap to the size of the buffer
      end = self.ph_buf_size
    elif end > self.ph_buf_size or end < 0:
      # indicate the end is past the end of the buffer
      raise IndexError
    return PyString_FromStringAndSize(<char *>self.ph_buf, end*sizeof(char))
    
  def setOutputBuffer(self, size=None):
    '''
    Sets an in memory buffer as the destination for synthesis. If size is not
    None, allocates a new buffer of the given size and makes it the target
    for all future synthesis operations. If size is None, deallocates any
    existing buffer and makes the default audio ouput device the target for all
    future synthesis operations.
    
    The registered listener will be notified with end indicies during synthesis.
    The listener should call getOutputBuffer during these callbacks to copy the
    contents of the buffer to a new location. After such a notification 
    completes, the data in the buffer will be overwritten.
    
    @param size: Size of the buffer to allocate or None to deallocate
    @type size: integer
    @raise ECIException: When setting the output buffer fails
    @raise MemoryError: When the buffer could not be allocated
    '''
    if size is None:
      # stop using the buffer
      if not eciSetOutputBuffer(self.heci, 0, NULL):
        eciError(self.heci)
      if self.buf is not NULL:
        free(self.buf)
        self.buf = NULL
        self.buf_size = 0
    else:
      self.buf = <short *>malloc(sizeof(short) * size)
      if self.buf is NULL:
        # watch for out of memory errors
        raise MemoryError()
      if not eciSetOutputBuffer(self.heci, size, self.buf):
        # deallocate the buffer if the engine refuses to use it
        free(self.buf)
        self.buf = NULL
        self.buf_size = 0
        eciError(self.heci)
      self.buf_size = size
        
  def getOutputBuffer(self, end=None):
    '''
    Gets the contents of the current output buffer. If end is not None, gets 
    the contents up to the index specified by end. If end is None, gets the
    content of the entire buffer.
       
    The contents of the buffer are 16-bit, one channel, signed PCM samples. 
    These contents are copied to a Python string and returned by this method. 
    The caller can use the Python array module to treat the returned string as
    a read/write array of 16-bit chunks if manipulation of the waveform data
    is necessary.
    
    @param end: Fetch buffer content up to but not including this index or fetch
      the whole buffer if None
    @type end: integer
    @return: Copy of the output buffer contents
    @rtype: string
    @raise IndexError: When end is greater than the size of the buffer
    '''
    if end is None:
      end = msize
    elif end > self.buf_size or end < 0:
      raise IndexError
    return PyString_FromStringAndSize(<char *>self.buf, end*sizeof(short))
      
  def setOutputDevice(self, int device):
    '''
    Sets the output device specified by the given number as the synthesis 
    destination.
    
    @param device: Device number
    @raise ECIException: When setting the output device fails
    '''
    if not eciSetOutputDevice(self.heci, device):
      eciError(self.heci)
      
  def setOutputFilename(self, char* filename):
    '''
    Sets the give filename as the synthesis destination.
    
    @param filename: Name of the file
    @raise ECIException: When setting the output file fails
    '''
    if not eciSetOutputFilename(self.heci, <void *>filename):
      eciError(self.heci)
   
  def getVoice(self, int number=0):
    '''
    Gets the voice stored in the slot with the given number. Defaults to getting
    the active voice (i.e. slot number 0).
    
    @param number: Slot number in which the desired voice is stored
    @return: Voice stored in the given slot
    @rtype: EngineVoice
    '''
    return EngineVoice(<int>&self.heci, number)
       
  def setActiveVoice(self, voice):
    '''
    Sets the active voice to the one provided. Returns a copy of this voice 
    with a slot number of zero indicating it is the active voice.
    
    @param voice: Voice to make active
    @type voice: EngineVoice
    @raise ECIException: When setting the active voice fails
    '''
    return voice.copy(0)
    
  def newDict(self):
    '''
    Creates a new dictionary for use in key-value replacement of text sent for
    synthesis. Does not automatically set the new dictionary as the active one.
    
    @return: The newly created dictionary object
    @rtype: EngineDictionary
    @raise ECIException: When creating or setting the new dictionary fails
    '''
    cdef ECIDictHand hdict
    hdict = eciNewDict(self.heci)
    if hdict == NULL:
      eciError(self.heci)
    d = EngineDictionary(<int>&self.heci, <int>&hdict)
    return d
    
  def setDict(self, d):
    '''
    Sets the given dictionary as the active dictionary.
    
    @param d: The dictionary to make active
    @type d: EngineDictionary
    @raise ECIException: When setting the given dictionary fails
    '''
    cdef ECIDictError rv
    if self.dict is None:
      rv = eciSetDict(self.heci, NULL)
      eciDictError(rv)
    else:
      d._activate()
    self.dict = d
  
  def getDict(self):
    '''
    @return: The active dictionary
    @rtype: EngineDictionary
    '''
    return self.dict
    
  def newFilter(self, int is_global):
    '''
    Creates a new filter for transforming text according to rules defined in the
    filter extension. Does not automatically add the new filter to the list of
    active engine filters.
    
    @param is_global: Is the filter global?
    @return: The new filter object
    @rtype: EngineFilter
    @raise ECIException: When creating the new filter fails
    '''
    cdef ECIFilterHand hfilt
    hfilt = eciNewFilter(self.heci, 0, is_global)
    if hfilt == NULL:
      eciError(self.heci)
    return Filter(<int>&self.heci, <int>&hfilt)
    
  def addFilter(self, filt):
    '''
    Activates the given filter and adds it to the list of active engine filters.
    
    @param filt: Filter to add
    @type filt: EngineFilter
    @raise ECIFilterException: When activating the new filter fails
    '''
    filt._activate()
    self.filters.append(filt)
    
  def removeFilter(self, filt):
    '''
    Removes the given filter from the list of active engine filters and 
    deactivates it.
    
    @param filt: Filter to remove
    @type filt: EngineFilter
    @raise ECIFilterException: When deactivating the new filter fails
    '''
    self.filters.remove(filt)
    filt._deactivate()
      
  def setListener(self, listener):
    '''
    Registers a callback for engine events and sets the given listener as the
    observer. If listener is None, ceases all callbacks.
    
    @param listener: Listener to be notified on engine events
    @type listener: EngineListener
    '''
    if listener is None:
      eciRegisterCallback(self.heci, NULL, NULL)
    elif self.listener is None:
      eciRegisterCallback(self.heci, _eci_callback, <void *>listener)
    self.listener = listener
    
  def getParam(self, int param):
    '''
    Gets the the current value for the given engine parameter. 

    For convenience, values of current engine parameters can be gotten using the
    properties defined on this engine instance instead of via this method.
    
    @param param: Engine parameter value to retrieve
    @return: Value of the given parameter
    @rtype: integer
    @raise ECIException: When getting the parameter fails
    '''
    cdef int val
    val = eciGetParam(self.heci, <ECIParam>param)
    if val < 0:
      eciError(self.heci)
    return val
  
  def setParam(self, int param, int val):
    '''
    Sets the the current value for the given engine parameter. 

    For convenience, values of current engine parameters can be set using the
    properties defined on this engine instance instead of via this method.
    
    @param param: Engine parameter to modify
    @param val: New value to set
    @return: Previous value of the given parameter
    @rtype: integer
    @raise ECIException: When setting the parameter fails
    '''
    cdef int prev
    prev = eciSetParam(self.heci, <ECIParam>param, val)
    if prev < -1:
      eciError(self.heci)
    return prev
    
  property synthMode:
    '''
    Gets/sets the synthesis mode.
    '''
    def __get__(self):
      return self.getParam(eciSynthMode)
      
    def __set__(self, val):
       self.setParam(eciSynthMode, val)
       
  property inputType:
    '''
    Gets/sets the input type.
    '''
    def __get__(self):
      return self.getParam(eciInputType)
      
    def __set__(self, val):
       self.setParam(eciInputType, val)
       
  property textMode:
    '''
    Gets/sets the text mode.
    '''
    def __get__(self):
      return self.getParam(eciTextMode)
      
    def __set__(self, val):
       self.setParam(eciTextMode, val)
       
  property dictionary:
    '''
    Gets/sets if a dictionary should be used.
    '''
    def __get__(self):
      return self.getParam(eciDictionary)
      
    def __set__(self, val):
       self.setParam(eciDictionary, val)

  property sampleRate:
    '''
    Gets/sets the sample rate.
    '''
    def __get__(self):
      return self.getParam(eciSampleRate)
      
    def __set__(self, val):
       self.setParam(eciSampleRate, val)
       
  property wantPhonemeIndices:
    '''
    Gets/sets if phoneme index callbacks should occur.
    '''
    def __get__(self):
      return self.getParam(eciWantPhonemeIndices)
      
    def __set__(self, val):
       self.setParam(eciWantPhonemeIndices, val)

  property wantWordIndices:
    '''
    Gets/sets if phoneme index callbacks should occur.
    
    @note: Word index callbacks only happen for multibyte character sets but
    not single byte character sets. (Why?!)
    '''
    def __get__(self):
      return self.getParam(eciWantWordIndex)
      
    def __set__(self, val):
       self.setParam(eciWantWordIndex, val)
       
  property realWorldUnits:
    '''
    Gets/sets if real world units should be used instead of engine units.
    '''
    def __get__(self):
      return self.getParam(eciRealWorldUnits)
      
    def __set__(self, val):
       self.setParam(eciRealWorldUnits, val)

  property languageDialect:
    '''
    Gets/sets the language dialect to use.
    '''
    def __get__(self):
      return self.getParam(eciLanguageDialect)
      
    def __set__(self, val):
       self.setParam(eciLanguageDialect, val)
       
  property numberMode:
    '''
    Gets/sets the number mode.
    '''
    def __get__(self):
      return self.getParam(eciNumberMode)
      
    def __set__(self, val):
       self.setParam(eciNumberMode, val)
       
  property numDeviceBlocks:
    '''
    Gets/sets the number of output device blocks.
    '''
    def __get__(self):
      return self.getParam(eciNumDeviceBlocks)
      
    def __set__(self, val):
       self.setParam(eciNumDeviceBlocks, val)

  property sizeDeviceBlocks:
    '''
    Gets/sets the size of the output device blocks.
    '''
    def __get__(self):
      return self.getParam(eciSizeDeviceBlocks)
      
    def __set__(self, val):
       self.setParam(eciSizeDeviceBlocks, val)
       
  property numPrerollDeviceBlocks:
    '''
    Gets/sets the number of preroll output device blocks.
    '''
    def __get__(self):
      return self.getParam(eciNumPrerollDeviceBlocks)
      
    def __set__(self, val):
       self.setParam(eciNumPrerollDeviceBlocks, val)
       
  property sizePrerollDeviceBlocks:
    '''
    Gets/sets the size of the preroll output device blocks.
    '''
    def __get__(self):
      return self.getParam(eciSizePrerollDeviceBlocks)
      
    def __set__(self, val):
       self.setParam(eciSizePrerollDeviceBlocks, val)

cdef class EngineVoice:
  '''
  Voice object. Provides methods and convenience properties for manipulating 
  voice parameters and copying voices.
  
  This class should never be instantiated directly. Use Engine.getVoice.
  
  @ivar num: Slot number in which this voice is stored by the engine
  @ivar heci: Opaque handle to the ECI instance
  '''
  cdef int num
  cdef ECIHand heci
    
  def __new__(self, int heci, int num):
    '''
    Initializes a new voice object by storing a reference to the engine instance
    and the slot number in which this voice resides.
    
    @param heci: Opaque handle to the ECI instance
    @param num: Slot number in which this voice is stored by the engine
    '''
    self.heci = (<ECIHand **>heci)[0]
    self.num = num
    
  def copy(self, int dest):
    '''
    Copies this voice to another voice slot by making a new voice object with
    the destination slot number with the same parameters as this voice object.
    
    Copying to slot zero makes this voice the active voice.
    
    @param dest: Destination slot number
    @return: New voice object at the given slot
    @rtype: EngineVoice 
    @raise ECIException: When copying the voice fails
    '''
    if self.num == dest:
      # don't set the voice to itself else we get an ignored eci command 
      # announcement at the beginning of the next speech stream
      return self
    if not eciCopyVoice(self.heci, self.num, dest):
      raise eciError(self.heci)
    return EngineVoice(<int>&self.heci, dest)
    
  def getParam(self, int param):
    '''
    Gets the the current value for the given voice parameter. 

    For convenience, values of current voice parameters can be gotten using the
    properties defined on this voice instance instead of via this method.
    
    @param param: Voice parameter value to retrieve
    @return: Value of the given parameter
    @rtype: integer
    @raise ECIException: When getting the parameter fails
    '''
    cdef int val
    val = eciGetVoiceParam(self.heci, self.num, <ECIVoiceParam>param)
    if val < 0 :
      eciError(self.heci)
    return val
    
  def setParam(self, int param, int val):
    '''
    Sets the the current value for the given voice parameter. 

    For convenience, values of current voice parameters can be set using the
    properties defined on this voice instance instead of via this method.
    
    @param param: Voice parameter to modify
    @param val: New value to set
    @return: Previous value of the given parameter
    @rtype: integer
    @raise ECIException: When setting the parameter fails
    '''
    cdef int prev
    prev = eciSetVoiceParam(self.heci, self.num, <ECIVoiceParam>param, val)
    if prev < 0:
      eciError(self.heci)
    return prev
      
  property name:
    '''
    Gets/sets the voice name.
    '''
    def __get__(self):
      cdef char name[ECI_VOICE_NAME_LENGTH + 1]
      if not eciGetVoiceName(self.heci, self.num, <void *>name):
        eciError(self.heci)
      return name
      
    def __set__(self, char* name):
      if not eciSetVoiceName(self.heci, self.num, name):
        eciError(self.heci)
        
  property gender:
    '''
    Gets/sets the voice gender.
    '''
    def __get__(self):
      return self.getParam(eciGender)
      
    def __set__(self, int val):
      self.setParam(eciGender, val)
      
  property headSize:
    '''
    Gets/sets the voice headsize.
    '''
    def __get__(self):
      return self.getParam(eciHeadSize)
      
    def __set__(self, int val):
      self.setParam(eciHeadSize, val)

  property pitchBaseline:
    '''
    Gets/sets the pitch baseline.
    '''
    def __get__(self):
      return self.getParam(eciPitchBaseline)
      
    def __set__(self, int val):
      self.setParam(eciPitchBaseline, val)

  property pitchFluctuation:
    '''
    Gets/sets the pitch fluctuation.
    '''
    def __get__(self):
      return self.getParam(eciFluctuation)
      
    def __set__(self, int val):
      self.setParam(eciFluctuation, val)

  property roughness:
    '''
    Gets/sets the voice roughness.
    '''
    def __get__(self):
      return self.getParam(eciRoughness)
      
    def __set__(self, int val):
      self.setParam(eciRoughness, val)

  property breathiness:
    '''
    Gets/sets the voice breathiness.
    '''
    def __get__(self):
      return self.getParam(eciBreathiness)
      
    def __set__(self, int val):
      self.setParam(eciBreathiness, val)

  property speed:
    '''
    Gets/sets the voice speed.
    '''
    def __get__(self):
      return self.getParam(eciSpeed)
      
    def __set__(self, int val):
      self.setParam(eciSpeed, val)

  property volume:
    '''
    Gets/sets the voice volume.
    '''
    def __get__(self):
      return self.getParam(eciVolume)
      
    def __set__(self, int val):
      self.setParam(eciVolume, val)

cdef class EngineDictionary:
  '''
  Dictionary object. Has methods for defining text replacement rules based on
  target/replacement pairs, iterating over defined pairs, loading/saving 
  pairs from/to disk, and removing pairs.
  
  A dictionary has multiple parts or sections. The constants MainDict, RootDict,
  AbbvDict, MainDictExt (for Asian languages) indicate these sections.
  
  @ivar heci: Opaque handle to the ECI instance
  @ivar hdict: Opaque handle to the ECI dictionary instance
  '''
  cdef ECIDictHand hdict
  cdef ECIHand heci
  
  def __new__(self, int heci, int hdict):
    '''
    Initializes a new voice object by storing references to the engine instance
    and the dictionary instance
    
    @param heci: Opaque handle to the ECI instance
    @param hdict: Opaque handle to the ECI dictionary instance
    '''
    self.heci = (<ECIHand **>heci)[0]
    self.hdict = (<ECIDictHand **>hdict)[0]
    
  def __dealloc__(self):
    '''
    Deallocates the dictionary by deleting the ECI dictionary instance.
    '''
    eciDeleteDict(self.heci, self.hdict)
      
  def _activate(self):
    '''
    Activates this dictionary. Should never be called directly. Use 
    Engine.setDict instead.
    
    @raise ECIDictException: When the dictionary could not be activated
    '''
    cdef ECIDictError rv
    rv = eciSetDict(self.heci, self.hdict)
    eciDictError(rv)
    
  def lookup(self, char* key, int which=MainDict, int asian=False):
    '''
    Look up the given text key in the given part of the dictionary. Get the
    replacement text for the key and the part of speech if the dictionary is for
    and Asian language.
    
    @param key: Target text to hash
    @param which: Which part of the dictionary to hash into
    @param asian: Is the dictionary for an asian language or not?
    @return: Tuple with the replacement value and its part of speech if asian
      is True or None if it is False; None is returned if the target is not 
      found
    @rtype: 2-tuple of string, integer
    @raise ECIDictException: When an Asian language lookup fails
    '''
    cdef char* val
    cdef ECIInputText aval[1]
    cdef ECIPartOfSpeech pos[1]
    cdef ECIDictError rv
    if not asian:
      # this method signature is odd, but correct
      val = eciDictLookup(self.heci, self.hdict, <ECIDictVolume>which, 
                          <ECIInputText>key)
      if val is NULL:
        return None
      else:
        return (val, None)
    else:
      rv = eciDictLookupA(self.heci, self.hdict, <ECIDictVolume>which, 
                          <ECIInputText>key, <ECIInputText *>aval, pos)
      eciDictError(rv)
      return (<char *>aval[0], <int>pos[0])
      
  def update(self, char* key, char* val, int which=MainDict, pos=None):
    '''
    Updates an entry in the dictionary. The key specifies the target to update
    and the value specifies the new replacement text. The remaining to 
    parameters specify into which part of the dictionary the key should be 
    inserted and what part of speech the key is if it is in an Asian language.

    If the key is not already in the dictionary, a new pair is created.
    
    @param key: Target text to hash
    @param val: Replacement text to store under the key
    @param which: Which part of the dictionary to hash into
    @param asian: Is the dictionary for an asian language or not?
    @raise ECIDictException: When the update fails
    '''
    cdef ECIDictError rv
    if pos is None:
      rv = eciUpdateDict(self.heci, self.hdict, which, <ECIInputText>key,
                         <ECIInputText>val)
    else:
      rv = eciUpdateDictA(self.heci, self.hdict, which, <ECIInputText>key,
                          <ECIInputText>val, <ECIPartOfSpeech>pos)
    eciDictError(rv)

  def findFirst(self, int which=MainDict, int asian=False):
    '''
    Locates the first entry in the given part of the dictionary.
    
    @param which: Which part of the dictionary to search
    @param asian: Is the dictionary for an asian language or not?
    @return: Target text, replacement text, and part of speech (for Asian langs
      only) or None (for non-Asian langs)
    @rtype: 3-tuple of string, string, integer/None
    @raise ECIDictException: When the lookup fails
    '''
    # need to allocate space for the actual pointer; weird but correct
    cdef ECIInputText val[1]
    cdef ECIInputText key[1]
    cdef ECIPartOfSpeech pos[1]
    cdef ECIDictError rv
    if not asian:
      rv = eciDictFindFirst(self.heci, self.hdict, <ECIDictVolume>which, 
                            key, val)
      eciDictError(rv)
      return (<char *>key[0], <char *>val[0], None)
    else:
      rv = eciDictFindFirstA(self.heci, self.hdict, <ECIDictVolume>which, 
                             key, val, pos)
      eciDictError(rv)
      return (<char *>key[0], <char *>val[0], <int>pos[0])
    
  def findNext(self, int which=MainDict, int asian=False):
    '''
    Locates the next entry in the given part of the dictionary.
    
    @param which: Which part of the dictionary to search
    @param asian: Is the dictionary for an asian language or not?
    @return: Target text, replacement text, and part of speech (for Asian langs
      only) or None (for non-Asian langs)
    @rtype: 3-tuple of string, string, integer/None
    @raise ECIDictException: When the lookup fails
    '''
    # need to allocate space for the actual pointer; weird but correct
    cdef ECIInputText val[1]
    cdef ECIInputText key[1]
    cdef ECIPartOfSpeech pos[1]
    cdef ECIDictError rv
    if not asian:
      rv = eciDictFindNext(self.heci, self.hdict, <ECIDictVolume>which, 
                           key, val)
      eciDictError(rv)
      return (<char *>key[0], <char *>val[0], None)
    else:
      rv = eciDictFindNextA(self.heci, self.hdict, <ECIDictVolume>which, 
                            key, val, pos)
      eciDictError(rv)
      return (<char *>key[0], <char *>val[0], <int>pos[0])
    
  def load(self, char* filename, int which=MainDict):
    '''
    Loads a part of a dictionary from disk.
    
    @param filename: Name of the file containing the dictionary portion
    @param which: Which part of the dictionary to load
    @raise ECIDictException: When the dictionary could not be loaded
    '''
    cdef ECIDictError rv
    rv = eciLoadDict(self.heci, self.hdict, which, <void *>filename)
    eciDictError(rv)
    
  def save(self, char* filename, int which=MainDict):
    '''
    Saves a part of a dictionary to disk.
    
    @param filename: Name of the file containing the dictionary portion
    @param which: Which part of the dictionary to save
    @raise ECIDictException: When the dictionary could not be saved
    '''
    cdef ECIDictError rv
    rv = eciSaveDict(self.heci, self.hdict, which, <void *>filename)
    eciDictError(rv)
    
cdef class EngineFilter:
  '''
  Filter object. Loads a dynamic module from disk capable of doing complex text
  processing and replacement.
  
  @ivar heci: Opaque handle to the ECI instance
  @ivar hdict: Opaque handle to the ECI filter instance
  '''
  cdef ECIHand heci
  cdef ECIFilterHand hfilt

  def __new__(self, int heci, int hfilt):
    '''
    Initializes a new voice object by storing references to the engine instance
    and the filter instance.
    
    @param heci: Opaque handle to the ECI instance
    @param hfilt: Opaque handle to the ECI filter instance
    '''
    self.heci = (<ECIHand **>heci)[0]
    self.hfilt = (<ECIFilterHand **>hfilt)[0]
    
  def __dealloc__(self):
    '''
    Deallocates the filter by deleting the ECI dictionary instance.
    '''
    eciDeleteFilter(self.heci, self.hfilt)
    
  def _activate(self):
    '''
    Activates this filter. Should never be called directly. Use 
    Engine.addFilter instead.
    
    @raise ECIFilterException: When the filter could not be activated
    '''
    cdef ECIFilterError rv
    rv = eciActivateFilter(self.heci, self.hfilt)
    eciFilterError(rv)
    
  def _deactivate(self):
    '''
    Deactivates this filter. Should never be called directly. Use 
    Engine.removeFilter instead.
    
    @raise ECIFilterException: When the filter could not be activated
    '''
    cdef ECIFilterError rv
    rv = eciDeactivateFilter(self.heci, self.hfilt)
    eciFilterError(rv)
    
  def getFilteredText(self, char* text):
    '''
    Gets the result of filter the given text using this filter.
    
    @param text: Text to filter
    @return: Filtered text
    @rtype: string
    @raise ECIFilterException: When text filtering could not be performed
    '''
    cdef ECIInputText out[1]
    cdef ECIFilterError rv
    rv = eciGetFilteredText(self.heci, self.hfilt, <ECIInputText>text, out)
    eciFilterError(rv)
    return <char *>out[0]
    
  def update(self, char* key, char* translation):
    '''
    Updates the text translation for the given text key.
    
    @param key: Target text
    @param translation: Translation text
    @raise ECIFilterException: When the update could not be performed
    '''
    cdef ECIFilterError rv
    rv = eciUpdateFilter(self.heci, self.hfilt, <ECIInputText>key,
                         <ECIInputText>translation)
    eciFilterError(rv)

class EngineMouthData:
  '''
  Python class used to store mouth data. Acts like a struct. The fields of the
  struct are defined in the IBM TTS documentation or the eci.h header file.
  '''
  def __init__(self, **kwargs):
    '''
    Stores whatever parameters are provided as instance variables of the same
    name.
    '''
    self.__dict__.update(kwargs)
      
class EngineListener:
  '''
  Convenience class for defining engine callback listeners. Derive from this
  class and override any desired methods.
  '''
  def onMessage(self, msg, param):
    '''
    Dispatches the given message and its associated parameter to the method
    defined in this class specifically intended to handle it.
    
    @param msg: Message type
    @type msg: integer
    @param param: Data associated with the message
    @type param: integer, string, or EngineMouthData
    @return: True to indicate the message was processed, False to indicate the
      message was not processed and should be resent, or None to indicate the
      operation that generated the message should be aborted
    @rtype: boolean or None
    '''
    # purposely not using a dictionary dispatch here to prevent cyclic 
    # references caused by hashing instance methods
    if msg == eciWaveformBuffer:
      return self.onWaveformBuffer(param)
    elif msg == eciPhonemeBuffer:
      return self.onPhonemeBuffer(param)
    elif msg == eciIndexReply:
      return self.onIndex(param)
    elif msg == eciPhonemeIndexReply:
      return self.onPhonemeIndex(param)
    elif msg == eciWordIndexReply:
      return self.onWordIndex(param)
    elif msg == eciStringIndexReply:
      return self.onStringIndex(param)
    elif msg == eciAudioIndexReply:
      return self.onAudioIndex(param)
    elif msg == eciSynthesisBreak:
      return self.onSynthesisBreak(param)
    else:
      return self.onUnknown(msg, param)
    
  def onWaveformBuffer(self, param):
    '''
    Handles a message indicating new data is available in the output buffer.
    
    Return values are the same as those for onMessage. Defaults to True.
    
    @param param: Last index filled in the output buffer
    @type param: integer
    '''
    return True
    
  def onPhonemeBuffer(self, param):
    '''
    Handles a message indicating new data is available in the phoneme buffer.
    
    Return values are the same as those for onMessage.  Defaults to True.
    
    @param param: Last index filled in the phoneme buffer
    @type param: integer
    '''
    return True
    
  def onIndex(self, param):
    '''
    Handles a message indicating an index marker has been reached.
    
    Return values are the same as those for onMessage. Defaults to True.
    
    @param param: Value of the index marker reached
    @type param: integer
    '''
    return True
    
  def onWordIndex(self, param):
    '''
    Handles a message indicating a word is about to be spoken. (?)
    
    Return values are the same as those for onMessage. Defaults to True.
    
    @param param: Characteristics of the speaker's mouth at this point
    @type param: EngineMouthData
    '''
    return True
    
  def onStringIndex(self, param):
    '''
    Handles a message indicating a SSML <mark> tag has been encountered.
    
    Return values are the same as those for onMessage. Defaults to True.
    
    @param param: String name of the mark element
    @type param: string
    '''
    return True
    
  def onAudioIndex(self, param):
    '''
    Handles a message indicating a SSML <audio> tag has been encountered.
    
    Return values are the same as those for onMessage. Defaults to True.
    
    @param param: String source of the mark element
    @type param: string
    '''
    return True
    
  def onSynthesisBreak(self, param):
    '''
    Handles a message indicating a break in concatenative synthesis has 
    occurred. Only generated when using a concatenative voice and an ouput 
    buffer.
    
    Return values are the same as those for onMessage. Defaults to True.
    
    @param param: Integer 3 means an intonational break, 4 means a sentence 
      break
    @type param: string
    '''
    return True

  def onUnknown(self, msg, param):
    '''
    Handles an unknown message. The msg and param values are of undefined type.
    
    Return values are the same as those for onMessage. Defaults to True.
    '''
    return True

cdef class SynthBuffer:
  '''
  Convenience class for capturing all events and waveform data from a synthesize
  call into buffers. An instance of this class is returned by 
  Engine.synthBuffer.
  '''
  cdef object waveform
  cdef int samps
  cdef object evts
  cdef object engine
  
  def __new__(self, engine):
    self.engine = engine
    self.samps = 0
    self.waveform = []
    self.evts = []
    
  def reset(self):
    self.waveform = []
    self.evts = []
    self.samps = 0
       
  property samples:
    def __get__(self):
      return self.samps

  property pcm:
    def __get__(self):
      return ''.join(self.waveform)

  property events:
    def __get__(self):
      return self.evts

  def onMessage(self, msg, param):
    '''
    Buffers the waveform data or parameter event.
    
    @param msg: Message type
    @type msg: integer
    @param param: Data associated with the message
    @type param: integer, string, or EngineMouthData
    @return: True to indicate the message was processed
    @rtype: boolean
    '''
    if msg == eciWaveformBuffer:
      self.samps = self.samps + param
      self.waveform.append(self.engine.getOutputBuffer(param))
    else:
      self.evts.append((self.samps, msg, param))
    return True
