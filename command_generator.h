#ifndef COMMAND_GENERATOR_H_
#define COMMAND_GENERATOR_H_

#include <string>
#include <unordered_map>

#include "commands.h"
#include "server_state.h"

class CommandRegistry {
 public:
  CommandRegistry(TTS* tts, ServerState* server_state);
  ~CommandRegistry() = default;

  Command* GetCommand(const std::string& command_name);

 private:
  TTS* tts_;
  ServerState* server_state_;

  std::unordered_map<std::string, std::unique_ptr<Command>> commands_map_;
};

#endif  // COMMAND_GENERATOR_H_
