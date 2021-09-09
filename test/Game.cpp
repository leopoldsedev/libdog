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
	DogGame game(false, false, false); \
	game.load_board(notation_start); \
	EXPECT_TRUE(game.play_notation(player_id, notation_move)); \
	EXPECT_TRUE(check_state(game)); \
	EXPECT_EQ(to_notation(game.board_state), notation_end); \
} while(0)

#define TEST_INVALID_MOVE_FROM(notation_start, player_id, notation_move) do { \
	DogGame game(false, false, false); \
	game.load_board(notation_start); \
	EXPECT_FALSE(game.play_notation(player_id, notation_move)); \
	EXPECT_TRUE(check_state(game)); \
	EXPECT_EQ(to_notation(game.board_state), notation_start); \
} while(0)

#define TEST_POSSIBLE_ACTIONS(player_id, game_var_name, actions_var_name, state_results_expected_var_name) do { \
	for (ActionVar action : actions_var_name) { \
		DogGame game_copy = game_var_name; \
		bool legal = game_copy.play(player_id, action, true, false); \
 \
		EXPECT_TRUE(legal); \
		EXPECT_THAT(state_results_expected_var_name, testing::Contains(game_copy.board_state)); \
	} \
} while(0)


// TODO Check that kennel order is kept
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
	DogGame game(false, false, false);

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
	DogGame game(false, false, false);

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
	DogGame game(false, false, false);

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
	DogGame game(false, false, false);

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

TEST(PrimitiveTest, Authorization) {
	DogGame game(false, false, false);

	EXPECT_TRUE(game.play(0, Start(Ace)));
	EXPECT_TRUE(game.play(1, Start(Ace)));

	EXPECT_FALSE(game.play(0, Move(Ace, PieceRef(1, 0), 1, false)));
	// TODO Check this for all other actions (that player is not allowed to move all pieces
}

TEST(CardTest, MovePiece) {
	TEST_MOVE_FROM_TO("|||", 0, "A#", "P0*|||");
	TEST_MOVE_FROM_TO("P0*|||", 0, "20", "P2|||");
}

TEST(CardTest, Start) {
	TEST_MOVE_FROM_TO("|||", 0, "A#", "P0*|||");
	TEST_MOVE_FROM_TO("P0*|||", 1, "K#", "P0*|P16*||");
	TEST_MOVE_FROM_TO("P0*|P16*||", 2, "XA#", "P0*|P16*|P32*|");
	TEST_MOVE_FROM_TO("P0*|P16*|P32*|", 3, "XK#", "P0*|P16*|P32*|P48*");
}

TEST(CardTest, StartInvalid) {
	DogGame game(false, false, false);

	TEST_MOVE_FROM_TO("P1P2P3|||", 0, "A#", "P0*P1P2P3|||");

	TEST_INVALID_MOVE_FROM("P0P2P3P4|||", 0, "A#");
	TEST_INVALID_MOVE_FROM("P1P2P3P4|||", 0, "A#");
	TEST_INVALID_MOVE_FROM("P0*P2P3|||", 0, "A#");
}

TEST(CardTest, Finish) {
	TEST_MOVE_FROM_TO("|||", 0, "K#", "P0*|||");
	TEST_MOVE_FROM_TO("P0*|||", 0, "Q0", "P12|||");
	TEST_MOVE_FROM_TO("P12|||", 0, "Q0", "P24|||");
	TEST_MOVE_FROM_TO("P24|||", 0, "Q0", "P36|||");
	TEST_MOVE_FROM_TO("P36|||", 0, "Q0", "P48|||");
	TEST_MOVE_FROM_TO("P48|||", 0, "Q0", "P60|||");
	TEST_MOVE_FROM_TO("P60|||", 0, "50", "F0|||");
	TEST_MOVE_FROM_TO("P60|||", 0, "90", "P5|||");
	TEST_MOVE_FROM_TO("F0|||", 0, "30", "F3|||");

	TEST_INVALID_MOVE_FROM("F0|||", 0, "40");
	TEST_INVALID_MOVE_FROM("F0|||", 0, "4'0");
	TEST_INVALID_MOVE_FROM("F3|||", 0, "4'0");

	TEST_MOVE_FROM_TO("P2|||", 0, "4'0", "P62|||");
}

TEST(CardTest, BackwardStartFinish) {
	TEST_MOVE_FROM_TO("|||", 0, "A#", "P0*|||");
	TEST_MOVE_FROM_TO("P0*|||", 0, "4'0", "P60|||");
	TEST_MOVE_FROM_TO("P60|||", 0, "80", "F3|||");
}

TEST(CardTest, NoFinishFromStart) {
	TEST_MOVE_FROM_TO("P0*|||", 0, "40", "P4|||");
}

TEST(CardTest, SendToKennel) {
	TEST_MOVE_FROM_TO("P12|P17||", 0, "50", "P17|||");
	TEST_MOVE_FROM_TO("P22|P12||", 1, "T0", "|P22||");
	TEST_MOVE_FROM_TO("P16|P12P22||", 1, "T0-", "P16|P22||");
	TEST_MOVE_FROM_TO("P16|P22||", 1, "K#", "|P16*P22||");

	TEST_MOVE_FROM_TO("P16|P22||", 0, "707", "P23|||");
	TEST_MOVE_FROM_TO("P15|P22||", 0, "707", "P22|||");
	TEST_MOVE_FROM_TO("P5P21P37P53|P6P22P38P54||", 0, "702122231", "P6P23P39P55|||");
	TEST_MOVE_FROM_TO("P5P21P37P53|P7P23P39P55||", 0, "702122231", "P6P23P39P55|P7||");
}

