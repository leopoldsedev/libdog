#pragma once

#include <array>
#include <vector>
#include <memory>
#include <cassert>

#include "Piece.hpp"
#include "PieceRef.hpp"
#include "BoardPosition.hpp"
#include "Util.hpp"
#include "Debug.hpp"
#include "Constants.hpp"


typedef std::unique_ptr<Piece> PiecePtr;

class BoardState {
	public:
		// TODO Think about a way to properly abstract away the fact that after the last path index the first path index begins again
		std::array<PiecePtr, PATH_LENGTH> path;
		std::array<std::array<PiecePtr, FINISH_LENGTH>, PLAYER_COUNT> finishes;
		std::array<std::array<PiecePtr, KENNEL_SIZE>, PLAYER_COUNT> kennels;

		BoardState() {
			reset();
		}

		void reset() {
			// TODO Maybe do not replace all references and instead reset those that already exist to not introduce inconsistencies from returned references to the old pointers
			for (std::size_t player = 0; player != kennels.size(); player++) {
				for (std::size_t j = 0; j != kennels.size(); j++) {
					kennels.at(player).at(j) = std::make_unique<Piece>(player, BoardPosition(Kennel, player, j), true);
				}
			}

			for (std::size_t player = 0; player != finishes.size(); player++) {
				for (std::size_t j = 0; j != finishes.size(); j++) {
					finishes.at(player).at(j) = nullptr;
				}
			}

			for (std::size_t i = 0; i != path.size(); i++) {
				path.at(i) = nullptr;
			}
		}

		PiecePtr& ref_to_piece(PieceRef piece_ref) {
			BoardPosition position = ref_to_pos(piece_ref);
			return get_piece(position);
		}

		BoardPosition ref_to_pos(PieceRef piece_ref) {
			int player = piece_ref.player;
			int rank = piece_ref.rank;

			int piece_idx = 0;

			// Check pieces in finish
			auto& finish = finishes.at(player);

			for (int i = finish.size() - 1; i >= 0; i--) {
				PiecePtr& piece = finish.at(i);

				if (piece != nullptr) {
					if (piece_idx == rank) {
						return BoardPosition(Finish, player, i);
					}
					piece_idx++;
				}
			}

			int start_path_idx = get_start_path_idx(player);

			// Check non-blocking pieces on path
			for (int i = start_path_idx; i >= (start_path_idx - (int) path.size()); i--) {
				int i_mod = positive_mod(i, path.size());

				PiecePtr& piece = path.at(i_mod);

				if (piece != nullptr && piece->player == player && !piece->blocking) {
					if (piece_idx == rank) {
						return BoardPosition(Path, player, i_mod);
					}
					piece_idx++;
				}
			}

			// Check potential blocking piece at start
			PiecePtr& piece = path.at(start_path_idx);

			if (piece != nullptr && piece->player == player && piece->blocking) {
				if (piece_idx == rank) {
					return BoardPosition(Path, player, start_path_idx);
				}
				piece_idx++;
			}

			// Check pieces in kennel
			auto& kennel = kennels.at(player);

			for (std::size_t i = 0; i < kennel.size(); i++) {
				PiecePtr& piece = kennel.at(i);

				if (piece != nullptr) {
					if (piece_idx == rank) {
						return BoardPosition(Kennel, player, i);
					}
					piece_idx++;
				}
			}

			return BoardPosition();
		}

		void start_piece(int player) {
			auto& kennel = kennels.at(player);

			for (std::size_t i = 0; i < kennel.size(); i++) {
				PiecePtr& piece = kennel.at(i);

				if (piece != nullptr) {
					int start_path_idx = get_start_path_idx(player);
					PiecePtr& start = path.at(start_path_idx);

					piece->position = BoardPosition(start_path_idx);
					start = std::move(piece);

					break;
				}
			}
		}

		PiecePtr& get_piece(BoardPosition position) {
			if (position.area == Path) {
				return path.at(position.idx);
			} else if (position.area == Kennel) {
				return kennels.at(position.player).at(position.idx);
			} else if (position.area == Finish) {
				return finishes.at(position.player).at(position.idx);
			}

			assert(false);
		}

