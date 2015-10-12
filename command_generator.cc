#include "command_generator.h"

#include <unordered_map>
#include <iostream>

#include "commands.h"


CommandRegistry::CommandRegistry(TTS* tts, ServerState* server_state)  : tts_(tts), server_state_(server_state) {
  commands_map_["version"] = new VersionCommand(tts_, server_state_);
    commands_map_["tts_say"] = new TtsSayCommand(tts_, server_state_);
    commands_map_["l"] = new LCommand(tts_, server_state_);
    commands_map_["tts_pause"] = new TtsPauseCommand(tts_, server_state_);
    commands_map_["tts_resume"] = new TtsResumeCommand(tts_, server_state_);

}

Command* CommandRegistry::GetCommand(
    const std::string& command_name) {
  if (commands_map_.find(command_name) == commands_map_.end()) {
 std::cout << "not found in the registry\n";
 return nullptr;
  }
  std::cout << "found in the registry\n";
  return commands_map_[command_name];
}
