#include <libdog/Card.hpp>


namespace libdog {

std::array<std::string, 15> card_to_str = { "-", "A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K", "X" };

std::map <std::string, Card> str_to_card {
	{ "-", None },
	{ "A", Ace },
	{ "2", Two },
	{ "3", Three },
	{ "4", Four },
	{ "5", Five },
	{ "6", Six },
	{ "7", Seven },
	{ "8", Eight },
	{ "9", Nine },
	{ "T", Ten },
	{ "J", Jack },
	{ "Q", Queen },
	{ "K", King },
	{ "X", Joker },
};

bool is_start_card(Card card) {
	for (auto& start_card : start_cards) {
		if (start_card == card) {
			return true;
		}
	}
	return false;
}

Card card_from_string(std::string card_str) {
	if (str_to_card.find(card_str) != str_to_card.end()) {
		return str_to_card.at(card_str);
	}

	return None;
}

std::string card_to_string(Card card) {
	return card_to_str.at(card);
}

std::vector<Card> cards_from_str(const std::string& card_str) {
	std::vector<Card> cards;

	for (char c : card_str) {
		Card card = card_from_string(std::string(1, c));
		cards.push_back(card);
	}

	return cards;
}

}