TEST(CardTest, Swap) {
	TEST_INVALID_MOVE_FROM("P0*|P16*||", 0, "J010");
	TEST_INVALID_MOVE_FROM("P0|P16*||", 0, "J010");
	TEST_INVALID_MOVE_FROM("P0*|P16||", 0, "J010");

	TEST_MOVE_FROM_TO("P1|P17||", 0, "J010", "P17|P1||");
	TEST_MOVE_FROM_TO("P17|P1||", 1, "J000", "P1|P17||");
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

	TEST_INVALID_MOVE_FROM("P12F3|P15P16*|P14|P46P58", 0, "707");
	TEST_INVALID_MOVE_FROM("P12F3|P15P16*|P14|P46P58", 0, "7061'1");
	TEST_INVALID_MOVE_FROM("P12F3|P15P16*|P14|P46P58", 0, "7051'2");
	TEST_INVALID_MOVE_FROM("P12F3|P15P16*|P14|P46P58", 0, "7041'3");
	TEST_INVALID_MOVE_FROM("P12F3|P15P16*|P14|P46P58", 0, "7031'4");
	TEST_INVALID_MOVE_FROM("P12F3|P15P16*|P14|P46P58", 0, "7021'5");
	TEST_INVALID_MOVE_FROM("P12F3|P15P16*|P14|P46P58", 0, "7011'6");
	TEST_INVALID_MOVE_FROM("P12F3|P15P16*|P14|P46P58", 0, "71'7");

	TEST_INVALID_MOVE_FROM("P32P35|||", 0, "71304");

	TEST_MOVE_FROM_TO("P0*||P33|", 0, "7030'4", "P3||P37|");
	if (!RULE_ALLOW_SEVEN_MOVE_TEAMMATE_IF_BLOCKED) {
		TEST_INVALID_MOVE_FROM("P0*||P32*|", 0, "7030'4");
		TEST_INVALID_MOVE_FROM("P0||P32*|", 0, "7030'4");
	} else {
		TEST_MOVE_FROM_TO("P0*||P32*|", 0, "7030'4", "P3||P36|");
		TEST_MOVE_FROM_TO("P0||P32*|", 0, "7030'4", "F2||P36|");
	}
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

	DogGame game(false, false, false);

	int player = 0;

	TEST_MOVE_FROM_TO("P60|||", player, "80", "F3|||");
	TEST_MOVE_FROM_TO("P60|||", player, "80-", "P4|||");
	TEST_MOVE_FROM_TO("P60|||", player, "30", "P63|||");
	TEST_MOVE_FROM_TO("P60|||", player, "30-", "P63|||");

	TEST_INVALID_MOVE_FROM("P0*P60|||", player, "T0");
	TEST_INVALID_MOVE_FROM("P0*P60|||", player, "T0-");
	TEST_INVALID_MOVE_FROM("P0*P60|||", player, "80");
	TEST_INVALID_MOVE_FROM("P0*P60|||", player, "80-");

	TEST_MOVE_FROM_TO("P60F1|||", player, "81", "P4F1|||");
	TEST_MOVE_FROM_TO("P60F1|||", player, "81-", "P4F1|||");
	TEST_MOVE_FROM_TO("P60F1|||", player, "51", "F0F1|||");
	TEST_MOVE_FROM_TO("P60F1|||", player, "51-", "P1F1|||");
}

TEST(CardTest, MoveInFinish) {
	DogGame game(false, false, false);

	int player = 3;

	TEST_INVALID_MOVE_FROM("|||F0F1F2", player, "A'1");
	TEST_INVALID_MOVE_FROM("|||F0F1F2", player, "A'2");
	TEST_INVALID_MOVE_FROM("|||F0F1F2", player, "A'3");
	TEST_MOVE_FROM_TO("|||F0F1F2", player, "A'0", "|||F0F1F3");

	TEST_INVALID_MOVE_FROM("|||F0F1F3", player, "A'0");
	TEST_INVALID_MOVE_FROM("|||F0F1F3", player, "A'2");
	TEST_INVALID_MOVE_FROM("|||F0F1F3", player, "A'3");
	TEST_MOVE_FROM_TO("|||F0F1F3", player, "A'1", "|||F0F2F3");

	TEST_INVALID_MOVE_FROM("|||F0F2F3", player, "A'0");
	TEST_INVALID_MOVE_FROM("|||F0F2F3", player, "A'1");
	TEST_INVALID_MOVE_FROM("|||F0F2F3", player, "A'3");
	TEST_MOVE_FROM_TO("|||F0F2F3", player, "A'2", "|||F1F2F3");

	TEST_INVALID_MOVE_FROM("|||F1F2F3", player, "A'0");
	TEST_INVALID_MOVE_FROM("|||F1F2F3", player, "A'1");
	TEST_INVALID_MOVE_FROM("|||F1F2F3", player, "A'2");
	TEST_INVALID_MOVE_FROM("|||F1F2F3", player, "A'3");
	TEST_MOVE_FROM_TO("|||F1F2F3", player, "A#", "|||P48*F1F2F3");

	TEST_INVALID_MOVE_FROM("|||P48*F1F2F3", player, "A'0");
	TEST_INVALID_MOVE_FROM("|||P48*F1F2F3", player, "A'1");
	TEST_INVALID_MOVE_FROM("|||P48*F1F2F3", player, "A'2");
	TEST_MOVE_FROM_TO("|||P48*F1F2F3", player, "A'3", "|||P49F1F2F3");
}

