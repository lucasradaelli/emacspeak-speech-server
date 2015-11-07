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

#ifndef SERVER_STATE_H_
#define SERVER_STATE_H_

#include <queue>
#include <memory>
#include <string>

#include "audio_manager.h"
#include "audio_tasks.h"
#include "text_formatter.h"

class ServerState {
 public:
  explicit ServerState(AudioManager* audio);
  ~ServerState() = default;

  AudioManager* audio() { return audio_; }
  std::queue<std::unique_ptr<AudioTask>>& queue() { return queue_; }

  void ClearQueue();

  TextFormatter* text_formatter() { return text_formatter_.get(); }

  bool verbose() const { return verbose_; }
  void set_verbose(bool value) { verbose_ = value; }

  TextFormatter::PunctuationMode punctuation_mode() const {
    return punctuation_mode_;
  }
  void set_punctuation_mode(
      const TextFormatter::PunctuationMode punctuation_mode) {
    punctuation_mode_ = punctuation_mode;
  }

  bool tts_split_caps() const { return tts_split_caps_; }

  void set_tts_split_caps(const bool tts_split_caps) {
    tts_split_caps_ = tts_split_caps;
  }

  bool tts_capitalize() const { return tts_capitalize_; }

  void set_tts_capitalize(const bool tts_capitalize) {
    tts_capitalize_ = tts_capitalize;
  }

  bool tts_allcaps_beep() const { return tts_allcaps_beep_; }

  void set_tts_allcaps_beep(const bool tts_allcaps_beep) {
    tts_allcaps_beep_ = tts_allcaps_beep;
  }

 private:
  AudioManager* audio_;
  std::queue<std::unique_ptr<AudioTask>> queue_;

  std::unique_ptr<TextFormatter> text_formatter_;

  bool verbose_ = false;
  TextFormatter::PunctuationMode punctuation_mode_ = TextFormatter::ALL;

  // Set state of split caps processing. Set this to true to
  // speak mixed-case (AKA Camel Case) identifiers.
  bool tts_split_caps_;

  // Set this to true to indicate capitalization via a voice  pitch.
  bool tts_capitalize_;

  //         Setting this to true produces  a high-pitched beep when speaking
  //         words that are in
  //  all-caps, e.g. abbreviations.
  bool tts_allcaps_beep_;
};

#endif  // SERVER_STATE_H_
