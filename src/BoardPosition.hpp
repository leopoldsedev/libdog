#pragma once

#include <sstream>

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

		friend std::ostream& operator<<(std::ostream& os, BoardPosition const& obj) {
			  return os << obj.to_str();
		}

		std::string to_str() const {
			std::stringstream ss;
			std::string area_name = get_area_name(area);

			ss << "{ Area: " << area_name << ", Player: " << player << ", Idx: " << idx << " }";

			return ss.str();
		}
};
