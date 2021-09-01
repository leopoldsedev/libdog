#pragma once

#include <array>
#include <cassert>

#include "Deck.hpp"
#include "Hand.hpp"
#include "Constants.hpp"


class CardsState {
	public:
		std::array<Hand, PLAYER_COUNT> hands;
		Deck deck;

		CardsState() {
			reset();
		}

		void reset() {
			deck = Deck();
			for (std::size_t i = 0; i != hands.size(); i++) {
				hands.at(i).clear();
			}
		}

		Hand& get_hand(int player) {
			return hands.at(player);
		}

		void hand_out_cards(int count) {
			for (std::size_t i = 0; i != hands.size(); i++) {
				hands.at(i).draw_cards(deck, count);
			}
		}

		bool check_player_has_card(int player, Card card) {
			Hand& hand = get_hand(player);
			return hand.has_card(card);
		}

		void remove_card_from_hand(int player, Card card) {
			Hand& hand = get_hand(player);
			hand.play_card(card);
		}

		void add_card_to_hand(int player, Card card) {
			Hand& hand = get_hand(player);
			hand.add_card(card);
		}

		bool hands_empty() {
			for (int player = 0; player < PLAYER_COUNT; player++) {
				Hand& hand = get_hand(player);
				if (hand.size() > 0) {
					return false;
				}
			}

			return true;
		}

		void swap_cards(int player1, Card card1, int player2, Card card2) {
			Hand& hand1 = get_hand(player1);
			Hand& hand2 = get_hand(player2);

			if (hand1.has_card(card1) && hand1.has_card(card1)) {
				hand1.play_card(card1);
				hand2.play_card(card2);

				hand1.add_card(card2);
				hand2.add_card(card1);
			}
		}
};
