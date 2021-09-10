#include <libdog/libdog.hpp>


using namespace libdog;

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char *argv[]) {
	// Initialize game with canadian rule active
	DogGame game(true);

	// Assumed card state per player
	// 0: A23456
	// 1: 789TJQ
	// 2: KA2345
	// 3: 6789TJ

	// Print game state
	std::cout << game << std::endl;

	// Play a card exchange move for all four players initially (every round starts like this)
	game.play_notation(0, "G2");
	game.play_notation(1, "G7");
	game.play_notation(2, "GA");
	game.play_notation(3, "G7");

	// Play card for player 0
	game.play_notation(0, "A#");

	// Print resulting game state
	std::cout << game << std::endl;

	// Get all possible legal actions of the player that has the next turn (player 1)
	std::vector<ActionVar> actions = game.get_possible_actions(game.player_turn);

	// Print the first action in the list in game notation
	std::cout << to_notation(game.player_turn, actions.at(0)) << std::endl;

	// Play the first action in the list in game notation
	game.play(game.player_turn, actions.at(0));

	// Print resulting game state
	std::cout << game << std::endl;
}
