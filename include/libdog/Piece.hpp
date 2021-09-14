#pragma once

#include <cassert>

#include <libdog/Area.hpp>
#include <libdog/BoardPosition.hpp>


namespace libdog {

class Piece {
	public:
		const int player;
		BoardPosition position;
		bool blocking;

		Piece(int player, BoardPosition position, bool blocking) : player(player), position(position), blocking(blocking) {
			assert(IS_VALID_PLAYER(player));
		}

		Piece(int player, BoardPosition position) : Piece(player, position, false) {
		}

		explicit Piece(int player) : Piece(player, BoardPosition(0)) {
		}

		friend bool operator==(const Piece& a, const Piece& b) = default;
};

};
