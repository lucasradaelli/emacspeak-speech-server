#include "audio_tasks.h"

// SpeechTask

SpeechTask::SpeechTask(ECI* eci)
    : eci_(eci) {}

SpeechTask::~SpeechTask() {}

void SpeechTask::Setup(Callback callback) {
  callback_ = std::move(callback);
}

void SpeechTask::Prepare(AlsaPlayer* player) {
  callback_(eci_);
}

AudioTask::TaskResult SpeechTask::Run(AlsaPlayer* player) {
  if (eci_->Speaking()) {
    return CONTINUE;
  } else {
    return FINISHED;
  }
}
