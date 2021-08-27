#pragma once

#include <iostream>
#include <sstream>
#include <array>
#include <memory>
#include <cassert>

#include "BoardState.hpp"
#include "CardsState.hpp"
#include "Piece.hpp"
#include "CardPlay.hpp"
#include "Debug.hpp"
#include "Util.hpp"


class DogGame {
	public:
		int player_turn;
		int next_hand_size;

		BoardState board_state;
		CardsState cards_state;

		DogGame() {
			reset();
		}

		void reset() {
			board_state = BoardState();
			cards_state = CardsState();

			player_turn = 0;
			next_hand_size = 6;

			cards_state.hand_out_cards(next_hand_size);
			next_hand_size = calc_next_hand_size(next_hand_size);
		}

		int calc_next_hand_size(int current_hand_size) {
			if (current_hand_size == 2) {
				return 6;
			} else {
				return next_hand_size - 1;
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
				bool player_has_card = cards_state.check_player_has_card(card_play.player, card_play.card);

				if (!player_has_card) {
					// Card being played must be in hand
					return false;
				}
			}

			CardPlay* play = card_play.get_play();

			bool legal;

			if (play->start_card) {
				legal = start_piece(play->player);
			} else {
				if (play->card == Seven) {
					// TODO Check that the seven can only be split between *one* of the teammate's pieces and any number of the player's own pieces
					bool legal = move_multiple_pieces(play->player, play->target_pieces, play->into_finish, play->counts, true);

					if (legal) {
						legal = move_multiple_pieces(play->player, play->target_pieces, play->into_finish, play->counts, false);
						assert(legal);
					}
				} else if (play->card == Jack) {
					int idx_player = board_state.ref_to_pos(play->target_pieces.at(0)).idx;
					int idx_other = board_state.ref_to_pos(play->target_pieces.at(1)).idx;

					legal = swap_pieces(play->player, idx_player, idx_other, false);
				} else {
					int count = play->move_count();

					bool into_finish = play->into_finish.at(0);
					BoardPosition position = board_state.ref_to_pos(play->target_pieces.at(0));

					legal = move_piece(play->player, position, count, into_finish, false, false);
				}
			}

			if (legal) {
				player_turn++;
				player_turn %= PLAYER_COUNT;

				cards_state.remove_card_from_hand(card_play.player, card_play.card);

				if (cards_state.hands_empty()) {
					// Round is done, new cards need to be handed out
					cards_state.hand_out_cards(next_hand_size);
					next_hand_size = calc_next_hand_size(next_hand_size);

					// Additional increment so that not the same player always starts in every round
					player_turn++;
				}
			}

			return legal;
		}

		bool start_piece(int player) {
			PiecePtr& start = board_state.get_start(player);

			if (start != nullptr && start->blocking) {
				// Start must not be blocked by a blocking piece
				return false;
			}

			if (start != nullptr) {
				board_state.place_at_kennel(start);
			}

			board_state.start_piece(player);

			return true;
		}

		bool move_piece(int player, BoardPosition position, int count, bool into_finish, bool legal_check, bool remove_all_on_way) {
			BoardPosition position_result;
			PiecePtr& piece = board_state.get_piece(position);

			if (piece == nullptr) {
				// Piece must exist at given position
				return false;
			}

			if (piece->player != player) {
				// Piece at given position must be of the player that wants to move the piece
				return false;
			}

			if (position.area == Kennel) {
				// Moving in kennel is not possible
				return false;
			}

			if (position.area == Path) {
				int steps_on_path = BoardState::calc_steps_on_path(player, position, piece->blocking, count, into_finish);
				int steps_into_finish = count - steps_on_path;

				if (steps_on_path != 0) {
					bool legal = check_move(position, steps_on_path, position_result);
					if (!legal) return false;
				}

				if (steps_into_finish > 0) {
					// Piece transitions from path area to finish area
					bool backwards = count < 0;
					assert(!backwards);

					if (steps_on_path == 0 && piece->blocking) {
						// Piece has to touch start twice
						return false;
					}

					// In this check position index "-1" in the finish represents the position right before the finish
					bool legal = check_move(BoardPosition(Finish, player, -1), steps_into_finish, position_result);
					if (!legal) return false;
				}
			} else {
				assert(position.area == Finish);

				bool legal = check_move(position, count, position_result);
				if (!legal) return false;
			}

			if (!legal_check) {
				// Change board state
				if (remove_all_on_way) {
					// TODO
					assert(false);
				} else {
					PiecePtr& piece_to_send_back = board_state.get_piece(position_result);
					if (piece_to_send_back != nullptr) {
						board_state.place_at_kennel(piece_to_send_back);
					}
				}

				board_state.move_piece(piece, position_result);
			}

			return true;
		}

