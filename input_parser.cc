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

#include <algorithm>
#include <sstream>

// Returns whether the given character is a valid word character.
static bool IsWordChar(int c) {
  return std::isalnum(c) ||
         (std::ispunct(c) && c != ' ' && c != '"' && c != ';' && c != '[' &&
          c != ']' && c != '\\' && c != '{' && c != '}');
}

// InputParser

InputParser::InputParser() : pos_(buffer_.begin()), end_(buffer_.end()) {
  Reset();
}

InputParser::~InputParser() {}

void InputParser::Feed(const char* input, std::size_t input_length) {
  // If pos_ is not at the start of the buffer, move the rest of the text back
  // to the beginning of the buffer.
  if (pos_ > buffer_.begin()) {
    std::copy(pos_, end_, buffer_.begin());
    buffer_.resize(end_ - pos_);
  }

  // Append the new input to the end of the buffer.
  buffer_.append(input, input_length);
  pos_ = buffer_.begin();
  end_ = buffer_.end();
}

std::unique_ptr<StatementInfo> InputParser::Parse() {
  // Keeps parsing the input buffer until there is a complete statement ready
  // or it reaches the end of the buffer.
  while (!statement_done_ && pos_ < end_) {
    (this->*rules_.top())();
  }

  // It there is a statement, return it.
  if (statement_done_) {
    statement_done_ = false;
    return std::move(statement_);
  } else {
    return nullptr;
  }
}

void InputParser::Reset() {
  decltype(rules_) empty;
  std::swap(rules_, empty);
  Push(&InputParser::Statement);
}

void InputParser::Push(InputParser::Rule rule) { rules_.push(rule); }
void InputParser::Pop() { rules_.pop(); }

void InputParser::Continue(InputParser::Rule rule) {
  Pop();
  Push(rule);
}

void InputParser::Unexpected(const std::string& context) {
  auto word_pos = pos_;
  if (IsWordChar(*pos_)) {
    while (pos_ < end_ && IsWordChar(*pos_)) {
      ++pos_;
    }
  } else {
    ++pos_;
  }
  std::string near(word_pos, pos_);
  while (pos_ < end_ && *pos_ != '\n') {
    ++pos_;
  }
  Reset();
  throw InputParsingError(context, near);
}

// Statement ::= Command ArgumentList EOL
void InputParser::Statement() {
  // Skip any spaces before the first token.
  while (pos_ < end_ && std::isspace(*pos_)) ++pos_;

  if (pos_ < end_) {
    // A statement starts with a command, which starts with a letter.
    if (std::isalpha(*pos_)) {
      statement_.reset(new StatementInfo());
      Continue(&InputParser::Command);
    } else {
      Unexpected("Statement");
    }
  }
}

// Command ::= WordChar+
void InputParser::Command() {
  // Extract a contiguous set of word chars for the command.
  auto word_pos = pos_;
  while (pos_ < end_ && IsWordChar(*pos_)) {
    ++pos_;
  }

  // Set the statement command.
  statement_->command.append(word_pos, pos_);

  // If a non-word char was found, start the argument list, but skip any
  // spaces first.
  if (pos_ < end_) {
    Pop();
    Push(&InputParser::ArgumentList);
    Push(&InputParser::Spaces);
  }
}

// ArgumentList ::= Argument Spaces ArgumentList
//                | BracedString Spaces ArgumentList
void InputParser::ArgumentList() {
  if (IsWordChar(*pos_)) {
    statement_->arguments.emplace_back();
    Push(&InputParser::Argument);
  } else if (*pos_ == '{') {
    ++pos_;
    statement_->arguments.emplace_back();
    Push(&InputParser::BracedString);
  } else if (*pos_ == '\n') {
    ++pos_;
    statement_done_ = true;
    Continue(&InputParser::Statement);
  } else {
    Unexpected("ArgumentList");
  }
}

// Argument ::= WordChar+
void InputParser::Argument() {
  // Extract a contiguous set of word chars for the argument.
  auto word_pos = pos_;
  while (pos_ < end_ && IsWordChar(*pos_)) {
    ++pos_;
  }

  // Append to the last argument.
  statement_->arguments.back().append(word_pos, pos_);

  // If a non-word char was found, start start a new argument, skipping any
  // spaces.
  if (pos_ < end_) {
    Continue(&InputParser::Spaces);
  }
}

// BracedString ::= '{' char* '}'
void InputParser::BracedString() {
  auto start = pos_;
  while (pos_ < end_ && *pos_ != '}') ++pos_;

  statement_->arguments.back().append(start, pos_);

  if (pos_ < end_) {
    ++pos_;  // Consume the '}'.
    Continue(&InputParser::Spaces);
  }
}

// Spaces ::= ' '*
void InputParser::Spaces() {
  while (pos_ < end_) {
    if (*pos_ == '\n' || !std::isspace(*pos_)) {
      Pop();
      break;
    }
    ++pos_;
  }
}

// StatementInfo

bool StatementInfo::operator==(const StatementInfo& o) {
  return command == o.command && arguments == o.arguments;
}

bool StatementInfo::operator!=(const StatementInfo& o) {
  return !operator==(o);
}

std::ostream& operator<<(std::ostream& o, const StatementInfo& s) {
  o << "StatementInfo(\"" << s.command << "\", {";
  for (unsigned i = 0; i < s.arguments.size(); ++i) {
    o << "," + (i == 0) << " \"" << s.arguments[i] << "\"";
  }
  return o << " })";
}

// InputParsingError

InputParsingError::InputParsingError(const std::string& context,
                                     const std::string& near)
    : runtime_error(GenerateMessage(context, near)) {}

std::string InputParsingError::GenerateMessage(const std::string& context,
                                               const std::string& near) {
  std::ostringstream sstr;
  sstr << "Failed to parse input: In context '" << context << "', near '"
       << near << "'.";
  return sstr.str();
}