TEST(CardTest, PlayForTeamMate) {
	TEST_MOVE_FROM_TO("F0F1F2F3||P0|", 0, "T0", "F0F1F2F3||P10|");
	TEST_MOVE_FROM_TO("F0F1F2F3||P0|", 0, "K#", "F0F1F2F3||P0P32*|");

	TEST_INVALID_MOVE_FROM("F0F1F2F3||P0P32*|", 0, "A#");
	TEST_INVALID_MOVE_FROM("F0F1F2F3||P0P33P34P35|", 0, "A#");

	TEST_INVALID_MOVE_FROM("F0F1F2F3||F0F1F2F3|", 0, "20");
	TEST_INVALID_MOVE_FROM("F0F1F2F3||F0F1F2F3|", 0, "K#");

	TEST_MOVE_FROM_TO("F0F1F2F3|P16|P0|P32", 0, "J010", "F0F1F2F3|P0|P16|P32");
	TEST_INVALID_MOVE_FROM("F0F1F2F3|P16|P0|P32", 0, "J130");

	TEST_MOVE_FROM_TO("F0F1F2F3||P0|", 0, "707", "F0F1F2F3||P7|");

	TEST_MOVE_FROM_TO("F1F2F3|F0F1F2F3|P13P32*F2F3|P37P46P48*F3", 1, "727", "F1F2F3|F0F1F2F3|P13P32*F2F3|P44P46P48*F3");
	TEST_INVALID_MOVE_FROM("F1F2F3|F0F1F2F3|P13P32*F2F3|P37P46P48*F3", 1, "72'7");


	TEST_INVALID_MOVE_FROM("F0F1F2F3|P30F1F2F3|P26P32*P39F3|P62F3", 0, "X72'12'12'12'12'12'12'1");

	DogGame game(false, false, false);
	std::string notation_str = "F0F1F2F3|P30F1F2F3|P26P32*P39F3|P62F3";
	game.load_board(notation_str);
	ActionVar action = from_notation(2, "X72'12'12'12'12'12'12'1");
	EXPECT_FALSE(game.play(0, action));
	EXPECT_TRUE(check_state(game));
	EXPECT_EQ(to_notation(game.board_state), notation_str);

	// TODO Add checks for possible actions
}

TEST(PossibleAction, Move) {
	DogGame game(false, false, false);
	std::vector<ActionVar> actions;

	game.load_board("P60|P17|P34|P50");

	actions = game.possible_actions_for_card(0, Four, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "40")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "4'0")));

	actions = game.possible_actions_for_card(0, Five, false);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "50")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "50-")));

	actions = game.possible_actions_for_card(0, Nine, false);
	EXPECT_EQ(actions.size(), 1);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "90")));
}

TEST(PossibleAction, MoveInFinish) {
	DogGame game(false, false, false);
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
	DogGame game(false, false, false);
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

	DogGame game(false, false, false);
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


	game.load_board("F0F1F2F3|P18F1F2F3|P37F1F2F3|P48P60F2F3");
	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 34);

	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 0'7")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 11 0'6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 12 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 13 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 14 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 15 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 16 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 17")));

	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01 0'6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01 11 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01 12 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01 13 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01 14 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01 15 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01 16")));

	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01- 0'6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01- 11 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01- 12 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01- 13 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01- 14 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01- 15 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 01- 16")));

	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02 11 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02 12 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02 13 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02 14 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02 15")));

	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02- 0'5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02- 11 0'4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02- 12 0'3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02- 13 0'2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02- 14 0'1")));
	EXPECT_THAT(actions, testing::Contains(from_notation(3, "7 02- 15")));
}

