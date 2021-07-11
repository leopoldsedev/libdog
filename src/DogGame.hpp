#include <iostream>
#include <sstream>
#include <array>
#include <memory>

#include "Deck.hpp"
#include "Piece.hpp"

#define PLAYER_COUNT (4)
#define MAX_CARDS_HAND (6)

#define PATH_LENGTH (64)
#define PATH_SECTION_LENGTH (PATH_LENGTH / PLAYER_COUNT)
#define FINISH_LENGTH (4)
#define KENNEL_SIZE (4)


typedef std::unique_ptr<Piece> PiecePtr;


class DogGame {
	// 0 = nothing, 1 = path, 2 = finish, 3 = kennel, 4 = char
	std::array<std::array<int, 41>, 21> vis_map_spec = {
		{
			{ 4, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4 },
			{ 0, 0, 0, 0, 0, 4, 0, 4, 0, 4, 0, 4, 0, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 4, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
			{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1 },
			{ 1, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 1 },
			{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
			{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 4 },
		}
	};
	std::array<std::array<int, 41>, 21> vis_map_val = {
		{
			{ '0', 0, 0, 0, 0, 3, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 63, 0, 62, 0, 61, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '3' },
			{ 0, 0, 0, 0, 0, '3', 0, '2', 0, '1', 0, '0', 0, 0, 0, 0, 1, 0, '\\', 0, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, '0', 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1, 0, '1', 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 2, 0, '2', 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 3, 0, '3', 0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 12, 0, 11, 0, 10, 0, 9, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 51, 0, 50, 0, 49, 0, 48 },
			{ 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '/', 0, 47 },
			{ 14, 0, 0, 10, 0, 11, 0, 12, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33, 0, 32, 0, 31, 0, 30, 0, 0, 46 },
			{ 15, 0, '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45 },
			{ 16, 0, 17, 0, 18, 0, 19, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 0, 41, 0, 42, 0, 43, 0, 44 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 22, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 21, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 20, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, '\\', 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ '1', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 29, 0, 30, 0, 31, 0, 32, 0, 0, 0, 0, 20, 0, 21, 0, 22, 0, 23, 0, 0, 0, 0, '2' },
		}
	};

	std::array<PiecePtr, PATH_LENGTH> path;
	std::array<std::array<PiecePtr, FINISH_LENGTH>, PLAYER_COUNT> finishes;
	std::array<std::array<PiecePtr, KENNEL_SIZE>, PLAYER_COUNT> kennels;

	std::array<std::array<Card, MAX_CARDS_HAND>, PLAYER_COUNT> hands;

	int player_next_turn;
	int next_hand_size;

	Deck deck;

	public:
		DogGame() {
			reset();
			deck = Deck();
		}

		void reset() {
			for (std::size_t player = 0; player != kennels.size(); player++) {
				for (std::size_t j = 0; j != kennels.size(); j++) {
					kennels.at(player).at(j) = PiecePtr(new Piece(player));
				}
			}

			for (std::size_t i = 0; i != hands.size(); i++) {
				hands[i].fill(None);
			}

			player_next_turn = 0;
			next_hand_size = 6;
		}

//        bool play_card(int player, Card card) {
//            bool legal = true;
//
//            return legal;
//        }

		bool start(int player) {
			bool legal = false;

			auto& kennel = kennels[player];

			for (std::size_t i = 0; i < kennel.size(); i++) {
				PiecePtr& piece = kennel.at(i);

				if (piece != nullptr) {
					int start_path_idx = player * PATH_SECTION_LENGTH;
					PiecePtr& start = path.at(start_path_idx);

					if (start != nullptr && start->blocking) {
						legal = false;
					} else {
						legal = true;

						if (start != nullptr) {
							send_to_kennel(player, start_path_idx, start_path_idx);
						}

						piece->area = Path;
						piece->position = start_path_idx;
						start = std::move(piece);
					}

					break;
				}
			}

			return legal;
		}

		int next_blockade(int from_path_idx) {
			// TODO Implement remembering that a second touch of the start by one marble makes in not blocking
			int next_section = from_path_idx / PATH_SECTION_LENGTH + 1;
			for (int i = next_section; i < next_section + PLAYER_COUNT; i++) {
				int section_id = i % PLAYER_COUNT;
				int possible_blockade_idx = section_id * PATH_SECTION_LENGTH;

				PiecePtr& piece = path.at(possible_blockade_idx);

				if (piece != nullptr && piece->blocking) {
					return possible_blockade_idx;
				}
			}

			return -1;
		}

		bool move_piece(int player, int path_idx, int count, bool into_finish, bool legal_check) {
			PiecePtr& piece = path.at(path_idx);

			if (piece == nullptr) {
				return false;
			}

			if (piece->player != player) {
				return false;
			}

			int steps_on_path;

			int start_path_idx = player * PATH_SECTION_LENGTH;
			int steps_into_finish;
			std::array<PiecePtr, FINISH_LENGTH>& finish = finishes[player];

			if (into_finish) {
				int steps_to_start;
				// TODO Handle case where count < 0

				if (start_path_idx < path_idx) {
					steps_to_start = (start_path_idx + PATH_LENGTH) - path_idx;
				} else {
					steps_to_start = start_path_idx - path_idx;
				}

				steps_on_path = std::min(steps_to_start, count);
			} else {
				steps_on_path = count;
			}

			steps_into_finish = count - steps_on_path;
			int new_path_idx_nofinish = (path_idx + steps_on_path) % PATH_LENGTH;

			if (steps_into_finish > 0) {
				if (steps_into_finish > FINISH_LENGTH) {
					// Piece cannot go further than length of finish
					return false;
				}

				if (steps_on_path == 0 && piece->blocking) {
					// Piece has to touch start twice
					return false;
				}

				// TODO Check if pieces in finish are blocking the move
			}

			// TODO Handle case where count < 0
			int block_idx = next_blockade(path_idx);

			bool blocked = (block_idx != path_idx && new_path_idx_nofinish >= block_idx);
			if (new_path_idx_nofinish < path_idx) {
				blocked = (block_idx == 0);
			}

			if (block_idx != -1 && blocked) {
				// Piece cannot leapfrog another piece that is blocking
				return false;
			}

			if (!legal_check) {
				// Change board state
				if (into_finish && steps_into_finish > 0) {
					piece->area = Finish;
					piece->position = steps_into_finish;

					finish.at(steps_into_finish - 1) = std::move(piece);
				} else {
					piece->blocking = false;
					piece->position = new_path_idx_nofinish;

					path.at(new_path_idx_nofinish) = std::move(piece);
				}
			}

			return true;
		}

		void send_to_kennel(int player, int path_idx_start, int path_idx_end) {
			if (path_idx_start > path_idx_end) {
				path_idx_end += PATH_LENGTH;
			}

			for (int i = path_idx_start; i < path_idx_end; i++) {
				int i_mod = i % PATH_LENGTH;

				PiecePtr& piece = path.at(i_mod);

				if (piece != nullptr && piece->player != player) {
					place_at_kennel(piece);
				}
			}
		}

		void place_at_kennel(PiecePtr& piece) {
			auto& kennel = kennels.at(piece->player);

			for (std::size_t i = 0; i < kennel.size(); i++) {
				if (kennel.at(i) == nullptr) {
					kennel.at(i) = std::move(piece);

					piece->area = Kennel;
					piece->blocking = true;
					piece->position = i;

					return;
				}
			}
		}

		bool swap_pieces(int player, int path_idx_player, int path_idx_other) {
			PiecePtr& piece_player = path.at(path_idx_player);
			PiecePtr& piece_other = path.at(path_idx_other);

			if (piece_player == nullptr || piece_other == nullptr) {
				return false;
			}

			if (piece_player->player != player) {
				return false;
			}

			if (path_idx_player == path_idx_other) {
				return false;
			}

			if (piece_player->blocking || piece_other->blocking) {
				return false;
			}

			PiecePtr temp = std::move(piece_other);
			piece_other = std::move(piece_player);
			piece_player = std::move(temp);

			return true;
		}

		friend std::ostream& operator<<(std::ostream& os, DogGame const& obj) {
			  return os << obj.to_str();
		}

		std::string to_str() const {
			std::stringstream ss;

			for (std::size_t row = 0; row != vis_map_spec.size(); row++) {
				for (std::size_t col = 0; col != vis_map_spec[0].size(); col++) {
					int spec = vis_map_spec[row][col];
					int val = vis_map_val[row][col];

					switch (spec) {
						case 1: {
							const PiecePtr& piece = path.at(val);

							if (piece == nullptr) {
								ss << 'o';
							} else {
								ss << piece->player;
							}

							break;
						}
						case 2: {
							int player = val / 10;
							int finish_idx = val % 10;

							bool occupied = (finishes.at(player).at(finish_idx) != nullptr);

							if (occupied) {
								ss << player;
							} else {
								ss << 'o';
							}

							break;
						}
						case 3: {
							int player = val / 10;
							int finish_idx = val % 10;

							bool occupied = (kennels.at(player).at(finish_idx) != nullptr);

							if (occupied) {
								ss << player;
							} else {
								ss << 'o';
							}

							break;
						}
						case 4: {
							ss << ((char) (val));
							break;
						}
						default: {
							ss << ' ';
							break;
						}
					}
				}
				ss << std::endl;
			}

			std::string hands_str = get_hands_str();
			ss << hands_str;
			return ss.str();
		}

		std::string get_hands_str() const {
			std::stringstream ss;

			for (std::size_t i = 0; i < hands.size(); i++) {
				ss << i << ": ";
				for (auto& card : hands[i]) {
					ss << Deck::card_to_str(card);
				}
				ss << std::endl;
			}

			return ss.str();
		}
};
