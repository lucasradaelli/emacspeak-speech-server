#include "commands.h"
#include <iostream>

using std::string;

bool VersionCommand::Run(TTS* tts, ServerState* server_state) {
  const string prefix = server_state->GetPrefixString();
  const string ttsversion = tts->TTSVersion();
  const string msg = prefix + "viavoice " + ttsversion;
  if (!tts->Say(msg)) {
    return false;
  }
  return true;
}

bool TtsSayCommand::Run(TTS* tts, ServerState* server_state) {
  const string prefix = server_state->GetPrefixString();
  const string args = server_state->GetLastArgs();
  const string msg = prefix + args;
  if (!tts->Say(msg)) {
    return false;
  }
  return true;
}

bool LCommand::Run(TTS* tts, ServerState* server_state) {
  const string prefix = server_state->GetPrefixString();
  const string args = server_state->GetLastArgs();
  if (args.size() != 1) {
    // It only speaks a single character.
    return false;
  }
  string letter_pitch;
  if (isalpha(args[0])) {
    letter_pitch = "`vb80 ";
  }
  const string msg = prefix + letter_pitch + "`ts2 " + args + " `ts0";
  if (!tts->Say(msg)) {
    return false;
  }
  return true;
}

bool TtsPauseCommand::Run(TTS* tts, ServerState* server_state) {
  tts->player()->Pause();
  return true;
}

bool TtsResumeCommand::Run(TTS* tts, ServerState* server_state) {
  tts->player()->Resume();
  return true;
}

bool SCommand::Run(TTS* tts, ServerState* server_state) {
  if (tts->Stop()) {
    return true;
  }
  return false;
}

bool QCommand::Run(TTS* tts, ServerState* server_state) {
  std::unique_ptr<Message> message(
      new SpeechMessage(server_state->GetLastArgs()));
  server_state->messages().push(std::move(message));
  return true;
}

bool DCommand::Run(TTS* tts, ServerState* server_state) {
  while (!server_state->messages().empty()) {
    server_state->messages().front()->Do(tts, server_state);
    server_state->messages().pop();
  }
  return true;
}
