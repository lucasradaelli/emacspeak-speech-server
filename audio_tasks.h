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

  // Prepares the task. This method is called once when the task reaches the
  // front of the queue.
  virtual void Prepare(AlsaPlayer* player) {}

  // Executes the task. Only the task at the front of the queue is executed
  // at once, and it is only executed when the player is ready to accept more
  // audio. The tasks is executed repeatedly until it returns FINISHED.
  virtual TaskResult Run(AlsaPlayer* player) = 0;

protected:
  AudioTask() {}
};

// Speech synthesis task.
//
// This task uses controls the ECI library to synthesize speech, then pass
// the result to the player. It provides some flexibility via the Setup()
// method, which sets a callback that receives the ECI object and can call
// any sequence of commands on that object.
class SpeechTask : public AudioTask {
public:
  // Type of the setup callback function.
  using Callback = std::function<void(ECI*)>;

  explicit SpeechTask(ECI* eci);
  ~SpeechTask() override;

  // Sets the function that is called to synthesize the speech using the ECI
  // object.
  void Setup(Callback callback);

  // Base class overrides.
  void Prepare(AlsaPlayer* player) override;
  TaskResult Run(AlsaPlayer* player) override;

private:
  ECI* eci_;
  Callback callback_;
};

#endif  // AUDIO_TASKS_H_
