#include "server_state.h"

#include <cstdio>
#include <sstream>

using std::string;

ServerState::ServerState() :server_status_(DATA_PROCESSED), last_args_(new string()) {
}

const string ServerState::GetPrefixString() const {
  std::ostringstream prefix_string;
  prefix_string << "`v1 `vs" << GetSpeechRate() << " ";
                                      return prefix_string.str();
}
