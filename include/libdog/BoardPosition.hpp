#pragma once

#include <sstream>

#include <libdog/Area.hpp>
#include "Constants.hpp"


namespace libdog {

class BoardPosition {
	public:
		Area area;
		int player;
		int idx;

		explicit BoardPosition(Area area, int player, int idx);

		explicit BoardPosition(int idx) : BoardPosition(Path, -1, idx) {
		}

		bool is_valid() const;

		std::string to_str() const;

		friend bool operator==(const BoardPosition& a, const BoardPosition& b)
		{
			if (a.area != b.area) {
				return false;
			}

			if (a.area == Path) {
				return a.idx == b.idx;
			} else {
				return a.player == b.player && a.idx == b.idx;
			}
		}

		friend std::ostream& operator<<(std::ostream& os, BoardPosition const& obj) {
			  return os << obj.to_str();
		}
};

}
