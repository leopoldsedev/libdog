#include <libdog/DogGame.hpp>

#include "Debug.hpp"


#define APPEND(x, y) do { \
	auto b = (y); \
	(x).insert((x).end(), b.begin(), b.end()); \
} while(0);

namespace libdog {

DogGame::DogGame(bool canadian_rule, bool check_turns, bool check_hands, bool check_give_phase) : canadian_rule(canadian_rule), check_turns(check_turns), check_hands(check_hands), check_give_phase(check_give_phase) {
	reset();
}

void DogGame::reset() {
	board_state = BoardState();
	cards_state = CardsState();

	_reset();
}

void DogGame::_reset() {
	player_turn = 0;
	next_hand_size = STARTING_HANDOUT_SIZE;

	cards_state.hand_out_cards(next_hand_size);
	next_hand_size = calc_next_hand_size(next_hand_size);

	give_phase_done = false;
}

void DogGame::reset_with_deck(const std::string& card_str) {
	std::vector<Card> cards = cards_from_str(card_str);
	cards_state = CardsState(cards);
	_reset();
}

void DogGame::load_board(const std::string& notation_str) {
	board_state = from_notation(notation_str);
}

// -1 ... undecided (game not concluded yet)
//  0 ... team of player 0 won
//  1 ... team of player 1 won
//  2 ... both teams won (invalid, should not be possible)
int DogGame::result() {
	std::array<bool, PLAYER_COUNT> players_finish_full;
	for (std::size_t i = 0; i < players_finish_full.size(); i++) {
		players_finish_full.at(i) = board_state.check_finish_full(i);
	}

	bool team_0_won = (players_finish_full.at(0) && players_finish_full.at(2));
	bool team_1_won = (players_finish_full.at(1) && players_finish_full.at(3));

	if (team_0_won && team_1_won) {
		return 2;
	}

	if (team_0_won) {
		return 0;
	}

	if (team_1_won) {
		return 1;
	}

	return -1;
}

int DogGame::calc_next_hand_size(int current_hand_size) const {
	if (current_hand_size == MIN_HANDOUT_SIZE) {
		return STARTING_HANDOUT_SIZE;
	}

	return next_hand_size - 1;
}

void DogGame::next_turn() {
	player_turn++;
	player_turn %= PLAYER_COUNT;
}

bool DogGame::check_common(int player, const ActionVar& action) {
	if (result() >= 0) {
		// Game is already over
		return false;
	}

	bool check_give = VAR_IS(action, Give) ? false : check_give_phase;
	if (check_give) {
		if (!give_phase_done) {
			return false;
		}
	}

	if (check_turns) {
		if (player != player_turn) {
			// It needs to be the player's turn
			return false;
		}
	}

	if (check_hands) {
		Card card = action_get_card(action);
		bool player_has_card = cards_state.check_player_has_card(player, card);

		if (!player_has_card) {
			// Card being played must be in hand
			return false;
		}
	}

	return true;
}

void DogGame::modify_state_common(int player, const ActionVar& action) {
	if (!VAR_IS(action, Give)) {
		cards_state.discard(player, action_get_card(action));
	}

	next_turn();

	if (cards_state.hands_empty()) {
		// Round is done, new cards need to be handed out
		cards_state.hand_out_cards(next_hand_size);
		next_hand_size = calc_next_hand_size(next_hand_size);

		// Additional increment so that not the same player always starts in every round
		next_turn();

		// Every player now has to give a card to their team mate
		give_phase_done = false;
	}
}

bool DogGame::play_notation(int player, std::string notation_str, bool modify_state) {
	int player_notation = switch_to_team_mate_if_done(player);

	optional<ActionVar> action = try_parse_notation(player_notation, notation_str);

	if (action.has_value()) {
		return play(player, action.value(), modify_state, true);
	} else {
		return false;
	}
}

bool DogGame::play(int player, ActionVar action, bool modify_state, bool common_checks) {
	bool legal;

	if (!action_is_valid(action)) {
		return false;
	}

	if (common_checks && !check_common(player, action)) {
		return false;
	}

	// TODO This can be done more elegantly with visit. See https://en.cppreference.com/w/cpp/utility/variant/visit
	if (MATCH(&action, Give, a)) {
		legal = try_play(player, *a, modify_state);
	} else if (MATCH(&action, Discard, a)) {
		legal = try_play(player, *a, modify_state);
	} else if (MATCH(&action, Swap, a)) {
		legal = try_play(player, *a, modify_state);
	} else if (MATCH(&action, Move, a)) {
		legal = try_play(player, *a, modify_state);
	} else if (MATCH(&action, MoveMultiple, a)) {
		legal = try_play(player, *a, modify_state);
	} else if (MATCH(&action, Start, a)) {
		legal = try_play(player, *a, modify_state);
	} else {
		assert(false);
	}

	if (legal && modify_state) {
		modify_state_common(player, action);
	}

	return legal;
}

int DogGame::switch_to_team_mate_if_done(int player) {
	int team_mate = GET_TEAM_PLAYER_IDX(player);
	if (board_state.check_finish_full(player) && !board_state.check_finish_full(team_mate)) {
		return team_mate;
	}

	return player;
}

bool DogGame::try_play(int player, const Give& give, bool modify_state) {
	assert(!give_phase_done || !cards_state.give_buffer_full(player));
	if (give_phase_done || cards_state.give_buffer_full(player)) {
		// Player already gave
		return false;
	}

	bool has_card = cards_state.check_player_has_card(player, give.get_card());

	if (!has_card) {
		// Card has to be in player's hand to be discarded
		return false;
	}

	// Card will be removed from hand in modify_state_common()
	if (modify_state) {
		cards_state.give_card(player, give.get_card());

		if (cards_state.give_buffer_full()) {
			// Every player gave a card, add them to the hand of their team mate
			cards_state.execute_give();
			give_phase_done = true;
		}
	}

	return true;
}

bool DogGame::try_play(int player, const Discard& discard, __attribute__((unused)) bool modify_state) {
	std::vector<ActionVar> possible_card_plays = get_possible_card_plays(player);
	if (!possible_card_plays.empty()) {
		// Can only discard a card if none of them can be played
		return false;
	}

	// Card has to be in player's hand to be discarded
	bool has_card = cards_state.check_player_has_card(player, discard.get_card());

	// Card will be removed from hand in modify_state_common()
	return has_card;
}

bool DogGame::try_play(int player, __attribute__((unused)) const Start& start, bool modify_state) {
	player = switch_to_team_mate_if_done(player);

	bool legal = board_state.start_piece(player, modify_state);

	return legal;
}

bool DogGame::try_play(int player, const Move& move, bool modify_state) {
	player = switch_to_team_mate_if_done(player);

	int count = move.get_count();
	bool avoid_finish = move.get_avoid_finish();

	PiecePtr& piece = board_state.ref_to_piece(move.get_piece_ref());

	if (piece->player != player) {
		return false;
	}

	bool legal = board_state.move_piece(piece, count, avoid_finish, modify_state, false);

	return legal;
}

bool DogGame::try_play(int player, const MoveMultiple& move_multiple, bool modify_state) {
	player = switch_to_team_mate_if_done(player);

	for (MoveSpecifier move_specifier : move_multiple.get_move_specifiers()) {
		PiecePtr& piece = board_state.ref_to_piece(move_specifier.piece_ref);

		if (canadian_rule) {
			if (piece->player != player && piece->player != GET_TEAM_PLAYER_IDX(player)) {
				// Can only move pieces of player or team mate
				return false;
			}

			if (!RULE_ALLOW_SEVEN_MOVE_TEAMMATE_IF_BLOCKED) {
				if (piece->player != player && piece->blocking) {
					// Cannot move pieces of team mate that are currently blocking
					return false;
				}
			}
		} else {
			if (piece->player != player) {
				// Can only move pieces of player
				return false;
			}
		}
	}

	bool legal = board_state.move_multiple_pieces(move_multiple.get_move_specifiers(), modify_state);

	return legal;
}

bool DogGame::try_play(int player, const Swap& swap, bool modify_state) {
	player = switch_to_team_mate_if_done(player);

	PiecePtr& piece_1 = board_state.ref_to_piece(swap.get_piece_1());
	PiecePtr& piece_2 = board_state.ref_to_piece(swap.get_piece_2());

	if (piece_1->player != player && piece_2->player != player) {
		// One of the pieces has to belong to the player that is playing the swap
		return false;
	}

	bool legal = board_state.swap_pieces(piece_1, piece_2, modify_state);

	return legal;
}

std::vector<ActionVar> DogGame::possible_gives(int player) {
	std::vector<ActionVar> result;

	std::vector<Card> cards = cards_state.get_hand_cards(player, true);

	for (Card card : cards) {
		Give give(card);
		result.push_back(give);
	}

	return result;
}

std::vector<ActionVar> DogGame::possible_discards(int player) {
	std::vector<ActionVar> result;

	std::vector<Card> cards = cards_state.get_hand_cards(player, true);

	for (Card card : cards) {
		Discard discard(card);
		result.push_back(discard);
	}

	return result;
}

std::vector<ActionVar> DogGame::possible_starts(int player, Card card, bool is_joker) {
	std::vector<ActionVar> result;

	Start start(card, is_joker);

	bool legal = play(player, start, false, false);
	if (legal) {
		result.push_back(start);
	}

	return result;
}

std::vector<ActionVar> DogGame::possible_moves(int player, Card card, int count, bool is_joker) {
	std::vector<ActionVar> result;

	for (int i = 0; i < PIECE_COUNT; i++) {
		PieceRef piece_ref(player, i);
		PiecePtr& piece = board_state.ref_to_piece(piece_ref);

		if (piece->position.area == Kennel) {
			continue;
		}

		Move move(card, piece_ref, count, false, is_joker);

		bool legal = play(player, move, false, false);
		if (legal) {
			result.push_back(move);
		}

		if (piece->position.area == Path) {
			// Check if avoid_finish flag would have an effect
			int count_on_path_result;
			// TODO This is not a good initialization
			BoardPosition position_result = BoardPosition(0);
			legal = board_state.try_enter_finish(player, piece->position.idx, count, piece->blocking, position_result, count_on_path_result);

			if (legal && position_result.area == Finish) {
				// avoid_finish flag would have an effect -> add it as possible action if it is legal (i.e. if the path isn't blocked)
				move = Move(card, piece_ref, count, true, is_joker);

				legal = play(player, move, false, false);
				if (legal) {
					result.push_back(move);
				}
			}
		}
	}

	return result;
}

// TODO Refactor this mess of a function
DogGame::my_set DogGame::_possible_move_multiples(int player, Card card, BoardState board, int count, bool is_joker, std::vector<std::tuple<PieceRef, BoardPosition>> pieces, std::vector<MoveSpecifier> move_specifiers) {
	assert(!pieces.empty());

	my_set result;

	if (count == 0) {
		MoveMultiple move_mult(card, move_specifiers, is_joker);

#ifndef NDEBUG
		bool legal = play(player, move_mult, false, false);
		if (!legal) {
			PRINT_DBG(board_state);
			for (auto m : move_specifiers) {
				PRINT_DBG(m.piece_ref);
				PRINT_DBG(m.count);
			}
		}
		assert(legal);
#endif

		std::tuple<ActionVar, BoardState> pair = std::make_tuple(move_mult, board);
		result.insert(pair);

		return result;
	}

	for (std::size_t i = 0; i < pieces.size(); i++) {
		BoardPosition position = std::get<1>(pieces.at(i));

		BoardState board_copy = board;
		PiecePtr& piece = board_copy.get_piece(position);
		assert(piece != nullptr);

		if (!RULE_ALLOW_SEVEN_MOVE_TEAMMATE_IF_BLOCKED) {
			if (piece->player != player && piece->blocking) {
				continue;
			}
		}

		PieceRef piece_ref = std::get<0>(pieces.at(i));
		MoveSpecifier move_specifier(piece_ref, 1, false);

		// Save all pointers to pieces to enable tracking piece positions through the following move_piece() call
		std::vector<Piece*> piece_ptrs;
		for (std::tuple<PieceRef, BoardPosition> t : pieces) {
			PiecePtr& piece = board_copy.get_piece(std::get<1>(t));
			piece_ptrs.push_back(piece.get());
		}

		bool legal = board_copy.move_piece(piece, move_specifier.count, move_specifier.avoid_finish, true, true);

		if (legal) {
			std::vector<std::tuple<PieceRef, BoardPosition>> pieces_copy = pieces;
			for (std::size_t i = 0; i < pieces_copy.size(); i++) {
				assert(piece_ptrs.at(i) != nullptr);
				std::get<1>(pieces_copy.at(i)) = piece_ptrs.at(i)->position;
			}

			std::vector<MoveSpecifier> move_specifiers_copy = move_specifiers;
			move_specifiers_copy.push_back(move_specifier);

			my_set s = _possible_move_multiples(player, card, board_copy, count - 1, is_joker, pieces_copy, move_specifiers_copy);

			result.insert(s.begin(), s.end());
		}
	}

	return result;
}

// TODO Make more efficient
// TODO Currently avoid_finish flag is always set to false, generate also the moves that have this flag set to true
// TODO Consolidate consecutive moves of the same piece
std::vector<ActionVar> DogGame::possible_move_multiples(int player, Card card, int count, bool is_joker) {
	std::vector<ActionVar> result;

	std::vector<PieceRef> piece_refs;

	APPEND(piece_refs, board_state.get_pieces_in_area(player, Path));
	APPEND(piece_refs, board_state.get_pieces_in_area(player, Finish));

	if (canadian_rule) {
		APPEND(piece_refs, board_state.get_pieces_in_area(GET_TEAM_PLAYER_IDX(player), Path));
		APPEND(piece_refs, board_state.get_pieces_in_area(GET_TEAM_PLAYER_IDX(player), Finish));
	}

	if (piece_refs.empty()) {
		return {};
	}

	std::vector<std::tuple<PieceRef, BoardPosition>> pieces;

	for (PieceRef& piece_ref : piece_refs) {
		PiecePtr& piece = board_state.ref_to_piece(piece_ref);
		assert(piece != nullptr);
		pieces.push_back(std::make_tuple(piece_ref, piece->position));
		assert(board_state.get_piece(piece->position) != nullptr);
	}

	my_set s = _possible_move_multiples(player, card, board_state, count, is_joker, pieces, {});

	for (std::tuple<ActionVar, BoardState> x : s) {
		auto action = std::get<0>(x);
		result.push_back(action);
	}

	return result;
}

std::vector<ActionVar> DogGame::possible_swaps(int player, Card card, bool is_joker) {
	std::vector<ActionVar> result;

	for (int i = 0; i < PIECE_COUNT; i++) {
		for (int j = 0; j < PLAYER_COUNT; j++) {
			for (int k = 0; k < PIECE_COUNT; k++) {
				Swap swap(card, PieceRef(player, i), PieceRef(j, k), is_joker);

				bool legal = play(player, swap, false, false);
				if (legal) {
					result.push_back(swap);
				}
			}
		}
	}

	return result;
}

std::vector<ActionVar> DogGame::get_possible_actions(int player) {
	if (result() >= 0) {
		// Game is already over
		return {};
	}

	// TODO Maybe also return empty set if it is not player's turn

	if (!give_phase_done) {
		return possible_gives(player);
	}

	std::vector<ActionVar> result = get_possible_card_plays(player);

	if (result.empty()) {
		// None of the cards can be played, player can only discard one of them

		APPEND(result, possible_discards(player));
	}

	// If it is a player's turn they either can play a card or discard a card
	// It should not be possible to be next and not have a possible action (i.e. no cards in hand).
	assert(player != player_turn || result.size() > 0);

	return result;
}

std::vector<ActionVar> DogGame::get_possible_card_plays(int player) {
	std::vector<ActionVar> result;

	int player_to_play_for = switch_to_team_mate_if_done(player);

	// Process hand cards
	std::vector<Card> cards = cards_state.get_hand_cards(player, true);

	for (Card card : cards) {
		std::vector<ActionVar> actions = possible_actions_for_card(player_to_play_for, card, false);
		APPEND(result, actions);
	}

	return result;
}

std::vector<ActionVar> DogGame::possible_actions_for_card(int player, Card card, bool is_joker) {
	if (card == None) {
		return {};
	}

	std::vector<ActionVar> result;

	switch (card) {
		case Two: case Three: case Five: case Six: case Eight: case Nine: case Ten: case Queen:
			APPEND(result, possible_moves(player, card, simple_card_get_count(card), is_joker));
			break;
		case Ace:
			APPEND(result, possible_starts(player, card, is_joker));
			APPEND(result, possible_moves(player, card, 1, is_joker));
			APPEND(result, possible_moves(player, card, 11, is_joker));
			break;
		case Four:
			APPEND(result, possible_moves(player, card, -4, is_joker));
			APPEND(result, possible_moves(player, card, 4, is_joker));
			break;
		case Seven:
			APPEND(result, possible_move_multiples(player, card, 7, is_joker));
			break;
		case Jack:
			APPEND(result, possible_swaps(player, card, is_joker));
			break;
		case King:
			APPEND(result, possible_starts(player, card, is_joker));
			APPEND(result, possible_moves(player, card, 13, is_joker));
			break;
		case Joker:
			for (int i = Ace; i != Joker; i++) {
				Card card = static_cast<Card>(i);
				std::vector<ActionVar> actions = possible_actions_for_card(player, card, true);
				APPEND(result, actions);
			}
		case None:
		default:
			break;
	}

	return result;
}

std::string DogGame::to_str() const {
	std::stringstream ss;

	ss << board_state << std::endl;

	ss << "Notation: " << to_notation(board_state) << std::endl;

	ss << cards_state;

	return ss.str();
}

}
