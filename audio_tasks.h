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
  TaskResult Run(AlsaPlayer* player) override;

 private:
  ECI* eci_;

  using Operation = std::function<void(ECI*)>;
  std::vector<Operation> ops_;
};

#endif  // AUDIO_TASKS_H_
