#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <libdog/libdog.hpp>


using namespace libdog;


#define EXPECT_PLAYER_AT(path_idx, player_id) do { \
	EXPECT_NE(game.board_state.path.at(path_idx), nullptr); \
	EXPECT_EQ(game.board_state.path.at(path_idx)->player, player_id); \
} while(0)

#define EXPECT_PLAYER_AT_FINISH(finish_idx, player_id) do { \
	EXPECT_NE(game.board_state.finishes.at(player).at(finish_idx), nullptr); \
	EXPECT_EQ(game.board_state.finishes.at(player).at(finish_idx)->player, player_id); \
} while(0)

#define TEST_MOVE_FROM_TO(notation_start, player_id, notation_move, notation_end) do { \
	DogGame game(true, false, false, false); \
	game.load_board(notation_start); \
	EXPECT_TRUE(game.play_notation(player_id, notation_move)); \
	EXPECT_TRUE(game.board_state.check_state()); \
	EXPECT_EQ(to_notation(game.board_state), notation_end); \
} while(0)

#define TEST_INVALID_MOVE_FROM(notation_start, player_id, notation_move) do { \
	DogGame game(true, false, false, false); \
	game.load_board(notation_start); \
	EXPECT_FALSE(game.play_notation(player_id, notation_move)); \
	EXPECT_TRUE(game.board_state.check_state()); \
	EXPECT_EQ(to_notation(game.board_state), notation_start); \
} while(0)

#define CHECK_POSSIBLE_ACTIONS(player_id, game_var_name, actions_var_name, state_results_expected_var_name) do { \
	for (ActionVar action : actions_var_name) { \
		DogGame game_copy = game_var_name; \
		bool legal = game_copy.play(player_id, action, true, false); \
 \
		EXPECT_TRUE(legal); \
		EXPECT_THAT(state_results_expected_var_name, testing::Contains(game_copy.board_state)); \
	} \
} while(0)

#define COMMA ,

#define TEST_POSSIBLE_ACTIONS(player_id, game_var_name, actions_var_name, actions_count, elements) do { \
	std::vector<BoardState> state_results_expected = elements; \
	bool has_duplicates = std::adjacent_find(state_results_expected.begin(), state_results_expected.end()) != state_results_expected.end(); \
	EXPECT_FALSE(has_duplicates); \
	if (actions_count == -1) { \
		EXPECT_EQ(actions.size(), state_results_expected.size()); \
	} else { \
		EXPECT_EQ(actions.size(), actions_count); \
	} \
 \
	CHECK_POSSIBLE_ACTIONS(player_id, game_var_name, actions_var_name, state_results_expected); \
} while(0)


TEST(BasicTest, Reset) {
	DogGame game(true);

	EXPECT_TRUE(game.board_state.check_state());

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
	DogGame game(true, false, false, false);

	game.board_state.start_piece(0, true);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_PLAYER_AT(0, 0);
	EXPECT_EQ(game.board_state.path.at(0)->blocking, true);

	game.board_state.start_piece(1, true);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_PLAYER_AT(16, 1);
	EXPECT_EQ(game.board_state.path.at(16)->blocking, true);

	game.board_state.start_piece(2, true);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_PLAYER_AT(32, 2);
	EXPECT_EQ(game.board_state.path.at(32)->blocking, true);

	game.board_state.start_piece(3, true);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_PLAYER_AT(48, 3);
	EXPECT_EQ(game.board_state.path.at(48)->blocking, true);
}

TEST(BasicTest, MovePiece) {
	DogGame game(true, false, false, false);

	game.board_state.start_piece(0, true);

	for (int i = 0; i < 2 * PATH_LENGTH; i++) {
		bool legal;

		legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(i % PATH_LENGTH)), 1, true, true, false);

		EXPECT_TRUE(legal);

		EXPECT_TRUE(game.board_state.check_state());
	}

	EXPECT_NE(game.board_state.path.at(0), nullptr);
}

TEST(BasicTest, Blockades) {
	DogGame game(true, false, false, false);

	game.board_state.start_piece(0, true);

	bool legal;

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_SECTION_LENGTH)), -PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), -PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_LENGTH - PATH_SECTION_LENGTH)), PATH_SECTION_LENGTH, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	game.board_state.start_piece(1, true);
	game.board_state.start_piece(2, true);
	game.board_state.start_piece(3, true);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), PATH_SECTION_LENGTH, true, true, false);
	EXPECT_FALSE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), -PATH_SECTION_LENGTH, true, true, false);
	EXPECT_FALSE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), PATH_SECTION_LENGTH - 1, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_SECTION_LENGTH - 1)), 1, true, true, false);
	EXPECT_FALSE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_SECTION_LENGTH - 1)), -(PATH_SECTION_LENGTH - 1), true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(0), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(0)), -(PATH_SECTION_LENGTH - 1), true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_LENGTH - (PATH_SECTION_LENGTH - 1))), -1, true, true, false);
	EXPECT_FALSE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(PATH_LENGTH - PATH_SECTION_LENGTH), nullptr);

	legal = game.board_state.move_piece(game.board_state.get_piece(BoardPosition(PATH_LENGTH - (PATH_SECTION_LENGTH - 1))), PATH_SECTION_LENGTH - 1, true, true, false);
	EXPECT_TRUE(legal);
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_NE(game.board_state.path.at(0), nullptr);
}

