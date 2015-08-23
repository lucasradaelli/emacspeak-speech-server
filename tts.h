#ifndef TTS_H_
#define TTS_H_

#include "alsa_player.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#define PACKAGENAME "tts"
#define PACKAGEVERSION "1.0"
#define ECILIBRARYNAME "libibmeci.so"

/* The declarations  bellow  are derived from the publically
   available eci.h (APIs for IBM Text To Speech) at http://ibmtts-sdk.sf.net

   Its license is copied below

 * Copyright (c) 2005, 2006, IBM Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.  *
 *     Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of IBM Corp. nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

enum ECILanguageDialect {
  NODEFINEDCODESET = 0x00000000,
  eciGeneralAmericanEnglish = 0x00010000,
  eciBritishEnglish = 0x00010001,
  eciCastilianSpanish = 0x00020000,
  eciMexicanSpanish = 0x00020001,
  eciStandardFrench = 0x00030000,
  eciCanadianFrench = 0x00030001,
  eciStandardGerman = 0x00040000,
  eciStandardItalian = 0x00050000,
  eciMandarinChinese = 0x00060000,
  eciMandarinChineseGB = eciMandarinChinese,
  eciMandarinChinesePinYin = 0x00060100,
  eciMandarinChineseUCS = 0x00060800,
  eciTaiwaneseMandarin = 0x00060001,
  eciTaiwaneseMandarinBig5 = eciTaiwaneseMandarin,
  eciTaiwaneseMandarinZhuYin = 0x00060101,
  eciTaiwaneseMandarinPinYin = 0x00060201,
  eciTaiwaneseMandarinUCS = 0x00060801,
  eciBrazilianPortuguese = 0x00070000,
  eciStandardJapanese = 0x00080000,
  eciStandardJapaneseSJIS = eciStandardJapanese,
  eciStandardJapaneseUCS = 0x00080800,
  eciStandardFinnish = 0x00090000,
  eciStandardKorean = 0x000A0000,
  eciStandardKoreanUHC = eciStandardKorean,
  eciStandardKoreanUCS = 0x000A0800,
  eciStandardCantonese = 0x000B0000,
  eciStandardCantoneseGB = eciStandardCantonese,
  eciStandardCantoneseUCS = 0x000B0800,
  eciHongKongCantonese = 0x000B0001,
  eciHongKongCantoneseBig5 = eciHongKongCantonese,
  eciHongKongCantoneseUCS = 0x000B0801,
  eciStandardDutch = 0x000C0000,
  eciStandardNorwegian = 0x000D0000,
  eciStandardSwedish = 0x000E0000,
  eciStandardDanish = 0x000F0000,
  eciStandardReserved = 0x00100000,
  eciStandardThai = 0x00110000,
  eciStandardThaiTIS = eciStandardThai
};

//
enum { ANNOTATION_MAX_SIZE = 10, LANG_INFO_MAX = 22 };

struct langInfo {
  enum ECILanguageDialect lang;
  const char* code;
  const char* encoding;
  const char* id;
  const char* label;
};

enum { LANGUAGE_MAX_LABEL = 30 };  // max size of the label field

// End of ECI declarations.

// >
// <decls and function prototypes

/*
 * The following declarations are derived from the publically
 * available documentation for ViaVoice TTS outloud. --they are
 * placed here to obviate the need for having the ViaVoice SDK
 * installed.
 */

typedef enum { eciDataNotProcessed, eciDataProcessed } ECICallbackReturn;

typedef enum {
  eciWaveformBuffer,
  eciPhonemeBuffer,
  eciIndexReply,
  eciPhonemeIndexReply
} ECIMessage;

typedef enum {
  eciSynthMode,
  eciInputType,
  eciTextMode,
  eciDictionary,
  eciSampleRate = 5,
  eciWantPhonemeIndices = 7,
  eciRealWorldUnits,
  eciLanguageDialect,
  eciNumberMode,
  eciPhrasePrediction,
  eciNumParams
} ECIParam;

class LangSwitcher;

class TTS {
 public:
  struct Options {
    Options() noexcept {}

    // todo: add enum for sample rates.
    int sample_rate = 1;
  };

  TTS(AlsaPlayer* alsa_player, const Options& options = Options());
  ~TTS();

