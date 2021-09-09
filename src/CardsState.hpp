#pragma once

#include <array>
#include <cassert>

#include "Util.hpp"
#include "CardStack.hpp"
#include "Constants.hpp"


using namespace std;

class CardsState {
	private:
		array<CardStack, PLAYER_COUNT> hands;
		array<CardStack, PLAYER_COUNT> give_buffer;

		CardStack deck;
		CardStack discarded;

		CardStack& get_hand(int player) {
			return hands.at(player);
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

	public:
		CardsState(vector<Card> cards) : deck(cards) {
		}

		CardsState() : CardsState(get_dog_card_set()) {
		}

		void hand_out_cards(int count) {
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

		bool check_player_has_card(int player, Card card) {
			CardStack& hand = get_hand(player);
			return hand.contains(card);
		}

		void discard(int player, Card card) {
			CardStack& hand = get_hand(player);

			if (hand.contains(card)) {
				hand.move_to(discarded, card);
			}
		}

		bool hands_empty() {
			for (int player = 0; player < PLAYER_COUNT; player++) {
				CardStack& hand = get_hand(player);

				if (hand.size() > 0) {
					return false;
				}
			}

			return true;
		}

		vector<Card> get_hand_cards(int player, bool deduplicate = false) {
			CardStack& hand = get_hand(player);

			vector<Card> result = hand.get_cards();

			if (deduplicate) {
				remove_duplicates(result);
			}

			return result;
		}

		void give_card(int player, Card card) {
			CardStack& hand = get_hand(player);
			CardStack& player_give_buffer = give_buffer.at(player);

			assert(hand.contains(card));

			hand.move_to(player_give_buffer, card);
		}

		bool give_buffer_full(int player) {
			CardStack& player_give_buffer = give_buffer.at(player);
			return !player_give_buffer.empty();
		}

		bool give_buffer_full() {
			for (int player = 0; player < PLAYER_COUNT; player++) {
				if (!give_buffer_full(player)) {
					return false;
				}
			}

			return true;
		}

		void execute_give() {
			assert(give_buffer_full());

			for (int player = 0; player < PLAYER_COUNT; player++) {
				int player_team = GET_TEAM_PLAYER_IDX(player);

				CardStack& player_give_buffer = give_buffer.at(player);
				CardStack& player_team_hand = get_hand(player_team);
				player_give_buffer.move_to(player_team_hand);
			}
		}

		friend ostream& operator<<(ostream& os, CardsState const& obj) {
			  return os << obj.to_str();
		}

		string to_str() const {
			stringstream ss;

			for (int player = 0; player < PLAYER_COUNT; player++) {
				const CardStack& hand = hands.at(player);
				ss << player << ": ";
				ss << hand;
				ss << std::endl;
			}

			return ss.str();
		}
};
