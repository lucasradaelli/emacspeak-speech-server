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
  return ctx.tts->Say(msg, TTS::DEFAULT_VOICE) && ctx.tts->SubmitTask();
}

bool TtsSayCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }
  const string processed_msg =
      ctx.server_state->text_formatter()->FormatPause(cmd.arguments[0]);
  return ctx.tts->Say(processed_msg, TTS::DEFAULT_VOICE) &&
         ctx.tts->SubmitTask();
}

bool LCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }
  const string msg =
      ctx.server_state->text_formatter()->FormatSingleChar(cmd.arguments[0][0]);
  return ctx.tts->Say(msg, TTS::DEFAULT_VOICE) && ctx.tts->SubmitTask();
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
      cmd.arguments[0], ctx.server_state->punctuation_mode(),
      ctx.server_state->tts_split_caps(), ctx.server_state->tts_capitalize(),
      ctx.server_state->tts_allcaps_beep());
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

bool ACommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }
  std::unique_ptr<PlayTask> task(new PlayTask(cmd.arguments[0]));
  ctx.server_state->queue().push(std::move(task));
  return true;
}

bool PCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }
  std::unique_ptr<PlayTask> task(new PlayTask(cmd.arguments[0]));
  ctx.server_state->audio()->Push(std::move(task));
  return true;
}

bool DCommand::Run(const StatementInfo& cmd, const CommandContext& ctx) {
  // Annotates all the messages to be dispatched to speak with the default
  // voice.
  ctx.server_state->audio()->Push(
      ctx.tts->UseSelectedVoice(TTS::DEFAULT_VOICE));
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

  ctx.server_state->set_punctuation_mode(punctuation_mode);
  return true;
}

bool TtsSplitCapsCommand::Run(const StatementInfo& cmd,
                              const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }

  const string& flag = cmd.arguments[0];
  if (flag != "1" && flag != "0") {
    return false;
  }

  ctx.server_state->set_tts_split_caps(flag == "1" ? true : false);
  return true;
}

bool TtsCapitalizeCommand::Run(const StatementInfo& cmd,
                               const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }

  const string& flag = cmd.arguments[0];
  if (flag != "1" && flag != "0") {
    return false;
  }

  ctx.server_state->set_tts_capitalize(flag == "1" ? true : false);
  return true;
}

bool TtsAllcapsBeepCommand::Run(const StatementInfo& cmd,
                                const CommandContext& ctx) {
  if (cmd.arguments.size() != 1) {
    return false;
  }

  const string& flag = cmd.arguments[0];
  if (flag != "1" && flag != "0") {
    return false;
  }

  ctx.server_state->set_tts_allcaps_beep(flag == "1" ? true : false);
  return true;
}

bool TtsSyncStateCommand::Run(const StatementInfo& cmd,
                              const CommandContext& ctx) {
  if (cmd.arguments.size() != 5) {
    return false;
  }

  const string mode = cmd.arguments[0];
  const int speech_rate = std::stoi(cmd.arguments[4]);

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

  if (speech_rate <= 0) {
    return false;
  }

  ctx.server_state->set_punctuation_mode(punctuation_mode);
  // TODO: set capitalize
  // TODO: set allcaps-beep
  // TODO: set split-caps
  ctx.tts->SetSpeechRate(speech_rate);

  return true;
}
