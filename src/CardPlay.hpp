#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "Card.hpp"
#include "Piece.hpp"
#include "PieceRef.hpp"
#include "BoardPosition.hpp"


class CardPlay {
	public:
		int player;
		Card card;

		std::vector<PieceRef> target_pieces;
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

		CardPlay() {
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

			if (target_pieces.size() != into_finish.size()) {
				return false;
			}

			if (card == None) {
				return false;
			}

			if (start_card) {
				if (target_pieces.size() != 0) {
					return false;
				}
			} else {
				if (card != Seven && card != Jack && target_pieces.size() > 1) {
					return false;
				}

				if (card == Jack && target_pieces.size() != 2) {
					return false;
				}

				if (card != Four && four_backwards) {
					return false;
				}

				if (card == Seven) {
					if (target_pieces.size() != counts.size()) {
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

		bool from_notation(int player, std::string notation_str) {
			// Remove all whitespace from string
			// Source: https://stackoverflow.com/a/83538/3118787
			notation_str.erase(remove_if(notation_str.begin(), notation_str.end(), ::isspace), notation_str.end());

			// TODO Reset all fields first

			std::string card_str = notation_str.substr(0, 1);
			notation_str.erase(0, 1);

			card = card_from_string(card_str);

			switch (card) {
				case None:
					break;
				case Ace:
					start_card = (notation_str.substr(0, 1) == "#");
					break;
				case Two:
					break;
				case Three:
					break;
				case Four:
					break;
				case Five:
					break;
				case Six:
					break;
				case Seven:
					break;
				case Eight:
					break;
				case Nine:
					break;
				case Ten:
					break;
				case Jack:
					break;
				case Queen:
					break;
				case King:
					start_card = (notation_str.substr(0, 1) == "#");
					break;
				case Joker:
					break;
				default:
					break;
			}

			return true;
		}
};
