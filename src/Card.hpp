#ifndef CARD_HPP
#define CARD_HPP

#include <vector>


enum Card {
	None = 0,
	Ace = 1,
	Two = 2,
	Three = 3,
	Four = 4,
	Five = 5,
	Six = 6,
	Seven = 7,
	Eight = 8,
	Nine = 9,
	Ten = 10,
	Jack = 11,
	Queen = 12,
	King = 13,
	Joker = 14,
};

const std::vector<Card> start_cards = { Ace, King };

bool is_start_card(Card card) {
	for (auto& start_card : start_cards) {
		if (start_card == card) {
			return true;
		}
	}
	return false;
}

#endif