TEST(BasicTest, PieceRefResolution) {
	DogGame game(true, false, false, false);

	// TODO Also check negative construction cases (for that the assertion in PieceRef will probably need to be replaced with an exception)
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
	DogGame game(true, false, false, false);
	game.load_board("P0*|P17|P33|P49");

	EXPECT_TRUE(game.play(1, Start(Ace, false), false));
	EXPECT_TRUE(game.play(1, Start(King, false), false));
	EXPECT_FALSE(game.play(1, Start(Two, false), false));
	EXPECT_FALSE(game.play(1, Start(Two, true), false));

	EXPECT_TRUE(game.play(0, Move(Ace, PieceRef(0, 0), 1, false), false));
	EXPECT_FALSE(game.play(0, Move(Ace, PieceRef(1, 0), 1, false), false));
	EXPECT_FALSE(game.play(0, Move(Ace, PieceRef(0, 0), 2, false), false));
	EXPECT_FALSE(game.play(0, Move(Ace, PieceRef(0, 0), 2, true), false));

	EXPECT_TRUE(game.play(0, MoveMultiple(Seven, {
		MoveSpecifier(PieceRef(0, 0), 4, false),
		MoveSpecifier(PieceRef(0, 0), 3, false),
	}, false), false));

	EXPECT_TRUE(game.play(0, MoveMultiple(Seven, {
		MoveSpecifier(PieceRef(2, 0), 4, false),
		MoveSpecifier(PieceRef(0, 0), 3, false),
	}, false), false));

	EXPECT_FALSE(game.play(0, MoveMultiple(Six, {
		MoveSpecifier(PieceRef(2, 0), 4, false),
		MoveSpecifier(PieceRef(0, 0), 3, false),
	}, false), false));

	EXPECT_FALSE(game.play(0, MoveMultiple(Six, {
		MoveSpecifier(PieceRef(2, 0), 4, false),
		MoveSpecifier(PieceRef(0, 0), 3, false),
	}, true), false));

	EXPECT_FALSE(game.play(0, MoveMultiple(Seven, {
		MoveSpecifier(PieceRef(0, 0), 4, false),
		MoveSpecifier(PieceRef(1, 0), 3, false),
	}, false), false));

	EXPECT_FALSE(game.play(0, MoveMultiple(Seven, {
		MoveSpecifier(PieceRef(1, 0), 4, false),
		MoveSpecifier(PieceRef(0, 0), 3, false),
	}, false), false));

	EXPECT_TRUE(game.play(1, Swap(Jack, PieceRef(1, 0), PieceRef(2, 0), false), false));
	EXPECT_TRUE(game.play(1, Swap(Jack, PieceRef(1, 0), PieceRef(3, 0), false), false));
	EXPECT_TRUE(game.play(1, Swap(Jack, PieceRef(2, 0), PieceRef(1, 0), false), false));
	EXPECT_TRUE(game.play(1, Swap(Jack, PieceRef(3, 0), PieceRef(1, 0), false), false));

	EXPECT_FALSE(game.play(1, Swap(Jack, PieceRef(1, 0), PieceRef(1, 0), false), false));
	EXPECT_FALSE(game.play(1, Swap(Jack, PieceRef(1, 0), PieceRef(1, 0), true), false));
	EXPECT_FALSE(game.play(1, Swap(Jack, PieceRef(1, 0), PieceRef(0, 0), false), false));

	EXPECT_FALSE(game.play(0, Swap(Jack, PieceRef(0, 0), PieceRef(2, 0), false), false));
	EXPECT_FALSE(game.play(0, Swap(Jack, PieceRef(0, 0), PieceRef(2, 0), true), false));
	EXPECT_FALSE(game.play(0, Swap(Jack, PieceRef(1, 0), PieceRef(2, 0), false), false));

	EXPECT_FALSE(game.play(1, Swap(Jack, PieceRef(2, 0), PieceRef(3, 0), false), false));
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
	DogGame game(true, false, false, false);

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

	DogGame game(true, false, false, false);

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
	DogGame game(true, false, false, false);

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

	DogGame game(true, false, false, false);
	std::string notation_str = "F0F1F2F3|P30F1F2F3|P26P32*P39F3|P62F3";
	game.load_board(notation_str);
	ActionVar action = from_notation(2, "X72'12'12'12'12'12'12'1");
	EXPECT_FALSE(game.play(0, action));
	EXPECT_TRUE(game.board_state.check_state());
	EXPECT_EQ(to_notation(game.board_state), notation_str);

	// TODO Add checks for possible actions
}

TEST(PossibleAction, Move) {
	DogGame game(true, false, false, false);
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
	DogGame game(true, false, false, false);
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

	// TODO
}

