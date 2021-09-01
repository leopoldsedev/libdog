#include <gtest/gtest.h>

#include "BoardState.hpp"
#include "DogGame.hpp"
#include "Debug.hpp"
#include "Notation.hpp"


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
				EXPECT_EQ(piece->position, BoardPosition(Kennel, player, j));
				EXPECT_EQ(piece->blocking, true);
			}
		}
	}

	for (std::size_t i = 0; i != game.board_state.path.size(); i++) {
		PiecePtr& piece = game.board_state.path.at(i);

		if (piece != nullptr) {
			EXPECT_EQ(piece->position, BoardPosition(i));

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
				EXPECT_EQ(piece->position, BoardPosition(Finish, player, j));
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

	game.start_piece(0, true);
	check_state(game);
	EXPECT_PLAYER_AT(0, 0);
	EXPECT_EQ(game.board_state.path.at(0)->blocking, true);

	game.start_piece(1, true);
	check_state(game);
	EXPECT_PLAYER_AT(16, 1);
	EXPECT_EQ(game.board_state.path.at(16)->blocking, true);

	game.start_piece(2, true);
	check_state(game);
	EXPECT_PLAYER_AT(32, 2);
	EXPECT_EQ(game.board_state.path.at(32)->blocking, true);

	game.start_piece(3, true);
	check_state(game);
	EXPECT_PLAYER_AT(48, 3);
	EXPECT_EQ(game.board_state.path.at(48)->blocking, true);
}

TEST(BasicTest, MovePiece) {
	DogGame game;

	game.start_piece(0, true);

	for (int i = 0; i < 2 * PATH_LENGTH; i++) {
		bool legal;

		legal = game.move_piece(game.board_state.get_piece(BoardPosition(i % PATH_LENGTH)), 1, true, true, false);

		EXPECT_TRUE(legal);

		check_state(game);
	}

	EXPECT_NE(game.board_state.path.at(0), nullptr);
}

TEST(BasicTest, Blockades) {
	DogGame game;

	game.start_piece(0, true);

	bool legal;

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(0)), PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(PATH_SECTION_LENGTH)), -PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(0)), -PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(PATH_LENGTH - PATH_SECTION_LENGTH)), PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	game.start_piece(1, true);
	game.start_piece(2, true);
	game.start_piece(3, true);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(0)), PATH_SECTION_LENGTH, true, true, false);
	EXPECT_FALSE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(0)), -PATH_SECTION_LENGTH, true, true, false);
	EXPECT_FALSE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(0)), PATH_SECTION_LENGTH - 1, true, true, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(PATH_SECTION_LENGTH - 1)), 1, true, true, false);
	EXPECT_FALSE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(PATH_SECTION_LENGTH - 1)), -(PATH_SECTION_LENGTH - 1), true, true, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(0)), -(PATH_SECTION_LENGTH - 1), true, true, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(PATH_LENGTH - (PATH_SECTION_LENGTH - 1))), -1, true, true, false);
	EXPECT_FALSE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.move_piece(game.board_state.get_piece(BoardPosition(PATH_LENGTH - (PATH_SECTION_LENGTH - 1))), PATH_SECTION_LENGTH - 1, true, true, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);
}

TEST(CardTest, MovePiece) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);
	EXPECT_TRUE(game.board_state.path.at(0)->blocking);

	EXPECT_TRUE(game.play_notation(0, "20"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(2), nullptr);
	EXPECT_FALSE(game.board_state.path.at(2)->blocking);
}

TEST(CardTest, Finish) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "K#"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(12), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(24), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(36), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(48), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(60), nullptr);

	EXPECT_TRUE(game.play_notation(0, "50"));
	check_state(game);
	EXPECT_NE(game.board_state.finishes.at(0).at(0), nullptr);

	EXPECT_TRUE(game.play_notation(0, "30"));
	check_state(game);
	EXPECT_NE(game.board_state.finishes.at(0).at(3), nullptr);
}

