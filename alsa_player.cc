#include "alsa_player.h"

#include <cstdio>

#include <alsa/asoundlib.h>
#include <sys/time.h>

AlsaPlayer::AlsaPlayer(const Options &options) : options_(options) {
  int error = snd_pcm_open(&pcm_, "default", SND_PCM_STREAM_PLAYBACK, 0);

  if (error < 0) {
    throw AlsaError(snd_strerror(error));
  }

  error = snd_output_stdio_attach(&output_, stderr, 0);
  if (error < 0) {
    throw AlsaError(snd_strerror(error));
  }

  std::size_t chunk_bytes, bits_per_sample, bits_per_frame = 0;
  snd_pcm_uframes_t chunk_size, buffer_size = 0;

  snd_pcm_hw_params_alloca(&params_);

  error = snd_pcm_hw_params_any(pcm_, params_);
  if (error < 0) {
    throw AlsaError("PCM: Broken configuration: no configurations available.");
  }

  error = snd_pcm_hw_params_set_format(pcm_, params_, options_.sample_format);
  if (error < 0) {
    throw AlsaError("Sample format non available");
  }

  error = snd_pcm_hw_params_set_channels(pcm_, params_, options_.channels);
  if (error < 0) {
    throw AlsaError("Channels count non available");
  }

  unsigned int sample_rate = options_.sample_rate;
  error = snd_pcm_hw_params_set_rate_near(pcm_, params_, sample_rate, 0);
  //TODO:handle error

  error = snd_pcm_hw_params_set_access(pcm_, params_,
                                       SND_PCM_ACCESS_RW_INTERLEAVED);
  if (error < 0) {
    throw AlsaError("Access type not available");
  }

  error = snd_pcm_hw_params(pcm_, params_);
  if (error < 0) {
    throw AlsaError("Unable to install hw params:");
  }

  snd_pcm_hw_params_get_period_size(params_, &period_size_, 0);
  snd_pcm_hw_params_get_buffer_size(params_, &buffer_size_);

  buffer_.reset(new char[buffer_size_]);
}

AlsaPlayer::~AlsaPlayer() {
  snd_pcm_close(pcm_);
}

void AlsaPlayer::Reset() {
  snd_pcm_drop(pcm_);
  snd_pcm_prepare(pcm_);
}

std::size_t AlsaPlayer::Play(std::size_t count) {
  ssize_t r;
  ssize_t result = 0;

  short* data = reinterpret_cast<short*>(buffer_.get());

  while (count > 0) {
    r = snd_pcm_writei(pcm_, data, count);
    if (r == -EAGAIN || (r >= 0 && (size_t)r < count)) {
      snd_pcm_wait(pcm_, 1000);
    } else if (r == -EPIPE) {
      xrun();
    } else if (r == -ESTRPIPE) {
      suspend();
    } else if (r < 0) {
      fprintf(stderr, "write error: %s", snd_strerror(r));
      exit(EXIT_FAILURE);
    }
    if (r > 0) {
      result += r;
      count -= r;
      data += r;
    }
  }

  return result;
}

void AlsaPlayer::xrun() {
  snd_pcm_status_t *status;
  int res;

  snd_pcm_status_alloca(&status);

  if ((res = snd_pcm_status(pcm_, status)) < 0) {
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
    return;  // ok, data should be accepted
    // again
  }

  fprintf(stderr, "read/write error, state = %s",
          snd_pcm_state_name(snd_pcm_status_get_state(status)));
  exit(EXIT_FAILURE);
}

void AlsaPlayer::suspend() {
  int res;

  fprintf(stderr, "Suspended. Trying resume. ");
  fflush(stderr);
  while ((res = snd_pcm_resume(pcm_)) == -EAGAIN)
    sleep(1); /* wait until suspend flag is * * released
               */
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

