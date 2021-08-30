#pragma once

#include "Area.hpp"
#include "BoardPosition.hpp"


class Piece {
	public:
		const int player;
		BoardPosition position;
		bool blocking;

		Piece(int player) : Piece(player, BoardPosition(0)) {
		}

		Piece(int player, BoardPosition position) : Piece(player, position, true) {
		}

		Piece(int player, BoardPosition position, bool blocking) : player(player), position(position), blocking(blocking) {
		}
};
