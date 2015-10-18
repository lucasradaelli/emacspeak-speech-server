#include "text_formatter.h"
#include <boost/regex.hpp>
#include <string>

using std::string;
using boost::regex;
using boost::regex_replace;

const regex TextFormatter::star_regex_("\\*");
const regex TextFormatter::dash_regex_("-");
const regex TextFormatter::semicolen_regex_(";");
const regex TextFormatter::left_paren_regex_("\\(");
const regex TextFormatter::right_paren_regex_("\\)");
const regex TextFormatter::at_regex_("@");
const regex TextFormatter::all_punctuation_regex_("([.,!?;:+=/'\\\"@$%&_*()])");

ECITextFormatter::ECITextFormatter() {
  all_punctuation_.push_back({&star_regex_, " `00 star "});
  all_punctuation_.push_back({&dash_regex_, " `00 dash "});
  all_punctuation_.push_back({&semicolen_regex_, " `00 semicolen "});
  all_punctuation_.push_back({&left_paren_regex_, " `00 left `00 paren "});
  all_punctuation_.push_back({&right_paren_regex_, " `00 right `00 paren "});
  all_punctuation_.push_back({&at_regex_, " `00 at "});
  all_punctuation_.push_back({&all_punctuation_regex_, " `00 \\1 `p10 "});
}

string ECITextFormatter::Format(const string& text,
                                const PunctuationMode punctuation_mode) {
  string output = text;
  if (punctuation_mode == ALL) {
    for (const auto& regex_to_replace_str : all_punctuation_) {
      // First value of the pair is the regex, second is the replace string.
      output = regex_replace(output, *regex_to_replace_str.first,
                             regex_to_replace_str.second);
    }
  }

  return output;
}
