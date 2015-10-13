#ifndef COMMAND_GENERATOR_H_
#define COMMAND_GENERATOR_H_

#include <string>
#include <unordered_map>

#include "commands.h"
#include "server_state.h"

class CommandRegistry {
 public:
  CommandRegistry();
  ~CommandRegistry() = default;

  Command* GetCommand(const std::string& command_name);

 private:

  std::unordered_map<std::string, std::unique_ptr<Command>> commands_map_;
};

#endif  // COMMAND_GENERATOR_H_
