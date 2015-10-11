#include "tts.h"
#include "alsa_player.h"

#include <sys/time.h>
#include <dlfcn.h>
#include <alloca.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>

using std::string;
using std::vector;

namespace {

void (*eciVersion_)(char *);
void *(*eciNewEx_)(ECILanguageDialect);
int (*eciGetAvailableLanguages_)(ECILanguageDialect *, int *);
void (*eciDelete_)(void *);
int (*eciReset_)(void *);
int (*eciStop_)(void *);
int (*eciClearInput_)(void *);
int (*eciPause_)(void *, int);
int (*eciSynthesize_)(void *);
int (*eciSynchronize_)(void *);
int (*eciSpeaking_)(void *);
int (*eciAddText_)(void *, const char *);
int (*eciInsertIndex_)(void *, int);
int (*eciSetParam_)(void *, int, int);
int (*eciGetVoiceParam_)(void *, int, int);
int (*eciSetVoiceParam_)(void *, int, int, int);
int (*eciSetOutputBuffer_)(void *, int, short *);
int (*eciSetOutputDevice_)(void *, int);
void (*eciRegisterCallback_)(void *, ECICallback, void *);

}  // namespace

bool TTS::InitECI() {
  void *eciLib;
  // < configure shared library symbols

  eciLib = dlopen(ECILIBRARYNAME, RTLD_LAZY);
  if (eciLib == NULL) {
    std::cerr << "Could not load   " << ECILIBRARYNAME << "\n";
    return false;
  }

  eciVersion_ = (void (*)(char *))dlsym(eciLib, "eciVersion");
  eciGetAvailableLanguages_ = (int (*)(ECILanguageDialect *, int *))dlsym(
      eciLib, "eciGetAvailableLanguages");
  eciNewEx_ = (void *(*)(ECILanguageDialect))dlsym(eciLib, "eciNewEx");
  eciDelete_ = (void (*)(void *))dlsym(eciLib, "eciDelete");
  eciReset_ = (int (*)(void *))dlsym(eciLib, "eciReset");
  eciStop_ = (int (*)(void *))dlsym(eciLib, "eciStop");
  eciClearInput_ = (int (*)(void *))dlsym(eciLib, "eciClearInput");
  eciPause_ = (int (*)(void *, int))dlsym(eciLib, "eciPause");
  eciSynthesize_ = (int (*)(void *))dlsym(eciLib, "eciSynthesize");
  eciSynchronize_ = (int (*)(void *))dlsym(eciLib, "eciSynchronize");
  eciSpeaking_ = (int (*)(void *))dlsym(eciLib, "eciSpeaking");
  eciInsertIndex_ = (int (*)(void *, int))dlsym(eciLib, "eciInsertIndex");
  eciAddText_ = (int (*)(void *, const char *))dlsym(eciLib, "eciAddText");
  eciSetParam_ = (int (*)(void *, int, int))dlsym(eciLib, "eciSetParam");
  eciGetVoiceParam_ =
      (int (*)(void *, int, int))dlsym(eciLib, "eciGetVoiceParam");
  eciSetVoiceParam_ =
      (int (*)(void *, int, int, int))dlsym(eciLib, "eciSetVoiceParam");
  eciRegisterCallback_ = (void (*)(void *, ECICallback, void *))dlsym(
      eciLib, "eciRegisterCallback");
  eciSetOutputBuffer_ =
      (int (*)(void *, int, short *))dlsym(eciLib, "eciSetOutputBuffer");
  eciSetOutputDevice_ =
      (int (*)(void *, int))dlsym(eciLib, "eciSetOutputDevice");

  // >
  // < check for needed symbols

  int okay = 1;
  if (!eciNewEx_) {
    okay = 0;
    std::cerr << "eciNewEx undef\n";
  }
  if (!eciDelete_) {
    okay = 0;
    std::cerr << "eciDelete undef\n";
  }
  if (!eciReset_) {
    okay = 0;
    std::cerr << "eciReset undef\n";
  }
  if (!eciStop_) {
    okay = 0;
    std::cerr << "eciStop undef\n";
  }
  if (!eciClearInput_) {
    okay = 0;
    std::cerr << "eciClearInput undef\n";
  }
  if (!eciPause_) {
    okay = 0;
    std::cerr << "eciPause undef\n";
  }
  if (!eciSynthesize_) {
    okay = 0;
    std::cerr << "eciSynthesize undef\n";
  }
  if (!eciSpeaking_) {
    okay = 0;
    std::cerr << "eciSpeaking undef\n";
  }
  if (!eciInsertIndex_) {
    okay = 0;
    std::cerr << "eciInsertIndex undef\n";
  }
  if (!eciAddText_) {
    okay = 0;
    std::cerr << "eciAddText undef\n";
  }
  if (!eciSetParam_) {
    okay = 0;
    std::cerr << "eciSetParam undef\n";
  }
  if (!eciSetParam_) {
    okay = 0;
    std::cerr << "eciSetParam undef\n";
  }
  if (!eciGetVoiceParam_) {
    okay = 0;
    std::cerr << "eciGetVoiceParam undef\n";
  }
  if (!eciSetVoiceParam_) {
    okay = 0;
    std::cerr << "eciSetVoiceParam undef\n";
  }
  if (!eciRegisterCallback_) {
    okay = 0;
    std::cerr << "eciRegisterCallback undef\n";
  }
  if (!eciSetOutputBuffer_) {
    okay = 0;
    std::cerr << "eciSetOutputBuffer undef\n";
  }
  if (!eciSetOutputDevice_) {
    okay = 0;
    std::cerr << "eciSetOutputDevice undef\n";
  }
  if (!eciGetAvailableLanguages_) {
    okay = 0;
    std::cerr << "eciGetAvailableLanguages undef\n";
  }

  if (!okay) {
    std::cerr << "Failed to  load the lib\n";
    return false;
  }

  return true;
}

