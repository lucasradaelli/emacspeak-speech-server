#include "tts.h"
#include "alsa_player.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>

using std::string;
using std::vector;

constexpr char TTS::kEciLibraryName[];

bool TTS::InitECI() {
  ECI::Init(kEciLibraryName);
  return true;
}

TTS::TTS(AudioManager* audio, const Options &options)
    : audio_(audio) {
  lang_switcher_.reset(new LangSwitcher(ECI::GetAvailableLanguages()));

  ECILanguageDialect a_default_language = lang_switcher_->InitLanguage();
  if (a_default_language == NODEFINEDCODESET) {
    throw TTSError("No languages found.");
  }

  eci_.reset(new ECI(a_default_language));

  // Initialize TTS.
  eci_->SetParam(eciInputType, 1);
  eci_->SetParam(eciSynthMode, 1);
  eci_->SetParam(eciSampleRate, options.sample_rate);

  eci_->SetCallback(eciWaveformBuffer, [=](long frames) {
    audio_->player()->Play(frames);
    return eciDataProcessed;
  });
  eci_->SetOutputBuffer(audio_->player()->period_size(),
                        reinterpret_cast<short*>(audio_->player()->buffer()));
}

TTS::~TTS() {}

SpeechTask *TTS::GetTask() {
  if (pending_task_ == nullptr) {
    pending_task_.reset(new SpeechTask(eci_.get()));
  }
  return pending_task_.get();
}

std::unique_ptr<SpeechTask> TTS::ReleaseTask() {
  return std::move(pending_task_);
}

bool TTS::SubmitTask() {
  if (pending_task_ == nullptr) return false;
  audio_->Push(ReleaseTask());
  return true;
}

bool TTS::AddText(const string &msg) {
  GetTask()->AddText(msg);
  return true;
}

bool TTS::Synthesize() {
  GetTask()->Synthesize();
  return true;
}

const string TTS::GetPrefixString() const {
  std::ostringstream prefix_string;
  prefix_string << "`v1 `vs" << GetSpeechRate() << " ";
  return prefix_string.str();
}

bool TTS::Output(const string &msg) {
  return AddText(msg) && Synthesize() && SubmitTask();
}

bool TTS::Say(const string &msg) {
  return Output(GetPrefixString() + msg);
}

bool TTS::GenerateSilence(const int duration) {
  // The ECI library has a special code to insert silence during speech. We
  // create a message of this form and simply Output() it.
  std::ostringstream msg;
  msg << "`p" << duration;
  return Output(msg.str());
}

bool TTS::Pause() {
  eci_->Pause(true);
  audio_->player()->Pause();
  return true;
}

bool TTS::Resume() {
  eci_->Pause(false);
  audio_->player()->Resume();
  return true;
}

bool TTS::Stop() {
  eci_->Stop();
  audio_->player()->Interrupt();
  usleep(10);
  return true;
}

string TTS::TTSVersion() {
  return eci_->Version();
}

string LangSwitcher::GetDefaultLanguageCode() {
  const char *a_default_lang = getenv("LANGUAGE");
  if (a_default_lang == nullptr) {
    a_default_lang = getenv("LANG");
    if (a_default_lang == nullptr) {
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
  for (const auto& lang : languages_) {
    for (int j = 0; j < static_cast<int>(the_languages_.size()); j++) {
      if (lang == the_languages_[j].lang) {
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