  static bool InitECI();

  int PlayTTS(const int count);

  bool IsSpeaking();

  bool Synchronize();

  void SetLastReply(const long lparam) { lparam_ = lparam; }

 private:
  void* eci_handle_;
  AlsaPlayer* alsa_player_;
  std::unique_ptr<LangSwitcher> lang_switcher_;
  long lparam_;
};

class TTSError : public std::runtime_error {
 public:
  explicit TTSError(const std::string& arg) : runtime_error(arg) {}
};

class LangSwitcher {
 public:
  LangSwitcher(const ECILanguageDialect* a_languages, const int n_languages)
      : a_languages_(a_languages), n_languages_(n_languages) {}
  ~LangSwitcher() = default;
  ECILanguageDialect InitLanguage();

 private:
  std::string GetDefaultLanguageCode();
  bool GetValidLanguages(std::vector<int>* available_languages_index);

  const ECILanguageDialect* a_languages_;
  int n_languages_;

  std::vector<langInfo> the_languages_{
      {NODEFINEDCODESET, NULL, NULL, NULL, NULL},
      {eciGeneralAmericanEnglish, "en_US", "iso8859-1", "1.0", "American"},
      {eciBritishEnglish, "en_GB", "iso8859-1", "1.1", "British"},
      {eciCastilianSpanish, "es_ES", "iso8859-1", "2.0", "Español"},
      {eciMexicanSpanish, "es_MX", "iso8859-1", "2.1", "Mexicano"},
      {eciStandardFrench, "fr_FR", "iso8859-1", "3.0", "Français"},
      {eciCanadianFrench, "fr_CA", "iso8859-1", "3.1", "Français Canadien"},
      {eciStandardGerman, "de_DE", "iso8859-1", "4.0", "Deutsch"},
      {eciStandardItalian, "it_IT", "iso8859-1", "5.0", "Italiano"},
      {eciMandarinChineseGB, "zh_CN", "gb2312", "6.0", "Chinese Simplified"},
      {eciMandarinChinesePinYin, "zh_CN", "gb2312", "6.0.1",
       "Chinese Simplified"},
      {eciMandarinChineseUCS, "zh_CN", "UCS2", "6.0.8", "Chinese Simplified"},
      {eciTaiwaneseMandarinBig5, "zh_HK", "big5", "6.1", "Chinese Traditional"},
      {eciTaiwaneseMandarinZhuYin, "zh_HK", "big5", "6.1.1",
       "Chinese Traditional"},
      {eciTaiwaneseMandarinPinYin, "zh_HK", "big5", "6.1.2",
       "Chinese Traditional"},
      {eciTaiwaneseMandarinUCS, "zh_HK", "UCS2", "6.1.8",
       "Chinese Traditional"},
      {eciBrazilianPortuguese, "pt_BR", "iso8859-1", "7.0",
       "Brazilian Portuguese"},
      {eciStandardJapaneseSJIS, "ja_JP", "shiftjis", "8.0", "Japanese"},
      {eciStandardJapaneseUCS, "ja_JP", "UCS2", "8.0.8", "Japanese"},
      {eciStandardFinnish, "fi_FI", "iso8859-1", "9.0", "Finnish"},
      {eciStandardKoreanUHC, NULL, NULL, NULL},
      {eciStandardKoreanUCS, NULL, NULL, NULL, NULL},
      {eciStandardCantoneseGB, NULL, NULL, NULL, NULL},
      {eciStandardCantoneseUCS, NULL, NULL, NULL, NULL},
      {eciHongKongCantoneseBig5, NULL, NULL, NULL, NULL},
      {eciHongKongCantoneseUCS, NULL, NULL, NULL, NULL},
      {eciStandardDutch, NULL, NULL, NULL, NULL},
      {eciStandardNorwegian, NULL, NULL, NULL, NULL},
      {eciStandardSwedish, NULL, NULL, NULL, NULL},
      {eciStandardDanish, NULL, NULL, NULL, NULL},
      {eciStandardReserved, NULL, NULL, NULL, NULL},
      {eciStandardThai, NULL, NULL, NULL, NULL},
      {eciStandardThaiTIS, NULL, NULL, NULL, NULL}};

  int current_language;
};

#endif  // TTS_H_
