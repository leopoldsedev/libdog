#ifndef DOG_GAME_HPP
#define DOG_GAME_HPP

#include <iostream>
#include <sstream>
#include <array>
#include <memory>
#include <cassert>

#include "BoardState.hpp"
#include "Deck.hpp"
#include "Hand.hpp"
#include "Piece.hpp"
#include "CardPlay.hpp"


class DogGame {
	public:
		// 0 = nothing, 1 = path, 2 = finish, 3 = kennel, 4 = char
		std::array<std::array<int, 41>, 21> vis_map_spec = {
			{
				{ 4, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4 },
				{ 0, 0, 0, 0, 0, 4, 0, 4, 0, 4, 0, 4, 0, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 4, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
				{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1 },
				{ 1, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 1 },
				{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
				{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 4 },
			}
		};
		std::array<std::array<int, 41>, 21> vis_map_val = {
			{
				{ '0', 0, 0, 0, 0, 3, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 63, 0, 62, 0, 61, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '3' },
				{ 0, 0, 0, 0, 0, '3', 0, '2', 0, '1', 0, '0', 0, 0, 0, 0, 1, 0, '\\', 0, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, '0', 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1, 0, '1', 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 2, 0, '2', 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 3, 0, '3', 0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 12, 0, 11, 0, 10, 0, 9, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 51, 0, 50, 0, 49, 0, 48 },
				{ 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '/', 0, 47 },
				{ 14, 0, 0, 10, 0, 11, 0, 12, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33, 0, 32, 0, 31, 0, 30, 0, 0, 46 },
				{ 15, 0, '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45 },
				{ 16, 0, 17, 0, 18, 0, 19, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 0, 41, 0, 42, 0, 43, 0, 44 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 22, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 21, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 20, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, '\\', 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ '1', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 29, 0, 30, 0, 31, 0, 32, 0, 0, 0, 0, 20, 0, 21, 0, 22, 0, 23, 0, 0, 0, 0, '2' },
			}
		};

		std::array<Hand, PLAYER_COUNT> hands;

		int player_turn;
		int next_hand_size;

		BoardState board_state;
		Deck deck;

		DogGame() {
			reset();
		}

		void reset() {
			deck = Deck();
			board_state = BoardState();

			for (std::size_t i = 0; i != hands.size(); i++) {
				hands.at(i).clear();
			}

			player_turn = 0;
			next_hand_size = 6;

			hand_out_cards();
		}

		void hand_out_cards() {
			for (std::size_t i = 0; i != hands.size(); i++) {
				hands.at(i).draw_cards(deck, next_hand_size);
			}

			if (next_hand_size == 2) {
				next_hand_size = 6;
			} else {
				next_hand_size--;
			}
		}

		bool play_card(CardPlay& card_play, bool check_turn, bool check_hand) {
			if (!card_play.is_valid()) {
				return false;
			}

			if (check_turn) {
				if (card_play.player != player_turn) {
					// It needs to be the player's turn
					return false;
				}
			}

			if (check_hand) {
				bool player_has_card = hands.at(card_play.player).has_card(card_play.card) > 0;

				if (!player_has_card) {
					// Card being played must be in hand
					return false;
				}
			}

			CardPlay* play = card_play.get_play();

			bool legal;

			if (play->start_card) {
				legal = start(play->player);
			} else {
				if (play->card == Seven) {
					// TODO Check that the seven can only be split between *one* of the teammate's pieces and any number of the player's own pieces
					bool legal = move_multiple_pieces(play->player, play->target_positions, play->into_finish, play->counts, true);

					if (legal) {
						legal = move_multiple_pieces(play->player, play->target_positions, play->into_finish, play->counts, false);
						assert(legal);
					}
				} else if (play->card == Jack) {
					int idx_player = play->target_positions.at(0).idx;
					int idx_other = play->target_positions.at(1).idx;

					legal = swap_pieces(play->player, idx_player, idx_other, false);
				} else {
					int count = play->move_count();

					bool into_finish = play->into_finish.at(0);
					int idx = play->target_positions.at(0).idx;

					if (play->target_positions.at(0).area == Finish) {
						legal = move_piece_in_finish(play->player, idx, count, false);
					} else {
						legal = move_piece(play->player, idx, count, into_finish, false);
					}
				}
			}

			if (legal) {
				player_turn++;
				player_turn %= PLAYER_COUNT;

				// TODO Hand out new cards if all hands are empty
				// TODO Increment player_turn once again after handing out new cards
			}

			return legal;
		}

		bool start(int player) {
			PiecePtr& start = board_state.get_start(player);

			if (start != nullptr && start->blocking) {
				// Start must not be blocked by a blocking piece
				return false;
			}

			if (start != nullptr) {
				board_state.place_at_kennel(start);
			}

			board_state.start(player);

			return true;
		}

		int next_blockade(int from_path_idx, bool backwards) {
			int next_section;
			int result = -1;

			if (backwards) {
				next_section = from_path_idx / PATH_SECTION_LENGTH;

				for (int i = next_section; i > next_section - PLAYER_COUNT; i--) {
					int section_id = i % PLAYER_COUNT;
					int possible_blockade_idx = section_id * PATH_SECTION_LENGTH;

					PiecePtr& piece = board_state.path.at(possible_blockade_idx);

					if (piece != nullptr && piece->blocking) {
						result = possible_blockade_idx;
						break;
					}
				}
			} else {
				next_section = from_path_idx / PATH_SECTION_LENGTH + 1;

				for (int i = next_section; i < next_section + PLAYER_COUNT; i++) {
					int section_id = i % PLAYER_COUNT;
					int possible_blockade_idx = section_id * PATH_SECTION_LENGTH;

					PiecePtr& piece = board_state.path.at(possible_blockade_idx);

					if (piece != nullptr && piece->blocking) {
						result = possible_blockade_idx;
						break;
					}
				}
			}

			if (result == from_path_idx) {
				result = -1;
			}

			return result;
		}

		int positive_mod(int i, int n) {
			return ((i % n) + n) % n;
		}

		bool move_piece(int player, int path_idx, int count, bool into_finish, bool legal_check) {
			PiecePtr& piece = board_state.path.at(path_idx);

			if (piece == nullptr) {
				return false;
			}

			if (piece->player != player) {
				return false;
			}

			if (into_finish && player) {
				return false;
			}

			bool backwards = count < 0;
			int steps_on_path;

			int start_path_idx = player * PATH_SECTION_LENGTH;
			int steps_into_finish;
			std::array<PiecePtr, FINISH_LENGTH>& finish = board_state.finishes.at(player);

			if (into_finish && !backwards) {
				int steps_to_start;

				if (start_path_idx < path_idx) {
					steps_to_start = (start_path_idx + PATH_LENGTH) - path_idx;
				} else {
					steps_to_start = start_path_idx - path_idx;
				}

				if (piece->blocking) {
					steps_on_path = count;
				} else {
					steps_on_path = std::min(steps_to_start, count);
				}
			} else {
				steps_on_path = count;
			}

			steps_into_finish = count - steps_on_path;
			int new_path_idx_nofinish = positive_mod(path_idx + steps_on_path, PATH_LENGTH);

			if (steps_into_finish > 0) {
				assert(!backwards);

				if (steps_into_finish > FINISH_LENGTH) {
					// Piece cannot go further than length of finish
					return false;
				}

				if (steps_on_path == 0 && piece->blocking) {
					// Piece has to touch start twice
					return false;
				}

				for (int i = 0; i < steps_into_finish; i++) {
					if (finish.at(i) != nullptr) {
						// Piece cannot leapfrog another piece in finish
						return false;
					}
				}
			}

			int block_idx;
			if (backwards) {
				block_idx = next_blockade(path_idx, true);
			} else {
				block_idx = next_blockade(path_idx, false);
			}

			bool blocked;
			if (block_idx == -1) {
				blocked = false;
			} else {
				if (backwards) {
					if(new_path_idx_nofinish <= block_idx) {
						blocked = true;
					} else {
						blocked = (block_idx == 0);
					}
				} else {
					if(new_path_idx_nofinish >= block_idx) {
						blocked = true;
					} else {
						blocked = (block_idx == 0);
					}
				}
			}

			if (blocked) {
				// Piece cannot leapfrog another piece that is blocking
				return false;
			}

			if (!legal_check) {
				// Change board state
				if (into_finish && steps_into_finish > 0) {
					assert(!backwards);
					piece->area = Finish;
					piece->position = steps_into_finish;

					finish.at(steps_into_finish - 1) = std::move(piece);
				} else {
					piece->blocking = false;
					piece->position = new_path_idx_nofinish;

					board_state.path.at(new_path_idx_nofinish) = std::move(piece);
				}
			}

			return true;
		}

		bool move_piece_in_finish(int player, int finish_idx, int count, bool legal_check) {
			std::array<PiecePtr, FINISH_LENGTH>& finish = board_state.finishes.at(player);

			PiecePtr& piece = finish.at(finish_idx);

			if (piece == nullptr) {
				// Piece needs to be at given index
				return false;
			}

			if (count < 0) {
				// Piece cannot go backwards in finish
				return false;
			}

			int finish_idx_target = finish_idx + count;

			if (finish_idx_target >= FINISH_LENGTH) {
				// Piece cannot go further than length of finish
				return false;
			}

			for (int i = finish_idx + 1; i < finish_idx_target; i++) {
				if (finish.at(i) != nullptr) {
					// Piece cannot leapfrog another piece in finish
					return false;
				}
			}

			if (!legal_check) {
				// Change board state
				finish.at(finish_idx_target) = std::move(piece);
			}

			return true;
		}

		bool move_multiple_pieces(int player, std::vector<BoardPosition> target_positions, std::vector<bool> into_finishes, std::vector<int> counts, bool legal_check) {
			bool legal = true;

			for (std::size_t i = 0; i < target_positions.size(); i++) {
				bool into_finish = into_finishes.at(i);
				int idx = target_positions.at(i).idx;
				int count = counts.at(i);

				if (into_finish) {
					legal = move_piece(player, idx, count, into_finish, legal_check);
				} else {
					legal = move_piece_in_finish(player, idx, count, legal_check);
				}

				if (!legal) {
					break;
				}
			}

			return legal;
		}

		void send_to_kennel(int player, int path_idx_start, int path_idx_end) {
			if (path_idx_start > path_idx_end) {
				path_idx_end += PATH_LENGTH;
			}

			for (int i = path_idx_start; i < path_idx_end; i++) {
				int i_mod = i % PATH_LENGTH;

				PiecePtr& piece = board_state.path.at(i_mod);

				if (piece != nullptr && piece->player != player) {
					board_state.place_at_kennel(piece);
				}
			}
		}

		bool swap_pieces(int player, int path_idx_player, int path_idx_other, bool legal_check) {
			PiecePtr& piece_player = board_state.path.at(path_idx_player);
			PiecePtr& piece_other = board_state.path.at(path_idx_other);

			if (piece_player == nullptr || piece_other == nullptr) {
				return false;
			}

			if (piece_player->player != player) {
				return false;
			}

			if (path_idx_player == path_idx_other) {
				return false;
			}

			if (piece_player->blocking || piece_other->blocking) {
				return false;
			}

			if (!legal_check) {
				PiecePtr temp = std::move(piece_other);
				piece_other = std::move(piece_player);
				piece_player = std::move(temp);
			}

			return true;
		}

		friend std::ostream& operator<<(std::ostream& os, DogGame const& obj) {
			  return os << obj.to_str();
		}

		std::string to_str() const {
			std::stringstream ss;

			for (std::size_t row = 0; row != vis_map_spec.size(); row++) {
				for (std::size_t col = 0; col != vis_map_spec.at(0).size(); col++) {
					int spec = vis_map_spec.at(row).at(col);
					int val = vis_map_val.at(row).at(col);

					switch (spec) {
						case 1: {
							const PiecePtr& piece = board_state.path.at(val);

							if (piece == nullptr) {
								ss << 'o';
							} else {
								ss << piece->player;
							}

							break;
						}
						case 2: {
							int player = val / 10;
							int finish_idx = val % 10;

							bool occupied = (board_state.finishes.at(player).at(finish_idx) != nullptr);

							if (occupied) {
								ss << player;
							} else {
								ss << 'o';
							}

							break;
						}
						case 3: {
							int player = val / 10;
							int finish_idx = val % 10;

							bool occupied = (board_state.kennels.at(player).at(finish_idx) != nullptr);

							if (occupied) {
								ss << player;
							} else {
								ss << 'o';
							}

							break;
						}
						case 4: {
							ss << ((char) (val));
							break;
						}
						default: {
							ss << ' ';
							break;
						}
					}
				}
				ss << std::endl;
			}

			std::string hands_str = get_hands_str();
			ss << hands_str;
			return ss.str();
		}

		std::string get_hands_str() const {
			std::stringstream ss;

			for (std::size_t i = 0; i < hands.size(); i++) {
				ss << i << ": ";
				ss << hands.at(i);
				ss << std::endl;
			}

			return ss.str();
		}
};

#endif
