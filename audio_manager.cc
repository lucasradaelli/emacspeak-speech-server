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

#include "alsa_player.h"
#include "audio_manager.h"
#include "eci-c++.h"

#include <iostream>

AudioManager::AudioManager(std::unique_ptr<AlsaPlayer> player)
    : player_(std::move(player)) {}

void AudioManager::Push(std::unique_ptr<AudioTask> task) {
  if (queue_.empty()) {
    task->StartTask(player_.get());
  }
  queue_.push(std::move(task));
}

void AudioManager::Run() {
  if (queue_.empty()) return;

  AlsaPlayer* player = player_.get();
  AudioTask* task = queue_.front().get();

  if (task->Run(player) == AudioTask::CONTINUE) {
    return;
  }

  task->EndTask(player, true);
  queue_.pop();

  if (queue_.empty()) {
    player->Idle();
  } else {
    AudioTask* next_task = queue_.front().get();
    next_task->StartTask(player);
  }
}

void AudioManager::Clear() {
  if (queue_.empty()) return;

  AlsaPlayer* player = player_.get();
  AudioTask* task = queue_.front().get();

  task->EndTask(player, false);

  decltype(queue_) empty;
  std::swap(queue_, empty);
}

std::vector<pollfd> AudioManager::GetPollDescriptors() const {
  if (!queue_.empty()) {
    return queue_.front()->GetPollDescriptors(player_.get());
  } else {
    return {};
  }
}

int AudioManager::GetPollEvents(pollfd* fds, int nfds) const {
  if (!queue_.empty()) {
    return queue_.front()->GetPollEvents(player_.get(), fds, nfds);
  } else {
    return 0;
  }
}
