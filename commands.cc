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

#include "commands.h"

#include <memory>
#include <sstream>

#include "text_formatter.h"

using std::string;
using std::unique_ptr;

bool VersionCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  const string msg = "ViaVoice " + ctx.tts->TTSVersion();
  return ctx.tts->Say(msg) && ctx.tts->SubmitTask();
}

bool TtsSayCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }
  return ctx.tts->Say(cmd.arguments[0]) && ctx.tts->SubmitTask();
}

bool LCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }
  const string args = cmd.arguments[0];
  string letter_pitch;
  if (isupper(args[0])) {
    letter_pitch = "`vb80 ";
  }
  const string msg = letter_pitch + "`ts2 " + args + " `ts0";
  return ctx.tts->Say(msg) && ctx.tts->SubmitTask();
}

bool TtsPauseCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  ctx.tts->Pause();
  return true;
}

bool TtsResumeCommand::Run(const StatementInfo& cmd,
                           const CommandContext& ctx) {
  ctx.tts->Resume();
  return true;
}

bool SCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (ctx.tts->Stop()) {
    ctx.server_state->ClearQueue();
    return true;
  }
  return false;
}

bool QCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }
  string processed_message = ctx.server_state->text_formatter()->Format(
      cmd.arguments[0], ctx.server_state->GetPunctuationMode());
  ctx.tts->Say(processed_message);
  ctx.server_state->queue().push(ctx.tts->ReleaseTask());
  return true;
}

bool CCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }
  ctx.tts->Output(cmd.arguments[0]);
  ctx.server_state->queue().push(ctx.tts->ReleaseTask());
  return true;
}

bool DCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  while (!ctx.server_state->queue().empty()) {
    auto& task = ctx.server_state->queue().front();
    ctx.server_state->audio()->Push(std::move(task));
    ctx.server_state->queue().pop();
  }
  return true;
}

bool ShCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }
  int duration = std::stoi(cmd.arguments[0]);
  if (duration <= 0) {
    return false;
  }

  ctx.tts->GenerateSilence(duration);
  ctx.server_state->queue().push(ctx.tts->ReleaseTask());
  return true;
}

bool TCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 2) {
    return false;
  }

  float frequency = std::stof(cmd.arguments[0]);
  float length = std::stof(cmd.arguments[1]);

  if (frequency <= 0.0f || length < 0.0f) {
    return false;
  }

  // Amplitude chosen to be comfortable to listen, more or less on the same
  // level as the speech.
  const float amplitude = 0.3f;

  std::unique_ptr<ToneTask> tone(new ToneTask(frequency, amplitude, length));
  ctx.server_state->queue().push(std::move(tone));
  return true;
}

bool TtsSetSpeechRateCommand::Run(const StatementInfo& cmd,
                                  const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }

  int speech_rate = std::stoi(cmd.arguments[0]);
  if (speech_rate <= 0) {
    return false;
  }

  ctx.tts->SetSpeechRate(speech_rate);
  return true;
}

bool TtsSetPunctuationsCommand::Run(const StatementInfo& cmd,
                                    const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }

  const string mode = cmd.arguments[0];
  TextFormatter::PunctuationMode punctuation_mode;
  if (mode == "all") {
    punctuation_mode = TextFormatter::ALL;
  } else if (mode == "some") {
    punctuation_mode = TextFormatter::SOME;
  } else if (mode == "none") {
    punctuation_mode = TextFormatter::NONE;
  } else {
    return false;
  }

  ctx.server_state->SetPunctuationMode(punctuation_mode);
  return true;
}
