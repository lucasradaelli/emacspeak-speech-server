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

#ifndef SPEECH_SERVER_H_
#define SPEECH_SERVER_H_

#include <tuple>

#include "audio_manager.h"
#include "command_generator.h"
#include "tts.h"
#include "server_state.h"

class SpeechServer {
 public:
  SpeechServer(AudioManager* audio, TTS* tts)
      : audio_(audio), tts_(tts), server_state_(audio_) {
    cmd_registry_.reset(new CommandRegistry());
  }
  ~SpeechServer() = default;

  int MainLoop();

  std::tuple<std::string, std::string> ProcessInput();

 private:
  std::string ReadLine();

  AudioManager* audio_;
  TTS* tts_;
  ServerState server_state_;
  std::unique_ptr<CommandRegistry> cmd_registry_;

  char read_buffer_[4096];
  std::size_t buffer_size_ = 0;
  std::size_t buffer_start_;
};

#endif  // SPEECH_SERVER_H_
