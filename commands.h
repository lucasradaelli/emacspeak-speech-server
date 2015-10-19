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
  Command() = default;
  virtual ~Command() = default;
  virtual bool Run(TTS* tts, ServerState* server_state) = 0;
};

// Returns the version of the underlying speech engine. It produces an speech
// output with the result.
class VersionCommand : public Command {
 public:
  VersionCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

// Produces a speech output with the provided message. This command does not
// interrupt the current output in case any is playing. It uses the current set
// speech speed.
class TtsSayCommand : public Command {
 public:
  TtsSayCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

// Speakes immediately the given letter.
class LCommand : public Command {
 public:
  LCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

// Pauses the speech. The speech can be resumed by calling TtsResumeCommand.
class TtsPauseCommand : public Command {
 public:
  TtsPauseCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

// Resumes the speech previously paused.
class TtsResumeCommand : public Command {
 public:
  TtsResumeCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

// Stops the speech.
class SCommand : public Command {
 public:
  SCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

class QCommand : public Command {
 public:
  QCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

class DCommand : public Command {
 public:
  DCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

// Queues a code to be sent to the speech engine. No text formatting is applied
// to the passed args.
class CCommand : public Command {
 public:
  CCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

// Queues a message of type silence.
class ShCommand : public Command {
 public:
  ShCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

class TCommand : public Command {
 public:
  TCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

class TtsSetSpeechRateCommand : public Command {
 public:
  TtsSetSpeechRateCommand() = default;
  bool Run(TTS* tts, ServerState* server_state) override;
};

#endif  // COMMANDS_H_
