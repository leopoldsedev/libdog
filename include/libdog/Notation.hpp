#pragma once

#include "Action.hpp"
#include "BoardState.hpp"


using std::optional;
using std::string;

namespace libdog {

class BoardState;

ActionVar from_notation(int player, string notation_str);

BoardState from_notation(string notation_str);

optional<ActionVar> try_parse_notation(int player, string notation_str);

optional<BoardState> try_parse_notation(string notation_str);

string to_notation(int player, ActionVar action);

string to_notation(const BoardState board);

};