		PiecePtr& get_start(int player) {
			int start_path_idx = get_start_path_idx(player);
			PiecePtr& piece = get_piece(BoardPosition(start_path_idx));
			return piece;
		}

		void send_to_kennel(int from_path_idx, int count) {
			bool backwards = (count < 0);
			int step = backwards ? -1 : 1;

			for (int i = 0; i != count; i += step) {
				int path_idx = positive_mod(from_path_idx + step + i, PATH_LENGTH);

				if (path_idx == from_path_idx) {
					continue;
				}

				PiecePtr& piece = get_piece(BoardPosition(path_idx));

				if (piece != nullptr) {
					assert(!piece->blocking);
					place_at_kennel(piece);
				}
			}
		}

		void place_at_kennel(PiecePtr& piece) {
			assert(piece != nullptr);
			assert(!piece->blocking);

			auto& kennel = kennels.at(piece->player);

			for (int i = kennel.size() - 1; i >= 0; i--) {
				if (kennel.at(i) == nullptr) {
					piece->blocking = true;
					piece->position = BoardPosition(Kennel, piece->player, i);

					kennel.at(i) = std::move(piece);

					return;
				}
			}
		}

		void move_piece(PiecePtr& piece, BoardPosition position) {
			assert(piece != nullptr);

			PiecePtr& target = get_piece(position);

			piece->position = position;
			piece->blocking = false;
			target = std::move(piece);
		}

		void swap_pieces(PiecePtr& piece1, PiecePtr& piece2) {
			assert(piece1 != nullptr);
			assert(piece2 != nullptr);

			PiecePtr temp = std::move(piece1);
			piece1 = std::move(piece2);
			piece2 = std::move(temp);
		}

		int possible_forward_steps_in_finish(int player, int from_finish_idx) {
			std::array<PiecePtr, FINISH_LENGTH>& finish = finishes.at(player);

			int result = 0;

			if (from_finish_idx < -1) {
				result = -from_finish_idx - 1;
				from_finish_idx = -1;
			}

			assert(from_finish_idx >= -1);

			for (int i = from_finish_idx + 1; i < FINISH_LENGTH; i++) {
				if (finish.at(i) == nullptr) {
					result++;
				}
			}

			return result;
		}

		bool check_block(int from_path_idx, int count) {
			bool backwards = (count < 0);
			int step = backwards ? -1 : 1;

			// TODO Loop can be stopped early if i < or > +-PATH_LENGTH, it could also be made faster by going in increments of PATH_SECTION_LENGTH
			for (int i = 0; i != count; i += step) {
				int path_idx = positive_mod(from_path_idx + step + i, PATH_LENGTH);

				if (path_idx == from_path_idx) {
					continue;
				}

				PiecePtr& piece = get_piece(BoardPosition(path_idx));

				if (piece != nullptr && piece->blocking) {
					return true;
				}
			}

			return false;
		}

		static int get_start_path_idx(int player) {
			return player * PATH_SECTION_LENGTH;
		}

		static int calc_steps_to_start(int player, int from_path_idx) {
			int start_path_idx = get_start_path_idx(player);

			int steps_to_start;

			if (start_path_idx < from_path_idx) {
				steps_to_start = (start_path_idx + PATH_LENGTH) - from_path_idx;
			} else {
				steps_to_start = start_path_idx - from_path_idx;
			}

			return steps_to_start;
		}

		static int calc_steps_on_path(int player, int from_path_idx, bool piece_blocking, int count, bool into_finish) {
			bool backwards = count < 0;

			int steps_on_path;

			// TODO Checking backwards here enforces a game rule and thus does not belong into this class
			if (into_finish && !backwards) {
				int steps_to_start = calc_steps_to_start(player, from_path_idx);

				if (piece_blocking) {
					// Piece is blocked, which means it cannot enter finish directly -> forced to take all steps on path
					steps_on_path = count;
				} else {
					// Piece may enter finish -> only needs to take steps on path to go right before finish
					steps_on_path = std::min(steps_to_start, count);
				}
			} else {
				// Piece going backwards or does not want to enter finish -> take all steps on path
				steps_on_path = count;
			}

			return steps_on_path;
		}
};
