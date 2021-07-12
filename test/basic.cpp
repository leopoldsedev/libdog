#include <gtest/gtest.h>

#include "DogGame.hpp"


void check_state(DogGame& game) {
	for (std::size_t player = 0; player != game.kennels.size(); player++) {
		for (std::size_t j = 0; j != game.kennels.size(); j++) {
			PiecePtr& piece = game.kennels.at(player).at(j);

			if (piece != nullptr) {
				EXPECT_EQ(piece->player, player);
				EXPECT_EQ(piece->area, Kennel);
//                EXPECT_EQ(piece->position, j);
				EXPECT_EQ(piece->blocking, true);
			}
		}
	}

	for (std::size_t i = 0; i != game.path.size(); i++) {
		PiecePtr& piece = game.path.at(i);

		if (piece != nullptr) {
//            EXPECT_EQ(piece->position, i);
			EXPECT_EQ(piece->area, Path);

			if (i % PATH_SECTION_LENGTH != 0) {
				EXPECT_EQ(piece->blocking, false);
			}
		}
	}

	for (std::size_t player = 0; player != game.finishes.size(); player++) {
		for (std::size_t j = 0; j != game.finishes.size(); j++) {
			PiecePtr& piece = game.finishes.at(player).at(j);

			if (piece != nullptr) {
				EXPECT_EQ(piece->player, player);
				EXPECT_EQ(piece->area, Finish);
//                EXPECT_EQ(piece->position, j);
				EXPECT_EQ(piece->blocking, false);
			}
		}
	}
}

TEST(BasicTest, Reset) {
	DogGame game;

    check_state(game);

	for (std::size_t player = 0; player != game.kennels.size(); player++) {
		for (std::size_t j = 0; j != game.kennels.size(); j++) {
			PiecePtr& piece = game.kennels.at(player).at(j);

			EXPECT_NE(piece, nullptr);
		}
	}

	for (std::size_t i = 0; i != game.path.size(); i++) {
		PiecePtr& piece = game.path.at(i);

		EXPECT_EQ(piece, nullptr);
	}

	for (std::size_t player = 0; player != game.finishes.size(); player++) {
		for (std::size_t j = 0; j != game.finishes.size(); j++) {
			PiecePtr& piece = game.finishes.at(player).at(j);

			EXPECT_EQ(piece, nullptr);
		}
	}
}

TEST(BasicTest, Start) {
	DogGame game;

	game.start(0);

	check_state(game);

	EXPECT_NE(game.path.at(0), nullptr);
}

TEST(BasicTest, MovePiece) {
	DogGame game;

	game.start(0);

	for (int i = 0; i < 2 * PATH_LENGTH; i++) {
		bool legal;

		legal = game.move_piece(0, i % PATH_LENGTH, 1, false, false);

		EXPECT_TRUE(legal);

		check_state(game);
	}

	EXPECT_NE(game.path.at(0), nullptr);
}
