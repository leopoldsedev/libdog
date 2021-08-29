#include "Card.hpp"


bool is_start_card(Card card) {
	for (auto& start_card : start_cards) {
		if (start_card == card) {
			return true;
		}
	}
	return false;
}

Card card_from_string(std::string card_str) {
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
	} else if (card_str == "K") {
		return King;
	} else if (card_str == "X") {
		return Joker;
	}

	return None;
}