TEST(CardTest, BackwardStartFinish) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	EXPECT_TRUE(game.play_notation(0, "4'0"));
	check_state(game);
	EXPECT_NE(game.board_state.path.at(60), nullptr);

	EXPECT_TRUE(game.play_notation(0, "80"));
	check_state(game);
	EXPECT_NE(game.board_state.finishes.at(0).at(3), nullptr);
}

TEST(CardTest, NoFinishFromStart) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(game.play_notation(0, "40"));

	EXPECT_NE(game.board_state.path.at(4), nullptr);
}

TEST(CardTest, SendToKennel) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(game.play_notation(1, "A#"));
	EXPECT_TRUE(game.play_notation(0, "Q0"));
	EXPECT_TRUE(game.play_notation(1, "A'0"));
	EXPECT_TRUE(game.play_notation(0, "50"));

	EXPECT_PLAYER_AT(17, 0);
	EXPECT_NE(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);

	EXPECT_TRUE(game.play_notation(1, "K#"));
	EXPECT_TRUE(game.play_notation(0, "30"));
	EXPECT_TRUE(game.play_notation(1, "4'0"));
	EXPECT_TRUE(game.play_notation(0, "20"));
	EXPECT_TRUE(game.play_notation(1, "T0"));

	EXPECT_PLAYER_AT(22, 1);
	EXPECT_NE(game.board_state.kennels.at(0).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(3), nullptr);

	EXPECT_TRUE(game.play_notation(0, "XA#"));
	EXPECT_TRUE(game.play_notation(1, "K#"));
	EXPECT_TRUE(game.play_notation(0, "XA0"));
	EXPECT_TRUE(game.play_notation(1, "4'1"));
	EXPECT_TRUE(game.play_notation(0, "X50"));
	EXPECT_TRUE(game.play_notation(1, "T0-"));

	EXPECT_PLAYER_AT(16, 0);
	EXPECT_PLAYER_AT(22, 1);
	EXPECT_EQ(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);

	EXPECT_TRUE(game.play_notation(0, "K#"));
	EXPECT_TRUE(game.play_notation(1, "K#"));

	EXPECT_PLAYER_AT(16, 1);
	EXPECT_EQ(game.board_state.kennels.at(0).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(3), nullptr);
}

TEST(CardTest, Swap) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(game.play_notation(1, "A#"));
	EXPECT_EQ(game.board_state.path.at(0)->blocking, true);
	EXPECT_EQ(game.board_state.path.at(16)->blocking, true);

	EXPECT_FALSE(game.play_notation(0, "J010"));

	EXPECT_TRUE(game.play_notation(0, "A'0"));
	EXPECT_EQ(game.board_state.path.at(1)->blocking, false);
	EXPECT_EQ(game.board_state.path.at(16)->blocking, true);

	EXPECT_FALSE(game.play_notation(0, "J010"));

	EXPECT_TRUE(game.play_notation(1, "A'0"));
	EXPECT_EQ(game.board_state.path.at(1)->blocking, false);
	EXPECT_EQ(game.board_state.path.at(17)->blocking, false);

	EXPECT_PLAYER_AT(1, 0);
	EXPECT_PLAYER_AT(17, 1);

	EXPECT_TRUE(game.play_notation(0, "J010"));

	EXPECT_PLAYER_AT(1, 1);
	EXPECT_PLAYER_AT(17, 0);
}

TEST(CardTest, Seven) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(game.play_notation(1, "A#"));
	EXPECT_TRUE(game.play_notation(0, "Q0"));
	EXPECT_TRUE(game.play_notation(1, "A'0"));
	EXPECT_TRUE(game.play_notation(0, "707"));

	EXPECT_PLAYER_AT(19, 0);
	EXPECT_EQ(game.board_state.path.at(17), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);

	game.reset();
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(0, 0)), BoardPosition(4));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(0, 1)), BoardPosition(6));
	// TODO Fix this test case
//    EXPECT_FALSE(game.play_notation(0, "71304"));
}

