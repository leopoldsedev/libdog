#pragma once

#include <iostream>
#include <algorithm>
#include <sstream>
#include <array>
#include <memory>
#include <vector>
#include <cassert>

#include "BoardState.hpp"
#include "CardsState.hpp"
#include "Piece.hpp"
#include "Debug.hpp"
#include "Util.hpp"
#include "Action.hpp"
#include "Notation.hpp"

// TODO Correctly use different ways of specifying pieces/positions (BoardPosition, PieceRef, PiecePtr, path_idx, finish_idx) depending on the minimum needed for any given function
// TODO Implement playing for team mate when a player has all pieces in the finish
// TODO Implement notation for board state

#define APPEND(x, y) do { \
	auto b = (y); \
	(x).insert((x).end(), b.begin(), b.end()); \
} while(0);


class DogGame {
	public:
		bool check_turns;
		bool check_hands;
		bool check_give_phase;

		int player_turn;
		int next_hand_size;

		std::array<Card, PLAYER_COUNT> give_buffer;

		BoardState board_state;
		CardsState cards_state;

		DogGame(bool check_turns, bool check_hands, bool check_give_phase) : check_turns(check_turns), check_hands(check_hands), check_give_phase(check_give_phase) {
			reset();
		}

		DogGame(bool check_turns, bool check_hands) : DogGame(check_turns, check_hands, false) {
		}

		DogGame() : DogGame(true, true) {
		}

		void reset() {
			board_state = BoardState();
			cards_state = CardsState();

			player_turn = 0;
			next_hand_size = 6;

			cards_state.hand_out_cards(next_hand_size);
			next_hand_size = calc_next_hand_size(next_hand_size);

			give_buffer.fill(None);
		}

		void load_board(std::string notation_str) {
			board_state = from_notation(notation_str);
		}

