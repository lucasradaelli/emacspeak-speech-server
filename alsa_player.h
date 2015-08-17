#ifndef _ALSA_PLAYER_H_
#define _ALSA_PLAYER_H_

#include <memory>
#include <stdexcept>

#include <alsa/asoundlib.h>

class AlsaPlayer {
 public:
  struct Options {
    snd_pcm_format_t sample_format = SND_PCM_FORMAT_S16;
    unsigned int sample_rate = 22050;
    unsigned int channels = 1;
  };

  AlsaPlayer(const Options& options = Options());
  virtual ~AlsaPlayer();

  snd_pcm_format_t sample_format() const { return options_.sample_format; }
  unsigned int sample_rate() const { return options_.sample_rate; }

  void Reset() const;
  unsigned int NumSamples() const { return period_size_; }
  char* Buffer() const { return buffer_.get(); }
  std::size_t Play(std::size_t count);

 private:
  void xrun();
  void suspend();

  const Options options_;
  snd_pcm_t* pcm_ = nullptr;
  snd_pcm_hw_params_t* params_ = nullptr;
  snd_output_t* output_ = nullptr;
  snd_pcm_uframes_t period_size_ = 0;
  snd_pcm_uframes_t buffer_size_ = 0;

  std::unique_ptr<char[]> buffer_;
};

class AlsaError : public std::runtime_error {
public:
  explicit AlsaError(const std::string& arg) : runtime_error(arg) {}
};

#endif
