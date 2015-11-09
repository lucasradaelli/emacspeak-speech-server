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

#ifndef ALSA_PLAYER_H_
#define ALSA_PLAYER_H_

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <poll.h>
#include <alsa/asoundlib.h>

// PCM Audio Player using ALSA.
//
// This class is responsible for handling communication with the ALSA drivers.
class AlsaPlayer {
 public:
  // Player options.
  struct Options {
    Options() noexcept {}

    // Whether to enable verbose output.
    bool verbose = false;

    // Which ALSA PCM device to open.
    std::string device = "default";

    // Sample format to use.
    snd_pcm_format_t sample_format = SND_PCM_FORMAT_S16;

    // Sample rate, in Hz.
    unsigned int sample_rate = 11025;  // Hz

    // Number of channels.
    unsigned int channels = 1;
  };

  AlsaPlayer(const Options& options = Options());
  virtual ~AlsaPlayer();

  // Read-only accessors.
  snd_pcm_format_t sample_format() const { return options_.sample_format; }
  unsigned int sample_rate() const { return options_.sample_rate; }
  unsigned int period_size() const { return period_size_; }
  char* buffer() const { return buffer_.get(); }

  // ALSA driver descriptors for polling.
  std::vector<struct pollfd> GetPollDescriptors() const;
  int GetPollEvents(struct pollfd *fds, int nfds) const;

  std::size_t Play(int count);
  void Drain();
  void Pause();
  void Resume();

  // Sets the end of a segment of audio, informing to the player that there
  // will be no more audio for awhile and that an eventual underrun is
  // expected.
  void Idle();

  void Interrupt();

 private:
  void SetupHwParams();
  void SetupSwParams();
  void RecoverFromUnderrun();
  void RecoverFromSuspend();

  // Returns a string reporting the ALSA PCM configuration, for debugging.
  std::string GetAlsaPcmDump();

  const Options options_;
  snd_pcm_t* pcm_ = nullptr;
  snd_pcm_uframes_t buffer_size_ = 0;
  snd_pcm_uframes_t period_size_ = 0;
  std::size_t frame_size_ = 0;
  bool idle_ = false;

  std::unique_ptr<char[]> buffer_;
};

// Exception thrown for ALSA errors.
class AlsaError : public std::runtime_error {
 public:
  explicit AlsaError(const std::string& arg, int code)
      : runtime_error(GenerateMessage(arg, code)), code_(code) {}
  virtual int code() const { return code_; }

 private:
  static std::string GenerateMessage(const std::string& arg, int code);

  int code_;
};

#endif  // ALSA_PLAYER_H_
