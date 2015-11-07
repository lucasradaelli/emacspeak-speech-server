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

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "input_parser.h"
#include "tts.h"
#include "server_state.h"

struct CommandContext {
  TTS* tts = nullptr;
  ServerState* server_state = nullptr;
};

// Base class that all commands of the speech server derive. Child classes are
// expected to implement their own version of Run(), with specific logic of each
// command. The communication with the speech server happens through calls to
// the tts and the server state objects.
class Command {
 public:
  Command() = default;
  virtual ~Command() = default;
  virtual bool Run(const StatementInfo& cmd, const CommandContext& ctx) = 0;
};

// Returns the version of the underlying speech engine. It produces an speech
// output with the result.
class VersionCommand : public Command {
 public:
  VersionCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

// Produces a speech output with the provided message. This command does not
// interrupt the current output in case any is playing. It uses the current set
// speech speed.
class TtsSayCommand : public Command {
 public:
  TtsSayCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

// Speakes immediately the given letter.
class LCommand : public Command {
 public:
  LCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

// Pauses the speech. The speech can be resumed by calling TtsResumeCommand.
class TtsPauseCommand : public Command {
 public:
  TtsPauseCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

// Resumes the speech previously paused.
class TtsResumeCommand : public Command {
 public:
  TtsResumeCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

// Stops the speech.
class SCommand : public Command {
 public:
  SCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

class QCommand : public Command {
 public:
  QCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

class DCommand : public Command {
 public:
  DCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

// Queues a code to be sent to the speech engine. No text formatting is applied
// to the passed args.
class CCommand : public Command {
 public:
  CCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

// Queues a file to be played.
class ACommand : public Command {
 public:
  ACommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

// Plays an audio file immediately.
class PCommand : public Command {
 public:
  PCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

// Queues a message of type silence.
class ShCommand : public Command {
 public:
  ShCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

class TCommand : public Command {
 public:
  TCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

class TtsSetSpeechRateCommand : public Command {
 public:
  TtsSetSpeechRateCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

class TtsSetPunctuationsCommand : public Command {
 public:
  TtsSetPunctuationsCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

class TtsSyncStateCommand : public Command {
 public:
  TtsSyncStateCommand() = default;
  bool Run(const StatementInfo& cmd, const CommandContext& ctx) override;
};

#endif  // COMMANDS_H_
