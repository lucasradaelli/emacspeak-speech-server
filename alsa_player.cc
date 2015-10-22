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

#include "alsa_player.h"

#include <cstdio>
#include <iostream>
#include <sstream>

#include <alsa/asoundlib.h>
#include <sys/time.h>

AlsaPlayer::AlsaPlayer(const Options& options) : options_(options) {
  int error = snd_pcm_open(&pcm_, options.device.c_str(),
                           SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
  if (error < 0) {
    throw AlsaError("Failed to open ALSA sound device", error);
  }

  SetupPCM();

  // Allocate the PCM buffer.
  const std::size_t sample_size =
      snd_pcm_format_physical_width(options_.sample_format) / 8;
  frame_size_ = options_.channels * sample_size;
  buffer_.reset(new char[period_size_ * frame_size_]);
}

AlsaPlayer::~AlsaPlayer() {
  // Cleanup.
  snd_pcm_close(pcm_);
}

void AlsaPlayer::SetupPCM() {
  auto check = [](int error) {
    if (error < 0) {
      throw AlsaError("Requested PCM configuration was rejected by ALSA",
                      error);
    }
  };

  snd_pcm_hw_params_t* params = nullptr;
  snd_pcm_hw_params_alloca(&params);

  // Set ALSA hardware parameters for the requested PCM stream.
  check(snd_pcm_hw_params_any(pcm_, params));
  check(snd_pcm_hw_params_set_access(pcm_, params,
                                     SND_PCM_ACCESS_RW_INTERLEAVED));
  check(snd_pcm_hw_params_set_format(pcm_, params, options_.sample_format));
  check(snd_pcm_hw_params_set_rate(pcm_, params, options_.sample_rate, 0));
  check(snd_pcm_hw_params_set_channels(pcm_, params, options_.channels));

  // Write ALSA hardware parameters to the device.
  check(snd_pcm_hw_params(pcm_, params));

  // Retrieve period and buffer sizes from ALSA.
  check(snd_pcm_hw_params_get_period_size(params, &period_size_, 0));
  check(snd_pcm_hw_params_get_buffer_size(params, &buffer_size_));
}

std::vector<struct pollfd> AlsaPlayer::GetPollDescriptors() const {
  std::vector<struct pollfd> fds;

  int num_descriptors = snd_pcm_poll_descriptors_count(pcm_);
  if (num_descriptors > 0) {
    fds.resize(num_descriptors);
    int error = snd_pcm_poll_descriptors(pcm_, fds.data(), fds.size());
    if (error < 0) {
      throw AlsaError("Failed to retrieve descriptors for poll() from ALSA.",
                      error);
    }
  }

  return fds;
}

int AlsaPlayer::GetPollEvents(struct pollfd* fds, int nfds) const {
  unsigned short revents;
  int error = snd_pcm_poll_descriptors_revents(pcm_, fds, nfds, &revents);
  if (error < 0) {
    throw AlsaError("Failed to retrieve descriptors for poll() from ALSA.",
                    error);
  }
  return revents;
}

void AlsaPlayer::Interrupt() {
  snd_pcm_drop(pcm_);
  snd_pcm_prepare(pcm_);
}

std::size_t AlsaPlayer::Play(int count) {
  std::size_t result = 0;
  char* data = buffer_.get();

  while (count > 0) {
    snd_pcm_sframes_t r = snd_pcm_writei(pcm_, data, count);
    if (r < 0) {
      if (r == -EAGAIN) {
        // TODO: Do not block on ALSA.
        snd_pcm_wait(pcm_, 1000);
      } else if (r == -EPIPE) {
        RecoverFromUnderrun();
      } else if (r == -ESTRPIPE) {
        RecoverFromSuspend();
      } else {
        throw AlsaError("Failed to write PCM to ALSA.", r);
      }
    } else {
      if (r < count) {
        // TODO: Do not block on ALSA.
        snd_pcm_wait(pcm_, 1000);
      }
      count -= r;
      result += r;
      data += r * frame_size_;
    }
  }

  return result;
}

void AlsaPlayer::Drain() {
  snd_pcm_drain(pcm_);
  snd_pcm_prepare(pcm_);
}

void AlsaPlayer::Pause() {
  snd_pcm_pause(pcm_, true);
}

void AlsaPlayer::Resume() {
  snd_pcm_pause(pcm_, false);
}

void AlsaPlayer::RecoverFromUnderrun() {
  snd_pcm_status_t* status;
  snd_pcm_status_alloca(&status);

  int error = snd_pcm_status(pcm_, status);
  if (error < 0) {
    throw AlsaError("Failed to get PCM status during recover", error);
  }

  snd_pcm_state_t state = snd_pcm_status_get_state(status);

  if (state != SND_PCM_STATE_XRUN) {
    std::ostringstream sstr;
    sstr << "Stream found in unexpected state " << snd_pcm_state_name(state)
         << " while recovering from underrun";
    throw AlsaError(sstr.str(), -EPIPE);
  }

  struct timeval now, diff, tstamp;
  gettimeofday(&now, 0);
  snd_pcm_status_get_trigger_tstamp(status, &tstamp);
  timersub(&now, &tstamp, &diff);

  std::cerr << "AlsaPlayer: Sound buffer underrun ("
            << (diff.tv_sec * 1000 + diff.tv_usec / 1000.0)
            << "ms)" << std::endl;

  if ((error = snd_pcm_prepare(pcm_)) < 0) {
    throw AlsaError("Failed to recover from underrun", error);
  }
}

void AlsaPlayer::RecoverFromSuspend() {
  std::cerr << "AlsaPlayer: ALSA was suspended, trying to resume..."
            << std::endl;

  int error;
  while ((error = snd_pcm_resume(pcm_)) == -EAGAIN) {
    // TODO: Do not block on ALSA.
    sleep(1);  // wait until suspend flag is released
  }

  if (error < 0) {
    std::cerr << "AlsaPlayer: Failed to resume, trying to restart..."
              << std::endl;

    if ((error = snd_pcm_prepare(pcm_)) < 0) {
      throw AlsaError("Failed to recover from suspend", error);
    }
  }

  std::cerr << "AlsaPlayer: Recovered from suspend." << std::endl;
}

std::string AlsaError::GenerateMessage(const std::string& arg, int code) {
  std::ostringstream sstr;
  sstr << "AlsaError: " << arg << " (" << snd_strerror(code) << ").";
  return sstr.str();
}
