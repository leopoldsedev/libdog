#pragma once

#include "Area.hpp"


class BoardPosition {
	public:
		Area area;
		int player;
		int idx;

		explicit BoardPosition(Area area, int player, int idx) : area(area), player(player), idx(idx) {
		}

		explicit BoardPosition(int idx) : area(Path), player(-1), idx(idx) {
		}

		explicit BoardPosition() : area(Path), player(-1), idx(-1) {
		}
};
