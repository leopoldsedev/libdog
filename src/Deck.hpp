#include <iostream>
#include <vector>
#include <algorithm>

#include "Card.hpp"

#define DECK_COUNT (2)
#define JOKER_COUNT (3 * DECK_COUNT)


class Deck {
	std::vector<Card> cards = {};
	std::vector<Card> drawn = {};

	public:
		Deck() {
			add_cards();
			shuffle();
		}

		Card draw() {
			if (cards.size() == 0) {
				reset_and_shuffle();
			}

			Card card = cards.back();
			cards.pop_back();
			drawn.push_back(card);
			return card;
		}

		void reset_and_shuffle() {
			reset();
			shuffle();
		}

		void reset() {
			cards = drawn;
			drawn = {};
		}

		void shuffle() {
			std::random_shuffle(cards.begin(), cards.end());
		}

		std::string card_to_str(Card card) {
			std::string map[] = { "A", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "J" };
			return map[card];
		}

		void print_state() {
			std::cout << "deck (" << cards.size() << "): ";
			for (auto c = cards.rbegin(); c != cards.rend(); c++) {
				std::cout << card_to_str(*c);
			}
			std::cout << std::endl;

			std::cout << "drawn (" << drawn.size() << "): ";
			for (auto c = drawn.rbegin(); c != drawn.rend(); c++) {
				std::cout << card_to_str(*c);
			}
			std::cout << std::endl;
		}

	private:
		void add_suite() {
			cards.push_back(Ace);
			cards.push_back(Two);
			cards.push_back(Three);
			cards.push_back(Four);
			cards.push_back(Five);
			cards.push_back(Six);
			cards.push_back(Seven);
			cards.push_back(Eight);
			cards.push_back(Nine);
			cards.push_back(Ten);
			cards.push_back(Jack);
			cards.push_back(Queen);
			cards.push_back(King);
		}

		void add_cards() {
			for (int i = 0; i < 4 * DECK_COUNT; i++) {
				add_suite();
			}

			for (int i = 0; i < JOKER_COUNT; i++) {
				cards.push_back(Joker);
			}
		}
};