TEST(PossibleAction, SevenBlockades) {
	DogGame game(false, false, false);
	std::vector<ActionVar> actions;


	// 0    0 0 . .    . . . . .               3
	//      3 2 1 0    . \     .
	//                 .  . 0  3
	//                 .  . 1  .               3
	//                 .  . 2  .               3
	//               .    0 3    .             .
	//             .               .           .
	//           .                   .
	// 0 . . . .                       . . . . .
	// .                                     / .
	// 2  . . . .                     . . . .  3
	// 1 /                                     .
	// 1 . . . .                       . . . . .
	// *         .                   .
	// .           .               .
	// .             .      .    .
	// 1               .    .  .
	// 1               .    .  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . .    . 2 2 2    2
	game.load_board("P12F3|P15P16*|P14|P46P58");
	actions = game.possible_actions_for_card(2, Seven, false);
	EXPECT_EQ(actions.size(), 0);

	std::vector<BoardState> state_results_expected;


	// 0    0 0 . .    . . . . .               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  . 1  .               3
	//                 .  . 2  3               .
	//               .    . 3    .             .
	//             .               .           .
	//           .                   .
	// 0 . . . .                       . . . . .
	// .                                     / .
	// .  . . 1 1                     3 . . .  0
	// 3 /                                     .
	// 1 . . . .                       . . . 1 .
	// *         .                   .
	// .           .               .
	// .             .      .    .
	// .               .    .  .
	// .               .    .  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . .    2 2 2 2    2
	game.load_board("P12P46|P16*P43F2F3||P15P56F3");
	actions = game.possible_actions_for_card(2, Seven, false);
	EXPECT_EQ(actions.size(), 4);

	state_results_expected = {
		from_notation("P12P53|P16*P43F2F3||P15P56F3"),
		from_notation("P13P52|P16*P43F2F3||P15P56F3"),
		from_notation("P14P51|P16*P43F2F3||P15P56F3"),
		from_notation("P15P50|P16*P43F2F3||P56F3"),
	};

	TEST_POSSIBLE_ACTIONS(2, game, actions, state_results_expected);

	// 0    0 . . .    . . . . .               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  . 1  .               3
	//                 .  . 2  3               .
	//               .    0 3    .             .
	//             .               .           .
	//           .                   .
	// 0 . . . .                       . . . . .
	// .                                     / .
	// .  . . 1 1                     3 . . .  0
	// 3 /                                     .
	// 1 . . . .                       . . . 1 .
	// *         .                   .
	// .           .               .
	// .             .      .    .
	// .               .    .  .
	// .               .    .  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . .    2 2 2 2    2
	game.load_board("P12P46F3|P16*P43F2F3||P15P56F3");
	actions = game.possible_actions_for_card(2, Seven, false);
	EXPECT_EQ(actions.size(), 4);

	state_results_expected = {
		from_notation("P12P53F3|P16*P43F2F3||P15P56F3"),
		from_notation("P13P52F3|P16*P43F2F3||P15P56F3"),
		from_notation("P14P51F3|P16*P43F2F3||P15P56F3"),
		from_notation("P15P50F3|P16*P43F2F3||P56F3"),
	};

	TEST_POSSIBLE_ACTIONS(2, game, actions, state_results_expected);

	game.load_board("P46F2|P16*P43F2F3||P15P56F3");
	actions = game.possible_actions_for_card(2, Seven, false);
	EXPECT_EQ(actions.size(), 2);

	state_results_expected = {
		from_notation("P53F2|P16*P43F2F3||P15P56F3"),
		from_notation("P52F3|P16*P43F2F3||P15P56F3"),
	};

	TEST_POSSIBLE_ACTIONS(2, game, actions, state_results_expected);

	// 0    . . . .    . . . . .               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  0 1  .               3
	//                 .  . 2  3               .
	//               .    0 3    .             .
	//             .               .           .
	//           .                   .
	// 0 . . . .                       . . . . .
	// .                                     / .
	// .  . . 1 1                     3 . . .  0
	// 3 /                                     .
	// 1 . . . .                       . . . 1 .
	// *         .                   .
	// .           .               .
	// .             .      .    .
	// .               .    .  .
	// .               .    .  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . .    2 2 2 2    2
	game.load_board("P12P46F1F3|P16*P43F2F3||P15P56F3");

	actions = game.possible_actions_for_card(2, Seven, false);
	EXPECT_EQ(actions.size(), 8);

	state_results_expected = {
		from_notation("P12P53F1F3|P16*P43F2F3||P15P56F3"),

		from_notation("P12P52F2F3|P16*P43F2F3||P15P56F3"),
		from_notation("P13P52F1F3|P16*P43F2F3||P15P56F3"),

		from_notation("P13P51F2F3|P16*P43F2F3||P15P56F3"),
		from_notation("P14P51F1F3|P16*P43F2F3||P15P56F3"),

		from_notation("P14P50F2F3|P16*P43F2F3||P15P56F3"),
		from_notation("P15P50F1F3|P16*P43F2F3||P56F3"),

		from_notation("P15P49F2F3|P16*P43F2F3||P56F3"),
	};

	TEST_POSSIBLE_ACTIONS(2, game, actions, state_results_expected);

	// 0    0 0 0 .    . . . . .               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  . 1  .               3
	//                 .  . 2  .               3
	//               .    . 3    .             3
	//             .               .           3
	//           .                   .
	// 0 . . . .                       . . . . .
	// .                                     / .
	// .  . . . .                     . . . .  .
	// . /                                     .
	// 1 . . . .                       . . . . .
	// *         .                   .
	// .           .               .
	// 1             .      .    .
	// 1               .    .  .
	// 1               .    .  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . .    2 2 2 2    2
	game.load_board("P12|P16*||");

	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 0);

	// 0    0 0 0 .    . . . . .               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  . 1  .               3
	//                 .  . 2  .               3
	//               .    . 3    .             3
	//             .               .           3
	//           .                   .
	// 0 . . . .                       . . . . .
	// .                                     / .
	// .  . . . .                     . . . .  .
	// . /                                     .
	// 1 . . . .                       . . . . .
	// *         .                   .
	// .           .               .
	// 1             .      .    .
	// 1               .    .  .
	// 1               .    .  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . 2*   . 2 2 2    2
	game.load_board("P12|P16*|P32*|");

	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 4);

	state_results_expected = {
		from_notation("P12|P16*|P39|"),
		from_notation("P13|P16*|P38|"),
		from_notation("P14|P16*|P37|"),
		from_notation("P15|P16*|P36|"),
	};

	TEST_POSSIBLE_ACTIONS(0, game, actions, state_results_expected);


	// 0    0 0 0 .    . . . . .               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  . 1  .               3
	//                 .  . 2  .               3
	//               .    . 3    .             3
	//             .               .           3
	//           .                   .
	// . . . . .                       . . . . .
	// .                                     / .
	// .  . . . .                     . . . .  .
	// . /                                     .
	// . . . . .                       . . . . .
	//           .                   .
	// 1           .               .
	// 1             .      .    .
	// 1               .    .  .
	// 1               .    .  .
	//                 .    .  .
	//                 .     \ .
	// 1               0 . . . 2*   . 2 2 2    2
	game.load_board("P28||P32*|");

	actions = game.possible_actions_for_card(0, Seven, false);
	EXPECT_EQ(actions.size(), 7);

	state_results_expected = {
		from_notation("P28||P39|"),
		from_notation("P29||P38|"),
		from_notation("P30||P37|"),
		from_notation("P31||P36|"),
		from_notation("P32||P35|"),
		from_notation("P33||P34|"),
		from_notation("P34|||"),
	};

	TEST_POSSIBLE_ACTIONS(0, game, actions, state_results_expected);
}

