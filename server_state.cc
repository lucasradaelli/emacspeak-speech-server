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

#include "server_state.h"

#include <cstdio>
#include <sstream>

using std::string;

ServerState::ServerState(AudioManager* audio)
    : audio_(audio),
      server_status_(DATA_PROCESSED),
      last_args_(new string()),
      text_formatter_(new ECITextFormatter()) {}

void ServerState::ClearQueue() {
  if (queue_.size() == 0) {
    return;
  }
  decltype(queue_) empty;
  std::swap(queue_, empty);
  return;
}
