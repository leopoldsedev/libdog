#pragma once

#include <sstream>

#include "Area.hpp"
#include "Constants.hpp"


class BoardPosition {
	public:
		Area area;
		int player;
		int idx;

		explicit BoardPosition(Area area, int player, int idx) : area(area), player(player), idx(idx) {
			assert(is_valid());
		}

		explicit BoardPosition(int idx) : area(Path), player(-1), idx(idx) {
		}

		bool is_valid() const {
			if (player >= PLAYER_COUNT) {
				return false;
			}

			if (area != Path && player < 0) {
				return false;
			}

			if (idx < 0) {
				return false;
			}

			if (area == Path) {
				if (idx >= PATH_LENGTH) {
					return false;
				}
			} else if (area == Kennel) {
				if (idx >= KENNEL_SIZE) {
					return false;
				}
			} else if (area == Finish) {
				if (idx >= FINISH_LENGTH) {
					return false;
				}
			} else {
				return false;
			}

			return true;
		}

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

		std::string to_str() const {
			std::stringstream ss;
			std::string area_name = get_area_name(area);

			ss << "{ Area: " << area_name << ", Player: " << player << ", Idx: " << idx << " }";

			return ss.str();
		}
};