TEST(PossibleAction, Swap) {
	DogGame game(true, false, false, false);
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

TEST(BasicTest, BoardStateEquality) {
	BoardState state_1 = from_notation("P12P53|P16*P43F2F3||P15P56F3");
	BoardState state_2 = from_notation("P12P53|P16*P43F2F3||P15P56F3");
	BoardState state_3 = from_notation("P12P53|P16P43F2F3||P15P56F3");

	EXPECT_TRUE(state_1 == state_2);
	EXPECT_FALSE(state_1 == state_3);
	EXPECT_FALSE(state_2 == state_3);
}

TEST(PossibleAction, SevenSimple) {
	DogGame game(true, false, false, false);
	std::vector<ActionVar> actions;

	// 0    0 0 0 0    . . . . .               3
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
	// 1               . . . . .    2 2 2 2    2
	game.load_board("|||");

	actions = game.possible_actions_for_card(0, Seven, false);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {});

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
	// . 0 . . .                       . . . . .
	//           .                   .
	// 1           .               .
	// 1             .      .    .
	// 1               .    .  .
	// 1               .    .  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . .    2 2 2 2    2
	game.load_board("P17|||");

	actions = game.possible_actions_for_card(0, Seven, false);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P24|||") COMMA
	});

	// 0    0 0 . .    . . . . .               3
	//      3 2 1 0    0 \     .
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
	// . 0 . . .                       . . . . .
	//           .                   .
	// 1           .               .
	// 1             .      .    .
	// 1               .    .  .
	// 1               .    .  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . .    2 2 2 2    2
	game.load_board("P1P17|||");

	actions = game.possible_actions_for_card(0, Seven, false);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P1P24|||") COMMA
		from_notation("P2P23|||") COMMA
		from_notation("P3P22|||") COMMA
		from_notation("P4P21|||") COMMA
		from_notation("P5P20|||") COMMA
		from_notation("P6P19|||") COMMA
		from_notation("P7P18|||") COMMA
		from_notation("P8P17|||") COMMA
	});

	// 0    0 0 . .    . . . . .               3
	//      3 2 1 0    0 \     .
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
	// . 0 . . .                       . . . . .
	//           .                   .
	// 1           .               .
	// 1             .      .    .
	// 1               .    .  .
	// 1               .    .  .
	//                 .    .  .
	//                 .     \ 2
	// 1               . . . . .    . 2 2 2    2
	game.load_board("P1P17||P33|");

	actions = game.possible_actions_for_card(0, Seven, false);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P1P24||P33|") COMMA
		from_notation("P2P23||P33|") COMMA
		from_notation("P3P22||P33|") COMMA
		from_notation("P4P21||P33|") COMMA
		from_notation("P5P20||P33|") COMMA
		from_notation("P6P19||P33|") COMMA
		from_notation("P7P18||P33|") COMMA
		from_notation("P8P17||P33|") COMMA

		from_notation("P1P23||P34|") COMMA
		from_notation("P2P22||P34|") COMMA
		from_notation("P3P21||P34|") COMMA
		from_notation("P4P20||P34|") COMMA
		from_notation("P5P19||P34|") COMMA
		from_notation("P6P18||P34|") COMMA
		from_notation("P7P17||P34|") COMMA

		from_notation("P1P22||P35|") COMMA
		from_notation("P2P21||P35|") COMMA
		from_notation("P3P20||P35|") COMMA
		from_notation("P4P19||P35|") COMMA
		from_notation("P5P18||P35|") COMMA
		from_notation("P6P17||P35|") COMMA

		from_notation("P1P21||P36|") COMMA
		from_notation("P2P20||P36|") COMMA
		from_notation("P3P19||P36|") COMMA
		from_notation("P4P18||P36|") COMMA
		from_notation("P5P17||P36|") COMMA

		from_notation("P1P20||P37|") COMMA
		from_notation("P2P19||P37|") COMMA
		from_notation("P3P18||P37|") COMMA
		from_notation("P4P17||P37|") COMMA

		from_notation("P1P19||P38|") COMMA
		from_notation("P2P18||P38|") COMMA
		from_notation("P3P17||P38|") COMMA

		from_notation("P1P18||P39|") COMMA
		from_notation("P2P17||P39|") COMMA

		from_notation("P1P17||P40|") COMMA
	});

	// 0    . . . .    . . . . 3               3
	//      3 2 1 0    . \     .
	//                 .  0 0  .
	//                 .  0 1  .               .
	//                 .  0 2  .               .
	//               .    0 3    .             .
	//             .               .           .
	//           .                   .
	// . . . . .                       . . . . 3
	// .                                     / .
	// .  . 1 1 1                     3 3 . .  .
	// . /                                     .
	// . . 1 . .                       . . . . .
	//           .                   .
	// .           .               .
	// .             .      2    2
	// .               .    2  .
	// .               .    2  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . .    . . . .    2
	game.load_board("F0F1F2F3|P18F1F2F3|P37F1F2F3|P48P60F2F3");

	actions = game.possible_actions_for_card(0, Seven, false);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("F0F1F2F3|P18F1F2F3|P44F1F2F3|P48P60F2F3") COMMA
	});

	// TODO Fix the following test cases (currently 7 moves where individual moves are avoiding the finish are not included in possible actions)
	// 0    . . . .    . . 0 . 3               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  0 1  .               .
	//                 .  0 2  .               .
	//               .    0 3    .             .
	//             .               .           .
	//           .                   .
	// . . . . .                       . . . . 3
	// .                                     / .
	// .  . 1 1 1                     3 3 . .  .
	// . /                                     .
	// . . 1 . .                       . . . . .
	//           .                   .
	// .           .               .
	// .             .      2    2
	// .               .    2  .
	// .               .    2  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . . .    . . . .    2
	game.load_board("P62F1F2F3|P18F1F2F3|P37F1F2F3|P48P60F2F3");

	actions = game.possible_actions_for_card(0, Seven, false);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P62F1F2F3|P18F1F2F3|P44F1F2F3|P48P60F2F3") COMMA
		from_notation("P63F1F2F3|P18F1F2F3|P43F1F2F3|P48P60F2F3") COMMA
		from_notation("P0F1F2F3|P18F1F2F3|P42F1F2F3|P48P60F2F3") COMMA
		// TODO Uncomment
//        from_notation("P1F1F2F3|P18F1F2F3|P41F1F2F3|P48P60F2F3") COMMA
//        from_notation("P2F1F2F3|P18F1F2F3|P40F1F2F3|P48P60F2F3") COMMA
//        from_notation("P3F1F2F3|P18F1F2F3|P39F1F2F3|P48P60F2F3") COMMA
//        from_notation("P4F1F2F3|P18F1F2F3|P38F1F2F3|P48P60F2F3") COMMA
//        from_notation("P5F1F2F3|P18F1F2F3|P37F1F2F3|P48P60F2F3") COMMA

		from_notation("F0F1F2F3|P18F1F2F3|P41F1F2F3|P48P60F2F3") COMMA
	});

	return; // TODO Remove

	// 0    . . . .    . . 0 . 3               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  0 1  .               .
	//                 .  0 2  .               .
	//               .    0 3    .             .
	//             .               .           .
	//           .                   .
	// . . . . .                       . . . . 3
	// .                                     / .
	// .  . 1 1 1                     3 3 . .  .
	// . /                                     .
	// . . 1 . .                       . . . . .
	//           .                   .
	// .           .               .
	// .             .      2    .
	// .               .    2  .
	// .               .    2  .
	//                 .    .  .
	//                 .     \ .
	// 1               . . . 2 .    . . . .    2
	game.load_board("P62F1F2F3|P18F1F2F3|P31F1F2F3|P48P60F2F3");

	actions = game.possible_actions_for_card(0, Seven, false);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P62F1F2F3|P18F1F2F3|P38F1F2F3|P48P60F2F3") COMMA
		from_notation("P63F1F2F3|P18F1F2F3|P37F1F2F3|P48P60F2F3") COMMA
		from_notation("P0F1F2F3|P18F1F2F3|P36F1F2F3|P48P60F2F3") COMMA
		from_notation("P1F1F2F3|P18F1F2F3|P35F1F2F3|P48P60F2F3") COMMA
		from_notation("P2F1F2F3|P18F1F2F3|P34F1F2F3|P48P60F2F3") COMMA
		from_notation("P3F1F2F3|P18F1F2F3|P33F1F2F3|P48P60F2F3") COMMA
		from_notation("P4F1F2F3|P18F1F2F3|P32F1F2F3|P48P60F2F3") COMMA
		from_notation("P5F1F2F3|P18F1F2F3|P31F1F2F3|P48P60F2F3") COMMA

		from_notation("P3F1F2F3|P18F1F2F3|F0F1F2F3|P48P60F2F3") COMMA

		from_notation("F0F1F2F3|P18F1F2F3|P35F1F2F3|P48P60F2F3") COMMA
	});
}

