#include "command_handler.h"
#include <iostream>

bool VersionCommandHandler::Run() {
 std::cout << "received version command\n";
 return true;
}
