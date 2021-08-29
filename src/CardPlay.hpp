#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <regex>

#include "Card.hpp"
#include "Piece.hpp"
#include "PieceRef.hpp"
#include "BoardPosition.hpp"
#include "Debug.hpp"

// TODO Duplicate definitions
#define KENNEL_SIZE (4)
#define PLAYER_COUNT (4)

#define GET_TEAM_PLAYER_IDX(player_idx) (((player_idx) + 2) % PLAYER_COUNT)


class CardPlay {
	public:
		// TODO remove player from this class? (then it also needs to be removed from PieceRef) -> think about jack card notation (it assumes the first specified rank belongs to the player playing the card)
		int player = -1;
		Card card = None;

		std::vector<PieceRef> target_pieces = {};
		std::vector<bool> into_finish = {};
		std::vector<int> counts = {};

		bool start_card = false;
		bool ace_one = false;
		bool four_backwards = false;

		std::unique_ptr<CardPlay> joker_card;

		CardPlay(int player, Card card) : CardPlay(player, card, false, false, false) {
		}

		CardPlay(int player, Card card, bool start_card) : CardPlay(player, card, start_card, false, false) {
		}

		CardPlay(int player, Card card, bool start_card, bool ace_one, bool four_backwards) : player(player), card(card), start_card(start_card), ace_one(ace_one), four_backwards(four_backwards) {
		}

		CardPlay() {
		}

		CardPlay* get_play() {
			CardPlay* play;

			if (card == Joker) {
				play = joker_card.get();
			} else {
				play = this;
			}

			return play;
		}

		int move_count() {
			int count = 0;

			switch (card) {
				case Ace: {
					count = ace_one ? 1 : 11;
					break;
				}
				case Two: {
					count = 2;
					break;
				}
				case Three: {
					count = 3;
					break;
				}
				case Four: {
					count = 4;
					break;
				}
				case Five: {
					count = 5;
					break;
				}
				case Six: {
					count = 6;
					break;
				}
				case Eight: {
					count = 8;
					break;
				}
				case Nine: {
					count = 9;
					break;
				}
				case Ten: {
					count = 10;
					break;
				}
				case Queen: {
					count = 12;
					break;
				}
				case King: {
					count = 13;
					break;
				}
				default: {
				}
			}

			if (four_backwards) {
				count *= -1;
			}

			return count;
		}

		bool is_valid() {
			if (start_card && !is_start_card(card)) {
				return false;
			}

			if (target_pieces.size() != into_finish.size()) {
				return false;
			}

			if (card == None) {
				return false;
			}

			if (start_card) {
				if (target_pieces.size() != 0) {
					return false;
				}
			} else {
				if (card != Seven && card != Jack && target_pieces.size() > 1) {
					return false;
				}

				if (card == Jack && target_pieces.size() != 2) {
					return false;
				}

				if (card != Four && four_backwards) {
					return false;
				}

				if (card == Seven) {
					if (target_pieces.size() != counts.size()) {
						return false;
					}

					int sum_count = 0;
					for (std::size_t i = 0; i < counts.size(); i++) {
						int count = counts.at(i);
						if (count < 0) {
							return false;
						}
						sum_count += counts.at(i);
					}
					if (sum_count != 7) {
						return false;
					}
				} else {
					if (counts.size() > 0) {
						return false;
					}
				}
			}

			if (card == Joker) {
				if (joker_card == nullptr) {
					return false;
				}

				if (joker_card->card == Joker) {
					return false;
				}

				if (!get_play()->is_valid()) {
					return false;
				}
			}

			return true;
		}

		// TODO Move notation into its own parser class
		bool from_notation(int player, std::string notation_str) {
			// Reset all fields
			// TODO I feel like this is bad practice
			*this = CardPlay();

			// Remove all whitespace from string
			// Source: https://stackoverflow.com/a/83538/3118787
			notation_str.erase(remove_if(notation_str.begin(), notation_str.end(), ::isspace), notation_str.end());

			std::string card_str = notation_str.substr(0, 1);
			notation_str.erase(0, 1);

			card = card_from_string(card_str);
			this->player = player;

			bool match = false;

			switch (card) {
				case Two: case Three: case Five: case Six: case Eight: case Nine: case Ten: case Queen:
					match = notation_parse_simple_forward(player, notation_str);
					break;
				case Ace:
					match = notation_parse_ace(player, notation_str);
					break;
				case Four:
					match = notation_parse_four(player, notation_str);
					break;
				case Seven:
					match = notation_parse_seven(player, notation_str);
					break;
				case Jack:
					match = notation_parse_jack(player, notation_str);
					break;
				case King:
					match = notation_parse_king(player, notation_str);
					break;
				case Joker:
					match = notation_parse_joker(player, notation_str);
					break;
				case None:
				default:
					break;
			}

			// Check if all piece players and piece ranks are valid (kennel
			// size defines number of pieces per player). Piece rank cannot be
			// larger than KENNEL_SIZE - 1)
			for (PieceRef& ref : target_pieces) {
				if (ref.rank < 0 || ref.player < 0 || ref.rank >= KENNEL_SIZE || ref.player >= PLAYER_COUNT) {
					match = false;
					break;
				}
			}

			assert(!match || is_valid());

			return match;
		}

