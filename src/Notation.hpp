#include "Action.hpp"

using std::optional;
using std::string;


ActionVar from_notation(int player, string notation_str);

optional<ActionVar> try_parse_notation(int player, string notation_str);

std::string to_notation(int player, ActionVar action);
