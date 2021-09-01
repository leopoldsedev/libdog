#include <regex>

#include "Notation.hpp"
#include "Debug.hpp"

using std::optional, std::nullopt;
using std::regex, std::smatch, std::sregex_iterator;
using std::stoi;


// notation_arg_str is the string without the card specifier (e.g. "#" if full notation string is "A#")
optional<ActionVar> notation_parse_forward(Card card, int player, string notation_arg_str) {
	regex regex(R"((\d)(-?))");
	smatch matches;

	bool match = regex_match(notation_arg_str, matches, regex);

	if (!match) {
		return nullopt;
	}

	int rank = stoi(matches[1]);
	bool avoid_finish = (matches[2] == "-");

	PieceRef piece_ref(player, rank);
	int count = simple_card_get_count(card);

	return Move(card, piece_ref, count, avoid_finish);
}

optional<ActionVar> notation_parse_ace(int player, string notation_arg_str) {
	regex regex(R"((#)|(('?)(\d)(-?)))");
	smatch matches;

	bool match = regex_match(notation_arg_str, matches, regex);

	if (!match) {
	}

	bool start_card = (matches[1] == "#");

	if (start_card) {
		return Start(Ace);
	} else {
		bool alt_action = (matches[3] == "'");
		int rank = stoi(matches[4]);
		bool avoid_finish = (matches[5] == "-");

		PieceRef piece_ref(player, rank);
		int count = (alt_action ? 1 : 11);

		return Move(Ace, piece_ref, count, avoid_finish);
	}
}

optional<ActionVar> notation_parse_four(int player, string notation_arg_str) {
	regex regex(R"(((')(\d))|((\d)(-?)))");
	smatch matches;

	bool match = regex_match(notation_arg_str, matches, regex);

	if (!match) {
		return nullopt;
	}
	int rank;
	bool alt_action;
	bool avoid_finish;

	if (matches[2] != "") {
		alt_action = true;
		avoid_finish = false;
		rank = stoi(matches[3]);
	} else {
		alt_action = false;
		rank = stoi(matches[5]);
		avoid_finish = (matches[6] == "-");
	}

	PieceRef piece_ref(player, rank);
	int count = (alt_action ? -4 : 4);

	return Move(Four, piece_ref, count, avoid_finish);
}

optional<ActionVar> notation_parse_seven(int player, string notation_arg_str) {
	regex regex_outer(R"((\d'?\d-?)+)");
	regex regex_inner(R"((\d)('?)(\d)(-?))");
	smatch matches;

	bool match = regex_match(notation_arg_str, matches, regex_outer);

	if (!match) {
		return nullopt;
	}

	int other_player_idx = GET_TEAM_PLAYER_IDX(player);
	int count_sum = 0;

	sregex_iterator iter;
	sregex_iterator end = sregex_iterator();
	iter = sregex_iterator(notation_arg_str.begin(), notation_arg_str.end(), regex_inner);

	std::vector<MoveSpecifier> move_specifiers;

	for(; iter != end; iter++)
	{
		int rank = stoi(iter->str(1));
		bool other_player = (iter->str(2) == "'");
		int count = stoi(iter->str(3));
		bool avoid_finish = (iter->str(4) == "-");

		int curr_player = other_player ? other_player_idx : player;

		PieceRef piece_ref(curr_player, rank);

		MoveSpecifier move_specifier(piece_ref, count, avoid_finish);
		move_specifiers.push_back(move_specifier);

		count_sum += count;
	}

	if (count_sum != 7) {
		return nullopt;
	}

	return MoveMultiple(Seven, move_specifiers);
}

optional<ActionVar> notation_parse_jack(int player, string notation_arg_str) {
	regex regex(R"((\d)(\d)(\d))");
	smatch matches;

	bool match = regex_match(notation_arg_str, matches, regex);

	if (!match) {
		return nullopt;
	}

	int rank_1 = stoi(matches[1]);
	int player_2 = stoi(matches[2]);
	int rank_2 = stoi(matches[3]);

	PieceRef piece_ref_1(player, rank_1);
	PieceRef piece_ref_2(player_2, rank_2);

	return Swap(Jack, piece_ref_1, piece_ref_2);
}

optional<ActionVar> notation_parse_king(int player, string notation_arg_str) {
	regex regex(R"((#)|((\d)(-?)))");
	smatch matches;

	bool match = regex_match(notation_arg_str, matches, regex);

	if (!match) {
		return nullopt;
	}

	bool start_card = (matches[1] == "#");

	if (start_card) {
		return Start(King);
	} else {
		int rank = stoi(matches[3]);
		bool avoid_finish = (matches[4] == "-");

		PieceRef piece_ref(player, rank);

		return Move(King, piece_ref, 13, avoid_finish);
	}
}

