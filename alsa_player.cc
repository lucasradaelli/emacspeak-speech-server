#include "alsa_player.h"

#include <cstdio>
#include <iostream>
#include <sstream>

#include <alsa/asoundlib.h>
#include <sys/time.h>

AlsaPlayer::AlsaPlayer(const Options& options) : options_(options) {
  int error =
      snd_pcm_open(&pcm_, options.device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
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

AlsaPlayer::~AlsaPlayer() { snd_pcm_close(pcm_); }

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

void AlsaPlayer::RecoverFromUnderrun() {
  snd_pcm_status_t* status;
  snd_pcm_status_alloca(&status);

  int res = snd_pcm_status(pcm_, status);
  if (res < 0) {
    fprintf(stderr, "status error: %s", snd_strerror(res));
    exit(EXIT_FAILURE);
  }

  if (snd_pcm_status_get_state(status) == SND_PCM_STATE_XRUN) {
    struct timeval now, diff, tstamp;
    gettimeofday(&now, 0);
    snd_pcm_status_get_trigger_tstamp(status, &tstamp);
    timersub(&now, &tstamp, &diff);
    fprintf(stderr, "Underrun!!! (at least %.3f ms long)\n",
            diff.tv_sec * 1000 + diff.tv_usec / 1000.0);
    if ((res = snd_pcm_prepare(pcm_)) < 0) {
      fprintf(stderr, "xrun: prepare error: %s", snd_strerror(res));
      exit(EXIT_FAILURE);
    }
    return;  // ok, data should be accepted again
  }

  fprintf(stderr, "read/write error, state = %s",
          snd_pcm_state_name(snd_pcm_status_get_state(status)));
  exit(EXIT_FAILURE);
}

void AlsaPlayer::RecoverFromSuspend() {
  int res;

  fprintf(stderr, "Suspended. Trying resume. ");
  fflush(stderr);
  while ((res = snd_pcm_resume(pcm_)) == -EAGAIN)
    sleep(1);  // wait until suspend flag is released
  if (res < 0) {
    fprintf(stderr, "Failed. Restarting stream. ");
    fflush(stderr);
    if ((res = snd_pcm_prepare(pcm_)) < 0) {
      fprintf(stderr, "suspend: prepare error: %s", snd_strerror(res));
      exit(EXIT_FAILURE);
    }
  }

  fprintf(stderr, "Done.\n");
}

std::string AlsaError::GenerateMessage(const std::string& arg, int code) {
  std::ostringstream sstr;
  sstr << "AlsaError: " << arg << " (" << snd_strerror(code) << ").";
  return sstr.str();
}
