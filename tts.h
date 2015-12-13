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

  enum ECIVoiceAnnotation {
    DEFAULT_VOICE,  // Adult male.
    NO_ANNOTATION,
    // TODO: add more.
  };

  // Options to configure the behavior of this class.
  struct Options {
    Options() noexcept {}

    // Sample rate of synthesized speech.
    SampleRate sample_rate = R_11025;

    // Default language to load the TTS.
    ECILanguageDialect default_language = eciGeneralAmericanEnglish;
  };

  static constexpr char kEciLibraryName[] = "libibmeci.so";

  TTS(AudioManager* audio, const Options& options = Options());
  ~TTS();

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
  // prefixing the text with a string to control the speed of the speech engine.
  // This would be the same as
  // AddText(GetPrefixString() + text), Synthesize().
  bool Say(const std::string& msg,
           const ECIVoiceAnnotation voice = NO_ANNOTATION);

  bool GenerateSilence(const int duration);

  bool Pause();

  bool Resume();

  bool Stop();

  std::string TTSVersion();

  // Selects the next available language.
  void NextLanguage();

  // Selects the previous available language.
  void PreviousLanguage();

  int GetSpeechRate() const { return speech_rate_; }

  void SetSpeechRate(const int speech_rate) { speech_rate_ = speech_rate; }

  const std::string GetPrefixString() const;

  // Helper method to output a task containing only the voice annotation, e.g, a
  // string annotation making the speech engine use the default parameters for
  // the selected voice.
  std::unique_ptr<SpeechTask> UseSelectedVoice(const ECIVoiceAnnotation voice);

  // Returns the sample rate configuration for the TTS/ECI classes equivalent
  // to the given sample rate in Hz.
  static SampleRate GetSampleRateConfig(int sample_rate);

 private:
  std::vector<ECILanguageDialect> languages_;
  int current_language_index_;

  std::unique_ptr<ECI> eci_;
  AudioManager* audio_;

  std::unique_ptr<SpeechTask> pending_task_;

  int speech_rate_ = 50;
};

class TTSError : public std::runtime_error {
 public:
  explicit TTSError(const std::string& arg) : runtime_error(arg) {}
};

#endif  // TTS_H_
