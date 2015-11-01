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

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

bool good = true;

void CheckParser(const std::string& input,
                 std::vector<StatementInfo> expected) {
  InputParser parser;
  parser.Feed(input.c_str(), input.size());

  std::cout << "---Test---------------------------------\n";
  std::cout << input;

  for (const auto& e : expected) {
    auto r = parser.Parse();
    if (r == nullptr) {
      good = false;
      std::cout << "Fewer results generated. Missing: " << e << "\n";
    } else if (*r != e) {
      good = false;
      std::cout << "[FAIL] expected=" << e << " actual=" << *r << "\n";
    } else {
      std::cout << "[GOOD] " << e << "\n";
    }
  }

  while (auto r = parser.Parse()) {
    good = false;
    std::cout << "Too many results generated. Excess: " << *r << "\n";
  }

  std::cout << "\n";
}

int main() {
  CheckParser("", {});
  CheckParser("  \n\n     \n\t \t\n  ", {});
  CheckParser("command_one  \n", {StatementInfo{"command_one", {}}});
  CheckParser("arg 1 2 3\n", {StatementInfo{"arg", {"1", "2", "3"}}});
  CheckParser("  arg  1  2  3  \n", {StatementInfo{"arg", {"1", "2", "3"}}});
  CheckParser(
      "\n\n"
      "command_one one two three \n"
      "command_two four five six  \n\n",
      {StatementInfo{"command_one", {"one", "two", "three"}},
       StatementInfo{"command_two", {"four", "five", "six"}}});
  CheckParser(
      "command_one {one one} two three \n"
      "command_two four {five five} { six six }  \n",
      {StatementInfo{"command_one", {"one one", "two", "three"}},
       StatementInfo{"command_two", {"four", "five five", " six six "}}});
  CheckParser("a /tmp/somefile.wav \n",
              {StatementInfo{"a", {"/tmp/somefile.wav"}}});

  return !good;
}
