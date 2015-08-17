#include <iostream>
#include <cstdio>
#include <cctype>
#include "command_generator.h"
#include "command_handler.h"
#include "tts.h"

using namespace std;

void SkipWhiteSpace(istream& in) {
  while (isspace(in.peek())) {
    in.get();
  }
  return;
}

string GetWordToken(istream& in) {
  string w_token;
  while (in.peek() != EOF && !isspace(in.peek())) {
    char c;
    in.get(c);
    w_token.push_back(c);
  }

  return w_token;
}

string GetBracedToken(istream& in) {
  string b_token;
  in.get();
  while (in.peek() != '}' && in.peek() != EOF) {
    char c;
    in.get(c);
    b_token.push_back(c);
  }
  if (in.peek() == '}') {
    in.get();
  }

  return b_token;
}

string GetToken(istream& in) {
  SkipWhiteSpace(in);
  if (in.peek() == EOF) {
    return "";
  }

  string token;
  if (in.peek() == '{') {
    token = GetBracedToken(in);
  } else {
    token = GetWordToken(in);
  }
  return token;
}

int main(int argc, char** argv) {
  TTS::InitECI();
  AlsaPlayer alsa_player;
  TTS tts(&alsa_player);

  CommandGeneratorRegistry cmd_registry;

  bool running = true;
  while (running) {
    string token = GetToken(cin);
    if (token.empty()) {
      running = false;
      continue;
    }
    cout << token << "\n";
    CommandGeneratorBase* command_generator =
        cmd_registry.GetCommandGenerator(token);
    if (command_generator == nullptr) {
      cout << "invalid command\n";
      continue;
    }
    CommandHandlerBase* command;
    if (command_generator->HasParams()) {
      string params = GetToken(cin);
      command = command_generator->MakeCommand(params);
    } else {
      command = command_generator->MakeCommand();
    }
    command->Run();
  }

  return 0;
}