TEST(CardTest, IntoFinishFlag) {
	/* Possibilities
	   avoid_finish  Finish   Path      outcome
	   ----------------------------------------
	   false         free     free      enter finish
	   false         free     blocked   enter finish      <-- not possible in game
	   false         blocked  free      continue on path
	   false         blocked  blocked   illegal
	   true          free     free      continue on path
	   true          free     blocked   illegal           <-- not possible in game
	   true          blocked  free      continue on path
	   true          blocked  blocked   illegal
	*/

	DogGame game(false, false);

	int player = 0;

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(player, 0)), BoardPosition(60));
	EXPECT_TRUE(game.play_notation(player, "80"));
	EXPECT_NE(game.board_state.get_piece(BoardPosition(Finish, player, 3)), nullptr);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(player, 0)), BoardPosition(60));
	EXPECT_TRUE(game.play_notation(player, "80-"));
	EXPECT_PLAYER_AT(4, 0);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(player, 0)), BoardPosition(60));
	EXPECT_TRUE(game.play_notation(player, "30"));
	EXPECT_PLAYER_AT(63, 0);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(player, 0)), BoardPosition(60));
	EXPECT_TRUE(game.play_notation(player, "30-"));
	EXPECT_PLAYER_AT(63, 0);

	EXPECT_TRUE(game.play_notation(player, "A#"));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(player, 0)), BoardPosition(60));
	EXPECT_FALSE(game.play_notation(player, "T0"));
	EXPECT_FALSE(game.play_notation(player, "T0-"));
	EXPECT_FALSE(game.play_notation(player, "80-"));
	EXPECT_FALSE(game.play_notation(player, "80"));

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(player, 0)), BoardPosition(Finish, player, 1));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(player, 1)), BoardPosition(60));
	EXPECT_TRUE(game.play_notation(player, "81"));
	EXPECT_PLAYER_AT(4, 0);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(player, 1)), BoardPosition(60));
	EXPECT_TRUE(game.play_notation(player, "81-"));
	EXPECT_PLAYER_AT(4, 0);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(player, 1)), BoardPosition(60));
	EXPECT_TRUE(game.play_notation(player, "51"));
	EXPECT_NE(game.board_state.get_piece(BoardPosition(Finish, player, 0)), nullptr);
}

TEST(FullGameTest, WinCondition) {
	DogGame game(false, false);

	EXPECT_EQ(game.result(), -1);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(0, 0)), BoardPosition(Finish, 0, 3));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(0, 1)), BoardPosition(Finish, 0, 2));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(0, 2)), BoardPosition(Finish, 0, 1));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(0, 3)), BoardPosition(Finish, 0, 0));
	EXPECT_EQ(game.result(), -1);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(1, 0)), BoardPosition(Finish, 1, 3));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(1, 1)), BoardPosition(Finish, 1, 2));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(1, 2)), BoardPosition(Finish, 1, 1));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(1, 3)), BoardPosition(Finish, 1, 0));
	EXPECT_EQ(game.result(), -1);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(2, 0)), BoardPosition(Finish, 2, 3));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(2, 1)), BoardPosition(Finish, 2, 2));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(2, 2)), BoardPosition(Finish, 2, 1));
	EXPECT_EQ(game.result(), -1);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(3, 0)), BoardPosition(Finish, 3, 3));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(3, 1)), BoardPosition(Finish, 3, 2));
	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(3, 2)), BoardPosition(Finish, 3, 1));
	EXPECT_EQ(game.result(), -1);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(2, 3)), BoardPosition(Finish, 2, 0));
	EXPECT_EQ(game.result(), 0);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(2, 3)), BoardPosition(Kennel, 2, 3));
	EXPECT_EQ(game.result(), -1);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(3, 3)), BoardPosition(Finish, 3, 0));
	EXPECT_EQ(game.result(), 1);

	game.board_state.move_piece(game.board_state.ref_to_piece(PieceRef(2, 3)), BoardPosition(Finish, 2, 0));
	EXPECT_EQ(game.result(), 2);
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
}
