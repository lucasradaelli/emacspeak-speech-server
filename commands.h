#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "tts.h"
#include "server_state.h"

// Base class that all commands of the speech server derive. Child classes are
// expected to implement their own version of Run(), with specific logic of each
// command. The communication with the speech server happens through calls to
// the tts and the server state objects.
class Command {
 public:
  Command(TTS* tts, ServerState* server_state) : tts_(tts), server_state_(server_state) {}
  virtual ~Command() = default;
  virtual bool Run() = 0;

 protected:
  TTS* tts_;
  ServerState* server_state_;
};

// Returns the version of the underlying speech engine. It produces an speech
// output with the result.
class VersionCommand : public Command {
 public:
  VersionCommand(TTS* tts, ServerState* server_state) : Command(tts,server_state) {}
  bool Run() override;
};

// Produces a speech output with the provided message. This command does not
// interrupt the current output in case any is playing. It uses the current set
// speech speed.
class TtsSayCommand : public Command {
 public:
  TtsSayCommand(TTS* tts, ServerState* server_state) : Command(tts,server_state) {}
  bool Run() override;
};

// Speakes immediately the given letter.
class LCommand : public Command{
public:
LCommand(TTS* tts, ServerState* server_state) : Command(tts,server_state) {}
  bool Run() override;
};

#endif // COMMANDS_H_