TEST(FullGameTest, WinCondition) {
	DogGame game(false, false, false);

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
	DogGame game(true, true, true);
	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");

	// Assumed hand state:
	// 0: 95A454
	// 1: 968X2X
	// 2: 924KQ8
	// 3: K923KA

	EXPECT_FALSE(game.cards_state.check_player_has_card(0, Queen));
	EXPECT_FALSE(game.cards_state.check_player_has_card(1, King));
	EXPECT_FALSE(game.cards_state.check_player_has_card(2, Five));
	EXPECT_FALSE(game.cards_state.check_player_has_card(3, Joker));

	EXPECT_FALSE(game.play_notation(0, "A#"));
	EXPECT_FALSE(game.play_notation(0, "G2"));
	EXPECT_TRUE(game.play_notation(0, "G5"));

	EXPECT_FALSE(game.play_notation(1, "XA#"));
	EXPECT_FALSE(game.play_notation(1, "GT"));
	EXPECT_TRUE(game.play_notation(1, "GX"));

	EXPECT_FALSE(game.play_notation(2, "K#"));
	EXPECT_FALSE(game.play_notation(2, "GX"));
	EXPECT_TRUE(game.play_notation(2, "GQ"));

	EXPECT_FALSE(game.play_notation(3, "K#"));
	EXPECT_FALSE(game.play_notation(3, "GQ"));
	EXPECT_TRUE(game.play_notation(3, "GK"));

	EXPECT_TRUE(game.cards_state.check_player_has_card(0, Queen));
	EXPECT_TRUE(game.cards_state.check_player_has_card(1, King));
	EXPECT_TRUE(game.cards_state.check_player_has_card(2, Five));
	EXPECT_TRUE(game.cards_state.check_player_has_card(3, Joker));

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(game.play_notation(1, "XA#"));
	EXPECT_TRUE(game.play_notation(2, "K#"));
	EXPECT_TRUE(game.play_notation(3, "K#"));

	EXPECT_EQ(to_notation(game.board_state), "P0*|P16*|P32*|P48*");

	// Same test repeated, but from a game state where most players are already finished
	game = DogGame(true, true, true);
	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");

	// Assumed hand state:
	// 0: 95A454
	// 1: 968X2X
	// 2: 924KQ8
	// 3: K923KA

	game.load_board("F0F1F2F3|F0F1F2F3|F1F2F3|F1F2F3");

	EXPECT_FALSE(game.cards_state.check_player_has_card(0, Queen));
	EXPECT_FALSE(game.cards_state.check_player_has_card(1, King));
	EXPECT_FALSE(game.cards_state.check_player_has_card(2, Five));
	EXPECT_FALSE(game.cards_state.check_player_has_card(3, Joker));

	EXPECT_FALSE(game.play_notation(0, "A#"));
	EXPECT_FALSE(game.play_notation(0, "G2"));
	EXPECT_TRUE(game.play_notation(0, "G5"));

	EXPECT_FALSE(game.play_notation(1, "XA#"));
	EXPECT_FALSE(game.play_notation(1, "GT"));
	EXPECT_TRUE(game.play_notation(1, "GX"));

	EXPECT_FALSE(game.play_notation(2, "K#"));
	EXPECT_FALSE(game.play_notation(2, "GX"));
	EXPECT_TRUE(game.play_notation(2, "GQ"));

	EXPECT_FALSE(game.play_notation(3, "K#"));
	EXPECT_FALSE(game.play_notation(3, "GQ"));
	EXPECT_TRUE(game.play_notation(3, "GK"));

	EXPECT_TRUE(game.cards_state.check_player_has_card(0, Queen));
	EXPECT_TRUE(game.cards_state.check_player_has_card(1, King));
	EXPECT_TRUE(game.cards_state.check_player_has_card(2, Five));
	EXPECT_TRUE(game.cards_state.check_player_has_card(3, Joker));
}

TEST(FullGameTest, Give) {
	// TODO Test give for when some players are finished
	// TODO Test in give phase and out of give phase
}

TEST(FullGameTest, Discard) {
	// TODO Test discard for when some players are finished
}

TEST(FullGameTest, Turns) {
	DogGame game(true, true, false);
	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");

	// Assumed hand state:
	// 0: 95A454
	// 1: 968X2X
	// 2: 924KQ8
	// 3: K923KA

	EXPECT_EQ(game.player_turn, 0);
	EXPECT_FALSE(game.play_notation(1, "XK#"));
	EXPECT_FALSE(game.play_notation(2, "K#"));
	EXPECT_FALSE(game.play_notation(3, "K#"));
	EXPECT_TRUE(game.play_notation(0, "A#"));

	EXPECT_EQ(game.player_turn, 1);
	EXPECT_FALSE(game.play_notation(0, "90"));
	EXPECT_FALSE(game.play_notation(2, "K#"));
	EXPECT_FALSE(game.play_notation(3, "K#"));
	EXPECT_TRUE(game.play_notation(1, "XK#"));

	EXPECT_EQ(game.player_turn, 2);
	EXPECT_FALSE(game.play_notation(0, "90"));
	EXPECT_FALSE(game.play_notation(1, "90"));
	EXPECT_FALSE(game.play_notation(3, "K#"));
	EXPECT_TRUE(game.play_notation(2, "K#"));

	EXPECT_EQ(game.player_turn, 3);
	EXPECT_FALSE(game.play_notation(0, "90"));
	EXPECT_FALSE(game.play_notation(1, "90"));
	EXPECT_FALSE(game.play_notation(2, "90"));
	EXPECT_TRUE(game.play_notation(3, "K#"));

	// Assumed hand state:
	// 0: 95454
	// 1: 968X2
	// 2: 924Q8
	// 3: K923A

	EXPECT_EQ(game.player_turn, 0);
	EXPECT_FALSE(game.play_notation(3, "90"));
	EXPECT_FALSE(game.play_notation(1, "90"));
	EXPECT_FALSE(game.play_notation(2, "90"));
	EXPECT_TRUE(game.play_notation(0, "90"));

	EXPECT_TRUE(game.play_notation(1, "90"));
	EXPECT_TRUE(game.play_notation(2, "90"));
	EXPECT_TRUE(game.play_notation(3, "90"));

	// Assumed hand state:
	// 0: 5454
	// 1: 68X2
	// 2: 24Q8
	// 3: K23A

	EXPECT_TRUE(game.play_notation(0, "50"));
	EXPECT_TRUE(game.play_notation(1, "60"));
	EXPECT_TRUE(game.play_notation(2, "80"));
	EXPECT_TRUE(game.play_notation(3, "A0"));

	// Assumed hand state:
	// 0: 544
	// 1: 8X2
	// 2: 24Q
	// 3: K23

	EXPECT_TRUE(game.play_notation(0, "50"));
	EXPECT_TRUE(game.play_notation(1, "80"));
	EXPECT_TRUE(game.play_notation(2, "Q0"));
	EXPECT_TRUE(game.play_notation(3, "30"));

	// Assumed hand state:
	// 0: 44
	// 1: X2
	// 2: 24
	// 3: K2

	EXPECT_TRUE(game.play_notation(0, "40"));
	EXPECT_TRUE(game.play_notation(1, "20"));
	EXPECT_TRUE(game.play_notation(2, "20"));
	EXPECT_TRUE(game.play_notation(3, "20"));

	// Assumed hand state:
	// 0: 4
	// 1: X
	// 2: 4
	// 3: K

	EXPECT_TRUE(game.play_notation(0, "40"));
	EXPECT_TRUE(game.play_notation(1, "X40"));
	EXPECT_TRUE(game.play_notation(2, "40"));
	EXPECT_TRUE(game.play_notation(3, "K#"));

	// Assumed hand state:
	// 0: 62AJ6
	// 1: 6396X
	// 2: T8984
	// 3: 3J34T

	// One player is skipped after new cards are given out
	EXPECT_EQ(game.player_turn, 1);
	EXPECT_FALSE(game.play_notation(0, "A#"));
	EXPECT_FALSE(game.play_notation(2, "40"));
	EXPECT_FALSE(game.play_notation(3, "30"));
	EXPECT_TRUE(game.play_notation(1, "XK#"));
}

