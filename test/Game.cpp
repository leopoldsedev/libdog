#include <gtest/gtest.h>

#include "BoardState.hpp"
#include "DogGame.hpp"
#include "Debug.hpp"


#define EXPECT_PLAYER_AT(path_idx, player_id) do { \
	EXPECT_NE(game.board_state.path.at(path_idx), nullptr); \
	EXPECT_EQ(game.board_state.path.at(path_idx)->player, player_id); \
} while(0)

void check_state(DogGame& game) {
	for (std::size_t player = 0; player != game.board_state.kennels.size(); player++) {
		for (std::size_t j = 0; j != game.board_state.kennels.size(); j++) {
			PiecePtr& piece = game.board_state.kennels.at(player).at(j);

			if (piece != nullptr) {
				EXPECT_EQ(piece->player, player);
				EXPECT_EQ(piece->area, Kennel);
				EXPECT_EQ(piece->position, j);
				EXPECT_EQ(piece->blocking, true);
			}
		}
	}

	for (std::size_t i = 0; i != game.board_state.path.size(); i++) {
		PiecePtr& piece = game.board_state.path.at(i);

		if (piece != nullptr) {
			EXPECT_EQ(piece->position, i);
			EXPECT_EQ(piece->area, Path);

			if (i % PATH_SECTION_LENGTH != 0) {
				EXPECT_EQ(piece->blocking, false);
			}
		}
	}

	for (std::size_t player = 0; player != game.board_state.finishes.size(); player++) {
		for (std::size_t j = 0; j != game.board_state.finishes.size(); j++) {
			PiecePtr& piece = game.board_state.finishes.at(player).at(j);

			if (piece != nullptr) {
				EXPECT_EQ(piece->player, player);
				EXPECT_EQ(piece->area, Finish);
				EXPECT_EQ(piece->position, j);
				EXPECT_EQ(piece->blocking, false);
			}
		}
	}
}

TEST(BasicTest, Reset) {
	DogGame game;

	check_state(game);

	for (std::size_t player = 0; player != game.board_state.kennels.size(); player++) {
		for (std::size_t j = 0; j != game.board_state.kennels.size(); j++) {
			PiecePtr& piece = game.board_state.kennels.at(player).at(j);

			EXPECT_NE(piece, nullptr);
		}
	}

	for (std::size_t i = 0; i != game.board_state.path.size(); i++) {
		PiecePtr& piece = game.board_state.path.at(i);

		EXPECT_EQ(piece, nullptr);
	}

	for (std::size_t player = 0; player != game.board_state.finishes.size(); player++) {
		for (std::size_t j = 0; j != game.board_state.finishes.size(); j++) {
			PiecePtr& piece = game.board_state.finishes.at(player).at(j);

			EXPECT_EQ(piece, nullptr);
		}
	}
}

TEST(BasicTest, Start) {
	DogGame game;

	game.start_piece(0);

	check_state(game);

	EXPECT_NE(game.board_state.path.at(0), nullptr);
}

TEST(BasicTest, MovePiece) {
	DogGame game;

	game.start_piece(0);

	for (int i = 0; i < 2 * PATH_LENGTH; i++) {
		bool legal;

		legal = game.move_piece(0, BoardPosition(i % PATH_LENGTH), 1, false, false, false);

		EXPECT_TRUE(legal);

		check_state(game);
	}

	EXPECT_NE(game.board_state.path.at(0), nullptr);
}

