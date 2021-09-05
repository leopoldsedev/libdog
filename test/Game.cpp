#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "BoardState.hpp"
#include "DogGame.hpp"
#include "Debug.hpp"
#include "Notation.hpp"
#include "PieceRef.hpp"


#define EXPECT_PLAYER_AT(path_idx, player_id) do { \
	EXPECT_NE(game.board_state.path.at(path_idx), nullptr); \
	EXPECT_EQ(game.board_state.path.at(path_idx)->player, player_id); \
} while(0)

#define EXPECT_PLAYER_AT_FINISH(finish_idx, player_id) do { \
	EXPECT_NE(game.board_state.finishes.at(player).at(finish_idx), nullptr); \
	EXPECT_EQ(game.board_state.finishes.at(player).at(finish_idx)->player, player_id); \
} while(0)

#define TEST_MOVE_FROM_TO(notation_start, player_id, notation_move, notation_end) do { \
	DogGame game(false, false); \
	game.load_board(notation_start); \
	EXPECT_TRUE(game.play_notation(player_id, notation_move)); \
	EXPECT_TRUE(check_state(game)); \
	EXPECT_EQ(to_notation(game.board_state), notation_end); \
} while(0)

#define TEST_INVALID_MOVE_FROM(notation_start, player_id, notation_move) do { \
	DogGame game(false, false); \
	game.load_board(notation_start); \
	EXPECT_FALSE(game.play_notation(player_id, notation_move)); \
	EXPECT_TRUE(check_state(game)); \
	EXPECT_EQ(to_notation(game.board_state), notation_start); \
} while(0)


bool check_state(DogGame& game) {
	std::array<int, PLAYER_COUNT> piece_cnt;
	piece_cnt.fill(0);

	for (int player = 0; player != PLAYER_COUNT; player++) {
		for (std::size_t j = 0; j != game.board_state.kennels.size(); j++) {
			PiecePtr& piece = game.board_state.kennels.at(player).at(j);

			if (piece != nullptr) {
				if (piece->player != player) {
					return false;
				}

				if (piece->position != BoardPosition(Kennel, player, j)) {
					return false;
				}

				piece_cnt.at(piece->player)++;
			}
		}
	}

	for (std::size_t i = 0; i != game.board_state.path.size(); i++) {
		PiecePtr& piece = game.board_state.path.at(i);

		if (piece != nullptr) {
			if (piece->position != BoardPosition(i)) {
				return false;
			}

			piece_cnt.at(piece->player)++;
		}
	}

	for (int player = 0; player != PLAYER_COUNT; player++) {
		for (std::size_t j = 0; j != game.board_state.finishes.size(); j++) {
			PiecePtr& piece = game.board_state.finishes.at(player).at(j);

			if (piece != nullptr) {
				if (piece->player != player) {
					return false;
				}

				if (piece->position != BoardPosition(Finish, player, j)) {
					return false;
				}

				piece_cnt.at(piece->player)++;
			}
		}
	}

	for (int count : piece_cnt) {
		if (count != PIECE_COUNT) {
			return false;
		}
	}

	return true;
}

TEST(BasicTest, Reset) {
	DogGame game;

	EXPECT_TRUE(check_state(game));

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

	game.board_state.start_piece(0, true);
	EXPECT_TRUE(check_state(game));
	EXPECT_PLAYER_AT(0, 0);
	EXPECT_EQ(game.board_state.path.at(0)->blocking, true);

	game.board_state.start_piece(1, true);
	EXPECT_TRUE(check_state(game));
	EXPECT_PLAYER_AT(16, 1);
	EXPECT_EQ(game.board_state.path.at(16)->blocking, true);

	game.board_state.start_piece(2, true);
	EXPECT_TRUE(check_state(game));
	EXPECT_PLAYER_AT(32, 2);
	EXPECT_EQ(game.board_state.path.at(32)->blocking, true);

	game.board_state.start_piece(3, true);
	EXPECT_TRUE(check_state(game));
	EXPECT_PLAYER_AT(48, 3);
	EXPECT_EQ(game.board_state.path.at(48)->blocking, true);
}

