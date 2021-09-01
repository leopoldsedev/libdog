#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cassert>

#include "DogGame.hpp"
#include "CardPlay.hpp"

#define sleep(x) (std::this_thread::sleep_for(std::chrono::milliseconds(x)))


using namespace std;

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char *argv[])
{
	DogGame game;

	while (game.result() == -1) {
		std::cout << game << std::endl;
		std::cout << "player " << game.player_turn << "'s turn" << std::endl;

		std::vector<CardPlay> plays = game.possible_actions(game.player_turn);

		for (std::size_t i = 0; i < plays.size(); i++) {
			CardPlay play = plays.at(i);
			std::cout << i << ": " << play.get_notation(game.player_turn) << std::endl;
		}

		std::size_t selection;
		do {
			std::cin >> selection;
		} while (selection >= plays.size());

		bool legal = game.play_card(plays.at(selection), true, true, false);
		assert(legal);
	}

	std::cout << "Game concluded" << std::endl;

	return 0;
}
