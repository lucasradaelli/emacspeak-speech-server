#ifndef TTS_H_
#define TTS_H_

#include "audio_manager.h"
#include "audio_tasks.h"
#include "eci-c++.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class LangSwitcher;

// Frontend class for speech synthesis using IBM ViaVoice Text-to-Speech.
//
// This class generates speech by calling methods to add processed or
// unprocessed text, then calls Synthesize to generate the PCM representation
// of the speech, and, finally, submits a speech task to the AudioManager that
// will handle output to the player.
//
// This design allows the programs wishing to use this class to be non-blocking,
// e.g. can perform other actions between calls to ECI::Speaking().
class TTS {
 public:
  // Synthesis sample rates supported by IBM ViaVoice.
  enum SampleRate {
    R_8000 = 0,
    R_11025 = 1,
    R_22050 = 2,
  };

  // Options to configure the behavior of this class.
  struct Options {
    Options() noexcept {}

    // Sample rate of synthesized speech.
    SampleRate sample_rate = R_11025;
  };

  static constexpr char kEciLibraryName[] = "libibmeci.so";

  TTS(AudioManager* audio, const Options& options = Options());
  ~TTS();

  // Dynamically loads the IBM Text-to-Speech library in memory. It must be
  // called before attempting to use this class. It will throw an exception if
  // it fails to open the library or to load any of its symbols.
  static bool InitECI();

  // Initializes a new speech task if none is available, then returns it.
  // Each task defines a packet of instructions that will control the TTS
  // library to synthesize a piece of audio output. This method implicitly
  // creates a pending speech-type task and returns it. The task will stay
  // alive until a call to ReleaseTask() or SubmitTask() is done. Several
  // methods of this class implicitly create a pending task.
  SpeechTask* GetTask();

  // Releases the current pending task, if any, and returns it, transferring
  // ownership to the caller.
  std::unique_ptr<SpeechTask> ReleaseTask();

  // Submits the current pending task for execution, if any.
  bool SubmitTask();

  // Adds the given text to be an output. The texts can be appended with
  // subsequent calls to this function. The final output will only be produced
  // when call to Synthesize() and SubmitTask() are made.
  bool AddText(const std::string& msg);

  // Generates the internal pcm representation of the speech containing the
  // text previously added with calls to AddText().
  bool Synthesize();

  // Helper method to generate the internal PCM representation of the speech,
  // without any processing on the text. This would be the same as
  // AddText(...), Synthesize().
  bool Output(const std::string& msg);

  // Helper method to generate the internal PCM representation of the speech,
  // prefixing the text with a string to control the speech engine. It will use
  // the default voice plus the current speech speed. This would be the same as
  // AddText(GetPrefixString() + text), Synthesize().
  bool Say(const std::string& msg);

  bool GenerateSilence(const int duration);

  bool Pause();

  bool Resume();

  bool Stop();

  std::string TTSVersion();

  int GetSpeechRate() const { return speech_rate_; }

  void SetSpeechRate(const int speech_rate) { speech_rate_ = speech_rate; }

  const std::string GetPrefixString() const;

 private:
  std::unique_ptr<LangSwitcher> lang_switcher_;
  std::unique_ptr<ECI> eci_;
  AudioManager* audio_;

  std::unique_ptr<SpeechTask> pending_task_;

  int speech_rate_ = 50;

};

class TTSError : public std::runtime_error {
 public:
  explicit TTSError(const std::string& arg) : runtime_error(arg) {}
};

class LangSwitcher {
 public:
  LangSwitcher(std::vector<ECILanguageDialect> languages)
      : languages_(languages) {}
  ~LangSwitcher() = default;

  ECILanguageDialect InitLanguage();

 private:
  struct langInfo {
    enum ECILanguageDialect lang;
    const char* code;
    const char* encoding;
    const char* id;
    const char* label;
  };

  std::string GetDefaultLanguageCode();
  bool GetValidLanguages(std::vector<int>* available_languages_index);

  std::vector<ECILanguageDialect> languages_;

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
