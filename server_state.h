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

  TextFormatter::PunctuationMode GetPunctuationMode() const {
    return punctuation_mode_;
  }

  void SetPunctuationMode(
      const TextFormatter::PunctuationMode punctuation_mode) {
    punctuation_mode_ = punctuation_mode;
  }

 private:
  AudioManager* audio_;
  std::queue<std::unique_ptr<AudioTask>> queue_;

  std::unique_ptr<TextFormatter> text_formatter_;

  TextFormatter::PunctuationMode punctuation_mode_ = TextFormatter::ALL;
};

#endif  // SERVER_STATE_H_
