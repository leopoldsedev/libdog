#include <libdog/CardStack.hpp>

#include <cassert>


namespace libdog {

void CardStack::remove(Card card) {
	for (size_t i = 0; i < cards.size(); i++) {
		if (cards.at(i) == card) {
			cards.erase(cards.begin() + i);
			break;
		}
	}
}

CardStack::CardStack(vector<Card> cards) : cards(cards) {
#ifndef NDEBUG
	rng = default_random_engine(0);
#else
	random_device r;
	rng = default_random_engine(r());
#endif
}

size_t CardStack::size() const {
	return cards.size();
}

bool CardStack::empty() const {
	return cards.empty();
}

bool CardStack::contains(Card card) const {
	for (Card c : cards) {
		if (c == card) {
			return true;
		}
	}

	return false;
}

void CardStack::move_to(CardStack& dest) {
	move_to(dest, size());
}

void CardStack::move_to(CardStack& dest, size_t count) {
	assert(cards.size() >= count);

	dest.cards.insert(dest.cards.end(), cards.begin(), cards.begin() + count);
	cards.erase(cards.begin(), cards.begin() + count);
}

void CardStack::move_to(CardStack& dest, Card card) {
	assert(contains(card));

	vector<Card>::iterator it = find(cards.begin(),cards.end(), card);
	assert(it != cards.end());

	dest.cards.push_back(*it);
	cards.erase(it);
}

void CardStack::shuffle() {
	::shuffle(cards.begin(), cards.end(), rng);
}

vector<Card> CardStack::get_cards() const {
	return cards;
}

string CardStack::to_str() const {
	stringstream ss;

	for (Card c : cards) {
		ss << card_to_string(c);
	}

	return ss.str();
}

};
