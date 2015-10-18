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

  // Returns the player object.
  AlsaPlayer* player() const { return player_.get(); }

  // Returns whether there are any pending tasks in the queue.
  bool pending() const { return !queue_.empty(); }

 private:
  std::unique_ptr<AlsaPlayer> player_;
  std::queue<std::unique_ptr<AudioTask>> queue_;
};

#endif  // AUDIO_MANAGER_H_
