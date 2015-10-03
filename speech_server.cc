#include "speech_server.h"

#include <cstdio>
#include <iostream>
#include <poll.h>

using std::cin;
using std::cout;
using std::string;


namespace {

void SkipWhiteSpace() {
  while (cin.peek() == '\n' || isspace(cin.peek())) {
    cin.get();
  }
  return;
}


string GetWordToken() {
  string w_token;
  while (cin.peek() != EOF && !isspace(cin.peek())) {
    char c;
    cin.get(c);
    w_token.push_back(c);
  }

  return w_token;
}

string GetBracedToken() {
  string b_token;
  // Ignores the first value -- it is the {.
  cin.get();
  while (cin.peek() != '}' && cin.peek() != EOF) {
    char c;
    cin.get(c);
    b_token.push_back(c);
  }
  if (cin.peek() == '}') {
    cin.get();
  }

  return b_token;
}


string GetToken() {
  SkipWhiteSpace();
  if (cin.peek() == EOF) {
    return "exit";
  }
  string token;
  if (cin.peek() == '{') {
    token = GetBracedToken();
  } else {
    token = GetWordToken();
  }
  return token;
}



} // namespace

ServerStatus SpeechServer::Service() {
  bool is_speaking = tts_->IsSpeaking();
  pollfd fds[1];
  fds[1].fd = fileno(stdin);
  fds[1].events = POLLIN;

  while (is_speaking) {
int rc =     poll(fds, 1, 5);
if (rc) {
  cout << "there is data to read";
  break;
} else {
  is_speaking = tts_->IsSpeaking();
}
  }
  return  !is_speaking ?DATA_PROCESSED : COMMAND_PENDING;
}



int SpeechServer::MainLoop() {
 bool running = true;
 while (running) {
   string command_name;
   std::tie(command_name, *server_state_.GetMutableLastArgs()) = ProcessInput();
   if (command_name.empty()) {
     continue;
   }
   cout << command_name << "\n";
   Command* command =
       cmd_registry_->GetCommand(command_name);
   if (command == nullptr) {
     cout << "invalid command\n";
     continue;
   }
   command->Run();

   Service();
   }

  return 0;
}



std::tuple<string,string> SpeechServer::ProcessInput() {
  string command_name, args = "";
  command_name = GetToken();
  cout << "read " << command_name << "\n";
  if (cin.peek() != '\n') {
    args = GetToken();
    cout << "read " << args << " as args\n";
  }
  while (cin.peek() != EOF) {
    cin.get();
  }
  cin.get();
  return std::make_tuple(command_name, args);
}
