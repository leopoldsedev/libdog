#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <libdog/libdog.hpp>


using namespace libdog;

#define GAME_COUNT (1000)

static std::vector<Card> generate_random_deck(int seed) {
	CardStack single_deck(get_dog_card_set(), default_random_engine(seed));
	single_deck.shuffle();
	CardStack deck;

	int N = 100;

	for (int i = 0; i < N; i++) {
		CardStack single_deck_copy = single_deck;
		single_deck_copy.move_to(deck);
	}

	return deck.get_cards();
}

static std::array<std::vector<Card>, GAME_COUNT> get_card_lists() {
	std::array<std::vector<Card>, GAME_COUNT> result;

	for (std::size_t i = 0; i < result.size(); i++) {
		int seed = 10 * i + 1;
		result[i] = generate_random_deck(seed);
	}

	return result;
}

static std::array<std::vector<Card>, GAME_COUNT> card_lists = get_card_lists();

static void play_game_to_end(DogGame& game, bool random_move) {
	default_random_engine rng(0);

	while (game.result() == -1) {
		std::vector<ActionVar> actions = game.get_possible_actions(game.player_turn);

		std::size_t selection = 0;

		if (random_move) {
			std::uniform_int_distribution<int> uniform(0, actions.size() - 1);
			selection = uniform(rng);
		}

		ActionVar action = actions.at(selection);

//        int player_notation = game.switch_to_team_mate_if_done(game.player_turn);
//        std::cout << game.player_turn << ": " << to_notation(player_notation, action) << std::endl;

		bool legal = game.play(game.player_turn, action);
		assert(legal);
	}
}

// 0.84 s
TEST(Performance, PossibleSeven) {
	int N = 10000;

	DogGame game(true, false, false, false);
	std::vector<ActionVar> actions;

	game.load_board("P1P17||P33|");

	for (int i = 0; i < N; i++) {
		actions = game.possible_actions_for_card(0, Seven, false);
	}
}

// 1.20 s
TEST(Performance, PossibleSevenFew) {
	int N = 1000000;

	DogGame game(true, false, false, false);
	std::vector<ActionVar> actions;

	game.load_board("P0|||");

	for (int i = 0; i < N; i++) {
		actions = game.possible_actions_for_card(0, Seven, false);
	}
}

// 2.98 s
TEST(Performance, PossibleSevenWorstCase) {
	int N = 10;

	DogGame game(true, false, false, false);
	std::vector<ActionVar> actions;

	game.load_board("P0P8P16P24||P32P40P48P56|");

	for (int i = 0; i < N; i++) {
		actions = game.possible_actions_for_card(0, Seven, false);
	}
}

// 0.17 s
TEST(Performance, FullGame) {

	DogGame game(true);
	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT8295A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");

	play_game_to_end(game, false);

	std::cout << game << std::endl;
}

// without repr: 35.02 s
// with repr: 18.20 s
TEST(Performance, RandomGames) {
	for (std::size_t i = 0; i < card_lists.size(); i++) {
		std::vector<Card> card_list = card_lists[i];
//        std::cout << "Game " << i << std::endl;

		DogGame game(true);
		game.reset_with_deck(card_list);

//        std::cout << game.cards_state.get_deck() << std::endl;

		play_game_to_end(game, true);
//        std::cout << game << std::endl;
	}
}
