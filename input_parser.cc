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

#include "input_parser.h"

#include <unistd.h>

namespace {

void SkipWhiteSpace(char** buffer) {
  while (**buffer == '\n' || isspace(**buffer)) {
    ++*buffer;
  }
  return;
}

std::string GetWordToken(char** buffer) {
  std::string w_token;
  while (**buffer != EOF && !isspace(**buffer)) {
    w_token.push_back(**buffer);
    ++*buffer;
  }

  return w_token;
}

std::string GetBracedToken(char** buffer) {
  std::string b_token;
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

std::string GetToken(char** buffer) {
  SkipWhiteSpace(buffer);

  std::string token;
  if (**buffer == '{') {
    token = GetBracedToken(buffer);
  } else {
    token = GetWordToken(buffer);
  }
  return token;
}

}  // namespace

InputParser::InputParser() {}

std::string InputParser::ReadLine() {
  std::string line;
  if (buffer_size_ > 0) {
    // If the buffer size is greater than zero, this means that there is still
    // data in our internal buffer. this can happen, for example, if we've read
    // two lines of input at once in the previous call to ReadLine. Since we
    // return only one line per call, we need to check the buffer to see if
    // there is still data left to be processed.
    for (size_t i = buffer_start_; i < buffer_size_; ++i) {
      line.push_back(buffer_[i]);
      if (buffer_[i] == '\n') {
        // End of line. Update the buffer information and return the line.
        buffer_start_ = i + 1;
        buffer_size_ = buffer_size_ - buffer_start_ - 1 - i;
        return line;
      }
    }
  }
  return line;
}

std::tuple<std::string, std::string> InputParser::ProcessInput(
    const char* input, std::size_t input_length) {
  buffer_.append(input, input_length);
  buffer_size_ += input_length;

  std::string command_name, args = "";
  // Read the next input line.
  std::string line = ReadLine();
  if (line.empty()) {
    return std::make_tuple("", "");
  }
  // Parses the line into command name and args.
  char* start = &line[0];

  command_name = GetToken(&start);
  if (*start != '\n' && *start != '\0') {
    args = GetToken(&start);
  }

  return std::make_tuple(command_name, args);
}
