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

#include "audio_tasks.h"

#include <cmath>
#include <sstream>

using std::string;
using std::vector;

namespace {

// Default program to play the wav files.
static const char kDefaultPlayProgram[] = "aplay";

}  // namespace

// AudioTask

vector<pollfd> AudioTask::GetPollDescriptors(AlsaPlayer* player) const {
  return player->GetPollDescriptors();
}

int AudioTask::GetPollEvents(AlsaPlayer* player, pollfd* fds, int nfds) const {
  return player->GetPollEvents(fds, nfds);
}

// SpeechTask

SpeechTask::SpeechTask(ECI* eci) : eci_(eci) {}

void SpeechTask::AddText(const string& text) {
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

void SoundTask::StartTask(AlsaPlayer* player) {
  std::ostringstream command;
  command << kDefaultPlayProgram << " " << file_path_;
  process_ = popen(command.str().c_str(), "w");
  if (process_ == nullptr) {
    // todo: implement correct error handling.
    return;
  }
  fd_ = fileno(process_);
  return;
}

void SoundTask::EndTask(AlsaPlayer* player, bool finished) {
  pclose(process_);
  return;
}

vector<pollfd> SoundTask::GetPollDescriptors(AlsaPlayer* player) const {
  vector<struct pollfd> fds(1);
  fds[0].fd = fd_;
  fds[0].events = POLLERR;
  fds[0].revents = 0;
  return fds;
}

int SoundTask::GetPollEvents(AlsaPlayer* player, pollfd* fds, int nfds) const {
  unsigned short revents = 0;
  for (int i = 0; i < nfds; ++i) {
    if (fds[i].fd != fd_) continue;
    // Waits for a POLLERR event -- this means that the external process has
    // finished.
    if (fds[i].revents == POLLERR) {
      revents = fds[i].revents;
      break;
    }
  }
  return revents;
}
