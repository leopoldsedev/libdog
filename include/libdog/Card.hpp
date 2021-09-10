#pragma once

#include <array>
#include <vector>
#include <map>
#include <iostream>


namespace libdog {

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

const std::array<Card, 3> start_cards = { Ace, King, Joker };

bool is_start_card(Card card);

Card card_from_string(std::string card_str);

std::string card_to_string(Card card);

std::vector<Card> cards_from_str(std::string card_str);

}