TEST(PossibleAction, SevenBlockades) {
	DogGame game(true, false, false, false);
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
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {});

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
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {
		from_notation("P12P53|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P13P52|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P14P51|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P15P50|P16*P43F2F3||P56F3") COMMA
	});

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
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {
		from_notation("P12P53F3|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P13P52F3|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P14P51F3|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P15P50F3|P16*P43F2F3||P56F3") COMMA
	});


	// 0    0 0 . .    . . . . .               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  . 1  .               3
	//                 .  0 2  3               .
	//               .    . 3    .             .
	//             .               .           .
	//           .                   .
	// . . . . .                       . . . . .
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
	game.load_board("P46F2|P16*P43F2F3||P15P56F3");

	actions = game.possible_actions_for_card(2, Seven, false);
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {
		from_notation("P53F2|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P52F3|P16*P43F2F3||P15P56F3") COMMA
	});

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
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {
		from_notation("P12P53F1F3|P16*P43F2F3||P15P56F3") COMMA

		from_notation("P12P52F2F3|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P13P52F1F3|P16*P43F2F3||P15P56F3") COMMA

		from_notation("P13P51F2F3|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P14P51F1F3|P16*P43F2F3||P15P56F3") COMMA

		from_notation("P14P50F2F3|P16*P43F2F3||P15P56F3") COMMA
		from_notation("P15P50F1F3|P16*P43F2F3||P56F3") COMMA

		from_notation("P15P49F2F3|P16*P43F2F3||P56F3") COMMA
	});

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
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {});

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
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P12|P16*|P39|") COMMA
		from_notation("P13|P16*|P38|") COMMA
		from_notation("P14|P16*|P37|") COMMA
		from_notation("P15|P16*|P36|") COMMA
	});

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
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P28||P39|") COMMA
		from_notation("P29||P38|") COMMA
		from_notation("P30||P37|") COMMA
		from_notation("P31||P36|") COMMA
		from_notation("P32||P35|") COMMA
		from_notation("P33||P34|") COMMA
		from_notation("P34|||") COMMA
	});
}