		// -1 ... undecided (game not concluded yet)
		//  0 ... team of player 0 won
		//  1 ... team of player 1 won
		//  2 ... both teams won (invalid, should not be possible)
		int result() {
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

		int calc_next_hand_size(int current_hand_size) {
			if (current_hand_size == 2) {
				return 6;
			} else {
				return next_hand_size - 1;
			}
		}

		void next_turn() {
			player_turn++;
			player_turn %= PLAYER_COUNT;
		}

		bool give_phase_completed() {
			for (Card card : give_buffer) {
				if (card == None) {
					return false;
				}
			}

			return true;
		}

		bool check_common(int player, const ActionVar& action) {
			if (result() >= 0) {
				// Game is already over
				return false;
			}

			if (check_give_phase) {
				if (!give_phase_completed()) {
					return false;
				}
			}

			if (!action_is_valid(action)) {
				return false;
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

		void modify_state_common(int player, const ActionVar& action) {
			cards_state.remove_card_from_hand(player, action_get_card(action));

			next_turn();

			if (cards_state.hands_empty()) {
				// Round is done, new cards need to be handed out
				cards_state.hand_out_cards(next_hand_size);
				next_hand_size = calc_next_hand_size(next_hand_size);

				// Additional increment so that not the same player always starts in every round
				next_turn();

				// Reset give_buffer because every player now has to give a card to their team mate
				give_buffer.fill(None);
			}
		}

		bool play_notation(int player, std::string notation_str, bool modify_state = true) {
			optional<ActionVar> action = try_parse_notation(player, notation_str);

			if (action.has_value()) {
				return play(player, action.value(), modify_state);
			} else {
				return false;
			}
		}

		bool play(int player, ActionVar action, bool modify_state = true) {
			// TODO This can be done more elegantly with visit. See https://en.cppreference.com/w/cpp/utility/variant/visit
			if (MATCH(&action, Give, a)) {
				return try_play(player, *a, modify_state);
			} else if (MATCH(&action, Discard, a)) {
				return try_play(player, *a, modify_state);
			} else if (MATCH(&action, Swap, a)) {
				return try_play(player, *a, modify_state);
			} else if (MATCH(&action, Move, a)) {
				return try_play(player, *a, modify_state);
			} else if (MATCH(&action, MoveMultiple, a)) {
				return try_play(player, *a, modify_state);
			} else if (MATCH(&action, Start, a)) {
				return try_play(player, *a, modify_state);
			}
			assert(false);
		}

		bool try_play(int player, const Give& give, bool modify_state = true) {
			// TODO This is a rather crude way of not checking give_phase. Add parameter to check_commong() or something similar instead
			bool check_give_phase_state = check_give_phase;
			check_give_phase = false;

			if (!check_common(player, give)) {
				return false;
			}

			check_give_phase = check_give_phase_state;

			Card& give_buffer_player = give_buffer.at(player);

			if (give_buffer_player != None) {
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
				give_buffer_player = give.get_card();

				if (give_phase_completed()) {
					// Every player gave a card, add them to the hand of their team mate
					for (int player = 0; player < PLAYER_COUNT; player++) {
						Card given_card = give_buffer.at(player);
						int player_team = GET_TEAM_PLAYER_IDX(player);

						cards_state.add_card_to_hand(player_team, given_card);
					}
				}

				modify_state_common(player, give);
			}

			return true;
		}

		bool try_play(int player, const Discard& discard, bool modify_state = true) {
			if (!check_common(player, discard)) {
				return false;
			}

			std::vector<ActionVar> possible_card_plays = get_possible_card_plays(player);
			if (possible_card_plays.size() > 0) {
				// Can only discard a card if none of them can be played
				return false;
			}

			bool has_card = cards_state.check_player_has_card(player, discard.get_card());

			if (!has_card) {
				// Card has to be in player's hand to be discarded
				return false;
			}

			// Card will be removed from hand in modify_state_common()
			if (modify_state) {
				modify_state_common(player, discard);
			}

			return true;
		}

		bool try_play(int player, const Start& start, bool modify_state = true) {
			if (!check_common(player, start)) {
				return false;
			}

			bool legal = board_state.start_piece(player, modify_state);

			if (legal && modify_state) {
				modify_state_common(player, start);
			}

			return legal;
		}

		bool try_play(int player, const Move& move, bool modify_state = true) {
			if (!check_common(player, move)) {
				return false;
			}

			int count = move.get_count();
			bool avoid_finish = move.get_avoid_finish();

			PiecePtr& piece = board_state.ref_to_piece(move.get_piece_ref());

			bool legal = board_state.move_piece(piece, count, avoid_finish, modify_state, false);

			if (legal && modify_state) {
				modify_state_common(player, move);
			}

			return legal;
		}

		bool try_play(int player, const MoveMultiple& move_multiple, bool modify_state = true) {
			if (!check_common(player, move_multiple)) {
				return false;
			}

			bool legal = board_state.move_multiple_pieces(move_multiple.get_move_specifiers(), modify_state);

			if (legal && modify_state) {
				modify_state_common(player, move_multiple);
			}

			return legal;
		}

		bool try_play(int player, const Swap& swap, bool modify_state = true) {
			if (!check_common(player, swap)) {
				return false;
			}

			PiecePtr& piece_1 = board_state.ref_to_piece(swap.get_piece_1());
			PiecePtr& piece_2 = board_state.ref_to_piece(swap.get_piece_2());

			bool legal = board_state.swap_pieces(piece_1, piece_2, modify_state);

			if (legal && modify_state) {
				modify_state_common(player, swap);
			}

			return legal;
		}

		std::vector<ActionVar> possible_gives(int player) {
			std::vector<ActionVar> result;

			std::vector<Card> cards = cards_state.get_hand(player).cards;

			for (Card card : cards) {
				Give give(card);
				result.push_back(give);
			}

			return result;
		}

		std::vector<ActionVar> possible_discards(int player) {
			std::vector<ActionVar> result;

			std::vector<Card> cards = cards_state.get_hand(player).cards;

			for (Card card : cards) {
				Discard discard(card);
				result.push_back(discard);
			}

			return result;
		}

		std::vector<ActionVar> possible_starts(int player, Card card, bool is_joker) {
			std::vector<ActionVar> result;

			Start start(card, is_joker);

			bool legal = try_play(player, start, false);
			if (legal) {
				result.push_back(start);
			}

			return result;
		}

		std::vector<ActionVar> possible_moves(int player, Card card, int count, bool is_joker) {
			std::vector<ActionVar> result;

			for (int i = 0; i < PIECE_COUNT; i++) {
				PieceRef piece_ref(player, i);
				PiecePtr& piece = board_state.ref_to_piece(piece_ref);

				if (piece->position.area == Kennel) {
					continue;
				}

				Move move(card, piece_ref, count, false, is_joker);

				bool legal = try_play(player, move, false);
				if (legal) {
					result.push_back(move);
				}

				if (piece->position.area == Path) {
					// Check if avoid_finish flag would have an effect
					int count_on_path_result;
					// TODO This is not a good initialization
					BoardPosition position_result = BoardPosition(0);
					legal = board_state.try_enter_finish(player, piece->position.idx, count, piece->blocking, position_result, count_on_path_result);
					assert(legal);

					if (position_result.area == Finish) {
						// avoid_finish flag would have an effect -> add it as possible action if it is legal (i.e. if the path isn't blocked)
						move = Move(card, piece_ref, count, true, is_joker);

						legal = try_play(player, move, false);
						if (legal) {
							result.push_back(move);
						}
					}
				}
			}

			return result;
		}

		// TODO Add parameter for max count per piece to avoid branching that will inevitably result in illegal moves
		std::vector<ActionVar> _possible_move_multiples(int player, Card card, int count, bool is_joker, std::vector<std::tuple<PieceRef, int>> pieces, std::vector<MoveSpecifier> move_specifiers) {
			assert(pieces.size() > 0);

			std::vector<ActionVar> result;

			PieceRef piece_ref = std::get<0>(pieces.back());
			int max_count = std::get<1>(pieces.back());

			if (pieces.size() == 1) {
				std::vector<MoveSpecifier> move_specifiers_extended = move_specifiers;
				MoveSpecifier move_specifier(piece_ref, count, false);
				if (count > 0) {
					move_specifiers_extended.insert(move_specifiers_extended.begin(), move_specifier);
				}
				assert(move_specifiers_extended.size() > 0);
				MoveMultiple move_mult(card, move_specifiers_extended);
				assert(action_is_valid(move_mult));

				result.push_back(move_mult);

				return result;
			}

			pieces.pop_back();

			for (int i = 0; i <= std::min(count, max_count); i++) {
				std::vector<MoveSpecifier> move_specifiers_extended = move_specifiers;

				MoveSpecifier move_specifier(piece_ref, i, false);
				if (i > 0) {
					move_specifiers_extended.insert(move_specifiers_extended.begin(), move_specifier);
				}

				std::vector<ActionVar> moves = _possible_move_multiples(player, card, count - i, is_joker, pieces, move_specifiers_extended);
				APPEND(result, moves);

				int count_on_path_result;
				// TODO This is not a good initialization
				BoardPosition position_result = BoardPosition(0);
				PiecePtr& piece = board_state.ref_to_piece(piece_ref);
				bool legal = board_state.try_enter_finish(player, piece->position.idx, i, piece->blocking, position_result, count_on_path_result);
				assert(legal);

				if (position_result.area == Finish) {
					// avoid_finish flag would have an effect -> continue recursion tree (move should be legal since blocked path is avoided by the max_counts)
					move_specifiers_extended = move_specifiers;

					move_specifier.avoid_finish = true;
					if (i > 0) {
						move_specifiers_extended.insert(move_specifiers_extended.begin(), move_specifier);
					}

					moves = _possible_move_multiples(player, card, count - i, is_joker, pieces, move_specifiers_extended);
					APPEND(result, moves);
				}
			}

			return result;
		}

		std::vector<ActionVar> possible_move_multiples(int player, Card card, int count, bool is_joker) {
			std::vector<ActionVar> result;

			std::vector<PieceRef> piece_refs;

			APPEND(piece_refs, board_state.get_pieces_in_area(player, Path));
			APPEND(piece_refs, board_state.get_pieces_in_area(GET_TEAM_PLAYER_IDX(player), Path));

			if (piece_refs.size() == 0) {
				return {};
			}

			std::vector<std::tuple<PieceRef, int>> pieces_with_max_counts;

			for (PieceRef piece_ref : piece_refs) {
				PiecePtr& piece = board_state.ref_to_piece(piece_ref);
				int max_count = board_state.possible_steps_of_piece(piece_ref.player, piece->position, piece->blocking, false);

				pieces_with_max_counts.push_back(std::make_tuple(piece_ref, max_count));
			}

			result = _possible_move_multiples(player, card, count, is_joker, pieces_with_max_counts, {});

			return result;
		}

		std::vector<ActionVar> possible_swaps(int player, Card card, bool is_joker) {
			std::vector<ActionVar> result;

			for (int i = 0; i < PIECE_COUNT; i++) {
				for (int j = 0; j < PLAYER_COUNT; j++) {
					for (int k = 0; k < PIECE_COUNT; k++) {
						Swap swap(card, PieceRef(player, i), PieceRef(j, k), is_joker);

						bool legal = try_play(player, swap, false);
						if (legal) {
							result.push_back(swap);
						}
					}
				}
			}

			return result;
		}

		std::vector<ActionVar> get_possible_card_plays(int player) {
			std::vector<ActionVar> result;

			std::vector<Card> cards;

			// Process joker card if in hand
			bool has_joker = cards_state.check_player_has_card(player, Joker);
			if (has_joker) {
				for (int i = Ace; i != Joker; i++) {
					Card card = static_cast<Card>(i);
					// TODO Switch player to team mate if player is already done
					std::vector<ActionVar> actions = possible_actions_for_card(player, card, true);
					APPEND(result, actions);
				}
			}

			// Process hand cards
			cards = cards_state.get_hand(player).cards;
			// Remove duplicates
			// Source: https://stackoverflow.com/a/1041939/3118787
			sort(cards.begin(), cards.end());
			cards.erase(unique(cards.begin(), cards.end()), cards.end());

			for (Card card : cards) {
				// TODO Switch player to team mate if player is already done
				std::vector<ActionVar> actions = possible_actions_for_card(player, card, false);
				APPEND(result, actions);
			}

			return result;
		}

		std::vector<ActionVar> get_possible_actions(int player) {
			if (!give_phase_completed()) {
				return possible_gives(player);
			}

			std::vector<ActionVar> result = get_possible_card_plays(player);

			if (result.size() == 0) {
				// None of the cards can be played, player can only discard one of them

				APPEND(result, possible_discards(player));
			}

			// If it is a player's turn they either can play a card or discard a card
			// It should not be possible to be next and not have a possible action (i.e. no cards in hand).
			assert(player != player_turn || result.size() > 0);

			return result;
		}

		// TODO Try removing is_joker parameter and instead just passing Joker as card
		std::vector<ActionVar> possible_actions_for_card(int player, Card card, bool is_joker) {
			if (card == None || card == Joker) {
				return {};
			}

			std::vector<ActionVar> result;

			// TODO This is a rather crude way of not checking hands. Add parameter to try_play() instead
			bool check_hands_state = check_hands;
			check_hands = false;

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
				case None:
				default:
					break;
			}

			check_hands = check_hands_state;

			return result;
		}

		friend std::ostream& operator<<(std::ostream& os, DogGame const& obj) {
			  return os << obj.to_str();
		}

		std::string to_str() const {
			std::stringstream ss;

			ss << board_state << std::endl;

			ss << "Notation: " << to_notation(board_state) << std::endl;

			std::string hands_str = get_hands_str();
			ss << hands_str;
			return ss.str();
		}

		std::string get_hands_str() const {
			std::stringstream ss;

			for (std::size_t i = 0; i < cards_state.hands.size(); i++) {
				ss << i << ": ";
				ss << cards_state.hands.at(i);
				ss << std::endl;
			}

			return ss.str();
		}
};
