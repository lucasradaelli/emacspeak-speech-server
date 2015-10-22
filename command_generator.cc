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

#include "command_generator.h"

#include <unordered_map>
#include <iostream>

#include "commands.h"

using std::unique_ptr;

CommandRegistry::CommandRegistry() {
  commands_map_["version"] = unique_ptr<Command>(new VersionCommand());
  commands_map_["tts_say"] = unique_ptr<Command>(new TtsSayCommand());
  commands_map_["l"] = unique_ptr<Command>(new LCommand());
  commands_map_["tts_pause"] = unique_ptr<Command>(new TtsPauseCommand());
  commands_map_["tts_resume"] = unique_ptr<Command>(new TtsResumeCommand());
  commands_map_["s"] = unique_ptr<Command>(new SCommand());
  commands_map_["q"] = unique_ptr<Command>(new QCommand());
  commands_map_["d"] = unique_ptr<Command>(new DCommand());
  commands_map_["c"] = unique_ptr<Command>(new CCommand());
  commands_map_["sh"] = unique_ptr<Command>(new ShCommand());
  commands_map_["t"] = unique_ptr<Command>(new TCommand());
  commands_map_["tts_set_speech_rate"] =
      unique_ptr<Command>(new TtsSetSpeechRateCommand());
}

Command* CommandRegistry::GetCommand(const std::string& command_name) {
  if (commands_map_.find(command_name) == commands_map_.end()) {
    std::cout << "not found in the registry\n";
    return nullptr;
  }
  std::cout << "found in the registry\n";
  return commands_map_[command_name].get();
}
