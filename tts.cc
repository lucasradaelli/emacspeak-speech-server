// Copyright 2015 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tts.h"
#include "alsa_player.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

using std::string;
using std::vector;

constexpr char TTS::kEciLibraryName[];

TTS::TTS(AudioManager *audio, const Options &options) : audio_(audio) {
  languages_ = ECI::GetAvailableLanguages();

  if (languages_.empty()) {
    throw TTSError("No languages found.");
  }
  int i;
  for (i = 0; i < languages_.size(); ++i) {
    if (languages_[i] == options.default_language) break;
  }

  current_language_index_ =
      i != languages_.size() ? i
                             : 0 /*Fallback to the first available language.*/;

  eci_.reset(new ECI(languages_[current_language_index_]));

  // Initialize TTS.
  eci_->SetParam(eciInputType, 1);
  eci_->SetParam(eciSynthMode, 1);
  eci_->SetParam(eciSampleRate, options.sample_rate);

  eci_->SetCallback(eciWaveformBuffer, [=](long frames) {
    audio_->player()->Play(frames);
    return eciDataProcessed;
  });
  eci_->SetOutputBuffer(audio_->player()->period_size(),
                        reinterpret_cast<short *>(audio_->player()->buffer()));
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
  prefix_string << "`vs" << GetSpeechRate() << " ";
  return prefix_string.str();
}

bool TTS::Output(const string &msg) { return AddText(msg) && Synthesize(); }

bool TTS::Say(const string &msg, const ECIVoiceAnnotation voice) {
  if (voice == NO_ANNOTATION) {
    return Output(GetPrefixString() + msg);
  }
  string voice_prefix;
  switch (voice) {
    case DEFAULT_VOICE:
      voice_prefix = "`v1 ";
      break;
    default:
      break;
  }

  return Output(voice_prefix + GetPrefixString() + msg);
}

std::unique_ptr<SpeechTask> TTS::UseSelectedVoice(
    const ECIVoiceAnnotation voice) {
  Say("", voice);
  return ReleaseTask();
}

bool TTS::GenerateSilence(const int duration) {
  // The ECI library has a special code to insert silence during speech. We
  // create a message of this form and simply Output() it.
  std::ostringstream msg;
  msg << "`p" << duration;
  return Output(msg.str());
}

bool TTS::Pause() {
  audio_->player()->Pause();
  return true;
}

bool TTS::Resume() {
  audio_->player()->Resume();
  return true;
}

bool TTS::Stop() {
  audio_->Clear();
  audio_->player()->Interrupt();
  return true;
}

string TTS::TTSVersion() { return eci_->Version(); }

void TTS::NextLanguage() {
  if (current_language_index_ == languages_.size() - 1) {
    current_language_index_ = 0;
  } else {
    ++current_language_index_;
  }
  eci_->SetParam(eciLanguageDialect, languages_[current_language_index_]);
}

void TTS::PreviousLanguage() {
  if (current_language_index_ == 0) {
    current_language_index_ = languages_.size() - 1;
  } else {
    --current_language_index_;
  }
  eci_->SetParam(eciLanguageDialect, languages_[current_language_index_]);
}

TTS::SampleRate TTS::GetSampleRateConfig(int sample_rate) {
  static const std::map<int, TTS::SampleRate> kSupportedSampleRates = {
      {8000, R_8000}, {11025, R_11025}, {22050, R_22050}};
  auto it = kSupportedSampleRates.find(sample_rate);
  if (it == kSupportedSampleRates.end()) {
    throw TTSError("Sample rate is not supported.");
  }
  return it->second;
}

ECILanguageDialect TTS::GetLanguageConfig(const string &language) {
  static const std::map<string, ECILanguageDialect> kSupportedLanguages = {
      {"en_US", eciGeneralAmericanEnglish}, {"en_GB", eciBritishEnglish},
      {"es_ES", eciCastilianSpanish},       {"es_MX", eciMexicanSpanish},
      {"fr_FR", eciStandardFrench},         {"fr_CA", eciCanadianFrench},
      {"de_DE", eciStandardGerman},         {"it_IT", eciStandardItalian},
      {"pt_BR", eciBrazilianPortuguese},    {"fi_FI", eciStandardFinnish}};
  auto it = kSupportedLanguages.find(language);
  if (it == kSupportedLanguages.end()) {
    throw TTSError("Unknown language.");
  }

  return it->second;
}
