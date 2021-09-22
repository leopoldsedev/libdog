#pragma once

#include <array>
#include <cassert>

#include <libdog/Card.hpp>
#include <libdog/CardStack.hpp>
#include <libdog/Constants.hpp>


namespace libdog {

using std::vector;

static void add_suite(vector<Card>& cards) {
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

static vector<Card> get_dog_card_set() {
	vector<Card> result;

	for (int i = 0; i < 4 * DECK_COUNT; i++) {
		add_suite(result);
	}

	for (int i = 0; i < JOKER_COUNT; i++) {
		result.push_back(Joker);
	}

	return result;
}

class CardsState {
	public:
		explicit CardsState(vector<Card> cards);

		CardsState() : CardsState(get_dog_card_set()) {
		}

		void hand_out_cards(int count);

		[[nodiscard]]
		bool check_player_has_card(int player, Card card) const;

		void discard(int player, Card card);

		[[nodiscard]]
		bool hands_empty() const;

		[[nodiscard]]
		vector<Card> get_hand_cards(int player, bool deduplicate = false) const;

		[[nodiscard]]
		const CardStack& get_deck() const;

		void give_card(int player, Card card);

		bool give_buffer_full(int player);

		bool give_buffer_full();

		void execute_give();

		[[nodiscard]]
		string to_str() const;

		friend ostream& operator<<(ostream& os, CardsState const& obj) {
			  return os << obj.to_str();
		}

	private:
		array<CardStack, PLAYER_COUNT> hands;
		array<CardStack, PLAYER_COUNT> give_buffer;

		CardStack deck;
		CardStack discarded;

		CardStack& get_hand(int player);

		[[nodiscard]]
		const CardStack& get_hand(int player) const;
};

}
