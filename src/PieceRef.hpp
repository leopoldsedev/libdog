#pragma once

class PieceRef {
	public:
		int player;
		int rank;

		explicit PieceRef(int player, int rank): player(player), rank(rank) {
		}

		explicit PieceRef(int player): player(player), rank(0) {
		}
};
