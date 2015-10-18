#ifndef TEXT_FORMATTER_H_
#define TEXT_FORMATTER_H_

#include <boost/regex.hpp>
#include <unordered_map>
#include <string>

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
  // adding tts control parameters to change intonation, etc.
  virtual std::string Format(const std::string& text,
                             const PunctuationMode mode) = 0;

 protected:
  TextFormatter() = default;

  using regex = boost::regex;

  std::vector<std::pair<const regex*, std::string>> all_punctuation_;
  std::unordered_map<const regex*, std::string> some_punctuation_;
  std::unordered_map<const regex*, std::string> no_punctuation_;

  static const regex star_regex_;
  static const regex dash_regex_;
  static const regex semicolen_regex_;
  static const regex left_paren_regex_;
  static const regex right_paren_regex_;
  static const regex at_regex_;
  static const regex all_punctuation_regex_;
};

class ECITextFormatter : public TextFormatter {
 public:
  ECITextFormatter();
  virtual ~ECITextFormatter() = default;

  std::string Format(const std::string& text,
                     const PunctuationMode punctuation_mode) override;

 private:
};

#endif  // TEXT_FORMATTER_H_