TEST(BasicTest, Blockades) {
	DogGame game;

	game.start_piece(0);

	bool legal;

	legal = game.move_piece(0, BoardPosition(0), PATH_SECTION_LENGTH, false, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(0, BoardPosition(PATH_SECTION_LENGTH), -PATH_SECTION_LENGTH, false, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.move_piece(0, BoardPosition(0), -PATH_SECTION_LENGTH, false, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(0, BoardPosition(PATH_LENGTH - PATH_SECTION_LENGTH), PATH_SECTION_LENGTH, false, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	game.start_piece(1);
	game.start_piece(2);
	game.start_piece(3);

	legal = game.move_piece(0, BoardPosition(0), PATH_SECTION_LENGTH, false, false, false);
	EXPECT_FALSE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.move_piece(0, BoardPosition(0), -PATH_SECTION_LENGTH, false, false, false);
	EXPECT_FALSE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.move_piece(0, BoardPosition(0), PATH_SECTION_LENGTH - 1, false, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(0, BoardPosition(PATH_SECTION_LENGTH - 1), 1, false, false, false);
	EXPECT_FALSE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(0, BoardPosition(PATH_SECTION_LENGTH - 1), -(PATH_SECTION_LENGTH - 1), false, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.move_piece(0, BoardPosition(0), -(PATH_SECTION_LENGTH - 1), false, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(0, BoardPosition(PATH_LENGTH - (PATH_SECTION_LENGTH - 1)), -1, false, false, false);
	EXPECT_FALSE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(0, BoardPosition(PATH_LENGTH - (PATH_SECTION_LENGTH - 1)), PATH_SECTION_LENGTH - 1, false, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);
}

TEST(CardTest, MovePiece) {
	DogGame game;

	EXPECT_TRUE(game.play_card(CardPlay(0, "A#"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);
	EXPECT_TRUE(game.board_state.path.at(0)->blocking);

	EXPECT_TRUE(game.play_card(CardPlay(0, "20"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(2), nullptr);
	EXPECT_FALSE(game.board_state.path.at(2)->blocking);
}

TEST(CardTest, Finish) {
	DogGame game;

	EXPECT_TRUE(game.play_card(CardPlay(0, "K#"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "Q0"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(12), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "Q0"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(24), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "Q0"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(36), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "Q0"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(48), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "Q0"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(60), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "50"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.finishes.at(0).at(0), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "30"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.finishes.at(0).at(3), nullptr);
}

TEST(CardTest, BackwardStartFinish) {
	DogGame game;

	EXPECT_TRUE(game.play_card(CardPlay(0, "A#"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "4'0"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(60), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "80"), false, false));
	check_state(game);
	EXPECT_NE(game.board_state.finishes.at(0).at(3), nullptr);
}

TEST(CardTest, NoFinishFromStart) {
	DogGame game;

	EXPECT_TRUE(game.play_card(CardPlay(0, "A#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(0, "40"), false, false));

	EXPECT_NE(game.board_state.path.at(4), nullptr);
}

TEST(CardTest, SendToKennel) {
	DogGame game;

	EXPECT_TRUE(game.play_card(CardPlay(0, "A#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "A#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(0, "Q0"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "A'0"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(0, "50"), false, false));

	EXPECT_PLAYER_AT(17, 0);
	EXPECT_NE(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(1, "K#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(0, "30"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "4'0"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(0, "20"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "T0"), false, false));

	EXPECT_PLAYER_AT(22, 1);
	EXPECT_NE(game.board_state.kennels.at(0).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(3), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "XA#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "K#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(0, "XA0"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "4'1"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(0, "X50"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "T0-"), false, false));

	EXPECT_PLAYER_AT(16, 0);
	EXPECT_PLAYER_AT(22, 1);
	EXPECT_EQ(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);

	EXPECT_TRUE(game.play_card(CardPlay(0, "K#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "K#"), false, false));

	EXPECT_PLAYER_AT(16, 1);
	EXPECT_EQ(game.board_state.kennels.at(0).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(3), nullptr);
}

TEST(CardTest, Swap) {
	DogGame game;

	EXPECT_TRUE(game.play_card(CardPlay(0, "A#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "A#"), false, false));

	EXPECT_FALSE(game.play_card(CardPlay(0, "J010"), false, false));

	EXPECT_TRUE(game.play_card(CardPlay(0, "A'0"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "A'0"), false, false));

	EXPECT_PLAYER_AT(1, 0);
	EXPECT_PLAYER_AT(17, 1);

	EXPECT_TRUE(game.play_card(CardPlay(0, "J010"), false, false));

	EXPECT_PLAYER_AT(1, 1);
	EXPECT_PLAYER_AT(17, 0);
}

TEST(CardTest, Seven) {
	// TODO Add cases where pieces included in the 7-move are sent back to kennel while executing the 7-move
	DogGame game;

	EXPECT_TRUE(game.play_card(CardPlay(0, "A#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "A#"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(0, "Q0"), false, false));
	EXPECT_TRUE(game.play_card(CardPlay(1, "A'0"), false, false));
	PRINT_DBG(game);
	EXPECT_TRUE(game.play_card(CardPlay(0, "707"), false, false));

	PRINT_DBG(game);

	EXPECT_PLAYER_AT(19, 0);
	EXPECT_EQ(game.board_state.path.at(17), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);
}

TEST(FullGameTest, CardExchange) {
	GTEST_SKIP();
	// TODO
}

TEST(FullGameTest, Turns) {
	GTEST_SKIP();
	// TODO
}

TEST(FullGameTest, HandCheck) {
	GTEST_SKIP();
	// TODO
}

TEST(FullGameTest, One) {
	GTEST_SKIP();
	DogGame game;

	PRINT_DBG(game);
}