TEST(FullGameTest, HandCheck) {
	DogGame game(false, true, false);
	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");

	// Assumed hand state:
	// 0: 95A454
	// 1: 968X2X
	// 2: 924KQ8
	// 3: K923KA

	EXPECT_FALSE(game.play_notation(0, "K#"));
	EXPECT_FALSE(game.play_notation(0, "XA#"));
	EXPECT_FALSE(game.play_notation(0, "XK#"));
	EXPECT_TRUE(game.play_notation(0, "A#"));

	EXPECT_FALSE(game.play_notation(1, "K#"));
	EXPECT_FALSE(game.play_notation(1, "A#"));
	EXPECT_TRUE(game.play_notation(1, "XA#", false));
	EXPECT_TRUE(game.play_notation(1, "XK#"));

	EXPECT_FALSE(game.play_notation(2, "A#"));
	EXPECT_FALSE(game.play_notation(2, "XA#"));
	EXPECT_FALSE(game.play_notation(2, "XK#"));
	EXPECT_TRUE(game.play_notation(2, "K#"));

	EXPECT_FALSE(game.play_notation(3, "XA#"));
	EXPECT_FALSE(game.play_notation(3, "XK#"));
	EXPECT_TRUE(game.play_notation(3, "K#", false));
	EXPECT_TRUE(game.play_notation(3, "A#"));

	EXPECT_EQ(to_notation(game.board_state), "P0*|P16*|P32*|P48*");

	// Assumed hand state:
	// 0: 95454
	// 1: 968X2
	// 2: 924Q8
	// 3: K923K

	EXPECT_FALSE(game.play_notation(0, "A0"));
	EXPECT_FALSE(game.play_notation(0, "20"));
	EXPECT_FALSE(game.play_notation(0, "30"));
	EXPECT_FALSE(game.play_notation(0, "60"));
	EXPECT_FALSE(game.play_notation(0, "707"));
	EXPECT_FALSE(game.play_notation(0, "80"));
	EXPECT_FALSE(game.play_notation(0, "T0"));
	EXPECT_FALSE(game.play_notation(0, "Q0"));
	EXPECT_FALSE(game.play_notation(0, "K0"));
	EXPECT_FALSE(game.play_notation(0, "X40"));
	EXPECT_TRUE(game.play_notation(0, "40", false));
	EXPECT_TRUE(game.play_notation(0, "50", false));
	EXPECT_TRUE(game.play_notation(0, "90", false));

	EXPECT_EQ(to_notation(game.board_state), "P0*|P16*|P32*|P48*");

	EXPECT_FALSE(game.play_notation(1, "A0"));
	EXPECT_FALSE(game.play_notation(1, "30"));
	EXPECT_FALSE(game.play_notation(1, "40"));
	EXPECT_FALSE(game.play_notation(1, "50"));
	EXPECT_FALSE(game.play_notation(1, "707"));
	EXPECT_FALSE(game.play_notation(1, "T0"));
	EXPECT_FALSE(game.play_notation(1, "Q0"));
	EXPECT_FALSE(game.play_notation(1, "K0"));
	EXPECT_TRUE(game.play_notation(1, "20", false));
	EXPECT_TRUE(game.play_notation(1, "60", false));
	EXPECT_TRUE(game.play_notation(1, "80", false));
	EXPECT_TRUE(game.play_notation(1, "90", false));
	EXPECT_TRUE(game.play_notation(1, "XA0", false));

	EXPECT_EQ(to_notation(game.board_state), "P0*|P16*|P32*|P48*");

	EXPECT_FALSE(game.play_notation(2, "A0"));
	EXPECT_FALSE(game.play_notation(2, "30"));
	EXPECT_FALSE(game.play_notation(2, "50"));
	EXPECT_FALSE(game.play_notation(2, "60"));
	EXPECT_FALSE(game.play_notation(2, "707"));
	EXPECT_FALSE(game.play_notation(2, "T0"));
	EXPECT_FALSE(game.play_notation(2, "K0"));
	EXPECT_FALSE(game.play_notation(2, "X20"));
	EXPECT_TRUE(game.play_notation(2, "20", false));
	EXPECT_TRUE(game.play_notation(2, "40", false));
	EXPECT_TRUE(game.play_notation(2, "80", false));
	EXPECT_TRUE(game.play_notation(2, "90", false));
	EXPECT_TRUE(game.play_notation(2, "Q0", false));

	EXPECT_EQ(to_notation(game.board_state), "P0*|P16*|P32*|P48*");

	EXPECT_FALSE(game.play_notation(3, "A0"));
	EXPECT_FALSE(game.play_notation(3, "40"));
	EXPECT_FALSE(game.play_notation(3, "50"));
	EXPECT_FALSE(game.play_notation(3, "60"));
	EXPECT_FALSE(game.play_notation(3, "707"));
	EXPECT_FALSE(game.play_notation(3, "80"));
	EXPECT_FALSE(game.play_notation(3, "T0"));
	EXPECT_FALSE(game.play_notation(3, "Q0"));
	EXPECT_FALSE(game.play_notation(3, "X20"));
	EXPECT_TRUE(game.play_notation(3, "20", false));
	EXPECT_TRUE(game.play_notation(3, "30", false));
	EXPECT_TRUE(game.play_notation(3, "90", false));
	EXPECT_TRUE(game.play_notation(3, "K0", false));

	EXPECT_EQ(to_notation(game.board_state), "P0*|P16*|P32*|P48*");
}

