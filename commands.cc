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

using std::string;
using std::unique_ptr;

bool VersionCommand::Run(TTS* tts, ServerState* server_state) {
  const string msg = "ViaVoice " + tts->TTSVersion();
  return tts->Say(msg) && tts->SubmitTask();
}

bool TtsSayCommand::Run(TTS* tts, ServerState* server_state) {
  return tts->Say(server_state->GetLastArgs()) && tts->SubmitTask();
}

bool LCommand::Run(TTS* tts, ServerState* server_state) {
  const string args = server_state->GetLastArgs();
  if (args.size() != 1) {
    // It only speaks a single character.
    return false;
  }
  string letter_pitch;
  if (isupper(args[0])) {
    letter_pitch = "`vb80 ";
  }
  const string msg = letter_pitch + "`ts2 " + args + " `ts0";
  return tts->Say(msg) && tts->SubmitTask();
}

bool TtsPauseCommand::Run(TTS* tts, ServerState* server_state) {
  tts->Pause();
  return true;
}

bool TtsResumeCommand::Run(TTS* tts, ServerState* server_state) {
  tts->Resume();
  return true;
}

bool SCommand::Run(TTS* tts, ServerState* server_state) {
  if (tts->Stop()) {
    server_state->ClearQueue();
    return true;
  }
  return false;
}

bool QCommand::Run(TTS* tts, ServerState* server_state) {
  tts->Say(server_state->GetLastArgs());
  server_state->queue().push(tts->ReleaseTask());
  return true;
}

bool CCommand::Run(TTS* tts, ServerState* server_state) {
  tts->Output(server_state->GetLastArgs());
  server_state->queue().push(tts->ReleaseTask());
  return true;
}

bool DCommand::Run(TTS* tts, ServerState* server_state) {
  while (!server_state->queue().empty()) {
    auto& task = server_state->queue().front();
    server_state->audio()->Push(std::move(task));
    server_state->queue().pop();
  }
  return true;
}

bool ShCommand::Run(TTS* tts, ServerState* server_state) {
  int duration = std::stoi(server_state->GetLastArgs());
  if (duration <= 0) {
    return false;
  }

  tts->GenerateSilence(duration);
  server_state->queue().push(tts->ReleaseTask());
  return true;
}

bool TCommand::Run(TTS* tts, ServerState* server_state) {
  std::istringstream args(server_state->GetLastArgs());
  float frequency, length;
  args >> frequency >> length;
  if (args.fail()) {
    return false;
  }

  // Amplitude chosen to be comfortable to listen, more or less on the same
  // level as the speech.
  const float amplitude = 0.3;

  std::unique_ptr<ToneTask> tone(new ToneTask(frequency, amplitude, length));
  server_state->queue().push(std::move(tone));
  return true;
}

bool TtsSetSpeechRateCommand::Run(TTS* tts, ServerState* server_state) {
  int speech_rate = atoi(server_state->GetLastArgs().c_str());
  if (speech_rate <= 0) {
    return false;
  }

  tts->SetSpeechRate(speech_rate);
  return true;
}