		// notation_arg_str is the string without the card specifier (e.g. "#" if full notation string is "A#")
		bool notation_parse_simple_forward(int player, std::string notation_arg_str) {
			std::regex regex(R"((\d)(-?))");
			std::smatch matches;

			bool match = std::regex_match(notation_arg_str, matches, regex);

			if (match) {
				int rank = std::stoi(matches[1]);
				bool avoid_finish = (matches[2] == "-");

				PieceRef piece_ref(player, rank);
				target_pieces.push_back(piece_ref);
				into_finish.push_back(!avoid_finish);
			}

			return match;
		}

		bool notation_parse_ace(int player, std::string notation_arg_str) {
			std::regex regex(R"((#)|(('?)(\d)(-?)))");
			std::smatch matches;

			bool match = std::regex_match(notation_arg_str, matches, regex);

			if (match) {
				start_card = (matches[1] == "#");

				if (!start_card) {
					ace_one = (matches[3] == "'");
					int rank = std::stoi(matches[4]);
					bool avoid_finish = (matches[5] == "-");

					PieceRef piece_ref(player, rank);
					target_pieces.push_back(piece_ref);
					into_finish.push_back(!avoid_finish);
				}
			}

			return match;
		}

		bool notation_parse_four(int player, std::string notation_arg_str) {
			std::regex regex(R"(((')(\d))|((\d)(-?)))");
			std::smatch matches;

			bool match = std::regex_match(notation_arg_str, matches, regex);

			if (match) {
				int rank;
				bool avoid_finish;

				if (matches[2] != "") {
					four_backwards = true;
					avoid_finish = false;
					rank = std::stoi(matches[3]);
				} else {
					four_backwards = false;
					rank = std::stoi(matches[5]);
					avoid_finish = (matches[6] == "-");
				}

				PieceRef piece_ref(player, rank);
				target_pieces.push_back(piece_ref);
				into_finish.push_back(!avoid_finish);
			}

			return match;
		}

		bool notation_parse_seven(int player, std::string notation_arg_str) {
			std::regex regex_outer(R"((\d'?\d-?)+)");
			std::regex regex_inner(R"((\d)('?)(\d)(-?))");
			std::smatch matches;

			bool match = std::regex_match(notation_arg_str, matches, regex_outer);

			if (match) {
				int other_player_idx = GET_TEAM_PLAYER_IDX(player);
				int count_sum = 0;

				std::sregex_iterator iter;
				std::sregex_iterator end = std::sregex_iterator();
				iter = std::sregex_iterator(notation_arg_str.begin(), notation_arg_str.end(), regex_inner);

				for(; iter != end; iter++)
				{
					int rank = std::stoi(iter->str(1));
					bool other_player = (iter->str(2) == "'");
					int count = std::stoi(iter->str(3));
					bool avoid_finish = (iter->str(4) == "-");

					int curr_player = other_player ? other_player_idx : player;

					PieceRef piece_ref(curr_player, rank);
					target_pieces.push_back(piece_ref);
					into_finish.push_back(!avoid_finish);
					counts.push_back(count);

					count_sum += count;
				}

				if (count_sum != 7) {
					match = false;
				}
			}

			return match;
		}

		bool notation_parse_jack(int player, std::string notation_arg_str) {
			std::regex regex(R"((\d)(\d)(\d))");
			std::smatch matches;

			bool match = std::regex_match(notation_arg_str, matches, regex);

			if (match) {
				int rank = std::stoi(matches[1]);

				PieceRef piece_ref(player, rank);
				target_pieces.push_back(piece_ref);
				into_finish.push_back(true);

				player = std::stoi(matches[2]);
				rank = std::stoi(matches[3]);

				piece_ref = PieceRef(player, rank);
				target_pieces.push_back(piece_ref);
				into_finish.push_back(true);
			}

			return match;
		}

		bool notation_parse_king(int player, std::string notation_arg_str) {
			std::regex regex(R"((#)|((\d)(-?)))");
			std::smatch matches;

			bool match = std::regex_match(notation_arg_str, matches, regex);

			if (match) {
				start_card = (matches[1] == "#");

				if (!start_card) {
					int rank = std::stoi(matches[3]);
					bool avoid_finish = (matches[4] == "-");

					PieceRef piece_ref(player, rank);
					target_pieces.push_back(piece_ref);
					into_finish.push_back(!avoid_finish);
				}
			}

			return match;
		}

		bool notation_parse_joker(int player, std::string notation_arg_str) {
			joker_card = std::make_unique<CardPlay>();

			// Look ahead to avoid double recursion
			std::string joker_card_str = notation_arg_str.substr(0, 1);
			Card joker_card_card = card_from_string(joker_card_str);

			bool match;

			if (joker_card_card != Joker) {
				match = joker_card->from_notation(player, notation_arg_str);
			} else {
				match = false;
			}

			return match;
		}

};
