#include "command_generator.h"

#include <unordered_map>
#include <iostream>

#include "commands.h"

using std::unique_ptr;

CommandRegistry::CommandRegistry() {
  commands_map_["version"] =
      unique_ptr<Command>(new VersionCommand());
  commands_map_["tts_say"] =
      unique_ptr<Command>(new TtsSayCommand());
  commands_map_["l"] = unique_ptr<Command>(new LCommand());
  commands_map_["tts_pause"] =
      unique_ptr<Command>(new TtsPauseCommand());
  commands_map_["tts_resume"] =
      unique_ptr<Command>(new TtsResumeCommand());
  commands_map_["s"] = unique_ptr<Command>(new SCommand());
  commands_map_["q"] = unique_ptr<Command>(new QCommand());
  commands_map_["d"] = unique_ptr<Command>(new DCommand());
  commands_map_["c"] = unique_ptr<Command>(new CCommand());
}

Command* CommandRegistry::GetCommand(const std::string& command_name) {
  if (commands_map_.find(command_name) == commands_map_.end()) {
    std::cout << "not found in the registry\n";
    return nullptr;
  }
  std::cout << "found in the registry\n";
  return commands_map_[command_name].get();
}