TEST(PossibleAction, FullGame) {
	DogGame game(true, true, true, true);
	std::vector<ActionVar> actions;

	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");

	// Assumed hand state:
	// 0: 95A454
	// 1: 968X2X
	// 2: 924KQ8
	// 3: K923KA

	actions = game.get_possible_actions(0);
	EXPECT_EQ(actions.size(), 4);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "GA")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G9")));

	actions = game.get_possible_actions(1);
	EXPECT_EQ(actions.size(), 5);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G8")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G9")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "GX")));

	actions = game.get_possible_actions(2);
	EXPECT_EQ(actions.size(), 6);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G8")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G9")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "GQ")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "GK")));

	actions = game.get_possible_actions(3);
	EXPECT_EQ(actions.size(), 5);
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "GA")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G3")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "G9")));
	EXPECT_THAT(actions, testing::Contains(from_notation(0, "GK")));

	EXPECT_TRUE(game.play_notation(0, "G4"));
	EXPECT_TRUE(game.play_notation(1, "GX"));
	EXPECT_TRUE(game.play_notation(2, "G2"));
	EXPECT_TRUE(game.play_notation(3, "GK"));

	// Assumed hand state:
	// 0: 95A542
	// 1: 9682XK
	// 2: 94KQ84
	// 3: 923KAX

	actions = game.get_possible_actions(0);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P0*|||") COMMA
	});

	EXPECT_TRUE(game.play_notation(0, "A#"));

	actions = game.get_possible_actions(1);
	TEST_POSSIBLE_ACTIONS(1, game, actions, 3, {
		from_notation("P0*|P16*||") COMMA
	});

	EXPECT_TRUE(game.play_notation(1, "K#"));

	actions = game.get_possible_actions(2);
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {
		from_notation("P0*|P16*|P32*|") COMMA
	});

	EXPECT_TRUE(game.play_notation(2, "K#"));

	actions = game.get_possible_actions(3);
	TEST_POSSIBLE_ACTIONS(3, game, actions, 5, {
		from_notation("P0*|P16*|P32*|P48*") COMMA
		from_notation("P0*|P23|P32*|") COMMA
	});

	EXPECT_TRUE(game.play_notation(3, "A#"));
	// Board state: P0*|P16*|P32*|P48*
	// Assumed hand state:
	// 0: 95542
	// 1: 9682X
	// 2: 94Q84
	// 3: 923KX

	// 0: 95542
	actions = game.get_possible_actions(0);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P9|P16*|P32*|P48*") COMMA
		from_notation("P5|P16*|P32*|P48*") COMMA
		from_notation("P4|P16*|P32*|P48*") COMMA
		from_notation("P2|P16*|P32*|P48*") COMMA
		from_notation("P60|P16*|P32*|P48*") COMMA
	});

	EXPECT_TRUE(game.play_notation(0, "4'0"));
	// Board state: P60|P16*|P32*|P48*

	// 1: 9682X
	actions = game.get_possible_actions(1);
	TEST_POSSIBLE_ACTIONS(1, game, actions, 25, {
		from_notation("P60|P17|P32*|P48*") COMMA // XA'0
		from_notation("P60|P18|P32*|P48*") COMMA // X20, 20
		from_notation("P60|P19|P32*|P48*") COMMA // X30
		from_notation("P60|P20|P32*|P48*") COMMA // X40
		from_notation("P60|P21|P32*|P48*") COMMA // X50
		from_notation("P60|P22|P32*|P48*") COMMA // X60, 60
		from_notation("P60|P23|P32*|P48*") COMMA // X707
		from_notation("P60|P24|P32*|P48*") COMMA // X80, 80
		from_notation("P60|P25|P32*|P48*") COMMA // X90, 90
		from_notation("P60|P26|P32*|P48*") COMMA // XT0
		from_notation("P60|P27|P32*|P48*") COMMA // XA0
		from_notation("P60|P28|P32*|P48*") COMMA // XQ0
		from_notation("P60|P29|P32*|P48*") COMMA // XK0

		from_notation("P60|P12|P32*|P48*") COMMA // X4'0

		from_notation("P60|P22|P32*|P49") COMMA // X7060'1
		from_notation("P60|P21|P32*|P50") COMMA // X7050'2
		from_notation("P60|P20|P32*|P51") COMMA // X7040'3
		from_notation("P60|P19|P32*|P52") COMMA // X7030'4
		from_notation("P60|P18|P32*|P53") COMMA // X7020'5
		from_notation("P60|P17|P32*|P54") COMMA // X7010'6
		from_notation("P60|P16*|P32*|P55") COMMA // X70'7
	});

	EXPECT_TRUE(game.play_notation(1, "X4'0"));
	// Board state: P60|P12|P32*|P48*

	// 2: 94Q84
	actions = game.get_possible_actions(2);
	TEST_POSSIBLE_ACTIONS(2, game, actions, 5, {
		from_notation("P60|P12|P36|P48*") COMMA // 40
		from_notation("P60|P12|P40|P48*") COMMA // 80
		from_notation("P60|P12|P41|P48*") COMMA // 90
		from_notation("P60|P12|P44|P48*") COMMA // Q0

		from_notation("P60|P12|P28|P48*") COMMA // 4'0
	});

	EXPECT_TRUE(game.play_notation(2, "4'0"));
	// Board state: P60|P12|P28|P48*

	// 3: 923KX
	actions = game.get_possible_actions(3);
	TEST_POSSIBLE_ACTIONS(3, game, actions, 25, {
		from_notation("P60|P12|P28|P49") COMMA // XA'0
		from_notation("P60|P12|P28|P50") COMMA // X20, 20
		from_notation("P60|P12|P28|P51") COMMA // X30, 30
		from_notation("P60|P12|P28|P52") COMMA // X40
		from_notation("P60|P12|P28|P53") COMMA // X50
		from_notation("P60|P12|P28|P54") COMMA // X60
		from_notation("P60|P12|P28|P55") COMMA // X707
		from_notation("P60|P12|P28|P56") COMMA // X80
		from_notation("P60|P12|P28|P57") COMMA // X90, 90
		from_notation("P60|P12|P28|P58") COMMA // XT0
		from_notation("P60|P12|P28|P59") COMMA // XA0
		from_notation("   |P12|P28|P60") COMMA // XQ0
		from_notation("P60|P12|P28|P61") COMMA // XK0, K0

		from_notation("P60|P12|P28|P44") COMMA // X4'0

		from_notation("P60|P13|P28|P54") COMMA // X7060'1
		from_notation("P60|P14|P28|P53") COMMA // X7050'2
		from_notation("P60|P15|P28|P52") COMMA // X7040'3
		from_notation("P60|P16|P28|P51") COMMA // X7030'4
		from_notation("P60|F0|P28|P50") COMMA // X7020'5
		from_notation("P60|F1|P28|P49") COMMA // X7010'6
		from_notation("P60|F2|P28|P48*") COMMA // X70'7

		// TODO Uncomment once fixed (currently 7 moves where individual moves are avoiding the finish are not included in possible actions)
//        from_notation("P60|P17|P28|P50") COMMA // X7020'5-
//        from_notation("P60|P18|P28|P49") COMMA // X7010'6-
//        from_notation("P60|P19|P28|P48*") COMMA // X70'7-
	});

	EXPECT_TRUE(game.play_notation(3, "X4'0"));
	// Board state: P60|P12|P28|P44
	// Assumed hand state:
	// 0: 9552
	// 1: 9682
	// 2: 94Q8
	// 3: 923K

	// 0: 9552
	actions = game.get_possible_actions(0);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P62|P12|P28|P44") COMMA // 20
		from_notation("F0|P12|P28|P44") COMMA // 50
		from_notation("P5|P12|P28|P44") COMMA // 90

		from_notation("P1|P12|P28|P44") COMMA // 50-
	});

	EXPECT_TRUE(game.play_notation(0, "50"));
	// Board state: F0|P12|P28|P44

	// 1: 9682
	actions = game.get_possible_actions(1);
	TEST_POSSIBLE_ACTIONS(1, game, actions, -1, {
		from_notation("F0|P14|P28|P44") COMMA // 20
		from_notation("F0|F1|P28|P44") COMMA // 60
		from_notation("F0|F3|P28|P44") COMMA // 80
		from_notation("F0|P21|P28|P44") COMMA // 90

		from_notation("F0|P18|P28|P44") COMMA // 60-
		from_notation("F0|P20|P28|P44") COMMA // 80-
	});

	EXPECT_TRUE(game.play_notation(1, "80"));
	// Board state: F0|F3|P28|P44

	// 2: 94Q8
	actions = game.get_possible_actions(2);
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {
		from_notation("F0|F3|P32|P44") COMMA // 40
		from_notation("F0|F3|F3|P44") COMMA // 80
		from_notation("F0|F3|P37|P44") COMMA // 90
		from_notation("F0|F3|P40|P44") COMMA // Q0

		from_notation("F0|F3|P24|P44") COMMA // 4'0

		from_notation("F0|F3|P36|P44") COMMA // 80-
	});

	EXPECT_TRUE(game.play_notation(2, "80"));
	// Board state: F0|F3|F3|P44

	// 3: 923K
	actions = game.get_possible_actions(3);
	TEST_POSSIBLE_ACTIONS(3, game, actions, -1, {
		from_notation("F0|F3|F3|P46") COMMA // 20
		from_notation("F0|F3|F3|P47") COMMA // 30
		from_notation("F0|F3|F3|P53") COMMA // 90
		from_notation("F0|F3|F3|P57") COMMA // K0

		from_notation("F0|F3|F3|P44P48*") COMMA // K#
	});

	EXPECT_TRUE(game.play_notation(3, "30"));
	// Board state: F0|F3|F3|P47
	// Assumed hand state:
	// 0: 952
	// 1: 962
	// 2: 94Q
	// 3: 92K

	// 0: 952
	actions = game.get_possible_actions(0);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("F2|F3|F3|P47") COMMA // 20
	});

	EXPECT_TRUE(game.play_notation(0, "20"));
	// Board state: F2|F3|F3|P47

	// 1: 962
	actions = game.get_possible_actions(1);
	EXPECT_EQ(actions.size(), 3);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D2")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D9")));

	EXPECT_TRUE(game.play_notation(1, "D2"));
	// Board state: F2|F3|F3|P47

	// 2: 94Q
	actions = game.get_possible_actions(2);
	EXPECT_EQ(actions.size(), 3);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D9")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "DQ")));

	EXPECT_TRUE(game.play_notation(2, "D9"));
	// Board state: F2|F3|F3|P47

	// 3: 92K
	actions = game.get_possible_actions(3);
	TEST_POSSIBLE_ACTIONS(3, game, actions, -1, {
		from_notation("F2|F3|F3|F0") COMMA // 20
		from_notation("F2|F3|F3|P56") COMMA // 90
		from_notation("F2|F3|F3|P60") COMMA // K0

		from_notation("F2|F3|F3|P47P48*") COMMA // K#

		from_notation("F2|F3|F3|P49") COMMA // 20-
	});

	EXPECT_TRUE(game.play_notation(3, "20"));
	// Board state: F2|F3|F3|F0
	// Assumed hand state:
	// 0: 95
	// 1: 96
	// 2: 4Q
	// 3: 9K

	// 0: 95
	actions = game.get_possible_actions(0);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D5")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D9")));

	EXPECT_TRUE(game.play_notation(0, "D5"));
	// Board state: F2|F3|F3|F0

	// 1: 96
	actions = game.get_possible_actions(1);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D6")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D9")));

	EXPECT_TRUE(game.play_notation(1, "D6"));
	// Board state: F2|F3|F3|F0

	// 2: 4Q
	actions = game.get_possible_actions(2);
	EXPECT_EQ(actions.size(), 2);
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "D4")));
	EXPECT_THAT(actions, testing::Contains(from_notation(1, "DQ")));

	EXPECT_TRUE(game.play_notation(2, "DQ"));
	// Board state: F2|F3|F3|F0

	// 3: 9K
	actions = game.get_possible_actions(3);
	TEST_POSSIBLE_ACTIONS(3, game, actions, -1, {
		from_notation("F2|F3|F3|P48*F0") COMMA // K#
	});

	EXPECT_TRUE(game.play_notation(3, "K#"));
	// Board state: F2|F3|F3|P48*F0
}

