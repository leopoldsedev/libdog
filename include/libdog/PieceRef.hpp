#pragma once

#include <libdog/Piece.hpp>
#include <libdog/Constants.hpp>


namespace libdog {

class PieceRef {
	public:
		int player;
		int rank;

		explicit PieceRef(int player, int rank): player(player), rank(rank) {
			assert(is_valid());
		}

		bool is_valid() const {
			if (!IS_VALID_PIECE_RANK(rank)) {
				return false;
			}

			if (!IS_VALID_PLAYER(player)) {
				return false;
			}

			return true;
		}

		friend bool operator==(const PieceRef& a, const PieceRef& b) = default;

		friend std::ostream& operator<<(std::ostream& os, PieceRef const& obj) {
			  return os << obj.to_str();
		}

		std::string to_str() const {
			std::stringstream ss;

			ss << "{ Player: " << player << ", Rank: " << rank << " }";

			return ss.str();
		}
};

}