TEST(BasicTest, MovePiece) {
	DogGame game;

	game.board_state.start_piece(0, true);

	for (int i = 0; i < 2 * PATH_LENGTH; i++) {
		bool legal;

		legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(i % PATH_LENGTH)), 1, true, true, false);

		EXPECT_TRUE(legal);

		EXPECT_TRUE(check_state(game));
	}

	EXPECT_NE(game.board_state.path.at(0), nullptr);
}

TEST(BasicTest, Blockades) {
	DogGame game;

	game.board_state.start_piece(0, true);

	bool legal;

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_SECTION_LENGTH)), -PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), -PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_LENGTH - PATH_SECTION_LENGTH)), PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	game.board_state.start_piece(1, true);
	game.board_state.start_piece(2, true);
	game.board_state.start_piece(3, true);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), PATH_SECTION_LENGTH, true, true, false);
	EXPECT_FALSE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), -PATH_SECTION_LENGTH, true, true, false);
	EXPECT_FALSE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), PATH_SECTION_LENGTH - 1, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_SECTION_LENGTH - 1)), 1, true, true, false);
	EXPECT_FALSE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_SECTION_LENGTH - 1)), -(PATH_SECTION_LENGTH - 1), true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), -(PATH_SECTION_LENGTH - 1), true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_LENGTH - (PATH_SECTION_LENGTH - 1))), -1, true, true, false);
	EXPECT_FALSE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_LENGTH - (PATH_SECTION_LENGTH - 1))), PATH_SECTION_LENGTH - 1, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(0), nullptr);
}

TEST(BasicTest, PieceRefResolution) {
	DogGame game(false, false);

	// TODO Also check negative construction cases
	for (int player = 0; player < PLAYER_COUNT; player++) {
		for (int rank = 0; rank < PIECE_COUNT; rank++) {
			PieceRef piece_ref = PieceRef(player, rank);
			EXPECT_TRUE(piece_ref.is_valid());
			EXPECT_EQ(game.board_state.ref_to_pos(piece_ref), BoardPosition(Kennel, player, rank));
		}
	}

	int player = 3;
	BoardPosition pos0 = BoardPosition(47), pos1 = BoardPosition(42), pos2 = BoardPosition(63), pos3 = BoardPosition(48);
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 0)), pos0);
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 1)), pos1);
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 2)), pos2);
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 3)), pos3);

	EXPECT_EQ(game.board_state.ref_to_pos(PieceRef(player, 0)), pos3);
	EXPECT_EQ(game.board_state.ref_to_pos(PieceRef(player, 1)), pos0);
	EXPECT_EQ(game.board_state.ref_to_pos(PieceRef(player, 2)), pos1);
	EXPECT_EQ(game.board_state.ref_to_pos(PieceRef(player, 3)), pos2);

	game.board_state.get_piece(pos3)->blocking = true;

	EXPECT_EQ(game.board_state.ref_to_pos(PieceRef(player, 0)), pos0);
	EXPECT_EQ(game.board_state.ref_to_pos(PieceRef(player, 1)), pos1);
	EXPECT_EQ(game.board_state.ref_to_pos(PieceRef(player, 2)), pos2);
	EXPECT_EQ(game.board_state.ref_to_pos(PieceRef(player, 3)), pos3);
}

TEST(CardTest, MovePiece) {
	DogGame game(false, false);

	TEST_MOVE_FROM_TO("|||", 0, "A#", "P0*|||");
	TEST_MOVE_FROM_TO("P0*|||", 0, "20", "P2|||");
}

TEST(CardTest, StartInvalid) {
	DogGame game(false, false);

	TEST_MOVE_FROM_TO("P1P2P3|||", 0, "A#", "P0*P1P2P3|||");

	TEST_INVALID_MOVE_FROM("P0P2P3P4|||", 0, "A#");
	TEST_INVALID_MOVE_FROM("P1P2P3P4|||", 0, "A#");
	TEST_INVALID_MOVE_FROM("P0*P2P3|||", 0, "A#");
}

