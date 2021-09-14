#pragma once

#include <cassert>
#include <algorithm>

#include <libdog/Constants.hpp>


namespace libdog {

// TODO Move these implementations into a .cpp file
inline int get_start_path_idx(int player) {
	return player * PATH_SECTION_LENGTH;
}

inline int calc_steps_to_start(int player, int from_path_idx) {
	int start_path_idx = get_start_path_idx(player);

	int steps_to_start;

	if (start_path_idx < from_path_idx) {
		steps_to_start = (start_path_idx + PATH_LENGTH) - from_path_idx;
	} else {
		steps_to_start = start_path_idx - from_path_idx;
	}

	return steps_to_start;
}

inline int calc_steps_to_start(int player, int from_path_idx, bool blocking) {
	int steps_to_start = calc_steps_to_start(player, from_path_idx);

	// blocking ==> piece is on start
	assert(!blocking || from_path_idx == get_start_path_idx(player));

	if (blocking) {
		return PATH_LENGTH;
	}

	return steps_to_start;
}

inline int calc_steps_on_path(int player, int from_path_idx, bool piece_blocking, int count, bool into_finish) {
	bool backwards = count < 0;

	int steps_on_path;

	if (into_finish && !backwards) {
		int steps_to_start = calc_steps_to_start(player, from_path_idx);

		if (piece_blocking) {
			// Piece is blocked, which means it cannot enter finish directly -> forced to take all steps on path
			steps_on_path = count;
		} else {
			// Piece may enter finish -> only needs to take steps on path to go right before finish
			steps_on_path = std::min(steps_to_start, count);
		}
	} else {
		// Piece going backwards or does not want to enter finish -> take all steps on path
		steps_on_path = count;
	}

	return steps_on_path;
}

}
