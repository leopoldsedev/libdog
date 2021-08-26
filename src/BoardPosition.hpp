#ifndef BOARD_POSITION_HPP
#define BOARD_POSITION_HPP

#include "Area.hpp"


class BoardPosition {
	public:
		Area area;
		int player;
		int idx;

		BoardPosition(Area area, int player, int idx) : area(area), player(player), idx(idx) {
		}

		BoardPosition(int idx) : area(Path), player(-1), idx(idx) {
		}
};

#endif
