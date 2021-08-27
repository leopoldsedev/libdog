#pragma once

#include <array>
#include <memory>
#include <cassert>

#include "Piece.hpp"
#include "BoardPosition.hpp"

#define PLAYER_COUNT (4)
#define PATH_LENGTH (64)
#define PATH_SECTION_LENGTH (PATH_LENGTH / PLAYER_COUNT)
#define FINISH_LENGTH (4)
#define KENNEL_SIZE (4)


typedef std::unique_ptr<Piece> PiecePtr;

class BoardState {
	public:
		std::array<PiecePtr, PATH_LENGTH> path;
		std::array<std::array<PiecePtr, FINISH_LENGTH>, PLAYER_COUNT> finishes;
		std::array<std::array<PiecePtr, KENNEL_SIZE>, PLAYER_COUNT> kennels;

		BoardState() {
			reset();
		}

		void reset() {
			// TODO Reset of finish and path
			// TODO Maybe do not replace all references and instead reset those that already exist to not introduce inconsistencies from returned references to the old pointers
			for (std::size_t player = 0; player != kennels.size(); player++) {
				for (std::size_t j = 0; j != kennels.size(); j++) {
					// TODO Is a delete/release needed somewhere?
					kennels.at(player).at(j) = PiecePtr(new Piece(player));
				}
			}
		}

		void start_piece(int player) {
			auto& kennel = kennels.at(player);

			for (std::size_t i = 0; i < kennel.size(); i++) {
				PiecePtr& piece = kennel.at(i);

				if (piece != nullptr) {
					int start_path_idx = player * PATH_SECTION_LENGTH;
					PiecePtr& start = path.at(start_path_idx);

					piece->area = Path;
					piece->position = start_path_idx;
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
			int start_path_idx = player * PATH_SECTION_LENGTH;
			PiecePtr& piece = get_piece(BoardPosition(start_path_idx));
			return piece;
		}

		void place_at_kennel(PiecePtr& piece) {
			assert(piece != nullptr);

			auto& kennel = kennels.at(piece->player);

			for (std::size_t i = 0; i < kennel.size(); i++) {
				if (kennel.at(i) == nullptr) {
					kennel.at(i) = std::move(piece);

					piece->area = Kennel;
					piece->blocking = true;
					piece->position = i;

					return;
				}
			}
		}

		void move_piece(PiecePtr& piece, BoardPosition position) {
			assert(piece != nullptr);

			PiecePtr& target = get_piece(position);

			piece->area = position.area;
			piece->position = position.idx;
			piece->blocking = false;
			target = std::move(piece);
			// TODO Is here a memory leak if there was already a piece at the target position?
		}

		void swap_pieces(PiecePtr& piece1, PiecePtr& piece2) {
			assert(piece1 != nullptr);
			assert(piece2 != nullptr);

			PiecePtr temp = std::move(piece1);
			piece1 = std::move(piece2);
			piece2 = std::move(temp);
		}
};
