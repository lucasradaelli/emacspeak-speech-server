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
#include <system_error>

#include <poll.h>
#include <unistd.h>

#include "audio_manager.h"

using std::cin;
using std::cout;
using std::string;

SpeechServer::SpeechServer(AudioManager* audio, TTS* tts)
    : audio_(audio),
      tts_(tts),
      server_state_(audio_),
      cmd_registry_(new CommandRegistry()) {}

SpeechServer::~SpeechServer() = default;

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
      auto audio_fds = audio_->GetPollDescriptors();
      std::copy(std::make_move_iterator(audio_fds.begin()),
                std::make_move_iterator(audio_fds.end()),
                std::back_inserter(fds));
    }

    // Wait for events.
    int descriptors_ready = poll(fds.data(), fds.size(), -1);
    if (descriptors_ready < 0) {
      if (errno == EINTR) {
        // Interrupted system call, try again.
        continue;
      }
      throw std::system_error(errno, std::system_category());
    }

    // If there was any audio tasks enqueued and the sound output is ready,
    // run those tasks now.
    if (audio_output_pending &&
        audio_->GetPollEvents(fds.data() + 1, fds.size() - 1)) {
      audio_->Run();
    }

    // If there was an input event, possibly process a server command.
    if (fds[0].fd == STDIN_FILENO && fds[0].revents != 0) {
      // Read the input waiting at the standard input.
      char buffer[4096];
      int size = read(STDIN_FILENO, buffer, sizeof(buffer));

      if (size < 0) {
        if (errno == EINTR) { /* Interrupted --> restart read() */
          continue;
        } else { /* Some other error*/
          throw std::system_error(errno, std::system_category());
        }
      } else if (size == 0) { /* Found EOF */
        break;
      }

      // Feed the input to the parser.
      input_parser_.Feed(buffer, size);

      // Process any complete statements from the input.
      while (auto statement = input_parser_.Parse()) {
        cout << *statement << "\n";
        Command* command = cmd_registry_->GetCommand(statement->command);
        if (command == nullptr) {
          cout << "invalid command\n";
          continue;
        }
        CommandContext context;
        context.tts = tts_;
        context.server_state = &server_state_;
        command->Run(*statement, context);
      }
    }
  }

  return 0;
}
