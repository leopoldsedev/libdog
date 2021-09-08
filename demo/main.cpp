#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cassert>

#include "DogGame.hpp"
#include "Notation.hpp"

#define sleep(x) (std::this_thread::sleep_for(std::chrono::milliseconds(x)))


using namespace std;

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char *argv[])
{
	DogGame game;

	int moves = 0;
	int actions_sum = 0;

	while (game.result() == -1) {
		std::cout << game << std::endl;
		std::cout << "player " << game.player_turn << "'s turn (move " << (moves + 1) << ")" << std::endl;

		std::vector<ActionVar> actions = game.get_possible_actions(game.player_turn);

		actions_sum += actions.size();

		for (std::size_t i = 0; i < actions.size(); i++) {
			ActionVar action = actions.at(i);
			std::cout << i << ": " << to_notation(game.player_turn, action) << std::endl;
		}

		std::size_t selection = 0;
//        do {
//            std::cin >> selection;
//        } while (selection >= actions.size());

		ActionVar action = actions.at(selection);
		std::cout << "Player " << game.player_turn << " is playing " << to_notation(game.player_turn, action) << std::endl;

//        std::cin.get();

		bool legal = game.play(game.player_turn, action);
		assert(legal);
		moves++;
	}

	std::cout << game << std::endl;
	std::cout << "Game concluded after " << moves << " moves" << std::endl;
	std::cout << (float) actions_sum / moves << " possible actions per move on average" << std::endl;

	return 0;
}
