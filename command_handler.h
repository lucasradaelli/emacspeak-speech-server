#ifndef _COMMAND_HANDLER_H_
#define _COMMAND_HANDLER_H_


class CommandHandlerBase {
 public:
  CommandHandlerBase() {}
  virtual ~CommandHandlerBase() {}
  virtual bool Run() = 0;
};


class VersionCommandHandler : public CommandHandlerBase {
 public:
  VersionCommandHandler()  {}
  ~VersionCommandHandler() {}
  virtual bool Run() override;
};

#endif