TEST(CardTest, Finish) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "K#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(12), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(24), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(36), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(48), nullptr);

	EXPECT_TRUE(game.play_notation(0, "Q0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(60), nullptr);

	EXPECT_TRUE(game.play_notation(0, "50"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.finishes.at(0).at(0), nullptr);

	EXPECT_TRUE(game.play_notation(0, "30"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.finishes.at(0).at(3), nullptr);
}

TEST(CardTest, BackwardStartFinish) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	EXPECT_TRUE(game.play_notation(0, "4'0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.path.at(60), nullptr);

	EXPECT_TRUE(game.play_notation(0, "80"));
	EXPECT_TRUE(check_state(game));
	EXPECT_NE(game.board_state.finishes.at(0).at(3), nullptr);
}

TEST(CardTest, NoFinishFromStart) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(0, "40"));
	EXPECT_TRUE(check_state(game));

	EXPECT_NE(game.board_state.path.at(4), nullptr);
}

TEST(CardTest, SendToKennel) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(1, "A#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(0, "Q0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(1, "A'0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(0, "50"));
	EXPECT_TRUE(check_state(game));

	EXPECT_PLAYER_AT(17, 0);
	EXPECT_NE(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);

	EXPECT_TRUE(game.play_notation(1, "K#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(0, "30"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(1, "4'0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(0, "20"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(1, "T0"));
	EXPECT_TRUE(check_state(game));

	EXPECT_PLAYER_AT(22, 1);
	EXPECT_NE(game.board_state.kennels.at(0).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(3), nullptr);

	EXPECT_TRUE(game.play_notation(0, "XA#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(1, "K#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(0, "XA0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(1, "4'1"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(0, "X50"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(1, "T0-"));
	EXPECT_TRUE(check_state(game));

	EXPECT_PLAYER_AT(16, 0);
	EXPECT_PLAYER_AT(22, 1);
	EXPECT_EQ(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);

	EXPECT_TRUE(game.play_notation(0, "K#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(1, "K#"));
	EXPECT_TRUE(check_state(game));

	EXPECT_PLAYER_AT(16, 1);
	EXPECT_EQ(game.board_state.kennels.at(0).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(3), nullptr);
}

TEST(CardTest, Swap) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_TRUE(game.play_notation(1, "A#"));
	EXPECT_TRUE(check_state(game));
	EXPECT_EQ(game.board_state.path.at(0)->blocking, true);
	EXPECT_EQ(game.board_state.path.at(16)->blocking, true);

	EXPECT_FALSE(game.play_notation(0, "J010"));

	EXPECT_TRUE(game.play_notation(0, "A'0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_EQ(game.board_state.path.at(1)->blocking, false);
	EXPECT_EQ(game.board_state.path.at(16)->blocking, true);

	EXPECT_FALSE(game.play_notation(0, "J010"));

	EXPECT_TRUE(game.play_notation(1, "A'0"));
	EXPECT_TRUE(check_state(game));
	EXPECT_EQ(game.board_state.path.at(1)->blocking, false);
	EXPECT_EQ(game.board_state.path.at(17)->blocking, false);

	EXPECT_PLAYER_AT(1, 0);
	EXPECT_PLAYER_AT(17, 1);

	EXPECT_TRUE(game.play_notation(0, "J010"));
	EXPECT_TRUE(check_state(game));

	EXPECT_PLAYER_AT(1, 1);
	EXPECT_PLAYER_AT(17, 0);
}

TEST(CardTest, Seven) {
	TEST_MOVE_FROM_TO("P12|P17||", 0, "707", "P19|||");

	TEST_MOVE_FROM_TO("P63P14|P16*||", 0, "71106", "P5P15|P16*||");
	TEST_MOVE_FROM_TO("P63P14|P16*||", 0, "71106-", "P5P15|P16*||");

	TEST_MOVE_FROM_TO("P63P13|P16*||", 0, "71205", "P15F3|P16*||");

	TEST_INVALID_MOVE_FROM("P0*P63|||", 0, "70116");
	TEST_INVALID_MOVE_FROM("P0*P63|||", 0, "70611");

	TEST_INVALID_MOVE_FROM("P4P6|||", 0, "71304");

	TEST_INVALID_MOVE_FROM("P15P63|P16*||", 0, "71304");
	TEST_INVALID_MOVE_FROM("P15P63|P16*||", 0, "71106");
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

TEST(CardTest, MoveInFinish) {
	DogGame game(false, false);

	int player = 3;
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 0)), BoardPosition(Finish, player, 2));
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 1)), BoardPosition(Finish, player, 1));
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 2)), BoardPosition(Finish, player, 0));

	EXPECT_FALSE(game.play_notation(player, "A'1"));
	EXPECT_FALSE(game.play_notation(player, "A'2"));
	EXPECT_FALSE(game.play_notation(player, "A'3"));
	EXPECT_TRUE(game.play_notation(player, "A'0"));

	EXPECT_PLAYER_AT_FINISH(3, player);
	EXPECT_PLAYER_AT_FINISH(1, player);
	EXPECT_PLAYER_AT_FINISH(0, player);

	EXPECT_FALSE(game.play_notation(player, "A'0"));
	EXPECT_FALSE(game.play_notation(player, "A'2"));
	EXPECT_FALSE(game.play_notation(player, "A'3"));
	EXPECT_TRUE(game.play_notation(player, "A'1"));

	EXPECT_PLAYER_AT_FINISH(3, player);
	EXPECT_PLAYER_AT_FINISH(2, player);
	EXPECT_PLAYER_AT_FINISH(0, player);

	EXPECT_FALSE(game.play_notation(player, "A'0"));
	EXPECT_FALSE(game.play_notation(player, "A'1"));
	EXPECT_FALSE(game.play_notation(player, "A'3"));
	EXPECT_TRUE(game.play_notation(player, "A'2"));

	EXPECT_PLAYER_AT_FINISH(3, player);
	EXPECT_PLAYER_AT_FINISH(2, player);
	EXPECT_PLAYER_AT_FINISH(1, player);

	EXPECT_FALSE(game.play_notation(player, "A'0"));
	EXPECT_FALSE(game.play_notation(player, "A'1"));
	EXPECT_FALSE(game.play_notation(player, "A'2"));
	EXPECT_TRUE(game.play_notation(player, "A#"));

	EXPECT_PLAYER_AT(48, player);

	EXPECT_FALSE(game.play_notation(player, "A'0"));
	EXPECT_FALSE(game.play_notation(player, "A'1"));
	EXPECT_FALSE(game.play_notation(player, "A'2"));
	EXPECT_TRUE(game.play_notation(player, "A'3"));

	EXPECT_PLAYER_AT(49, player);
}

TEST(PossibleAction, MoveInFinish) {
	DogGame game(false, false);
	std::vector<ActionVar> actions;

	int player = 3;

	actions = game.possible_actions_for_card(player, Ace, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A#")));

	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 0)), BoardPosition(Finish, player, 2));
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 1)), BoardPosition(Finish, player, 1));
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, player, 2)), BoardPosition(Finish, player, 0));

	actions = game.possible_actions_for_card(player, Ace, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A#")));
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A'0")));

	EXPECT_TRUE(game.play_notation(player, "A'0"));

	actions = game.possible_actions_for_card(player, Ace, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A#")));
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A'1")));

	EXPECT_TRUE(game.play_notation(player, "A'1"));

	actions = game.possible_actions_for_card(player, Ace, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A#")));
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A'2")));

	EXPECT_TRUE(game.play_notation(player, "A#"));

	actions = game.possible_actions_for_card(player, Ace, false);
	EXPECT_EQ(actions.size(), 3);
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A'2")));

	EXPECT_TRUE(game.play_notation(player, "A'2"));

	actions = game.possible_actions_for_card(player, Ace, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(player, "A3")));
}

TEST(PossibleAction, AvoidFinish) {
	GTEST_SKIP();
}

TEST(PossibleAction, Swap) {
	DogGame game(false, false);
	std::vector<ActionVar> actions;

	actions = game.possible_actions_for_card(0, Jack, false);
	EXPECT_EQ(actions.size(), 0);

	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, 0, 0)), BoardPosition(1));
	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, 1, 0)), BoardPosition(2));

	actions = game.possible_actions_for_card(0, Jack, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J010")));

	actions = game.possible_actions_for_card(1, Jack, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J000")));

	EXPECT_TRUE(game.play_notation(3, "A#"));

	actions = game.possible_actions_for_card(0, Jack, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J010")));

	actions = game.possible_actions_for_card(1, Jack, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J000")));

	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, 1, 1)), BoardPosition(Finish, 1, 0));

	actions = game.possible_actions_for_card(0, Jack, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J011")));

	actions = game.possible_actions_for_card(1, Jack, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J100")));

	EXPECT_TRUE(game.play_notation(2, "A#"));

	actions = game.possible_actions_for_card(0, Jack, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J011")));

	actions = game.possible_actions_for_card(1, Jack, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J100")));

	EXPECT_TRUE(game.play_notation(2, "A0"));

	actions = game.possible_actions_for_card(0, Jack, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J011")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J020")));

	actions = game.possible_actions_for_card(1, Jack, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J100")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J120")));

	actions = game.possible_actions_for_card(2, Jack, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(2, "J000")));
	EXPECT_THAT(actions, testing::Contains(from_notation(2, "J011")));

	EXPECT_TRUE(game.play_notation(0, "A#"));

	actions = game.possible_actions_for_card(0, Jack, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J011")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J020")));

	actions = game.possible_actions_for_card(1, Jack, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J100")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J120")));

	actions = game.possible_actions_for_card(2, Jack, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(2, "J000")));
	EXPECT_THAT(actions, testing::Contains(from_notation(2, "J011")));

	EXPECT_TRUE(game.play_notation(0, "4'1"));

	actions = game.possible_actions_for_card(0, Jack, false);
	EXPECT_EQ(actions.size(), 4);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J011")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J020")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J111")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "J120")));

	actions = game.possible_actions_for_card(1, Jack, false);
	EXPECT_EQ(actions.size(), 3);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J100")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J120")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "J101")));

	actions = game.possible_actions_for_card(2, Jack, false);
	EXPECT_EQ(actions.size(), 3);
	EXPECT_THAT(actions, testing::Contains(from_notation(2, "J000")));
	EXPECT_THAT(actions, testing::Contains(from_notation(2, "J011")));
	EXPECT_THAT(actions, testing::Contains(from_notation(2, "J001")));
}

