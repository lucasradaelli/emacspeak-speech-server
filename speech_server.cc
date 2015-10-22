// Copyright 2015 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "speech_server.h"

#include <cerrno>
#include <cstdio>
#include <iostream>

#include <poll.h>
#include <unistd.h>

#include "audio_manager.h"

using std::cin;
using std::cout;
using std::string;

namespace {

void SkipWhiteSpace(char** buffer) {
  while (**buffer == '\n' || isspace(**buffer)) {
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

}  // namespace

int SpeechServer::MainLoop() {
  for (;;) {
    // Always expect input from the stdin descriptor, to process commands.
    std::vector<struct pollfd> fds(1);
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN | POLLERR;

    // If there is an audio task in the queue, expect for sound output buffer
    // availability.
    const bool audio_output_pending = audio_->pending();

    if (audio_output_pending) {
      auto alsa_fds = audio_->player()->GetPollDescriptors();
      std::copy(std::make_move_iterator(alsa_fds.begin()),
                std::make_move_iterator(alsa_fds.end()),
                std::back_inserter(fds));
    }

    // Wait for events.
    int descriptors_ready = poll(fds.data(), fds.size(), -1);
    if (descriptors_ready < 0) {
      if (errno == EINTR) {
        // Interrupted system call, try again.
        continue;
      }
      throw;  // TODO: make a proper exception
    }

    // If there was any audio tasks enqueued and the sound output is ready,
    // run those tasks now.
    if (audio_output_pending &&
        audio_->player()->GetPollEvents(fds.data() + 1, fds.size() - 1)) {
      audio_->Run();
    }

    // If there was an input event, possibly process a server command.
    if (fds[0].fd == STDIN_FILENO && fds[0].revents != 0) {
      string command_name;
      std::tie(command_name, *server_state_.GetMutableLastArgs()) =
          ProcessInput();
      if (command_name.empty()) {
        continue;
      }
      cout << command_name << "\n";
      Command* command = cmd_registry_->GetCommand(command_name);
      if (command == nullptr) {
        cout << "invalid command\n";
        continue;
      }
      command->Run(tts_, &server_state_);
    }
  }

  return 0;
}

string SpeechServer::ReadLine() {
  string line;
  if (buffer_size_ > 0) {
    // If the buffer size is greater than zero, this means that there is still
    // data in our internal buffer. this can happen, for example, if we've read
    // two lines of input at once in the previous call to ReadLine. Since we
    // return only one line per call, we need to check the buffer to see if
    // there is still data left to be processed.
    for (size_t i = buffer_start_; i < buffer_size_; ++i) {
      line.push_back(read_buffer_[i]);
      if (read_buffer_[i] == '\n') {
        // End of line. Update the buffer information and return the line.
        buffer_start_ = i;
        buffer_size_ = buffer_size_ - 1 - i;
        return line;
      }
    }
  }

  while (true) {
    auto size = read(0, read_buffer_, sizeof(read_buffer_));
    if (size == -1) {
      if (errno == EINTR) { /* Interrupted --> restart read() */
        continue;
      } else { /* Some other error*/
        return "";
      }
    } else if (size == 0) { /* Found EOF */
      return "exit\n";
    } else {
      for (ssize_t i = 0; i < size; ++i) {
        line.push_back(read_buffer_[i]);
        if (read_buffer_[i] == '\n') {
          buffer_start_ = i;
          buffer_size_ = size - 1 - i;
          return line;
        }
      }
    }
  }
  return line;
}

std::tuple<string, string> SpeechServer::ProcessInput() {
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
  if (*start != '\n' && *start != '\0') {
    args = GetToken(&start);
    cout << "read " << args << " as args\n";
  }

  return std::make_tuple(command_name, args);
}
