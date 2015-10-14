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

// Wrapper class used to communicate with the IBM Text-to-Speech library.
class TTS {
 public:
  struct Options {
    Options() noexcept {}

    // todo: add enum for sample rates.
    int sample_rate = 1;
  };

  // This class generates speech by calling methods to add processed /
  // non-processed text, then a call to Synthesize to generate the pcm
  // representation of the speech, and, finally, produces the output by calling
  // IsSpeaking(), repeatedly,until it returns false,  to give the IBM
  // Text-to-Speech library the
  // possibility to write the output into the audio buffer and ask to the player
  // to really play the content. This design allows the programs wishing to use
  // this class to be non-blocking, e.g. can perform other actions between calls
  // to IsSpeaking().
  TTS(AlsaPlayer* alsa_player, const Options& options = Options());
  ~TTS();

  // Loads dynamically the IBM Text-to-Speech library into memory. You must call
  // this method before attempting to use this class. Will return false if it
  // fails to open the library or get any of its symbols. Returns true
  // otherwise.
  static bool InitECI();

  // Generates the internal pcm representation of the speech containing the text
  // previously added with calls to AddText().
  bool Synthesize();

  // Adds the given text to be an output. The texts can be appended with
  // subsequent calls to this function. The final output will only be produced
  // when a call to Synthesize() is made.
  bool AddText(const std::string& msg);

  // Helper method to generate the internal pcm representation of the speech,
  // without any processing on the text. This would be the same as AddText(...),
  // Synthesize().
  bool Output(const std::string& msg);

  // Helper method to generate the internal pcm representation of the speech,
  // prefixing the text with a string to control the speech engine. It will use
  // the default voice plus the current speech speed. This would be the same as
  // AddText(GetPrefixString() + text),
  // Synthesize().
  bool Say(const std::string& msg);

  bool GenerateSilence(const int duration);

  int PlayTTS(const int count);

  bool IsSpeaking();

  bool Synchronize();

  bool Pause();

  bool Resume();

  bool Stop();

  void SetLastReply(const long lparam) { lparam_ = lparam; }

  std::string TTSVersion();

  AlsaPlayer* player() const { return alsa_player_; }
  bool speaking() const { return speaking_; }

  int GetSpeechRate() const { return speech_rate_; }

  void SetSpeechRate(const int speech_rate) { speech_rate_ = speech_rate; }

  const std::string GetPrefixString() const;

 private:
  void* eci_handle_;
  AlsaPlayer* alsa_player_;
  std::unique_ptr<LangSwitcher> lang_switcher_;

  int speech_rate_ = 50;

  // Is true whenever there is data to send to the sound output.
  bool speaking_ = false;

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
