#ifndef _COMMAND_GENERATOR_H_
#define _COMMAND_GENERATOR_H_
#include <map>
#include "command_handler.h"
#include <string>




class CommandGeneratorBase {
 public:
  CommandGeneratorBase() = default;
  virtual ~CommandGeneratorBase() = default;
  virtual CommandHandlerBase* MakeCommand(const std::string& params)     { return nullptr;}
  virtual CommandHandlerBase* MakeCommand()  { return nullptr;}
  virtual bool HasParams() = 0;
};


class VersionCommandGenerator : public CommandGeneratorBase {
 public:
  VersionCommandGenerator() = default;
virtual ~VersionCommandGenerator() = default;
 bool HasParams() { return false;}
  virtual CommandHandlerBase* MakeCommand()  override;
};

class CommandGeneratorRegistry {
 public:
 CommandGeneratorRegistry();
  ~CommandGeneratorRegistry() = default;
 CommandGeneratorBase* GetCommandGenerator(const std::string& command_name);
 private:
  std::map<std::string,CommandGeneratorBase*> commands_map_;

};
#endif
