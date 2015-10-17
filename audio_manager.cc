#include "alsa_player.h"
#include "audio_manager.h"
#include "eci-c++.h"

#include <iostream>

AudioManager::AudioManager(std::unique_ptr<AlsaPlayer> player)
    : player_(std::move(player)) {}

void AudioManager::Push(std::unique_ptr<AudioTask> task) {
  if (queue_.empty()) {
    task->Prepare(player_.get());
  }
  queue_.push(std::move(task));
}

void AudioManager::Run() {
  if (pending() && queue_.front()->Run(player_.get()) == AudioTask::FINISHED) {
    queue_.pop();
    if (pending()) {
      queue_.front()->Prepare(player_.get());
    }
  }
}