TEST(FullGameTest, WinCondition) {
	DogGame game(true, false, false, false);

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
	DogGame game(true, true, true, true);
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
	game = DogGame(true, true, true, true);
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
	// TODO Check hand states before/after
	DogGame game(true, true, true, true);
	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");

	// Assumed hand state:
	// 0: 95A454
	// 1: 968X2X
	// 2: 924KQ8
	// 3: K923KA

	EXPECT_FALSE(game.play_notation(0, "G2"));
	EXPECT_FALSE(game.play_notation(0, "G3"));
	EXPECT_FALSE(game.play_notation(0, "G6"));
	EXPECT_FALSE(game.play_notation(0, "G7"));
	EXPECT_FALSE(game.play_notation(0, "G8"));
	EXPECT_FALSE(game.play_notation(0, "GT"));
	EXPECT_FALSE(game.play_notation(0, "GJ"));
	EXPECT_FALSE(game.play_notation(0, "GQ"));
	EXPECT_FALSE(game.play_notation(0, "GK"));
	EXPECT_FALSE(game.play_notation(0, "GX"));
	EXPECT_TRUE(game.play_notation(0, "GA", false));
	EXPECT_TRUE(game.play_notation(0, "G4", false));
	EXPECT_TRUE(game.play_notation(0, "G5", false));
	EXPECT_TRUE(game.play_notation(0, "G9", false));

	EXPECT_TRUE(game.play_notation(0, "G4"));

	EXPECT_FALSE(game.play_notation(1, "GA"));
	EXPECT_FALSE(game.play_notation(1, "G3"));
	EXPECT_FALSE(game.play_notation(1, "G4"));
	EXPECT_FALSE(game.play_notation(1, "G5"));
	EXPECT_FALSE(game.play_notation(1, "G7"));
	EXPECT_FALSE(game.play_notation(1, "GT"));
	EXPECT_FALSE(game.play_notation(1, "GJ"));
	EXPECT_FALSE(game.play_notation(1, "GQ"));
	EXPECT_FALSE(game.play_notation(1, "GK"));
	EXPECT_TRUE(game.play_notation(1, "G2", false));
	EXPECT_TRUE(game.play_notation(1, "G6", false));
	EXPECT_TRUE(game.play_notation(1, "G8", false));
	EXPECT_TRUE(game.play_notation(1, "G9", false));
	EXPECT_TRUE(game.play_notation(1, "GX", false));

	EXPECT_TRUE(game.play_notation(1, "GX"));

	EXPECT_FALSE(game.play_notation(2, "GA"));
	EXPECT_FALSE(game.play_notation(2, "G3"));
	EXPECT_FALSE(game.play_notation(2, "G5"));
	EXPECT_FALSE(game.play_notation(2, "G6"));
	EXPECT_FALSE(game.play_notation(2, "G7"));
	EXPECT_FALSE(game.play_notation(2, "GT"));
	EXPECT_FALSE(game.play_notation(2, "GJ"));
	EXPECT_FALSE(game.play_notation(2, "GX"));
	EXPECT_TRUE(game.play_notation(2, "G2", false));
	EXPECT_TRUE(game.play_notation(2, "G4", false));
	EXPECT_TRUE(game.play_notation(2, "G8", false));
	EXPECT_TRUE(game.play_notation(2, "G9", false));
	EXPECT_TRUE(game.play_notation(2, "GQ", false));
	EXPECT_TRUE(game.play_notation(2, "GK", false));

	EXPECT_TRUE(game.play_notation(2, "G2"));

	EXPECT_FALSE(game.play_notation(3, "G4"));
	EXPECT_FALSE(game.play_notation(3, "G5"));
	EXPECT_FALSE(game.play_notation(3, "G6"));
	EXPECT_FALSE(game.play_notation(3, "G7"));
	EXPECT_FALSE(game.play_notation(3, "G8"));
	EXPECT_FALSE(game.play_notation(3, "GT"));
	EXPECT_FALSE(game.play_notation(3, "GJ"));
	EXPECT_FALSE(game.play_notation(3, "GQ"));
	EXPECT_FALSE(game.play_notation(3, "GX"));
	EXPECT_TRUE(game.play_notation(3, "GA", false));
	EXPECT_TRUE(game.play_notation(3, "G2", false));
	EXPECT_TRUE(game.play_notation(3, "G3", false));
	EXPECT_TRUE(game.play_notation(3, "G9", false));
	EXPECT_TRUE(game.play_notation(3, "GK", false));

	EXPECT_TRUE(game.play_notation(3, "GK"));

	EXPECT_FALSE(game.play_notation(0, "GA"));
	EXPECT_FALSE(game.play_notation(0, "G2"));
	EXPECT_FALSE(game.play_notation(0, "G3"));
	EXPECT_FALSE(game.play_notation(0, "G4"));
	EXPECT_FALSE(game.play_notation(0, "G5"));
	EXPECT_FALSE(game.play_notation(0, "G6"));
	EXPECT_FALSE(game.play_notation(0, "G7"));
	EXPECT_FALSE(game.play_notation(0, "G8"));
	EXPECT_FALSE(game.play_notation(0, "G9"));
	EXPECT_FALSE(game.play_notation(0, "GT"));
	EXPECT_FALSE(game.play_notation(0, "GJ"));
	EXPECT_FALSE(game.play_notation(0, "GQ"));
	EXPECT_FALSE(game.play_notation(0, "GK"));
	EXPECT_FALSE(game.play_notation(0, "GX"));

	game = DogGame(true, true, true, true);
	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");
	game.load_board("F0F1F2F3|F0F1F2F3|F1F2F3|F1F2F3"); \

	// Assumed hand state:
	// 0: 95A454
	// 1: 968X2X
	// 2: 924KQ8
	// 3: K923KA

	EXPECT_FALSE(game.play_notation(0, "G2"));
	EXPECT_FALSE(game.play_notation(0, "G3"));
	EXPECT_FALSE(game.play_notation(0, "G6"));
	EXPECT_FALSE(game.play_notation(0, "G7"));
	EXPECT_FALSE(game.play_notation(0, "G8"));
	EXPECT_FALSE(game.play_notation(0, "GT"));
	EXPECT_FALSE(game.play_notation(0, "GJ"));
	EXPECT_FALSE(game.play_notation(0, "GQ"));
	EXPECT_FALSE(game.play_notation(0, "GK"));
	EXPECT_FALSE(game.play_notation(0, "GX"));
	EXPECT_TRUE(game.play_notation(0, "GA", false));
	EXPECT_TRUE(game.play_notation(0, "G4", false));
	EXPECT_TRUE(game.play_notation(0, "G5", false));
	EXPECT_TRUE(game.play_notation(0, "G9", false));
}

