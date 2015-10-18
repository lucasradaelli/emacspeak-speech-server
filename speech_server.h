#ifndef SPEECH_SERVER_H_
#define SPEECH_SERVER_H_

#include <tuple>

#include "audio_manager.h"
#include "command_generator.h"
#include "tts.h"
#include "server_state.h"

class SpeechServer {
 public:
  SpeechServer(AudioManager* audio, TTS* tts)
      : audio_(audio), tts_(tts), server_state_(audio_) {
    cmd_registry_.reset(new CommandRegistry());
  }
  ~SpeechServer() = default;

  int MainLoop();

  std::tuple<std::string, std::string> ProcessInput();

 private:
  std::string ReadLine();

  AudioManager* audio_;
  TTS* tts_;
  ServerState server_state_;
  std::unique_ptr<CommandRegistry> cmd_registry_;

  char read_buffer_[4096];
  std::size_t buffer_size_ = 0;
  std::size_t buffer_start_;
};

#endif  // SPEECH_SERVER_H_
