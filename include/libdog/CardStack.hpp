#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include <sstream>

#include <libdog/Card.hpp>


using namespace std;

namespace libdog {

// TODO Track suites as well to make library usable for full game interfaces
class CardStack {
	private:
		vector<Card> cards;
		default_random_engine rng;

		void remove(Card card);

	public:
		CardStack(vector<Card> cards);

		CardStack() : CardStack(vector<Card>()) {
		}

		size_t size() const;

		bool empty() const;

		bool contains(Card card) const;

		void move_to(CardStack& dest);

		void move_to(CardStack& dest, size_t count);

		void move_to(CardStack& dest, Card card);

		void shuffle();

		vector<Card> get_cards() const;

		string to_str() const;

		friend ostream& operator<<(ostream& os, CardStack const& obj) {
			  return os << obj.to_str();
		}
};

};