TEST(FullGameTest, Discard) {
	DogGame game(true, false, true, false);
	game.reset_with_deck("95A454968X2X924KQ8K923KA62AJ66396XT89843J34T27397T5JJT73QX34JT6KQAQ6A2T798QQJK3554AJXQ7QA84AK5572J7KXK8576TT82");

	// Assumed hand state:
	// 0: 95A454
	// 1: 968X2X
	// 2: 924KQ8
	// 3: K923KA

	EXPECT_TRUE(game.play_notation(0, "A#"));
	EXPECT_TRUE(game.play_notation(1, "XK#"));
	EXPECT_TRUE(game.play_notation(2, "K#"));
	EXPECT_TRUE(game.play_notation(3, "A#"));

	// Assumed hand state:
	// 0: 95454
	// 1: 9682X
	// 2: 924Q8
	// 3: K923K

	game.load_board("|||"); \

	EXPECT_FALSE(game.play_notation(0, "DA"));
	EXPECT_FALSE(game.play_notation(0, "D2"));
	EXPECT_FALSE(game.play_notation(0, "D3"));
	EXPECT_FALSE(game.play_notation(0, "D6"));
	EXPECT_FALSE(game.play_notation(0, "D7"));
	EXPECT_FALSE(game.play_notation(0, "D8"));
	EXPECT_FALSE(game.play_notation(0, "DT"));
	EXPECT_FALSE(game.play_notation(0, "DJ"));
	EXPECT_FALSE(game.play_notation(0, "DQ"));
	EXPECT_FALSE(game.play_notation(0, "DK"));
	EXPECT_FALSE(game.play_notation(0, "DX"));
	EXPECT_TRUE(game.play_notation(0, "D4", false));
	EXPECT_TRUE(game.play_notation(0, "D5", false));
	EXPECT_TRUE(game.play_notation(0, "D9", false));

	EXPECT_FALSE(game.play_notation(1, "DA"));
	EXPECT_FALSE(game.play_notation(1, "D2"));
	EXPECT_FALSE(game.play_notation(1, "D3"));
	EXPECT_FALSE(game.play_notation(1, "D4"));
	EXPECT_FALSE(game.play_notation(1, "D5"));
	EXPECT_FALSE(game.play_notation(1, "D6"));
	EXPECT_FALSE(game.play_notation(1, "D7"));
	EXPECT_FALSE(game.play_notation(1, "D8"));
	EXPECT_FALSE(game.play_notation(1, "D9"));
	EXPECT_FALSE(game.play_notation(1, "DT"));
	EXPECT_FALSE(game.play_notation(1, "DJ"));
	EXPECT_FALSE(game.play_notation(1, "DQ"));
	EXPECT_FALSE(game.play_notation(1, "DK"));
	EXPECT_FALSE(game.play_notation(1, "DX"));

	EXPECT_FALSE(game.play_notation(2, "DA"));
	EXPECT_FALSE(game.play_notation(2, "D3"));
	EXPECT_FALSE(game.play_notation(2, "D5"));
	EXPECT_FALSE(game.play_notation(2, "D6"));
	EXPECT_FALSE(game.play_notation(2, "D7"));
	EXPECT_FALSE(game.play_notation(2, "DT"));
	EXPECT_FALSE(game.play_notation(2, "DJ"));
	EXPECT_FALSE(game.play_notation(2, "DK"));
	EXPECT_FALSE(game.play_notation(2, "DX"));
	EXPECT_TRUE(game.play_notation(2, "D2", false));
	EXPECT_TRUE(game.play_notation(2, "D4", false));
	EXPECT_TRUE(game.play_notation(2, "D8", false));
	EXPECT_TRUE(game.play_notation(2, "D9", false));
	EXPECT_TRUE(game.play_notation(2, "DQ", false));

	EXPECT_FALSE(game.play_notation(3, "DA"));
	EXPECT_FALSE(game.play_notation(3, "D2"));
	EXPECT_FALSE(game.play_notation(3, "D3"));
	EXPECT_FALSE(game.play_notation(3, "D4"));
	EXPECT_FALSE(game.play_notation(3, "D5"));
	EXPECT_FALSE(game.play_notation(3, "D6"));
	EXPECT_FALSE(game.play_notation(3, "D7"));
	EXPECT_FALSE(game.play_notation(3, "D8"));
	EXPECT_FALSE(game.play_notation(3, "D9"));
	EXPECT_FALSE(game.play_notation(3, "DT"));
	EXPECT_FALSE(game.play_notation(3, "DJ"));
	EXPECT_FALSE(game.play_notation(3, "DQ"));
	EXPECT_FALSE(game.play_notation(3, "DK"));
	EXPECT_FALSE(game.play_notation(3, "DX"));

	game.load_board("F0F1F2F3|F0F1F2F3|F1F2F3|F1F2F3"); \

	EXPECT_FALSE(game.play_notation(0, "DA"));
	EXPECT_FALSE(game.play_notation(0, "D2"));
	EXPECT_FALSE(game.play_notation(0, "D3"));
	EXPECT_FALSE(game.play_notation(0, "D6"));
	EXPECT_FALSE(game.play_notation(0, "D7"));
	EXPECT_FALSE(game.play_notation(0, "D8"));
	EXPECT_FALSE(game.play_notation(0, "DT"));
	EXPECT_FALSE(game.play_notation(0, "DJ"));
	EXPECT_FALSE(game.play_notation(0, "DQ"));
	EXPECT_FALSE(game.play_notation(0, "DK"));
	EXPECT_FALSE(game.play_notation(0, "DX"));
	EXPECT_TRUE(game.play_notation(0, "D4", false));
	EXPECT_TRUE(game.play_notation(0, "D5", false));
	EXPECT_TRUE(game.play_notation(0, "D9", false));

	EXPECT_FALSE(game.play_notation(1, "DA"));
	EXPECT_FALSE(game.play_notation(1, "D2"));
	EXPECT_FALSE(game.play_notation(1, "D3"));
	EXPECT_FALSE(game.play_notation(1, "D4"));
	EXPECT_FALSE(game.play_notation(1, "D5"));
	EXPECT_FALSE(game.play_notation(1, "D6"));
	EXPECT_FALSE(game.play_notation(1, "D7"));
	EXPECT_FALSE(game.play_notation(1, "D8"));
	EXPECT_FALSE(game.play_notation(1, "D9"));
	EXPECT_FALSE(game.play_notation(1, "DT"));
	EXPECT_FALSE(game.play_notation(1, "DJ"));
	EXPECT_FALSE(game.play_notation(1, "DQ"));
	EXPECT_FALSE(game.play_notation(1, "DK"));
	EXPECT_FALSE(game.play_notation(1, "DX"));

	EXPECT_FALSE(game.play_notation(2, "DA"));
	EXPECT_FALSE(game.play_notation(2, "D3"));
	EXPECT_FALSE(game.play_notation(2, "D5"));
	EXPECT_FALSE(game.play_notation(2, "D6"));
	EXPECT_FALSE(game.play_notation(2, "D7"));
	EXPECT_FALSE(game.play_notation(2, "DT"));
	EXPECT_FALSE(game.play_notation(2, "DJ"));
	EXPECT_FALSE(game.play_notation(2, "DK"));
	EXPECT_FALSE(game.play_notation(2, "DX"));
	EXPECT_TRUE(game.play_notation(2, "D2", false));
	EXPECT_TRUE(game.play_notation(2, "D4", false));
	EXPECT_TRUE(game.play_notation(2, "D8", false));
	EXPECT_TRUE(game.play_notation(2, "D9", false));
	EXPECT_TRUE(game.play_notation(2, "DQ", false));

	EXPECT_FALSE(game.play_notation(3, "DA"));
	EXPECT_FALSE(game.play_notation(3, "D2"));
	EXPECT_FALSE(game.play_notation(3, "D3"));
	EXPECT_FALSE(game.play_notation(3, "D4"));
	EXPECT_FALSE(game.play_notation(3, "D5"));
	EXPECT_FALSE(game.play_notation(3, "D6"));
	EXPECT_FALSE(game.play_notation(3, "D7"));
	EXPECT_FALSE(game.play_notation(3, "D8"));
	EXPECT_FALSE(game.play_notation(3, "D9"));
	EXPECT_FALSE(game.play_notation(3, "DT"));
	EXPECT_FALSE(game.play_notation(3, "DJ"));
	EXPECT_FALSE(game.play_notation(3, "DQ"));
	EXPECT_FALSE(game.play_notation(3, "DK"));
	EXPECT_FALSE(game.play_notation(3, "DX"));
}

