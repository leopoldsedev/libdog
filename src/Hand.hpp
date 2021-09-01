#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "Card.hpp"
#include "Deck.hpp"


// TODO Deck and Hand could be combined into a CardStack class. See https://codereview.stackexchange.com/a/20265

class Hand {
	public:
		std::vector<Card> cards;
		Hand() {
		}

		void clear() {
			cards.clear();
		}

		void add_card(Card card) {
			cards.push_back(card);
		}

		void draw_cards(Deck& deck, int count) {
			for (int i = 0; i < count; i++) {
				Card new_card = deck.draw();
				cards.push_back(new_card);
			}
		}

		int size() {
			return cards.size();
		}

		bool has_card(Card card) {
			return std::any_of(
				cards.begin(),
				cards.end(),
				[card](Card c) {
					return c == card;
				}
			);
		}

		bool play_card(Card card) {
			for (std::size_t i = 0; i < cards.size(); i++) {
				if (cards.at(i) == card) {
					cards.erase(cards.begin() + i);
					return true;
				}
			}

			return false;
		}

		friend std::ostream& operator<<(std::ostream& os, Hand const& obj) {
			  return os << obj.to_str();
		}

		std::string to_str() const {
			std::stringstream ss;

			for (auto& card : cards) {
				ss << card_to_string(card);
			}

			return ss.str();
		}
};
