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

#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>

#include "audio_manager.h"
#include "command_generator.h"
#include "input_parser.h"
#include "tts.h"
#include "server_state.h"

class SpeechServer {
 public:
  SpeechServer(AudioManager* audio, TTS* tts);
  ~SpeechServer();

  int MainLoop();

  bool verbose() const { return server_state_.verbose(); }
  void set_verbose(bool value) { server_state_.set_verbose(value); }

 private:
  void ProcessCommands();

  AudioManager* audio_;
  TTS* tts_;
  ServerState server_state_;
  InputParser input_parser_;
  std::unique_ptr<CommandRegistry> cmd_registry_;
};

// Irrecoverable error in speech server.
class SpeechServerError : public std::runtime_error {
 public:
  explicit SpeechServerError(const std::string& arg) : runtime_error(arg) {}
};

#endif  // SPEECH_SERVER_H_
