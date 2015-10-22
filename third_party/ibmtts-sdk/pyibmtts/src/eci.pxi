'''
Declares all structs, enumerations, typedefs, and methods found in eci.h for
use in Pyrex.

@author: Peter Parente
@organization: IBM Corporation
@copyright: Copyright (c) 2006 IBM Corporation
@license: Berkeley Software Distribution (BSD) License

All rights reserved. This program and the accompanying materials are made 
available under the terms of the Berkeley Software Distribution (BSD) license 
which accompanies this distribution, and is available at
U{http://www.opensource.org/licenses/bsd-license.php}
'''

cdef extern from "eci.h":
  ctypedef void* ECIHand
  ctypedef void* ECIFilterHand
  ctypedef void* ECIInputText
  ctypedef void* ECIDictHand
  ctypedef int Boolean
  
  enum:
    ECI_PRESET_VOICES = 8
    ECI_USER_DEFINED_VOICES = 8
    ECI_VOICE_NAME_LENGTH = 30
    eciPhonemeLength = 4
    
  enum ECIError:
    ECI_NOERROR = 0x00000000
    ECI_SYSTEMERROR = 0x00000001
    ECI_MEMORYERROR = 0x00000002
    ECI_MODULELOADERROR = 0x00000004
    ECI_DELTAERROR = 0x00000008
    ECI_SYNTHERROR = 0x00000010
    ECI_DEVICEERROR = 0x00000020
    ECI_DICTERROR = 0x00000040
    ECI_PARAMETERERROR = 0x00000080
    ECI_SYNTHESIZINGERROR = 0x00000100
    ECI_DEVICEBUSY = 0x00000200
    ECI_SYNTHESISPAUSED = 0x00000400
    ECI_REENTRANTCALL = 0x00000800
    ECI_ROMANIZERERROR = 0x00001000
    ECI_SYNTHESIZING = 0x00002000

  enum ECIParam:
    eciSynthMode
    eciInputType
    eciTextMode
    eciDictionary
    eciSampleRate = 5
    eciWantPhonemeIndices = 7
    eciRealWorldUnits
    eciLanguageDialect
    eciNumberMode
    eciWantWordIndex = 12
    eciNumDeviceBlocks
    eciSizeDeviceBlocks
    eciNumPrerollDeviceBlocks
    eciSizePrerollDeviceBlocks
    eciNumParams
    
  enum ECIVoiceParam:
    eciGender
    eciHeadSize
    eciPitchBaseline
    eciPitchFluctuation
    eciRoughness
    eciBreathiness
    eciSpeed
    eciVolume
    eciNumVoiceParams

  enum ECIDictError:
    DictNoError
    DictFileNotFound
    DictOutOfMemory
    DictInternalError
    DictNoEntry
    DictErrLookUpKey
    DictAccessError
    DictInvalidVolume

  enum ECIVoiceError:
    VoiceNoError
    VoiceSystemError
    VoiceNotRegisteredError
    VoiceInvalidFileFormatError
    
  enum ECIDictVolume:
    eciMainDict = 0
    eciRootDict = 1
    eciAbbvDict = 2
    eciMainDictExt = 3
    
  enum ECILanguageDialect:
    NODEFINEDCODESET                = 0x00000000
    eciGeneralAmericanEnglish       = 0x00010000
    eciBritishEnglish               = 0x00010001
    eciCastilianSpanish             = 0x00020000
    eciMexicanSpanish               = 0x00020001
    eciStandardFrench               = 0x00030000
    eciCanadianFrench               = 0x00030001
    eciStandardGerman               = 0x00040000
    eciStandardItalian              = 0x00050000
    eciMandarinChinese              = 0x00060000
    eciMandarinChineseGB            = eciMandarinChinese
    eciMandarinChinesePinYin        = 0x00060100
    eciMandarinChineseUCS           = 0x00060800
    eciTaiwaneseMandarin            = 0x00060001
    eciTaiwaneseMandarinBig5        = eciTaiwaneseMandarin
    eciTaiwaneseMandarinZhuYin      = 0x00060101
    eciTaiwaneseMandarinPinYin      = 0x00060201
    eciTaiwaneseMandarinUCS         = 0x00060801
    eciBrazilianPortuguese          = 0x00070000
    eciStandardJapanese             = 0x00080000
    eciStandardJapaneseSJIS         = eciStandardJapanese
    eciStandardJapaneseUCS          = 0x00080800
    eciStandardFinnish              = 0x00090000
    eciStandardKorean               = 0x000A0000
    eciStandardKoreanUHC            = eciStandardKorean
    eciStandardKoreanUCS            = 0x000A0800
    eciStandardCantonese            = 0x000B0000
    eciStandardCantoneseGB          = eciStandardCantonese
    eciStandardCantoneseUCS         = 0x000B0800
    eciHongKongCantonese            = 0x000B0001
    eciHongKongCantoneseBig5        = eciHongKongCantonese
    eciHongKongCantoneseUCS         = 0x000B0801
    eciStandardDutch                = 0x000C0000
    eciStandardNorwegian            = 0x000D0000
    eciStandardSwedish              = 0x000E0000
    eciStandardDanish               = 0x000F0000
    eciStandardReserved             = 0x00100000
    eciStandardThai                 = 0x00110000
    eciStandardThaiTIS              = eciStandardThai
    
  enum ECIPartOfSpeech:
    eciUndefinedPOS = 0
    eciFutsuuMeishi = 1
    eciKoyuuMeishi
    eciSahenMeishi
    eciMingCi
    
  ctypedef union ECIPhoneme:
    unsigned char sz[eciPhonemeLength+1]
    unsigned short wsz[eciPhonemeLength+1]
    
  ctypedef struct ECIMouthData:
    ECIPhoneme phoneme    
    ECILanguageDialect eciLanguageDialect
    unsigned char mouthHeight
    unsigned char mouthWidth
    unsigned char mouthUpturn
    unsigned char jawOpen
    unsigned char teethUpperVisible
    unsigned char teethLowerVisible
    unsigned char tonguePosn
    unsigned char lipTension
    
  cdef struct ECIVoiceAttrib:
    int eciSampleRate
    ECILanguageDialect languageID
    
  enum ECIMessage:
    eciWaveformBuffer
    eciPhonemeBuffer
    eciIndexReply
    eciPhonemeIndexReply
    eciWordIndexReply
    eciStringIndexReply
    eciAudioIndexReply
    eciSynthesisBreak
  
  enum ECICallbackReturn:
    eciDataNotProcessed
    eciDataProcessed
    eciDataAbort
    
  enum ECIFilterError:
    FilterNoError     
    FilterFileNotFound
    FilterOutOfMemory         
    FilterInternalError
    FilterAccessError
    
  ctypedef ECICallbackReturn (*ECICallback)(ECIHand, ECIMessage, long, void *)

  ECIHand eciNew()
  ECIHand eciNewEx(ECILanguageDialect)
  ECIHand eciDelete(ECIHand)
  Boolean eciReset(ECIHand)
  int eciGetAvailableLanguages(ECILanguageDialect *aLanguages, int *nLanguages)
  Boolean eciIsBeingReentered(ECIHand)
  
  void eciVersion(char *)
  int eciProgStatus(ECIHand)
  void eciErrorMessage(ECIHand, void *)
  void eciClearErrors(ECIHand)
  Boolean eciTestPhrase(ECIHand)

  Boolean eciSpeakText(ECIInputText, Boolean)
  Boolean eciSpeakTextEx(ECIInputText, Boolean, ECILanguageDialect)
  
  int eciGetParam(ECIHand, ECIParam)
  int eciSetParam(ECIHand, ECIParam, int)
  int eciGetDefaultParam(ECIParam)
  int eciSetDefaultParam(ECIParam, int)
  
  Boolean eciCopyVoice(ECIHand, int, int)
  Boolean eciGetVoiceName(ECIHand, int, void *)
  Boolean eciSetVoiceName(ECIHand, int, void *)
  int eciGetVoiceParam(ECIHand, int, ECIVoiceParam)
  int eciSetVoiceParam(ECIHand, int, ECIVoiceParam, int)

  Boolean eciAddText(ECIHand, ECIInputText *)
  Boolean eciInsertIndex(ECIHand, int)
  Boolean eciSynthesize(ECIHand)
  Boolean eciSynthesizeFile(ECIHand, void *)
  Boolean eciClearInput(ECIHand)
  
  Boolean eciGeneratePhonemes(ECIHand, int, void *)
  int eciGetIndex(ECIHand)
  
  Boolean eciStop(ECIHand)
  Boolean eciSpeaking(ECIHand)
  Boolean eciPause(ECIHand, Boolean)
  Boolean eciSynchronize(ECIHand)
  
  Boolean eciSetOutputBuffer(ECIHand, int, short *)
  Boolean eciSetOutputFilename(ECIHand, void *)
  Boolean eciSetOutputDevice(ECIHand, int)

  void eciRegisterCallback(ECIHand, ECICallback, void *)

  ECIDictHand eciNewDict(ECIHand)
  ECIDictHand eciGetDict(ECIHand)
  ECIDictError eciSetDict(ECIHand, ECIDictHand)
  ECIDictHand eciDeleteDict(ECIHand, ECIDictHand)
  ECIDictError eciLoadDict(ECIHand, ECIDictHand, ECIDictVolume, ECIInputText)
  ECIDictError eciSaveDict(ECIHand, ECIDictHand, ECIDictVolume, ECIInputText)
  
  ECIDictError eciUpdateDict(ECIHand, ECIDictHand, ECIDictVolume, ECIInputText,
                             ECIInputText)
  ECIDictError eciDictFindFirst(ECIHand, ECIDictHand, ECIDictVolume,
                                ECIInputText *, ECIInputText *)
  ECIDictError eciDictFindNext(ECIHand, ECIDictHand, ECIDictVolume,
                               ECIInputText*, ECIInputText *)
  char * eciDictLookup(ECIHand hEngine, ECIDictHand, ECIDictVolume,
                       ECIInputText)
  ECIDictError eciUpdateDictA(ECIHand, ECIDictHand, ECIDictVolume, ECIInputText,
                              ECIInputText, ECIPartOfSpeech)
  ECIDictError eciDictFindFirstA(ECIHand, ECIDictHand, ECIDictVolume, 
                                 ECIInputText *, ECIInputText *, ECIPartOfSpeech *)
  ECIDictError eciDictFindNextA(ECIHand, ECIDictHand, ECIDictVolume, 
                                ECIInputText *, ECIInputText *, ECIPartOfSpeech *)
  ECIDictError  eciDictLookupA(ECIHand, ECIDictHand, ECIDictVolume, 
                               ECIInputText, ECIInputText *, ECIPartOfSpeech *)
  
  ECIFilterError eciDeactivateFilter(ECIHand, ECIFilterHand)
  ECIFilterHand  eciNewFilter(ECIHand, unsigned int, Boolean)
  ECIFilterError eciActivateFilter(ECIHand, ECIFilterHand)
  ECIFilterHand  eciDeleteFilter(ECIHand, ECIFilterHand)
  ECIFilterError eciUpdateFilter(ECIHand, ECIFilterHand, ECIInputText, 
                                 ECIInputText)
  ECIFilterError  eciGetFilteredText(ECIHand, ECIFilterHand, ECIInputText,
                                     ECIInputText *)

  # @note: PP: I'm not sure what these are since they're not in the sdk doc
  #ECIVoiceError eciRegisterVoice(ECIHand eciHand, int voiceNumber, void *vData,
  #                               ECIVoiceAttrib *vAttrib)
  #ECIVoiceError eciUnregisterVoice(ECIHand eciHand, int voiceNumber, 
  #                                 ECIVoiceAttrib *vAttrib, void **vData)
