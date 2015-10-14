#include "commands.h"
#include <iostream>

using std::string;
using std::unique_ptr;

bool VersionCommand::Run(TTS* tts, ServerState* server_state) {
  const string ttsversion = tts->TTSVersion();
  const string msg = "viavoice " + ttsversion;
  if (!tts->Say(msg)) {
    return false;
  }
  return true;
}

bool TtsSayCommand::Run(TTS* tts, ServerState* server_state) {
  if (!tts->Say(server_state->GetLastArgs())) {
    return false;
  }
  return true;
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
    server_state->ClearMessageQueue();
    return true;
  }
  return false;
}

bool QCommand::Run(TTS* tts, ServerState* server_state) {
  unique_ptr<Message> message(new SpeechMessage(server_state->GetLastArgs()));
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

bool CCommand::Run(TTS* tts, ServerState* server_state) {
  unique_ptr<Message> message(new CodeMessage(server_state->GetLastArgs()));
  server_state->messages().push(std::move(message));
  return true;
}

bool ShCommand::Run(TTS* tts, ServerState* server_state) {
  int duration = atoi(server_state->GetLastArgs().c_str());
  if (duration <= 0) {
    return false;
  }

  unique_ptr<Message> message(new SilenceMessage(duration));
  server_state->messages().push(std::move(message));
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
