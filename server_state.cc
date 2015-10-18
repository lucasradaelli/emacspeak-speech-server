#include "server_state.h"

#include <cstdio>
#include <sstream>

using std::string;

ServerState::ServerState(AudioManager* audio)
    : audio_(audio), server_status_(DATA_PROCESSED), last_args_(new string()) {}

void ServerState::ClearQueue() {
  if (queue_.size() == 0) {
    return;
  }
  decltype(queue_) empty;
  std::swap(queue_, empty);
  return;
}
