#include <libdog/CardsState.hpp>

#include "Util.hpp"


namespace libdog {

CardsState::CardsState(vector<Card> cards) : deck(cards) {
}

CardStack& CardsState::get_hand(int player) {
	return hands.at(player);
}

const CardStack& CardsState::get_hand(int player) const {
	return hands.at(player);
}

void CardsState::hand_out_cards(int count) {
	for (int player = 0; player < PLAYER_COUNT; player++) {
		CardStack& hand = get_hand(player);

		for (int i = 0; i < count; i++) {
			deck.move_to(hand, 1);

			if (deck.empty()) {
				discarded.move_to(deck);
				deck.shuffle();
			}
		}
	}
}

bool CardsState::check_player_has_card(int player, Card card) const {
	const CardStack& hand = get_hand(player);
	return hand.contains(card);
}

void CardsState::discard(int player, Card card) {
	CardStack& hand = get_hand(player);

	if (hand.contains(card)) {
		hand.move_to(discarded, card);
	}
}

bool CardsState::hands_empty() const {
	for (int player = 0; player < PLAYER_COUNT; player++) {
		const CardStack& hand = get_hand(player);

		if (!hand.empty()) {
			return false;
		}
	}

	return true;
}

vector<Card> CardsState::get_hand_cards(int player, bool deduplicate) const {
	const CardStack& hand = get_hand(player);

	vector<Card> result = hand.get_cards();

	if (deduplicate) {
		remove_duplicates(result);
	}

	return result;
}

void CardsState::give_card(int player, Card card) {
	CardStack& hand = get_hand(player);
	CardStack& player_give_buffer = give_buffer.at(player);

	assert(hand.contains(card));

	hand.move_to(player_give_buffer, card);
}

bool CardsState::give_buffer_full(int player) {
	CardStack& player_give_buffer = give_buffer.at(player);
	return !player_give_buffer.empty();
}

bool CardsState::give_buffer_full() {
	for (int player = 0; player < PLAYER_COUNT; player++) {
		if (!give_buffer_full(player)) {
			return false;
		}
	}

	return true;
}

void CardsState::execute_give() {
	assert(give_buffer_full());

	for (int player = 0; player < PLAYER_COUNT; player++) {
		int player_team = GET_TEAM_PLAYER_IDX(player);

		CardStack& player_give_buffer = give_buffer.at(player);
		CardStack& player_team_hand = get_hand(player_team);
		player_give_buffer.move_to(player_team_hand);
	}
}

string CardsState::to_str() const {
	stringstream ss;

	for (int player = 0; player < PLAYER_COUNT; player++) {
		const CardStack& hand = hands.at(player);
		ss << player << ": ";
		ss << hand;
		ss << std::endl;
	}

	return ss.str();
}

}
