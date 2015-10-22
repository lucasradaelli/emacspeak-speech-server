'''
Declares global constants that should be exposed by the Python wrapper. The 
constants defined are those that indicate the language to use, the engine
parameters, the voice parameters, the dictionary constants, and the Asian part
of speech constants.

@author: Peter Parente
@organization: IBM Corporation
@copyright: Copyright (c) 2006 IBM Corporation
@license: Berkeley Software Distribution (BSD) License

All rights reserved. This program and the accompanying materials are made 
available under the terms of the Berkeley Software Distribution (BSD) license 
which accompanies this distribution, and is available at
U{http://www.opensource.org/licenses/bsd-license.php}
'''

# language constants
GeneralAmericanEnglish = eciGeneralAmericanEnglish
BritishEnglish = eciBritishEnglish
CastilianSpanish = eciCastilianSpanish
MexicanSpanish = eciMexicanSpanish
StandardFrench = eciStandardFrench
CanadianFrench = eciCanadianFrench
StandardGerman = eciStandardGerman 
StandardItalian = eciStandardItalian
MandarinChinese = eciMandarinChinese
MandarinChineseGB = eciMandarinChineseGB
MandarinChinesePinYin = eciMandarinChinesePinYin
MandarinChineseUCS = eciMandarinChineseUCS
TaiwaneseMandarin = eciTaiwaneseMandarin
TaiwaneseMandarinBig5 = eciTaiwaneseMandarinBig5
TaiwaneseMandarinZhuYin = eciTaiwaneseMandarinZhuYin
TaiwaneseMandarinPinYin = eciTaiwaneseMandarinPinYin
TaiwaneseMandarinUCS = eciTaiwaneseMandarinUCS
BrazilianPortuguese = eciBrazilianPortuguese
StandardJapanese = eciStandardJapanese
StandardJapaneseSJIS = eciStandardJapaneseSJIS
StandardJapaneseUCS = eciStandardJapaneseUCS
StandardFinnish = eciStandardFinnish
StandardKorean = eciStandardKorean
StandardKoreanUHC = eciStandardKoreanUHC
StandardKoreanUCS = eciStandardKoreanUCS
StandardCantonese = eciStandardCantonese
StandardCantoneseGB = eciStandardCantoneseGB 
StandardCantoneseUCS = eciStandardCantoneseUCS
HongKongCantonese = eciHongKongCantonese
HongKongCantoneseBig5 = eciHongKongCantoneseBig5
HongKongCantoneseUCS = eciHongKongCantoneseUCS
StandardDutch = eciStandardDutch 
StandardNorwegian = eciStandardNorwegian
StandardSwedish = eciStandardSwedish 
StandardDanish = eciStandardDanish 
StandardReserved = eciStandardReserved
StandardThai = eciStandardThai
StandardThaiTIS = eciStandardThaiTIS  

# engine/default param constants
SynthMode = eciSynthMode
InputType = eciInputType
TextMode = eciTextMode
Dictionary = eciDictionary
SampleRate = eciSampleRate
WantPhonemeIndices = eciWantPhonemeIndices
RealWorldUnits = eciRealWorldUnits
LanguageDialect = eciLanguageDialect
NumberMode = eciNumberMode
WantWordIndex = eciWantWordIndex
NumDeviceBlocks = eciNumDeviceBlocks
SizeDeviceBlocks = eciSizeDeviceBlocks
NumPrerollDeviceBlocks = eciNumPrerollDeviceBlocks
SizePrerollDeviceBlocks = eciSizePrerollDeviceBlocks
NumParams = eciNumParams

# voice param constants
Gender = eciGender
HeadSize = eciHeadSize
PitchBaseline = eciPitchBaseline
PitchFluctuation = eciPitchFluctuation
Roughness = eciRoughness
Breathiness = eciBreathiness
Speed = eciSpeed
Volume = eciVolume
NumVoiceParams = eciNumVoiceParams

# dictionary constants
MainDict = eciMainDict
RootDict = eciRootDict
AbbvDict =  eciAbbvDict
MainDictExt = eciMainDictExt

# Asian part of speech constants
UndefinedPOS = eciUndefinedPOS
FutsuuMeishi = eciFutsuuMeishi
KoyuuMeishi = eciKoyuuMeishi
SahenMeishi = eciSahenMeishi
MingCi = eciMingCi