optional<ActionVar> notation_parse_joker(int player, string notation_arg_str) {
	// Look ahead to avoid second recursion
	string joker_card_str = notation_arg_str.substr(0, 1);
	Card joker_card = card_from_string(joker_card_str);

	if (joker_card == Joker) {
		return nullopt;
	}

	optional<ActionVar> result = try_parse_notation(player, notation_arg_str);
	if (result.has_value()) {
		action_set_joker(result.value(), true);
	}

	return result;
}

ActionVar from_notation(int player, string notation_str) {
	return try_parse_notation(player, notation_str).value();
}

// TODO Extend notation so that ommitting the digit for forward moving cards implies piece rank of 0
// TODO Extend notation so that "X#" becomes valid
optional<ActionVar> try_parse_notation(int player, string notation_str) {
	// Remove all whitespace from string
	// Source: https://stackoverflow.com/a/83538/3118787
	notation_str.erase(remove_if(notation_str.begin(), notation_str.end(), ::isspace), notation_str.end());

	string card_str = notation_str.substr(0, 1);
	notation_str.erase(0, 1);

	Card card = card_from_string(card_str);
	if (card == None) {
		return nullopt;
	}

	optional<ActionVar> result;

	switch (card) {
		case Two: case Three: case Five: case Six: case Eight: case Nine: case Ten: case Queen:
			result = notation_parse_forward(card, player, notation_str);
			break;
		case Ace:
			result = notation_parse_ace(player, notation_str);
			break;
		case Four:
			result = notation_parse_four(player, notation_str);
			break;
		case Seven:
			result = notation_parse_seven(player, notation_str);
			break;
		case Jack:
			result = notation_parse_jack(player, notation_str);
			break;
		case King:
			result = notation_parse_king(player, notation_str);
			break;
		case Joker:
			result = notation_parse_joker(player, notation_str);
			break;
		case None:
		default:
			break;
	}

	if (result.has_value()) {
		if (!action_is_valid(result.value())) {
			result = nullopt;
		}
	}

	return result;
}

string notation_move(Start start) {
	std::stringstream ss;

	ss << card_to_string(start.get_card());

	ss << "#";

	return ss.str();
}

string notation_move(Move move) {
	std::stringstream ss;

	ss << card_to_string(move.get_card());

	if (move.get_count() == -4 || move.get_count() == 1) {
		ss << "'";
	}

	ss << move.get_piece_ref().rank;

	if (move.get_avoid_finish()) {
		ss << "-";
	}

	return ss.str();
}

string notation_move_multiple(int player, MoveMultiple move_multiple) {
	std::stringstream ss;

	ss << card_to_string(move_multiple.get_card());

	for (std::size_t i = 0; i < move_multiple.get_move_specifiers().size(); i++) {
		MoveSpecifier move_specifier = move_multiple.get_move_specifiers().at(i);

		ss << move_specifier.piece_ref.rank;

		if (player != move_specifier.piece_ref.player) {
			ss << "'";
		}

		ss << move_specifier.count;

		if (move_specifier.avoid_finish) {
			ss << "-";
		}
	}

	return ss.str();
}

string notation_swap(int player, Swap swap) {
	std::stringstream ss;

	ss << card_to_string(swap.get_card());

	if (swap.get_piece_1().player == player) {
		ss << swap.get_piece_1().rank;
		ss << swap.get_piece_2().player;
		ss << swap.get_piece_2().rank;
	} else {
		ss << swap.get_piece_2().rank;
		ss << swap.get_piece_1().player;
		ss << swap.get_piece_1().rank;
	}

	return ss.str();
}

std::string to_notation(int player, ActionVar action) {
	assert(action_is_valid(action));

	std::stringstream ss;

	if (action_is_joker(action)) {
		ss << card_to_string(Joker);
	}

	// TODO This can be done more elegantly with visit. See https://en.cppreference.com/w/cpp/utility/variant/visit
	if (MATCH(&action, Give, a)) {
		// TODO
		std::cout << a;
		assert(false);
	} else if (MATCH(&action, Discard, a)) {
		// TODO
		std::cout << a;
		assert(false);
	} else if (MATCH(&action, Start, a)) {
		ss << notation_move(*a);
	} else if (MATCH(&action, Move, a)) {
		ss << notation_move(*a);
	} else if (MATCH(&action, MoveMultiple, a)) {
		ss << notation_move_multiple(player, *a);
	} else if (MATCH(&action, Swap, a)) {
		ss << notation_swap(player, *a);
	}

	return ss.str();
}