TEST(PossibleAction, SevenSimple) {
	GTEST_SKIP();

	DogGame game(false, false);
	std::vector<ActionVar> actions;

	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 0);

	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, 0, 0)), BoardPosition(17));

	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 1);

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 07")));

	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, 0, 1)), BoardPosition(1));

	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 8);

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 07")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 06 11")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 05 12")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 04 13")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 03 14")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 02 15")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 16")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 17")));

	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, 2, 0)), BoardPosition(33));

	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 36);

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 0'7")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 11 0'6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 12 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 13 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 14 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 15 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 16 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 17")));

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 0'6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 11 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 12 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 13 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 14 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 15 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 16")));

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 02 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 02 11 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 02 12 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 02 13 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 02 14 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 02 15")));

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 03 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 03 11 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 03 12 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 03 13 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 03 14")));

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 04 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 04 11 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 04 12 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 04 13")));

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 05 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 05 11 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 05 12")));

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 06 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 06 11")));

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 07")));
}

TEST(PossibleAction, SevenBlockades) {
	GTEST_SKIP();

	DogGame game(false, false);
	std::vector<ActionVar> actions;

	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, 0, 0)), BoardPosition(12));
	game.board_state.start_piece(1);

	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 0);

	game.board_state.start_piece(2);

	PRINT_DBG(game);
	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 4);

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 0'7")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 0'6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 02 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 03 0'4")));

	game.reset();

	game.board_state.move_piece(game.board_state.get_piece(BoardPosition(Kennel, 0, 0)), BoardPosition(28));
	game.board_state.start_piece(6);

	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 0'7")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 01 0'6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 02 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 03 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 04 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 0'2 05")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "7 0'1 06")));
}

TEST(FullGameTest, WinCondition) {
	DogGame game(false, false);

	EXPECT_EQ(game.result(), -1);

	game.load_board("F0F1F2F3|||");
	EXPECT_EQ(game.result(), -1);

	game.load_board("F0F1F2F3|F0F1F2F3||");
	EXPECT_EQ(game.result(), -1);

	game.load_board("F0F1F2F3|F0F1F2F3|F1F2F3|");
	EXPECT_EQ(game.result(), -1);

	game.load_board("F0F1F2F3|F0F1F2F3|F1F2F3|F1F2F3");
	EXPECT_EQ(game.result(), -1);

	game.load_board("F0F1F2F3|F0F1F2F3|F0F1F2F3|F1F2F3");
	EXPECT_EQ(game.result(), 0);

	game.load_board("F0F1F2F3|F0F1F2F3|F1F2F3|F0F1F2F3");
	EXPECT_EQ(game.result(), 1);

	game.load_board("F0F1F2F3|F0F1F2F3|F0F1F2F3|F0F1F2F3");
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
