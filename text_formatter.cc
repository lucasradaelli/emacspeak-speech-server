#include "text_formatter.h"
#include <boost/regex.hpp>
#include <string>

using std::string;
using std::vector;
using std::pair;
using boost::regex;
using boost::regex_replace;

const regex TextFormatter::star_regex_("\\*", boost::regex::extended);
const regex TextFormatter::dash_regex_("-", boost::regex::extended);
const regex TextFormatter::semicolen_regex_(";", boost::regex::extended);
const regex TextFormatter::left_paren_regex_("\\(", boost::regex::extended);
const regex TextFormatter::right_paren_regex_("\\)", boost::regex::extended);
const regex TextFormatter::at_regex_("@", boost::regex::extended);
// Todo: Is this useful at all? Some punctuation was already removed by regex
// defined above.
const regex TextFormatter::all_punctuation_with_pause_regex_(
    "([.,!?;:+=/'\\\"@$%&_*()])", boost::regex::extended);

const regex TextFormatter::some_punctuation_remove_list_(
    "[*&()\"{}\\[\\]']", boost::regex::extended);

// todo: find a better name and discover exactly what this regex trys to match.
// It appears to be inserting a small pause between a letter followed by a
// letter or number.
const regex TextFormatter::some_punctuation_pause_list_(
    "([[:alnum:]])([@!;/:()=\\#,.\"])+([[:word:]])", boost::regex::extended);

const regex TextFormatter::none_punctuation_removal_list_(
    "([[:punct:]])", boost::regex::extended);

const regex TextFormatter::capitalize_regex_("([[:upper:]])",
                                             boost::regex::extended);

// Received as a pause symbol in IBM VIAVOICE TTS speech servers .
const regex ECITextFormatter::pause_symbol_("\\[\\*\\]",
                                            boost::regex::extended);

ECITextFormatter::ECITextFormatter() {
  all_punctuation_.push_back({&star_regex_, " `00 star "});
  all_punctuation_.push_back({&dash_regex_, " `00 dash "});
  all_punctuation_.push_back({&semicolen_regex_, " `00 semicolen "});
  all_punctuation_.push_back({&left_paren_regex_, " `00 left `00 paren "});
  all_punctuation_.push_back({&right_paren_regex_, " `00 right `00 paren "});
  all_punctuation_.push_back({&at_regex_, " `00 at "});
  all_punctuation_.push_back(
      {&all_punctuation_with_pause_regex_, " `00 \\1 `p10 "});

  some_punctuation_.push_back({&some_punctuation_remove_list_, ""});
  some_punctuation_.push_back({&dash_regex_, " `00 dash "});
  some_punctuation_.push_back({&some_punctuation_pause_list_, "\\0 `p5 \\2"});

  no_punctuation_.push_back({&none_punctuation_removal_list_, "\\1 `p10 "});
}

string ECITextFormatter::Format(const string& text,
                                const PunctuationMode punctuation_mode,
                                const bool split_caps, const bool capitalized,
                                const bool allcaps_beep) {
  string output = text;
  RegexToReplaceStringContainer* punctuation_config_ptr = nullptr;
  switch (punctuation_mode) {
    case ALL:
      punctuation_config_ptr = &all_punctuation_;
      break;
    case SOME:
      punctuation_config_ptr = &some_punctuation_;
      break;
    case NONE:
      punctuation_config_ptr = &no_punctuation_;
      break;
    default:
      // Todo: implement proper error handling here.
      return output;
  }

  for (const auto& regex_to_replace_str : *punctuation_config_ptr) {
    // First value of the pair is the regex, second is the replace string.
    output = regex_replace(output, *regex_to_replace_str.first,
                           regex_to_replace_str.second);
  }

  if (capitalized) {
    output = regex_replace(output, capitalize_regex_, " `ar `p10 \\1");
  }

  if (split_caps) {
    if (!capitalized) {
      // If capitalized is set to true, pauses have been already added.
      output = regex_replace(output, capitalize_regex_, " `p1 \\1");
    }
    // Todo: add possibly missing other variatons.
  }

  return output;
}

string ECITextFormatter::FormatSingleChar(const char chr) {
  string letter_pitch;
  if (isupper(chr)) {
    letter_pitch = "`vb80 ";
  }
  const string msg = letter_pitch + "`ts2 " + chr + " `ts0";
  return msg;
}

string ECITextFormatter::FormatPause(const string& text) {
  string output = regex_replace(text, pause_symbol_, " `p1 ");
  return output;
}
