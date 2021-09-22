#pragma once

#include <cassert>

#include <libdog/Area.hpp>
#include <libdog/BoardPosition.hpp>
#include <libdog/BoardUtil.hpp>


namespace libdog {

class Piece {
	public:
		const int player;
		const int idx;
		BoardPosition position;
		bool blocking;

		Piece(int player, int idx, BoardPosition position, bool blocking) : player(player), idx(idx), position(position), blocking(blocking) {
			// Throw exception instead of using assert. See https://stackoverflow.com/questions/4747706/standard-or-custom-exception-in-c
			assert(IS_VALID_PLAYER(player));
		}

		Piece(int player, int idx, BoardPosition position) : Piece(player, idx, position, false) {
		}

		explicit Piece(int player, int idx) : Piece(player, idx, BoardPosition(0)) {
		}

		Piece(const Piece& other) : Piece(other.player, other.idx, other.position, other.blocking) {
		}

		Piece& operator=(const Piece& other) {
			if (this != &other) {
				position = other.position;
				blocking = other.blocking;
			}

			return *this;
		}

		friend bool operator==(const Piece& a, const Piece& b) {
			return (a.player == b.player && a.position == b.position && a.blocking == b.blocking);
		}

		bool is_behind(const Piece& other) {
			// Can only compare rank of two pieces from the same player
			assert(player == other.player);
			// Can only compare different pieces
			assert(*this != other);

			Area area_a = this->position.area;
			Area area_b = other.position.area;
			int idx_a = this->position.idx;
			int idx_b = other.position.idx;
			int player = this->player;

			if (area_a != area_b) {
				return area_a < area_b;
			}

			assert(area_a == area_b);

			switch(area_a) {
				case Kennel:
					return idx_a > idx_b;
				case Path: {
					int steps_to_start_a = calc_steps_to_start(player, idx_a, this->blocking);
					int steps_to_start_b = calc_steps_to_start(player, idx_b, other.blocking);
					return steps_to_start_a > steps_to_start_b;
				}
				case Finish:
					return idx_a < idx_b;
				default:
					assert(false);
					break;
			}

			assert(false);
		}

		std::string to_str() const {
			std::stringstream ss;

			ss << "{ Player: " << player << ", Idx: " << idx << ", Position: " << position << ", Blocking: " << blocking << " }";

			return ss.str();
		}

		friend std::ostream& operator<<(std::ostream& os, Piece const& obj) {
			  return os << obj.to_str();
		}
};

using PiecePtr = Piece*;

};