TTS::TTS(AlsaPlayer *alsa_player, const Options &options)
    : alsa_player_(alsa_player) {
  int rc;
  ECILanguageDialect a_languages[LANG_INFO_MAX];
  int n_languages = LANG_INFO_MAX;
  eciGetAvailableLanguages_(a_languages, &n_languages);

  lang_switcher_.reset(new LangSwitcher(a_languages, n_languages));

  ECILanguageDialect a_default_language = lang_switcher_->InitLanguage();

  if (a_default_language == NODEFINEDCODESET) {
    throw TTSError("No languages found\n");
  }

  eci_handle_ = eciNewEx_(a_default_language);
  if (eci_handle_ == nullptr) {
    throw TTSError("Could not open text-to-speech engine.\n");
  }

  // Initialize TTS
  if ((eciSetParam_(eci_handle_, eciInputType, 1) == -1) ||
      (eciSetParam_(eci_handle_, eciSynthMode, 1) == -1) ||
      (eciSetParam_(eci_handle_, eciSampleRate, options.sample_rate) == -1)) {
    eciDelete_(eci_handle_);
    throw TTSError("Could not initialized tts");
  }

  eciRegisterCallback_(
      eci_handle_,
      +[](void *eci_handle, ECIMessage msg, long lparam, void *data) {
        TTS *tts = static_cast<TTS *>(data);
        if (msg == eciIndexReply) {
          tts->SetLastReply(lparam);
        } else if ((msg == eciWaveformBuffer) && (lparam > 0)) {
          tts->PlayTTS(lparam);
        }
        return eciDataProcessed;
      },
      this);

  // Set output to bufferl.
  rc = eciSynchronize_(eci_handle_);
  if (!rc) {
    throw TTSError("Error resetting TTS engine.\n");
  }
  rc = eciSetOutputBuffer_(eci_handle_, alsa_player_->period_size(),
                           reinterpret_cast<short *>(alsa_player_->buffer()));
  if (!rc) {
    throw TTSError("Error setting output buffer.\n");
  }
}

TTS::~TTS() { eciDelete_(eci_handle_); }

bool TTS::Synthesize() {
  if (eciSynthesize_(eci_handle_)) {
    return true;
  } else {
    return false;
  }
}

bool TTS::AddText(const string& msg) {
  if (eciAddText_(eci_handle_, msg.c_str())) {
      return true;
    } else {
      return false;
    }
}

bool TTS::Say(const string& msg) {
  if (!AddText(msg)) {
    return false;
  }
  if (!Synthesize()) {
    return false;
  }
  return true;
}

int TTS::PlayTTS(const int count) {
  alsa_player_->Play(count);
  return eciDataProcessed;
}

bool TTS::IsSpeaking() {
  if (eciSpeaking_(eci_handle_)) {
    return true;
  } else {
    return false;
  }
}

bool TTS::Synchronize() {
  int rc = eciSynchronize_(eci_handle_);
  if (rc) {
    return true;
  } else {
    return false;
  }
}


string TTS::TTSVersion() {
  std::unique_ptr<char[]> version( new char[20]);
  eciVersion_(version.get());
  return string(version.get());
}


string LangSwitcher::GetDefaultLanguageCode() {
  const char *a_default_lang = getenv("LANGUAGE");
  if (a_default_lang == NULL) {
    a_default_lang = getenv("LANG");
    if (a_default_lang == NULL) {
      a_default_lang = "en";
    }
  }
  if (strlen(a_default_lang) < 2) {
    a_default_lang = "en";
  }
  // TODO: Test if the language is available on the list of supported languages.
  return a_default_lang;
}

bool LangSwitcher::GetValidLanguages(vector<int> *available_languages_index) {
  for (int i = 0; i < n_languages_; i++) {
    for (int j = 0; j < static_cast<int>(the_languages_.size()); j++) {
      if (a_languages_[i] == the_languages_[j].lang) {
        available_languages_index->push_back(j);
      }
    }
  }
  return !available_languages_index->empty();
}

ECILanguageDialect LangSwitcher::InitLanguage() {
  vector<int> available_languages_index;
  if (!GetValidLanguages(&available_languages_index)) {
    return NODEFINEDCODESET;
  }

  const string a_default_lang_code = GetDefaultLanguageCode();
  ECILanguageDialect a_current_language, a_english_language, a_first_language;
  a_current_language = a_english_language = a_first_language = NODEFINEDCODESET;
  for (const int index : available_languages_index) {
    if (a_current_language == NODEFINEDCODESET) {
      if (strncmp(a_default_lang_code.c_str(), the_languages_[index].code, 2) ==
          0) {
        a_current_language = the_languages_[index].lang;
      } else if (strncmp("en", the_languages_[index].code, 2) == 0) {
        a_english_language = the_languages_[index].lang;
      }
    }
  }

  a_first_language = the_languages_[available_languages_index[0]].lang;
  if (a_current_language == NODEFINEDCODESET) {
    if (a_english_language != NODEFINEDCODESET) {
      a_current_language = a_english_language;
    } else if (a_first_language != NODEFINEDCODESET) {
      a_current_language = a_first_language;
    }
  }

  return a_current_language;
}
