#include <gtest/gtest.h>

#include "BoardState.hpp"
#include "DogGame.hpp"
#include "Debug.hpp"


void check_state(DogGame& game) {
	for (std::size_t player = 0; player != game.board_state.kennels.size(); player++) {
		for (std::size_t j = 0; j != game.board_state.kennels.size(); j++) {
			PiecePtr& piece = game.board_state.kennels.at(player).at(j);

			if (piece != nullptr) {
				EXPECT_EQ(piece->player, player);
				EXPECT_EQ(piece->area, Kennel);
//                EXPECT_EQ(piece->position, j);
				EXPECT_EQ(piece->blocking, true);
			}
		}
	}

	for (std::size_t i = 0; i != game.board_state.path.size(); i++) {
		PiecePtr& piece = game.board_state.path.at(i);

		if (piece != nullptr) {
//            EXPECT_EQ(piece->position, i);
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
//                EXPECT_EQ(piece->position, j);
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
	bool legal;
	CardPlay play = CardPlay(0, Ace, true);

	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	play.card = Two;
	play.start_card = false;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(2), nullptr);
}

TEST(CardTest, Finish) {
	DogGame game;
	bool legal;
	CardPlay play = CardPlay(0, King, true);

	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	play.card = Queen;
	play.start_card = false;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(12), nullptr);

	play.card = Queen;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(24), nullptr);

	play.card = Queen;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(36), nullptr);

	play.card = Queen;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(48), nullptr);

	play.card = Queen;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(60), nullptr);

	play.card = Five;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.finishes.at(0).at(0), nullptr);

	play.card = Three;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.finishes.at(0).at(3), nullptr);
}

TEST(CardTest, BackwardStartFinish) {
	DogGame game;
	bool legal;
	CardPlay play = CardPlay(0, Ace, true);

	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	play.card = Four;
	play.start_card = false;
	play.four_backwards = true;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.path.at(60), nullptr);

	play.card = Eight;
	play.four_backwards = false;
	play.target_pieces.clear();
	play.into_finish.clear();
	play.target_pieces.push_back(PieceRef(0, 0));
	play.into_finish.push_back(true);
	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
	check_state(game);
	EXPECT_NE(game.board_state.finishes.at(0).at(3), nullptr);
}

TEST(CardTest, SendToKennel) {
	GTEST_SKIP();
	// TODO
	DogGame game;
	bool legal;

	CardPlay play = CardPlay(0, Ace, true);

	legal = game.play_card(play, false, false);
	EXPECT_TRUE(legal);
}

TEST(CardTest, Swap) {
	GTEST_SKIP();
	// TODO
}

TEST(CardTest, Seven) {
	GTEST_SKIP();
	// TODO
}

TEST(FullGameTest, One) {
	GTEST_SKIP();
	DogGame game;
	bool legal;

	PRINT_DBG(game);
}
