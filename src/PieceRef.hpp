#pragma once

#include "Piece.hpp"
#include "Constants.hpp"


class PieceRef {
	public:
		int player;
		int rank;

		explicit PieceRef(int player, int rank): player(player), rank(rank) {
		}

		explicit PieceRef(int player): player(player), rank(0) {
		}

		bool is_valid() const {
			if (rank < 0 || rank >= PIECE_COUNT) {
				return false;
			}

			if (player < 0 || player >= PLAYER_COUNT) {
				return false;
			}

			return true;
		}

		friend bool operator==(const PieceRef& a, const PieceRef& b) {
			return (a.player == b.player) && (a.rank == b.rank);
		}

		friend std::ostream& operator<<(std::ostream& os, PieceRef const& obj) {
			  return os << obj.to_str();
		}

		std::string to_str() const {
			std::stringstream ss;

			ss << "{ Player: " << player << ", Rank: " << rank << " }";

			return ss.str();
		}
};