TEST(FullGameTest, Turns) {
	DogGame game(true, true, true, false);
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
	DogGame game(true, false, true, false);
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
	DogGame game(true, true, true, false);
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
	std::vector<Card> cards = cards_from_str("76TT82AX9KA79TX876T47668AJQJAQXAJ7XX43KT2A8Q5A6K97T83524QQ4456Q68T4QTT5475526475A89QJAQ9JQJ6JA94KA48Q227T8X7357K785TK679793266326QK2586KJ6TK9JTJ38X329JJ235X38593K98JJ247A2TQ2483T4J948A9XTXTQX3Q39J349Q6K57K5452AXA7T5K83KA62K");
	game.cards_state = CardsState(cards);
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

TEST(NoCanadianRuleOff, MoveMultiple) {
	DogGame game(false, false, false, false);
	game.load_board("P0*|P17|P33|P49");

	EXPECT_TRUE(game.play(0, MoveMultiple(Seven, {
		MoveSpecifier(PieceRef(0, 0), 4, false),
		MoveSpecifier(PieceRef(0, 0), 3, false),
	}, false), false));

	EXPECT_FALSE(game.play(0, MoveMultiple(Seven, {
		MoveSpecifier(PieceRef(2, 0), 4, false),
		MoveSpecifier(PieceRef(0, 0), 3, false),
	}, false), false));
}

TEST(CanadianRuleOff, PossibleAction) {
	DogGame game(false, false, false, false);
	std::vector<ActionVar> actions;

	// 0    0 0 . .    . . . . .               3
	//      3 2 1 0    0 \     .
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
	// . 0 . . .                       . . . . .
	//           .                   .
	// 1           .               .
	// 1             .      .    .
	// 1               .    .  .
	// 1               .    .  .
	//                 .    .  .
	//                 .     \ 2
	// 1               . . . . .    . 2 2 2    2
	game.load_board("P1P17||P33|");

	actions = game.possible_actions_for_card(0, Seven, false);
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {
		from_notation("P1P24||P33|") COMMA
		from_notation("P2P23||P33|") COMMA
		from_notation("P3P22||P33|") COMMA
		from_notation("P4P21||P33|") COMMA
		from_notation("P5P20||P33|") COMMA
		from_notation("P6P19||P33|") COMMA
		from_notation("P7P18||P33|") COMMA
		from_notation("P8P17||P33|") COMMA
	});

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
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {});

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
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {});

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
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {});

	// 0    0 0 . .    . . . . .               3
	//      3 2 1 0    . \     .
	//                 .  . 0  .
	//                 .  . 1  .               3
	//                 .  0 2  3               .
	//               .    . 3    .             .
	//             .               .           .
	//           .                   .
	// . . . . .                       . . . . .
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
	game.load_board("P46F2|P16*P43F2F3||P15P56F3");

	actions = game.possible_actions_for_card(2, Seven, false);
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {});

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
	TEST_POSSIBLE_ACTIONS(2, game, actions, -1, {});

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
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {});

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
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {});

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
	TEST_POSSIBLE_ACTIONS(0, game, actions, -1, {});
}
