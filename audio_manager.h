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

#ifndef AUDIO_MANAGER_H_
#define AUDIO_MANAGER_H_

#include <memory>
#include <queue>
#include <string>

#include "audio_tasks.h"

class AlsaPlayer;
class ECI;

// Task manager for audio tasks.
//
// Manages a queue of AudioTask objects, invoking them when necessary in order
// to generate audio samples on demand and send them to the player.
class AudioManager {
public:
  explicit AudioManager(std::unique_ptr<AlsaPlayer> player);

  // Pushes one audio task into the queue.
  void Push(std::unique_ptr<AudioTask> task);

  // Runs the task at the front of the queue.  If the task returns FINISHED,
  // the task is removed from the queue.
  void Run();

  // Ends the current running task and clears all tasks from the queue.
  void Clear();

  // Returns the set of file descriptors that the AudioManager must wait for in
  // order to run the task at the front of the queue.
  std::vector<struct pollfd> GetPollDescriptors() const;

  // Returns the number of interesting events for the task at the front of the
  // queue.
  int GetPollEvents(struct pollfd* fds, int nfds) const;

  // Returns the player object.
  AlsaPlayer* player() const { return player_.get(); }

  // Returns whether there are any pending tasks in the queue.
  bool pending() const { return !queue_.empty(); }

 private:
  std::unique_ptr<AlsaPlayer> player_;
  std::queue<std::unique_ptr<AudioTask>> queue_;
};

#endif  // AUDIO_MANAGER_H_
