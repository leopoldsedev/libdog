#pragma once

#include <iostream>
#include <algorithm>
#include <sstream>
#include <array>
#include <memory>
#include <vector>
#include <unordered_set>
#include <cassert>

#include "BoardState.hpp"
#include "CardsState.hpp"
#include "Action.hpp"


namespace libdog {

class DogGame {
	public:
		int player_turn;

		bool give_phase_done;

		BoardState board_state;
		CardsState cards_state;

		DogGame(bool check_turns, bool check_hands, bool check_give_phase);

		DogGame(bool check_turns, bool check_hands) : DogGame(check_turns, check_hands, false) {}

		DogGame() : DogGame(true, true, true) {}

		void reset();

		void reset_with_deck(std::string card_str);

		void load_board(std::string notation_str);

		// -1 ... undecided (game not concluded yet)
		//  0 ... team of player 0 won
		//  1 ... team of player 1 won
		//  2 ... both teams won (invalid, should not be possible)
		int result();

		bool play_notation(int player, std::string notation_str, bool modify_state = true);

		bool play(int player, ActionVar action, bool modify_state = true, bool common_checks = true);

		std::vector<ActionVar> get_possible_actions(int player);

		std::vector<ActionVar> possible_actions_for_card(int player, Card card, bool is_joker);

		int switch_to_team_mate_if_done(int player);

	private:
		bool check_turns;
		bool check_hands;
		bool check_give_phase;

		int next_hand_size;

		void _reset();

		int calc_next_hand_size(int current_hand_size);

		void next_turn();

		bool check_common(int player, const ActionVar& action);

		void modify_state_common(int player, const ActionVar& action);

		bool try_play(int player, const Give& give, bool modify_state = true);

		bool try_play(int player, const Discard& discard, __attribute__((unused)) bool modify_state = true);

		bool try_play(int player, __attribute__((unused)) const Start& start, bool modify_state = true);

		bool try_play(int player, const Move& move, bool modify_state = true);

		bool try_play(int player, const MoveMultiple& move_multiple, bool modify_state = true);

		bool try_play(int player, const Swap& swap, bool modify_state = true);

		std::vector<ActionVar> possible_gives(int player);

		std::vector<ActionVar> possible_discards(int player);

		std::vector<ActionVar> possible_starts(int player, Card card, bool is_joker);

		std::vector<ActionVar> possible_moves(int player, Card card, int count, bool is_joker);

		struct action_state_equal_to {
			bool operator()(const std::tuple<ActionVar, BoardState> &a, const std::tuple<ActionVar, BoardState> &b) const {
				return std::get<1>(a) == std::get<1>(b);
			}
		};

		struct action_state_hash {
			size_t operator()(const std::tuple<ActionVar, BoardState> &t) const {
				// TODO Optimize this hash
				BoardPosition pos0 = std::get<1>(t).ref_to_pos(PieceRef(0, 0));
				BoardPosition pos1 = std::get<1>(t).ref_to_pos(PieceRef(1, 0));
				return pos0.idx + pos1.idx;
			}
		};

		typedef std::unordered_set<std::tuple<ActionVar, BoardState>, action_state_hash, action_state_equal_to> my_set;

		my_set _possible_move_multiples(int player, Card card, BoardState board, int count, bool is_joker, std::vector<std::tuple<PieceRef, BoardPosition>> pieces, std::vector<MoveSpecifier> move_specifiers);

		std::vector<ActionVar> possible_move_multiples(int player, Card card, int count, bool is_joker);

		std::vector<ActionVar> possible_swaps(int player, Card card, bool is_joker);

		std::vector<ActionVar> get_possible_card_plays(int player);

		std::string to_str() const;

		friend std::ostream& operator<<(std::ostream& os, DogGame const& obj) {
			  return os << obj.to_str();
		}
};

}
