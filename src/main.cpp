#include <iostream>
#include <chrono>
#include <thread>

#include "DogGame.hpp"

#define sleep(x) (std::this_thread::sleep_for(std::chrono::milliseconds(x)))


using namespace std;

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char *argv[])
{
    DogGame game;

	game.print_state();

	game.start(0);
	game.start(1);
	game.start(2);
	game.start(3);
	game.print_state();

	int i = 0;
	bool legal = true;
	while (1) {
		legal = game.move_piece(0, i % 64, 1, i != 0, false);
		if (!legal) {
			std::cout << "illegal move" << std::endl;
			break;
		}
		i++;
		game.print_state();

		sleep(100);
	}

	return 0;
}
