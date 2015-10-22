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

#ifndef COMMAND_GENERATOR_H_
#define COMMAND_GENERATOR_H_

#include <string>
#include <unordered_map>

#include "commands.h"
#include "server_state.h"

class CommandRegistry {
 public:
  CommandRegistry();
  ~CommandRegistry() = default;

  Command* GetCommand(const std::string& command_name);

 private:
  std::unordered_map<std::string, std::unique_ptr<Command>> commands_map_;
};

#endif  // COMMAND_GENERATOR_H_
