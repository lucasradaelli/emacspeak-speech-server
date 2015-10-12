#include "commands.h"
#include <iostream>

using std::string;

bool VersionCommand::Run() {
  const string prefix = server_state_->GetPrefixString();
  const string tts_version = tts_->TTSVersion();
  const string msg = prefix + "viavoice " + tts_version;
  if (!tts_->Say(msg)) {
    return false;
  }
  return true;
}

bool TtsSayCommand::Run() {
  const string prefix = server_state_->GetPrefixString();
  const string args = server_state_->GetLastArgs();
  const string msg = prefix + args;
  if (!tts_->Say(msg)) {
    return false;
  }
  return true;
}

bool LCommand::Run() {
  const string prefix = server_state_->GetPrefixString();
  const string args = server_state_->GetLastArgs();
  if (args.size() != 1) {
    // It only speaks a single character.
    return false;
  }
  string letter_pitch;
  if (isalpha(args[0])) {
    letter_pitch = "`vb80 ";
  }
  const string msg = prefix + letter_pitch + "`ts2 " + args + " `ts0";
  if (!tts_->Say(msg)) {
    return false;
  }
  return true;
}

bool TtsPauseCommand::Run() {
  if (tts_->Pause()) {
    return true;
  }
  return false;
}

bool TtsResumeCommand::Run() {
  if (tts_->Resume()) {
    return true;
  }
  return false;
}
