#ifndef TTS_H_
#define TTS_H_

#include "alsa_player.h"
#include "eci.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#define PACKAGENAME "tts"
#define PACKAGEVERSION "1.0"
#define ECILIBRARYNAME "libibmeci.so"

enum { ANNOTATION_MAX_SIZE = 10, LANG_INFO_MAX = 22 };

struct langInfo {
  enum ECILanguageDialect lang;
  const char* code;
  const char* encoding;
  const char* id;
  const char* label;
};

enum { LANGUAGE_MAX_LABEL = 30 };  // max size of the label field

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

  bool Synthesize();

  bool AddText(const std::string& msg);

  bool Say(const std::string& msg);

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
