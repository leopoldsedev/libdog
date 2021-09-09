#include <vector>
#include <algorithm>
#include <random>
#include <sstream>
#include <cassert>

#include "Card.hpp"


using namespace std;

// TODO Track suites as well to make library usable for full game interfaces
class CardStack {
	private:
		vector<Card> cards;
		default_random_engine rng;

		void remove(Card card) {
			for (size_t i = 0; i < cards.size(); i++) {
				if (cards.at(i) == card) {
					cards.erase(cards.begin() + i);
					break;
				}
			}
		}

	public:
		CardStack(vector<Card> cards) : cards(cards) {
#ifndef NDEBUG
			rng = default_random_engine(0);
#else
			random_device r;
			rng = default_random_engine(r());
#endif
		}

		CardStack() : CardStack(vector<Card>()) {
		}

		size_t size() const {
			return cards.size();
		}

		bool empty() const {
			return cards.empty();
		}

		bool contains(Card card) const {
			for (Card c : cards) {
				if (c == card) {
					return true;
				}
			}

			return false;
		}

		void move_to(CardStack& dest) {
			move_to(dest, size());
		}

		void move_to(CardStack& dest, size_t count) {
			assert(cards.size() >= count);

			dest.cards.insert(dest.cards.end(), cards.begin(), cards.begin() + count);
			cards.erase(cards.begin(), cards.begin() + count);
		}

		void move_to(CardStack& dest, Card card) {
			assert(contains(card));

			vector<Card>::iterator it = find(cards.begin(),cards.end(), card);
			assert(it != cards.end());

			dest.cards.push_back(*it);
			cards.erase(it);
		}

		static CardStack from_str(string card_str) {
			vector<Card> cards = cards_from_str(card_str);
			return CardStack(cards);
		}

		void shuffle() {
			::shuffle(cards.begin(), cards.end(), rng);
		}

		vector<Card> get_cards() const {
			return cards;
		}

		friend ostream& operator<<(ostream& os, CardStack const& obj) {
			  return os << obj.to_str();
		}

		string to_str() const {
			stringstream ss;

			for (Card c : cards) {
				ss << card_to_string(c);
			}

			return ss.str();
		}
};
