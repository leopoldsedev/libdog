#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

#include "Card.hpp"
#include "Debug.hpp"

#define DECK_COUNT (2)
#define JOKER_COUNT (3 * DECK_COUNT)


// TODO Currently the deck assumes that it is only reshuffled after all cards that were drawn have been played. If this assumption is violated it may happen that cards are given out again even though they have not been played, yet.
// TODO Track suites as well to make library usable for full game interfaces
class Deck {
	std::default_random_engine rng;

	std::vector<Card> cards = {};
	std::vector<Card> drawn = {};

	public:
		Deck() {
#ifndef NDEBUG
			rng = std::default_random_engine(0);
#else
			std::random_device r;
			rng = std::default_random_engine(r());
#endif

			add_cards();
			shuffle();
		}

		void set_cards(std::string card_str) {
			cards.clear();
			drawn.clear();

			for (char c : card_str) {
				Card card = card_from_string(std::string(1, c));
				cards.insert(cards.begin(), card);
			}
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
			while (drawn.size() > 0) {
				Card card = drawn.back();
				drawn.pop_back();
				cards.insert(cards.begin(), card);
			}
		}

		void shuffle() {
			std::shuffle(cards.begin(), cards.end(), rng);
		}

		void print_state() {
			std::cout << "deck (" << cards.size() << "): ";
			for (auto c = cards.rbegin(); c != cards.rend(); c++) {
				std::cout << card_to_string(*c);
			}
			std::cout << std::endl;

			std::cout << "drawn (" << drawn.size() << "): ";
			for (auto c = drawn.rbegin(); c != drawn.rend(); c++) {
				std::cout << card_to_string(*c);
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
