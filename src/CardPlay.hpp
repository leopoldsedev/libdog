#pragma once

#include <vector>
#include <memory>

#include "Piece.hpp"
#include "Card.hpp"
#include "BoardPosition.hpp"


class CardPlay {
	public:
		int player;
		Card card;

		std::vector<BoardPosition> target_positions;
		std::vector<bool> into_finish;
		std::vector<int> counts;

		bool start_card;
		bool ace_one;
		bool four_backwards;

		std::unique_ptr<CardPlay> joker_card;

		CardPlay(int player, Card card) : CardPlay(player, card, false, false, false) {
		}

		CardPlay(int player, Card card, bool start_card) : CardPlay(player, card, start_card, false, false) {
		}

		CardPlay(int player, Card card, bool start_card, bool ace_one, bool four_backwards) : player(player), card(card), start_card(start_card), ace_one(ace_one), four_backwards(four_backwards) {
		}

		CardPlay* get_play() {
			CardPlay* play;

			if (card == Joker) {
				play = joker_card.get();
			} else {
				play = this;
			}

			return play;
		}

		int move_count() {
			int count = 0;

			switch (card) {
				case Ace: {
					count = ace_one ? 1 : 11;
					break;
				}
				case Two: {
					count = 2;
					break;
				}
				case Three: {
					count = 3;
					break;
				}
				case Four: {
					count = 4;
					break;
				}
				case Five: {
					count = 5;
					break;
				}
				case Six: {
					count = 6;
					break;
				}
				case Eight: {
					count = 8;
					break;
				}
				case Nine: {
					count = 9;
					break;
				}
				case Ten: {
					count = 10;
					break;
				}
				case Queen: {
					count = 12;
					break;
				}
				case King: {
					count = 13;
					break;
				}
				default: {
				}
			}

			if (four_backwards) {
				count *= -1;
			}

			return count;
		}

		bool is_valid() {
			if (start_card && !is_start_card(card)) {
				return false;
			}

			if (target_positions.size() != into_finish.size()) {
				return false;
			}

			if (card == None) {
				return false;
			}

			if (start_card) {
				if (target_positions.size() != 0) {
					return false;
				}
			} else {
				if (card != Seven && card != Jack && target_positions.size() > 1) {
					return false;
				}

				if (card == Jack && target_positions.size() != 2) {
					return false;
				}

				if (card != Four && four_backwards) {
					return false;
				}

				if (card == Seven) {
					if (target_positions.size() != counts.size()) {
						return false;
					}

					int sum_count = 0;
					for (std::size_t i = 0; i < counts.size(); i++) {
						sum_count += counts.at(i);
					}
					if (sum_count != 7) {
						return false;
					}
				} else {
					if (counts.size() > 0) {
						return false;
					}
				}
			}

			if (card == Joker && joker_card == nullptr) {
				return false;
			}

			return true;
		}
};