		// This function assumes no area crossing (piece stays in the area that it was in)
		bool check_move(BoardPosition from_position, int count, BoardPosition& position_result) {
			bool backwards = count < 0;

			// TODO Split if into functions
			if (from_position.area == Finish) {
				if (backwards) {
					// Piece cannot go backwards in finish
					return false;
				}

				int steps_possible = board_state.possible_forward_steps_in_finish(from_position.player, from_position.idx);

				if (steps_possible < count) {
					// Piece cannot go further than length of finish
					// Piece cannot leapfrog another piece in finish
					return false;
				}

				int finish_idx_target = from_position.idx + count;

				position_result = BoardPosition(Finish, from_position.player, finish_idx_target);

				return true;
			} else if (from_position.area == Path) {
				int path_idx = from_position.idx;
				int target_path_idx = positive_mod(path_idx + count, PATH_LENGTH);

				int next_block_idx = board_state.next_blockade(path_idx, backwards);

				bool blocked;
				if (next_block_idx == -1) {
					blocked = false;
				} else {
					// TODO Can this be simplified by implementing a function to check if there is a blockade in a given range (similar to the send_to_kennel() function)?
					if (backwards) {
						if (next_block_idx <= target_path_idx) {
							blocked = target_path_idx <= next_block_idx;
						} else {
							blocked = (target_path_idx + PATH_LENGTH) <= next_block_idx;
						}
					} else {
						blocked = target_path_idx >= next_block_idx;

						if (next_block_idx >= target_path_idx) {
							blocked = target_path_idx >= next_block_idx;
						} else {
							blocked = (target_path_idx - PATH_LENGTH) <= next_block_idx;
						}
					}
				}

				if (blocked) {
					// Piece cannot leapfrog another piece that is blocking
					return false;
				}

				position_result = BoardPosition(target_path_idx);

				return true;
			} else if (from_position.area == Kennel) {
				// Cannot move in kennel
				return false;
			}

			assert(false);
		}

		bool move_multiple_pieces(int player, std::vector<PieceRef> target_pieces, std::vector<bool> into_finishes, std::vector<int> counts, bool legal_check) {
			bool legal = true;

			for (std::size_t i = 0; i < target_pieces.size(); i++) {
				bool into_finish = into_finishes.at(i);
				BoardPosition position = board_state.ref_to_pos(target_pieces.at(i));
				int count = counts.at(i);

				legal = move_piece(player, position, count, into_finish, legal_check, false);

				if (!legal) {
					break;
				}
			}

			return legal;
		}

		bool swap_pieces(int player, int path_idx_player, int path_idx_other, bool legal_check) {
			PiecePtr& piece_player = board_state.get_piece(BoardPosition(path_idx_player));
			PiecePtr& piece_other = board_state.get_piece(BoardPosition(path_idx_other));

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
				board_state.swap_pieces(piece_other, piece_player);
			}

			return true;
		}

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

			for (std::size_t i = 0; i < cards_state.hands.size(); i++) {
				ss << i << ": ";
				ss << cards_state.hands.at(i);
				ss << std::endl;
			}

			return ss.str();
		}
};
