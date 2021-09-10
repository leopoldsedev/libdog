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

	// Play an Ace for player 0 to move a piece out of the kennel
	game.play_notation(0, "A#");

	// Print resulting game state
	std::cout << game << std::endl;

	// Get player that has the next turn (player 1)
	int player = game.player_turn;

	// Get all possible legal actions from the current position
	std::vector<ActionVar> actions = game.get_possible_actions(player);

	// Print the first action in the list in game notation
	std::cout << to_notation(player, actions.at(0)) << std::endl;

	// Play the first action in the list in game notation
	game.play(player, actions.at(0));

	// Print resulting game state
	std::cout << game << std::endl;

	// Load a board state and print it
	game.load_board("P0*F1|P14P16|F0F3|P36P48*");
	std::cout << game << std::endl;
}
