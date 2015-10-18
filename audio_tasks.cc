#include "audio_tasks.h"

// SpeechTask

SpeechTask::SpeechTask(ECI* eci)
    : eci_(eci) {}

void SpeechTask::AddText(const std::string& text) {
  ops_.push_back([=](ECI* eci){ eci->AddText(text); });
}

void SpeechTask::Synthesize() {
  ops_.push_back([=](ECI* eci){ eci->Synthesize(); });
}

void SpeechTask::StartTask(AlsaPlayer* player) {
  for (const auto& op : ops_) {
    op(eci_);
  }
  ops_.clear();
}

AudioTask::TaskResult SpeechTask::Run(AlsaPlayer* player) {
  if (eci_->Speaking()) {
    return CONTINUE;
  } else {
    return FINISHED;
  }
}
