#include <iostream>

#include "DogGame.hpp"

using namespace std;

int main(int argc, const char *argv[])
{
    DogGame game;
	game.print_state();
	game.reset();
	game.print_state();

	return 0;
}
