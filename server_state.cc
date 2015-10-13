#include "server_state.h"

#include <cstdio>
#include <sstream>

using std::string;

ServerState::ServerState()
    : server_status_(DATA_PROCESSED), last_args_(new string()) {}

void ServerState::ClearMessageQueue() {
  if (messages_.size() == 0) {
    return;
  }
  std::queue<std::unique_ptr<Message>> empty;
  std::swap(messages_, empty);
  return;
}
