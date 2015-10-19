#include "audio_tasks.h"

#include <cmath>

// SpeechTask

SpeechTask::SpeechTask(ECI* eci) : eci_(eci) {}

void SpeechTask::AddText(const std::string& text) {
  ops_.push_back([=](ECI* eci) { eci->AddText(text); });
}

void SpeechTask::Synthesize() {
  ops_.push_back([=](ECI* eci) { eci->Synthesize(); });
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

// ToneTask

ToneTask::ToneTask(float frequency, float amplitude, int duration_ms)
    : frequency_(frequency), amplitude_(amplitude), duration_ms_(duration_ms) {}

void ToneTask::StartTask(AlsaPlayer* player) {
  sample_rate_ = player->sample_rate();
  duration_samples_ = duration_ms_ * sample_rate_ / 1000;
}

AudioTask::TaskResult ToneTask::Run(AlsaPlayer* player) {
  // TODO: Only works in S16 mono format, make it more generic.
  short* buffer = reinterpret_cast<short*>(player->buffer());
  const std::size_t buffer_size = player->period_size();  // in samples.

  std::size_t pos = 0;
  while (pos < buffer_size && t_ < duration_samples_) {
    const double x = 2 * M_PI * frequency_ * (t_++) / sample_rate_;
    const int y = amplitude_ * std::sin(x) * (1 << 15);
    buffer[pos++] = y;
  }

  player->Play(pos);
  return t_ < duration_samples_ ? CONTINUE : FINISHED;
}
