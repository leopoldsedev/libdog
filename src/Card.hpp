#pragma once

#include <vector>
#include <iostream>


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

// TODO Cannot move these function into Card.cpp for some reason (results in linker error when running tests)
inline Card card_from_string(std::string card_str) {
	if (card_str == "A") {
		return Ace;
	} else if (card_str == "2") {
		return Two;
	} else if (card_str == "3") {
		return Three;
	} else if (card_str == "4") {
		return Four;
	} else if (card_str == "5") {
		return Five;
	} else if (card_str == "6") {
		return Six;
	} else if (card_str == "7") {
		return Seven;
	} else if (card_str == "8") {
		return Eight;
	} else if (card_str == "9") {
		return Nine;
	} else if (card_str == "T") {
		return Ten;
	} else if (card_str == "J") {
		return Jack;
	} else if (card_str == "Q") {
		return Queen;
	} else if (card_str == "Q") {
		return Queen;
	} else if (card_str == "K") {
		return King;
	}

	return None;
}

inline bool is_start_card(Card card) {
	for (auto& start_card : start_cards) {
		if (start_card == card) {
			return true;
		}
	}
	return false;
}
