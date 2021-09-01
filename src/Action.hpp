#pragma once

#include <variant>
#include <vector>

#include "Card.hpp"
#include "PieceRef.hpp"
#include "Debug.hpp"


const std::map <int, Card> count_to_card {
	{ -4, Four },
	{ 1, Ace },
	{ 2, Two },
	{ 3, Three },
	{ 4, Four },
	{ 5, Five },
	{ 6, Six },
	{ 7, Seven },
	{ 8, Eight },
	{ 9, Nine },
	{ 10, Ten },
	{ 11, Ace },
	{ 12, Queen },
	{ 13, King }
};

inline int simple_card_get_count(Card card) {
	switch (card) {
		case Two:
			return 2;
		case Three:
			return 3;
		case Five:
			return 5;
		case Six:
			return 6;
		case Eight:
			return 8;
		case Nine:
			return 9;
		case Ten:
			return 10;
		case Queen:
			return 12;
		default:
			assert(false);
			break;
	}
}

class Action {
	protected:
		Card card;
		bool joker;

	public:
		Action(Card card, bool joker) : card(card), joker(joker) {
		}

		Action(Card card) : Action(card, false) {
		}

		virtual bool is_valid() const {
			if (card == None) {
				return false;
			}

			if (card == Joker) {
				return false;
			}

			return true;
		}

		Card get_card() const {
			if (joker) {
				return Joker;
			}

			return get_card_raw();
		}

		Card get_card_raw() const {
			return card;
		}

		bool is_joker() const {
			return joker;
		}

		void set_joker(bool joker) {
			this->joker = joker;
		}

		friend bool operator==(const Action& a, const Action& b) = default;
};

class Give : public Action {
	public:
		Give(Card card, bool joker) : Action(card, joker) {
		}

		Give(Card card) : Give(card, false) {
		}

		virtual bool is_valid() const {
			return (card != None);
		}

		friend bool operator==(const Give& a, const Give& b) = default;
};

class Discard : public Action {
	public:
		Discard(Card card) : Action(card, false) {
		}

		virtual bool is_valid() const {
			return Action::is_valid() && (card != None);
		}

		friend bool operator==(const Discard& a, const Discard& b) = default;
};

class Start : public Action {
	public:
		Start(Card card, bool joker) : Action(card, joker) {
		}

		Start(Card card) : Start(card, false) {
		}

		virtual bool is_valid() const {
			return Action::is_valid() && is_start_card(card);
		}

		friend bool operator==(const Start& a, const Start& b) = default;
};

class MoveSpecifier {
	public:
		PieceRef piece_ref;
		int count;
		bool avoid_finish;

		MoveSpecifier(PieceRef piece_ref, int count, bool avoid_finish) : piece_ref(piece_ref), count(count), avoid_finish(avoid_finish) {
		}

		friend bool operator==(const MoveSpecifier& a, const MoveSpecifier& b) = default;
};

class Move : public Action {
	private:

	public:
		MoveSpecifier move_specifier;

		Move(Card card, MoveSpecifier move_specifier, bool joker) : Action(card, joker), move_specifier(move_specifier) {
		}

		Move(Card card, PieceRef piece_ref, int count, bool avoid_finish, bool joker) : Move(card, MoveSpecifier(piece_ref, count, avoid_finish), joker) {
		}

		Move(Card card, PieceRef piece_ref, int count, bool avoid_finish) : Move(card, piece_ref, count, avoid_finish, false) {
		}

		virtual bool is_valid() const {
			if (card == None) {
				return false;
			}

			if (count_to_card.find(move_specifier.count) == count_to_card.end()) {
				return false;
			}

			Card expected_card = count_to_card.at(move_specifier.count);
			if (expected_card != card && !joker) {
				return false;
			}

			if (!move_specifier.piece_ref.is_valid()) {
				return false;
			}

			return Action::is_valid();
		}

		PieceRef get_piece_ref() const {
			return move_specifier.piece_ref;
		}

		int get_count() const {
			return move_specifier.count;
		}

		bool get_avoid_finish() const {
			return move_specifier.avoid_finish;
		}

		friend bool operator==(const Move& a, const Move& b) = default;
};

class MoveMultiple : public Action {
	private:
		std::vector<MoveSpecifier> move_specifiers;

	public:
		MoveMultiple(Card card, std::vector<MoveSpecifier> move_specifiers, bool joker) : Action(card, joker), move_specifiers(move_specifiers) {
		}

		MoveMultiple(Card card, MoveSpecifier move_specifier) : Action(card) {
			move_specifiers.push_back(move_specifier);
		}

		MoveMultiple(Card card, std::vector<MoveSpecifier> move_specifiers) : MoveMultiple(card, move_specifiers, false) {
		}