TEST(FullGameTest, One) {
	DogGame game(true, true, false);
	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");

	EXPECT_TRUE(game.play_notation(0, "G4"));
	EXPECT_TRUE(game.play_notation(1, "GX"));
	EXPECT_TRUE(game.play_notation(2, "G2"));
	EXPECT_TRUE(game.play_notation(3, "GK"));

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(game.play_notation(1, "K#"));
	EXPECT_TRUE(game.play_notation(2, "K#"));
	EXPECT_TRUE(game.play_notation(3, "A#"));

	EXPECT_TRUE(game.play_notation(0, "4'0"));
	EXPECT_TRUE(game.play_notation(1, "X4'0"));
	EXPECT_TRUE(game.play_notation(2, "4'0"));
	EXPECT_TRUE(game.play_notation(3, "X4'0"));

	EXPECT_TRUE(game.play_notation(0, "50"));
	EXPECT_TRUE(game.play_notation(1, "80"));
	EXPECT_TRUE(game.play_notation(2, "80"));
	EXPECT_TRUE(game.play_notation(3, "30"));

	EXPECT_TRUE(game.play_notation(0, "20"));
	EXPECT_TRUE(game.play_notation(1, "D2"));
	EXPECT_TRUE(game.play_notation(2, "D9"));
	EXPECT_TRUE(game.play_notation(3, "20"));

	EXPECT_TRUE(game.play_notation(0, "D5"));
	EXPECT_TRUE(game.play_notation(1, "D6"));
	EXPECT_TRUE(game.play_notation(2, "DQ"));
	EXPECT_FALSE(game.play_notation(3, "D9"));
	EXPECT_TRUE(game.play_notation(3, "K#"));

	EXPECT_TRUE(game.play_notation(0, "D9"));
	EXPECT_TRUE(game.play_notation(1, "D9"));
	EXPECT_TRUE(game.play_notation(2, "D4"));
	EXPECT_TRUE(game.play_notation(3, "91"));

	EXPECT_TRUE(game.play_notation(1, "G3"));
	EXPECT_TRUE(game.play_notation(2, "G4"));
	EXPECT_TRUE(game.play_notation(3, "G4"));
	EXPECT_TRUE(game.play_notation(0, "G6"));

	EXPECT_TRUE(game.play_notation(1, "XK#"));
	EXPECT_TRUE(game.play_notation(2, "D6"));
	EXPECT_TRUE(game.play_notation(3, "T1"));
	EXPECT_TRUE(game.play_notation(0, "A#"));

	EXPECT_TRUE(game.play_notation(1, "4'1"));
	EXPECT_TRUE(game.play_notation(2, "D8"));
	EXPECT_TRUE(game.play_notation(3, "31"));
	EXPECT_TRUE(game.play_notation(0, "4'1"));

	EXPECT_TRUE(game.play_notation(1, "61"));
	EXPECT_TRUE(game.play_notation(2, "D8"));
	EXPECT_TRUE(game.play_notation(3, "J101"));
	EXPECT_TRUE(game.play_notation(0, "J131"));

	EXPECT_TRUE(game.play_notation(1, "D6"));
	EXPECT_TRUE(game.play_notation(2, "D9"));
	EXPECT_TRUE(game.play_notation(3, "31"));
	EXPECT_TRUE(game.play_notation(0, "61"));

	EXPECT_TRUE(game.play_notation(1, "D9"));
	EXPECT_TRUE(game.play_notation(2, "DT"));
	EXPECT_TRUE(game.play_notation(3, "30"));
	EXPECT_TRUE(game.play_notation(0, "D2"));

	EXPECT_TRUE(game.play_notation(2, "G7"));
	EXPECT_TRUE(game.play_notation(3, "GX"));
	EXPECT_TRUE(game.play_notation(0, "G7"));
	EXPECT_TRUE(game.play_notation(1, "G7"));

	EXPECT_TRUE(game.play_notation(2, "D3"));
	EXPECT_TRUE(game.play_notation(3, "Q1"));
	EXPECT_TRUE(game.play_notation(0, "D2"));
	EXPECT_TRUE(game.play_notation(1, "XK#"));

	EXPECT_TRUE(game.play_notation(2, "DT"));
	EXPECT_TRUE(game.play_notation(3, "7161'1"));
	EXPECT_TRUE(game.play_notation(0, "D3"));
	EXPECT_TRUE(game.play_notation(1, "T2"));

	EXPECT_TRUE(game.play_notation(2, "DJ"));
	EXPECT_TRUE(game.play_notation(3, "41"));
	EXPECT_TRUE(game.play_notation(0, "D9"));
	EXPECT_TRUE(game.play_notation(1, "J231"));

	EXPECT_TRUE(game.play_notation(2, "D7"));
	EXPECT_TRUE(game.play_notation(3, "31"));
	EXPECT_TRUE(game.play_notation(0, "D7"));
	EXPECT_TRUE(game.play_notation(1, "52"));

	EXPECT_TRUE(game.play_notation(3, "G7"));
	EXPECT_TRUE(game.play_notation(0, "GJ"));
	EXPECT_TRUE(game.play_notation(1, "GA"));
	EXPECT_TRUE(game.play_notation(2, "G6"));

	EXPECT_TRUE(game.play_notation(3, "T1"));
	EXPECT_TRUE(game.play_notation(0, "D6"));
	EXPECT_TRUE(game.play_notation(1, "K#"));
	EXPECT_TRUE(game.play_notation(2, "A#"));

	EXPECT_TRUE(game.play_notation(3, "A1"));
	EXPECT_TRUE(game.play_notation(0, "D6"));
	EXPECT_TRUE(game.play_notation(1, "Q2"));
	EXPECT_TRUE(game.play_notation(2, "Q1"));

	EXPECT_TRUE(game.play_notation(3, "D2"));
	EXPECT_TRUE(game.play_notation(0, "DT"));
	EXPECT_TRUE(game.play_notation(1, "727"));
	EXPECT_TRUE(game.play_notation(2, "J112"));

	EXPECT_TRUE(game.play_notation(0, "G9"));
	EXPECT_TRUE(game.play_notation(1, "GQ"));
	EXPECT_TRUE(game.play_notation(2, "GK"));
	EXPECT_TRUE(game.play_notation(3, "G5"));

	EXPECT_TRUE(game.play_notation(0, "K#"));
	EXPECT_TRUE(game.play_notation(1, "52"));
	EXPECT_TRUE(game.play_notation(2, "J112"));
	EXPECT_TRUE(game.play_notation(3, "D3"));

	EXPECT_TRUE(game.play_notation(0, "82"));
	EXPECT_TRUE(game.play_notation(1, "Q2"));
	EXPECT_TRUE(game.play_notation(2, "91"));
	EXPECT_TRUE(game.play_notation(3, "DQ"));

	EXPECT_TRUE(game.play_notation(1, "GA"));
	EXPECT_TRUE(game.play_notation(2, "G7"));
	EXPECT_TRUE(game.play_notation(3, "GK"));
	EXPECT_TRUE(game.play_notation(0, "GA"));

	EXPECT_TRUE(game.play_notation(1, "727"));
	EXPECT_TRUE(game.play_notation(2, "J112"));
	EXPECT_TRUE(game.play_notation(3, "K#"));
	EXPECT_TRUE(game.play_notation(0, "XK#"));

	EXPECT_TRUE(game.play_notation(1, "A'2"));
	EXPECT_TRUE(game.play_notation(2, "51"));
	EXPECT_TRUE(game.play_notation(3, "X4'2"));
	EXPECT_TRUE(game.play_notation(0, "4'2"));

	EXPECT_TRUE(game.play_notation(1, "Q2"));
	EXPECT_TRUE(game.play_notation(2, "A#"));
	EXPECT_TRUE(game.play_notation(3, "52"));
	EXPECT_TRUE(game.play_notation(0, "52"));

	EXPECT_TRUE(game.play_notation(1, "4'3"));
	EXPECT_TRUE(game.play_notation(2, "51"));
	EXPECT_TRUE(game.play_notation(3, "711212'5"));
	EXPECT_TRUE(game.play_notation(0, "70111211'4"));

	EXPECT_TRUE(game.play_notation(1, "83"));
	EXPECT_TRUE(game.play_notation(2, "22"));
	EXPECT_TRUE(game.play_notation(3, "A#"));
	EXPECT_TRUE(game.play_notation(0, "DQ"));

	EXPECT_TRUE(game.play_notation(1, "K3"));
	EXPECT_TRUE(game.play_notation(2, "K#"));
	EXPECT_TRUE(game.play_notation(3, "83"));
	// Here reshuffling of the deck occurs
	EXPECT_TRUE(game.play_notation(0, "DJ"));
	// Second shuffle must result in "AX9KA79TX876T47668AJQJAQXAJ7XX43KT2A8Q5A6K97T83524QQ4456Q68T4QTT5475526475A89QJAQ9JQJ6JA94KA48Q227T8X7357K785TK679793266326QK2586KJ6TK9JTJ38X329JJ235X38593K98JJ247A2TQ2483T4J948A9XTXTQX3Q39J349Q6K57K5452AXA7T5K83KA62K"

	// This is a hack to make this test independent of the shuffling algorithm
	game.cards_state.reset();
	game.cards_state.deck.set_cards("76TT82AX9KA79TX876T47668AJQJAQXAJ7XX43KT2A8Q5A6K97T83524QQ4456Q68T4QTT5475526475A89QJAQ9JQJ6JA94KA48Q227T8X7357K785TK679793266326QK2586KJ6TK9JTJ38X329JJ235X38593K98JJ247A2TQ2483T4J948A9XTXTQX3Q39J349Q6K57K5452AXA7T5K83KA62K");
	game.cards_state.hand_out_cards(5);

	EXPECT_TRUE(game.play_notation(2, "GA"));
	EXPECT_TRUE(game.play_notation(3, "G7"));
	EXPECT_TRUE(game.play_notation(0, "G7"));
	EXPECT_TRUE(game.play_notation(1, "GX"));

	EXPECT_TRUE(game.play_notation(2, "X4'3"));
	EXPECT_TRUE(game.play_notation(3, "XJ321"));
	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(game.play_notation(1, "K#"));

	EXPECT_TRUE(game.play_notation(2, "717"));
	EXPECT_TRUE(game.play_notation(3, "D6"));
	EXPECT_TRUE(game.play_notation(0, "T3"));
	EXPECT_TRUE(game.play_notation(1, "A'2"));

	EXPECT_TRUE(game.play_notation(2, "71423"));
	EXPECT_TRUE(game.play_notation(3, "D8"));
	EXPECT_TRUE(game.play_notation(0, "D6"));
	EXPECT_TRUE(game.play_notation(1, "93"));

	EXPECT_TRUE(game.play_notation(2, "92"));
	EXPECT_TRUE(game.play_notation(3, "DT"));
	EXPECT_TRUE(game.play_notation(0, "83"));
	EXPECT_TRUE(game.play_notation(1, "23"));

	EXPECT_TRUE(game.play_notation(2, "T2"));
	EXPECT_TRUE(game.play_notation(3, "D4"));
	EXPECT_TRUE(game.play_notation(0, "T3"));
	EXPECT_TRUE(game.play_notation(1, "737"));

	EXPECT_TRUE(game.play_notation(3, "GX"));
	EXPECT_TRUE(game.play_notation(0, "G7"));
	EXPECT_TRUE(game.play_notation(1, "GA"));
	EXPECT_TRUE(game.play_notation(2, "GA"));

	EXPECT_TRUE(game.play_notation(3, "A#"));
	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(game.play_notation(1, "J322"));
	EXPECT_TRUE(game.play_notation(2, "XJ213"));

	EXPECT_TRUE(game.play_notation(3, "X4'3"));
	EXPECT_TRUE(game.play_notation(0, "63"));
	EXPECT_TRUE(game.play_notation(1, "J322"));
	EXPECT_TRUE(game.play_notation(2, "A#"));

	EXPECT_TRUE(game.play_notation(3, "7353'2"));
	EXPECT_TRUE(game.play_notation(0, "83"));
	EXPECT_TRUE(game.play_notation(1, "XA'3"));

	EXPECT_EQ(game.result(), 1);
}
