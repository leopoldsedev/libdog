#ifndef PIECE_HPP
#define PIECE_HPP

#include "Area.hpp"


class Piece {
	public:
		const int player;
		int position;
		Area area;
		bool blocking;

		Piece(int player) : Piece(player, 0) {
		}

		Piece(int player, int position) : Piece(player, position, Kennel, true) {
		}

		Piece(int player, int position, Area area, bool blocking) : player(player), position(position), area(area), blocking(blocking) {
		}
};

#endif
