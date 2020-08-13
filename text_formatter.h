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

#ifndef TEXT_FORMATTER_H_
#define TEXT_FORMATTER_H_

#include <boost/regex.hpp>
#include <string>
#include <unordered_map>

// Base class that formats a message to be synthesized. As different speech
// engines pronounce punctuations and some other symbols differently, this class
// provides a way for the application to output a consistent pronunciation
// regardless of the speech engine being used. Each speech engine can have some
// specific aspects to treat those symbols, thus, to implement this
// functionality it is expected to subclass TextFormatter, and at construction
// time, link each regex defined in this class with the substitution string to
// be used. Please see ECITextFormatter defined bellow, for a concrete example.
class TextFormatter {
 public:
  enum PunctuationMode { NONE, SOME, ALL };

  virtual ~TextFormatter() = default;

  // Returns the input text formatted, ready to be consumed by the speech
  // engine.
  // Each speech engine may apply a different processing, but, in general, this
  // consists at changing the punctuation symbols by their written
  // representation,
  // adding tts control parameters to change intonation, etc. Punctuation mode
  // indicates the amount of punctuation to keep in the returned text, which can
  // be either totally removed or rewritten, E.G, replace every occurrence of
  // '*' to star, so that the pronunciation of the symbol will be the same
  // regardless of the speech engine. Split caps indicates whether words in all
  // caps or camel case words should be splitted, so the speech engine will
  // pronounce them as seprat words, E.G TTS with split_caps  set to true should
  // be spoken by the speech engine as 't t s'. The word in camel case
  // 'ThisIsATest' should be spoken as 'this is a test'. Capitalized indicates
  // whether to apply a higher pitch when letters are uppercase. Supporting
  // speech engines will receive the text annotated to be spoken with the
  // information to use a higher pitch. Finally, allcaps_bip indicates that
  // words all in caps, like abreviations, should produce a bip or use a higher
  // pitch to differentiate from the rest.
  virtual std::string Format(const std::string& text,
                             const PunctuationMode mode, const bool split_caps,
                             const bool capitalized,
                             const bool allcaps_beep) = 0;

  // Formats a single char to be spoken. Some speech engines apply a different
  // emphasis, pitch or speed to pronounce a single letter.
  virtual std::string FormatSingleChar(const char chr) = 0;

  // Some messages are annotated in a special form, with characters to indicate
  // that a small pause should be applied. Children classes should override and
  // implement their own version, annotating the returned text with special
  // codes, if
  // supported, instructing the speech engine to add a pause.
  virtual std::string FormatPause(const std::string& text) = 0;

 protected:
  TextFormatter() = default;

  using regex = boost::regex;
  using RegexToReplaceStringContainer =
      std::vector<std::pair<const regex*, std::string>>;

  RegexToReplaceStringContainer all_punctuation_;
  RegexToReplaceStringContainer some_punctuation_;
  RegexToReplaceStringContainer no_punctuation_;

  static const regex star_regex_;
  static const regex dash_regex_;
  static const regex semicolen_regex_;
  static const regex left_paren_regex_;
  static const regex right_paren_regex_;
  static const regex at_regex_;
  static const regex all_punctuation_with_pause_regex_;

  static const regex some_punctuation_remove_list_;
  static const regex some_punctuation_pause_list_;

  static const regex none_punctuation_removal_list_;

  static const regex capitalize_word_;
  static const regex capitalize_regex_;
};

class ECITextFormatter : public TextFormatter {
 public:
  ECITextFormatter();
  virtual ~ECITextFormatter() = default;

  std::string Format(const std::string& text,
                     const PunctuationMode punctuation_mode,
                     const bool split_caps, const bool capitalized,
                     const bool allcaps_beep) override;

  std::string FormatSingleChar(const char chr) override;

  std::string FormatPause(const std::string& text) override;

 private:
  static const regex pause_symbol_;
};

#endif  // TEXT_FORMATTER_H_
