#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "tts.h"
#include "server_state.h"


class Command {
 public:
  Command(TTS* tts, ServerState* server_state) : tts_(tts), server_state_(server_state) {}
  virtual ~Command() = default;
  virtual bool Run() = 0;

 protected:
  TTS* tts_;
  ServerState* server_state_;
};


class VersionCommand : public Command {
 public:
  VersionCommand(TTS* tts, ServerState* server_state) : Command(tts,server_state) {}
  bool Run() override;
};


class TtsSayCommand : public Command {
 public:
  TtsSayCommand(TTS* tts, ServerState* server_state) : Command(tts,server_state) {}
  bool Run() override;

};

#endif // COMMANDS_H_
