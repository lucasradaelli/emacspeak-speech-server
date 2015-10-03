#include "commands.h"
#include <iostream>


using std::string;

bool VersionCommand::Run() {

  return true;
}


bool TtsSayCommand::Run() {
  const string prefix = server_state_->GetPrefixString();
  const string args = server_state_->GetLastArgs();
  const string  msg = prefix + args;
  if (!tts_->Say(msg)) {
    return false;
  }
  return true;
}
