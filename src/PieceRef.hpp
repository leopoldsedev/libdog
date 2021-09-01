#pragma once

class PieceRef {
	public:
		int player;
		int rank;

		explicit PieceRef(int player, int rank): player(player), rank(rank) {
		}

		explicit PieceRef(int player): player(player), rank(0) {
		}

		friend bool operator==(const PieceRef& a, const PieceRef& b)
		{
			return a.player == b.player && a.rank == b.rank;
		}

		friend bool operator==(const PieceRef& a, const PieceRef& b) {
			return (a.player == b.player) && (a.rank == b.rank);
		}
};
