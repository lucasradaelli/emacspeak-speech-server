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

#ifndef INPUT_PARSER_H_
#define INPUT_PARSER_H_

#include <cstdlib>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

// Information about an input statement.
struct StatementInfo {
  // The command name string.
  std::string command;

  // The various arguments passed to the command.
  std::vector<std::string> arguments;

  // Compare two StatementInfo objects for equality.
  bool operator==(const StatementInfo& o);
  bool operator!=(const StatementInfo& o);

  friend std::ostream& operator<<(std::ostream& o, const StatementInfo& s);
};

// Parses the text input to the server.
class InputParser {
 public:
  InputParser();
  virtual ~InputParser();

  // Feeds the given input to the parser.
  void Feed(const char* input, std::size_t input_length);

  // Parses the current accumulated input in the parser and returns the next
  // complete statement parsed. It returns nullptr if it reaches the end of
  // the current buffer and there are no more statements to return.
  std::unique_ptr<StatementInfo> Parse();

 private:
  using Rule = void (InputParser::*)();

  // Resets the grammar stack.
  void Reset();

  // Pushes a new grammar rule into the stack, to run while the current one.
  void Push(Rule rule);

  // Pops the current grammar rule from the stack.
  void Pop();

  // Replaces/switches the current grammar rule with a new one.
  void Continue(Rule rule);

  // Throws an exception for an unexpected token.
  void Unexpected(const std::string& context);

  // Implementation of various production rules.
  void Statement();
  void Command();
  void ArgumentList();
  void Argument();
  void BracedString();
  void Spaces();

  // Stack of production rules.
  std::stack<Rule> rules_;

  // Accumulation buffer.
  std::string buffer_;
  std::string::iterator pos_;
  std::string::iterator end_;

  // Current statement being parsed.
  bool statement_done_ = false;
  std::unique_ptr<StatementInfo> statement_;
};

// Exception thrown when a parsing errors occurs.
class InputParsingError : public std::runtime_error {
 public:
  InputParsingError(const std::string& context, const std::string& near);

 private:
  static std::string GenerateMessage(const std::string& context,
                                     const std::string& near);
};

#endif  // INPUT_PARSER_H_
