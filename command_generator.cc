#include "command_generator.h"
#include "command_handler.h"
#include <map>
#include <iostream>

CommandHandlerBase* VersionCommandGenerator::MakeCommand() {
  CommandHandlerBase* handler = new VersionCommandHandler();
  return handler;
}

CommandGeneratorRegistry::CommandGeneratorRegistry() {
  commands_map_["version"] = new VersionCommandGenerator();
  // commands_map_["tts_say"] = new TtsSayCommandGenerator();
  // commands_map_["l"] = new LCommandGenerator();
}

CommandGeneratorBase* CommandGeneratorRegistry::GetCommandGenerator(
    const std::string& command_name) {
  if (commands_map_.find(command_name) == commands_map_.end()) {
 std::cout << "not found in the registry\n";
 return nullptr;
  }
  std::cout << "found in the registry\n";
  return commands_map_[command_name];
}
