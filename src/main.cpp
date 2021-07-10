#include <iostream>
#include <chrono>
#include <thread>

#include "DogGame.hpp"

#define sleep(x) (std::this_thread::sleep_for(std::chrono::milliseconds(x)))


using namespace std;

int main(int argc, const char *argv[])
{
    DogGame game;

	game.print_state();

	game.start(0);
	game.start(1);
	game.start(2);
	game.start(3);
	game.print_state();

	int i = 0;
	while (1) {
		game.push_forward(0, i % 64, 1, false, false);
		i++;
		game.print_state();

		sleep(100);
	}

	return 0;
}
