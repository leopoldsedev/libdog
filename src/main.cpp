#include <iostream>
#include <chrono>
#include <thread>

#include "DogGame.hpp"

#define sleep(x) (std::this_thread::sleep_for(std::chrono::milliseconds(x)))


using namespace std;

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char *argv[])
{
	DogGame game;

	std::cout << game << std::endl;

	game.start(0);
//    game.start(1);
//    game.start(2);
//    game.start(3);
	std::cout << game << std::endl;

	int i = 0;
	bool legal = true;
//    while (true) {
	for (i = 0; i < 65; i++) {
		legal = game.move_piece(0, i % 64, 1, i != 0, false);
		if (!legal) {
			std::cout << "illegal move" << std::endl;
			break;
		}
//        i++;
		std::cout << game << std::endl;

		sleep(100);
	}

	i = 0;
	while (true) {
		legal = game.move_piece_in_finish(0, i, 1, false);
		if (!legal) {
			std::cout << "illegal move" << std::endl;
			break;
		}

		i++;
		std::cout << game << std::endl;
		sleep(100);
	}

	return 0;
}
