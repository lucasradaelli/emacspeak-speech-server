#include "text_formatter.h"
#include <boost/regex.hpp>
#include <string>

using std::string;
using std::vector;
using std::pair;
using boost::regex;
using boost::regex_replace;

const regex TextFormatter::star_regex_("\\*");
const regex TextFormatter::dash_regex_("-");
const regex TextFormatter::semicolen_regex_(";");
const regex TextFormatter::left_paren_regex_("\\(");
const regex TextFormatter::right_paren_regex_("\\)");
const regex TextFormatter::at_regex_("@");
// Todo: Is this useful at all? Some punctuation was already removed by regex
// defined above.
const regex TextFormatter::all_punctuation_regex_("([.,!?;:+=/'\\\"@$%&_*()])");

const regex TextFormatter::some_punctuation_remove_list_("[*&()\"]");

// todo: find a better name and discover exactly what this regex trys to match.
// It appears to be inserting a small pause between a letter followed by a
// letter or number.
const regex TextFormatter::some_punctuation_pause_list_(
    "([a-zA-Z])([@!;/:()=\\#,.\"])+([0-9a-zA-Z])");

ECITextFormatter::ECITextFormatter() {
  all_punctuation_.push_back({&star_regex_, " `00 star "});
  all_punctuation_.push_back({&dash_regex_, " `00 dash "});
  all_punctuation_.push_back({&semicolen_regex_, " `00 semicolen "});
  all_punctuation_.push_back({&left_paren_regex_, " `00 left `00 paren "});
  all_punctuation_.push_back({&right_paren_regex_, " `00 right `00 paren "});
  all_punctuation_.push_back({&at_regex_, " `00 at "});
  all_punctuation_.push_back({&all_punctuation_regex_, " `00 \\1 `p10 "});

  some_punctuation_.push_back({&some_punctuation_remove_list_, ""});
  some_punctuation_.push_back({&some_punctuation_pause_list_, "\\0 `p5 \\2"});

  // todo: add the no punctuation rules.
}

string ECITextFormatter::Format(const string& text,
                                const PunctuationMode punctuation_mode) {
  string output = text;
  vector<pair<const regex*, string>>* punctuation_config = nullptr;
  switch (punctuation_mode) {
    case ALL:
      punctuation_config = &all_punctuation_;
      break;
    case SOME:
      punctuation_config = &some_punctuation_;
      break;
    case NONE:
      punctuation_config = &no_punctuation_;
      break;
    default:
      // Todo: implement proper error handling here.
      return output;
  }

  for (const auto& regex_to_replace_str : *punctuation_config) {
    // First value of the pair is the regex, second is the replace string.
    output = regex_replace(output, *regex_to_replace_str.first,
                           regex_to_replace_str.second);
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
