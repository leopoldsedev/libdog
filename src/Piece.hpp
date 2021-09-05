#pragma once

#include "Area.hpp"
#include "BoardPosition.hpp"


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

		Piece(int player) : Piece(player, BoardPosition(0)) {
		}

		friend bool operator==(const Piece& a, const Piece& b) = default;
};
