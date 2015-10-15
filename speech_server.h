#ifndef SPEECH_SERVER_H_
#define SPEECH_SERVER_H_

#include <tuple>

#include "command_generator.h"
#include "tts.h"
#include "server_state.h"

class SpeechServer {
 public:
  SpeechServer(TTS* tts) : tts_(tts) {
    cmd_registry_.reset(new CommandRegistry());
  }
  ~SpeechServer() = default;

  int MainLoop();

  std::tuple<std::string, std::string> ProcessInput();

 private:
  std::string ReadLine();

  TTS* tts_;
  ServerState server_state_;
  std::unique_ptr<CommandRegistry> cmd_registry_;

  char read_buffer_[4096];
  size_t buffer_size_ = 0;
  size_t buffer_start_;
};

#endif  // SPEECH_SERVER_H_
