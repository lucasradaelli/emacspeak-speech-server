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

enum ServerStatus { DATA_PROCESSED = 0, COMMAND_PENDING = 1 };

class ServerState {
 public:
  explicit ServerState(AudioManager* audio);
  ~ServerState() = default;

  std::string* GetMutableLastArgs() const { return last_args_.get(); }

  const std::string& GetLastArgs() const { return *last_args_.get(); }

  ServerStatus GetServerStatus() const { return server_status_; }

  void SetServerStatus(const ServerStatus server_status) {
    server_status_ = server_status;
  }

  AudioManager* audio() { return audio_; }
  std::queue<std::unique_ptr<AudioTask>>& queue() { return queue_; }

  void ClearQueue();

 private:
  AudioManager* audio_;
  std::queue<std::unique_ptr<AudioTask>> queue_;

  std::string last_command_;
  ServerStatus server_status_;

  std::unique_ptr<std::string> last_args_;
};

#endif  // SERVER_STATE_H_