		// TODO Check that the same piece is not references more than once in the move list
		virtual bool is_valid() const {
			if (card != Seven && !joker) {
				return false;
			}

			int sum_count = 0;

			for (std::size_t i = 0; i < move_specifiers.size(); i++) {
				MoveSpecifier move_specifier = move_specifiers.at(i);

				if (move_specifier.count <= 0) {
					return false;
				}

				if (!move_specifier.piece_ref.is_valid()) {
					return false;
				}

				sum_count += move_specifier.count;
			}

			if (sum_count != 7) {
				return false;
			}

			return Action::is_valid();
		}

		std::vector<MoveSpecifier> get_move_specifiers() const {
			return move_specifiers;
		}

		friend bool operator==(const MoveMultiple& a, const MoveMultiple& b) = default;
};

class Swap : public Action {
	private:
		PieceRef piece_1;
		PieceRef piece_2;

	public:
		Swap(Card card, PieceRef piece_1, PieceRef piece_2, bool joker) : Action(card, joker), piece_1(piece_1), piece_2(piece_2) {
		}

		Swap(Card card, PieceRef piece_1, PieceRef piece_2) : Swap(card, piece_1, piece_2, false) {
		}

		virtual bool is_valid() const {
			if (card != Jack && !joker) {
				return false;
			}

			if (piece_1.player == piece_2.player) {
				return false;
			}

			if (!piece_1.is_valid() || !piece_2.is_valid()) {
				return false;
			}

			return Action::is_valid();
		}

		PieceRef get_piece_1() const {
			return piece_1;
		}

		PieceRef get_piece_2() const {
			return piece_2;
		}

		// TODO Use unordered_set so that the default implementation of operator== can be used
		friend bool operator==(const Swap& a, const Swap& b) {
			bool in_order_match = (a.piece_1 == b.piece_1) && (a.piece_2 == b.piece_2);
			bool out_of_order_match = (a.piece_1 == b.piece_2) && (a.piece_1 == b.piece_2);
			return in_order_match || out_of_order_match;
		}
};

typedef std::variant<Give, Discard, Start, Move, MoveMultiple, Swap> ActionVar;

#define MATCH(x, class_name, bind_name) const class_name* bind_name = std::get_if<class_name>(x)
#define MATCH_NON_CONST(x, class_name, bind_name) class_name* bind_name = std::get_if<class_name>(x)

inline bool action_is_valid(const ActionVar& action) {
	// TODO This can be done more elegantly with visit. See https://en.cppreference.com/w/cpp/utility/variant/visit
	if (MATCH(&action, Give, a)) {
		return a->is_valid();
	} else if (MATCH(&action, Discard, a)) {
		return a->is_valid();
	} else if (MATCH(&action, Swap, a)) {
		return a->is_valid();
	} else if (MATCH(&action, Move, a)) {
		return a->is_valid();
	} else if (MATCH(&action, MoveMultiple, a)) {
		return a->is_valid();
	} else if (MATCH(&action, Start, a)) {
		return a->is_valid();
	}
	assert(false);
}

inline bool action_is_joker(const ActionVar& action) {
	// TODO This can be done more elegantly with visit. See https://en.cppreference.com/w/cpp/utility/variant/visit
	if (MATCH(&action, Give, a)) {
		return a->is_joker();
	} else if (MATCH(&action, Discard, a)) {
		return a->is_joker();
	} else if (MATCH(&action, Swap, a)) {
		return a->is_joker();
	} else if (MATCH(&action, Move, a)) {
		return a->is_joker();
	} else if (MATCH(&action, MoveMultiple, a)) {
		return a->is_joker();
	} else if (MATCH(&action, Start, a)) {
		return a->is_joker();
	}
	assert(false);
}

inline Card action_get_card(const ActionVar& action) {
	// TODO This can be done more elegantly with visit. See https://en.cppreference.com/w/cpp/utility/variant/visit
	if (MATCH(&action, Give, a)) {
		return a->get_card();
	} else if (MATCH(&action, Discard, a)) {
		return a->get_card();
	} else if (MATCH(&action, Swap, a)) {
		return a->get_card();
	} else if (MATCH(&action, Move, a)) {
		return a->get_card();
	} else if (MATCH(&action, MoveMultiple, a)) {
		return a->get_card();
	} else if (MATCH(&action, Start, a)) {
		return a->get_card();
	}
	assert(false);
}

inline void action_set_joker(ActionVar& action, bool joker) {
	// TODO This can be done more elegantly with visit. See https://en.cppreference.com/w/cpp/utility/variant/visit
	if (MATCH_NON_CONST(&action, Give, a)) {
		a->set_joker(joker);
	} else if (MATCH_NON_CONST(&action, Discard, a)) {
		a->set_joker(joker);
	} else if (MATCH_NON_CONST(&action, Swap, a)) {
		a->set_joker(joker);
	} else if (MATCH_NON_CONST(&action, Move, a)) {
		a->set_joker(joker);
	} else if (MATCH_NON_CONST(&action, MoveMultiple, a)) {
		a->set_joker(joker);
	} else if (MATCH_NON_CONST(&action, Start, a)) {
		a->set_joker(joker);
	} else {
		assert(false);
	}
}
