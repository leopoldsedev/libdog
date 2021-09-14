#pragma once

#include <cassert>

#include <libdog/Area.hpp>
#include <libdog/BoardPosition.hpp>


namespace libdog {

class Piece {
	public:
		const int player;
		const int idx;
		BoardPosition position;
		bool blocking;

		Piece(int player, int idx, BoardPosition position, bool blocking) : player(player), idx(idx), position(position), blocking(blocking) {
			assert(IS_VALID_PLAYER(player));
		}

		Piece(int player, int idx, BoardPosition position) : Piece(player, idx, position, false) {
		}

		explicit Piece(int player, int idx) : Piece(player, idx, BoardPosition(0)) {
		}

		Piece& operator=(const Piece& other) {
			if (this != &other) {
				position = other.position;
				blocking = other.blocking;
			}

			return *this;
		}

		friend bool operator==(const Piece& a, const Piece& b) {
			return (a.player == b.player && a.position == b.position && a.blocking == b.blocking);
		}

		std::string to_str() const {
			std::stringstream ss;

			ss << "{ Player: " << player << ", Idx: " << idx << ", Position: " << position << ", Blocking: " << blocking << " }";

			return ss.str();
		}

		friend std::ostream& operator<<(std::ostream& os, Piece const& obj) {
			  return os << obj.to_str();
		}
};

};
