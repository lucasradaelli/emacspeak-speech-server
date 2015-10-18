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

  if (task->Run(player) == AudioTask::FINISHED) {
    task->EndTask(player, true);
    queue_.pop();

    if (!queue_.empty()) {
      AudioTask* next_task = queue_.front().get();
      next_task->StartTask(player);
    }
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
