#include <libdog/BoardPosition.hpp>

#include <cassert>


namespace libdog {

BoardPosition::BoardPosition(Area area, int player, int idx) : area(area), player(player), idx(idx) {
	assert(is_valid());
}

bool BoardPosition::is_valid() const {
	if (!IS_VALID_PLAYER(player)) {
		// If piece is on path the player is set to a negative value (typically -1)
		if (!(area == Path && player < 0)) {
			return false;
		}
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

std::string BoardPosition::to_str() const {
	std::stringstream ss;
	std::string area_name = get_area_name(area);

	ss << "{ Area: " << area_name << ", Player: " << player << ", Idx: " << idx << " }";

	return ss.str();
}

}
