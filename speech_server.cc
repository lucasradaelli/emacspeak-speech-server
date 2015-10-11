#include "speech_server.h"

#include <cstdio>
#include <iostream>
#include <poll.h>


using std::cin;
using std::cout;
using std::string;


namespace {

void SkipWhiteSpace(char** buffer) {
  while (**buffer  == '\n' || isspace(**buffer)) {
    ++*buffer;
  }
  return;
}


string GetWordToken(char** buffer) {
  string w_token;
  while (**buffer != EOF && !isspace(**buffer)) {
    w_token.push_back(**buffer);
    ++*buffer;
  }

  return w_token;
}

string GetBracedToken(char** buffer) {
  string b_token;
  // Ignores the first value -- it is the {.
  ++*buffer;
  while (**buffer != '}' && **buffer != EOF) {
    b_token.push_back(**buffer);
    ++*buffer;
  }
  if (**buffer == '}') {
    ++*buffer;
  }

  return b_token;
}


string GetToken(char** buffer) {
  SkipWhiteSpace(buffer);

  string token;
  if (**buffer == '{') {
    token = GetBracedToken(buffer);
  } else {
    token = GetWordToken(buffer);
  }
  return token;
}



} // namespace

ServerStatus SpeechServer::Service() {
  bool is_speaking = tts_->IsSpeaking();
  pollfd fds[1];
  fds[0].fd = fileno(stdin);
  fds[0].events = POLLIN;

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


string SpeechServer::ReadLine() {
  string line;
  if (buffer_size_ > 0) {
for (size_t i = buffer_start_; i < buffer_size_; ++i) {
line.push_back(read_buffer_[i]);
if (read_buffer_[i] == '\n') {
buffer_start_ = i;
buffer_size_ = buffer_size_ -1 - i;
return line;
}
}
  }

  size_t size;
  while (true) {
size  = read(0, read_buffer_, sizeof(read_buffer_));
  if (size <= 0) {
    // TODO: Handle errors.
    break;
  } else if (size == 0) {
return "";
} else {
for (size_t i = 0; i < size; ++i) {
line.push_back(read_buffer_[i]);
if (read_buffer_[i] == '\n') {
buffer_start_ = i;
buffer_size_ = size -1 - i;
return line;
}
}
}
  }
return line;
}




std::tuple<string,string> SpeechServer::ProcessInput() {
  string command_name, args = "";
  // Read the next input line.
  string line = ReadLine();
  if (line.empty()) {
    return std::make_tuple("", "");
  }
  // Parses the line into command name and args.
  char* start = &line[0];

  command_name = GetToken(&start);
  cout << "read " << command_name << "\n";
  if (*start != '\n') {
    args = GetToken(&start);
    cout << "read " << args << " as args\n";
  }

  return std::make_tuple(command_name, args);
}
