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

#ifndef AUDIO_TASKS_H_
#define AUDIO_TASKS_H_

#include <string>
#include <vector>

#include "alsa_player.h"
#include "eci-c++.h"

// Audio task.
//
// This abstract base class represents a single task which is enqueued to
// produce audio output from the server, such as speech, tone or play.
class AudioTask {
 public:
  enum TaskResult {
    CONTINUE,
    FINISHED,
  };

  virtual ~AudioTask() {}

  // Starts the task. This method is called only once when the task reaches
  // the front of the queue, it already has exclusive access to the player
  // and will start running. It is used to prepare the task before the
  // Run() method is called.
  virtual void StartTask(AlsaPlayer* player) {}

  // Ends the task. This method is only called if StartTask() was previously
  // called for this task. The task may end either when it finished producing
  // its output (if Run() returnes FINISHED), or when some other server
  // command requested cleanup of any running tasks. The parameter finished
  // is used to determine the reason the task is being ended.
  virtual void EndTask(AlsaPlayer* player, bool finished) {}

  // Executes the task. Only the task at the front of the queue is executed
  // at once, and it is only executed when the player is ready to accept more
  // audio. The tasks is executed repeatedly until it returns FINISHED.
  virtual TaskResult Run(AlsaPlayer* player) = 0;

  // Returns the set of file descriptors that this task must wait for in order
  // to run. By default, all tasks wait on the AlsaPlayer object, but specific
  // kinds of tasks can override this method to wait on a different descriptor,
  // e.g. an external process.
  virtual std::vector<struct pollfd> GetPollDescriptors(
      AlsaPlayer* player) const;

  // Returns the number of interesting events for this task, given the output
  // descriptor list from a previous poll() call. If the return value is
  // different from zero, Run() will be called. By default, all tasks will run
  // whenever the AlsaPlayer object gets events on the ALSA descriptors.
  virtual int GetPollEvents(AlsaPlayer* player, struct pollfd* fds,
                            int nfds) const;

 protected:
  AudioTask() {}
};

// Speech synthesis task.
//
// This task controls the ECI library to synthesize speech, then pass the
// result to the player. Several ECI operations can be scheduled before the
// task starts. When the task starts, it invokes all operations on the ECI
// object in sequence to synthesize speech.
class SpeechTask : public AudioTask {
 public:
  explicit SpeechTask(ECI* eci);

  // Schedules an AddText(text) operation on ECI.
  void AddText(const std::string& text);

  // Schedules a Synthesize() operation on ECI.
  void Synthesize();

  // Base class overrides.
  void StartTask(AlsaPlayer* player) override;
  void EndTask(AlsaPlayer* player, bool finished) override;
  TaskResult Run(AlsaPlayer* player) override;

 private:
  ECI* eci_;

  using Operation = std::function<void(ECI*)>;
  std::vector<Operation> ops_;
};

// Tone synthesis task.
//
// This task generates a sinusoidal tone with the given parameters.
class ToneTask : public AudioTask {
 public:
  ToneTask(float frequency, float amplitude, int duration_ms);

  // Base class overrides.
  void StartTask(AlsaPlayer* player) override;
  TaskResult Run(AlsaPlayer* player) override;

 private:
  const float frequency_;
  const float amplitude_;
  const int duration_ms_;

  unsigned int sample_rate_ = 0;
  unsigned int duration_samples_ = 0;
  unsigned int t_ = 0;
};

// Play audio task.
//
// This class plays via an external process the given file.
class PlayTask : public AudioTask {
 public:
  explicit PlayTask(const std::string& file_path);

  // Starts the external process to play the file.
  void StartTask(AlsaPlayer* player) override;

  // Run() will only be called when the poll descriptors for this task are
  // returned. In this case, we wait for the external process to finish. For
  // this reason, this task does not need to do anything in this method .
  TaskResult Run(AlsaPlayer* player) override;

 private:
  // File path of the .wav file to be played.
  const std::string file_path_;
};

#endif  // AUDIO_TASKS_H_
