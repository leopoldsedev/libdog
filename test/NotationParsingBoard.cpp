#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Notation.hpp"
#include "Debug.hpp"


#define NOTATION_INVALID(notation) do { \
	optional<BoardState> var1 = try_parse_notation(notation); \
	EXPECT_FALSE(var1.has_value()); \
} while(0)

#define NOTATION_TEST(var1, notation) do { \
	optional<BoardState> board_opt = try_parse_notation(notation); \
	EXPECT_TRUE(board_opt.has_value()); \
	var1 = board_opt.value(); \
	EXPECT_EQ(notation, to_notation(board_opt.value())); \
} while(0)

#define EXPECT_PLAYER_AT(position, player_id, is_blocking) do { \
	EXPECT_NE(board.get_piece(position), nullptr); \
	EXPECT_EQ(board.get_piece(position)->player, player_id); \
	EXPECT_EQ(board.get_piece(position)->blocking, is_blocking); \
} while(0)


// TODO Add check_state() function for every test case from Game tests

TEST(NotationParsingBoard, Empty) {
	BoardState board;

	NOTATION_TEST(board, "|||");

	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 0), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 1), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 2), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 3), 0, false);

	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 0), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 1), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 2), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 3), 1, false);

	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 0), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 1), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 2), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 3), 2, false);

	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 0), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 1), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 2), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 3), 3, false);
}

TEST(NotationParsingBoard, Simple) {
	BoardState board;

	NOTATION_TEST(board, "P0|P63|F0|F3");

	PRINT_DBG(board);

	EXPECT_PLAYER_AT(BoardPosition(0), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 1), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 2), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 3), 0, false);

	EXPECT_PLAYER_AT(BoardPosition(63), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 1), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 2), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 3), 1, false);

	EXPECT_PLAYER_AT(BoardPosition(Finish, 2, 0), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 1), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 2), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 3), 2, false);

	EXPECT_PLAYER_AT(BoardPosition(Finish, 3, 3), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 1), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 2), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 3), 3, false);
}

TEST(NotationParsingBoard, SimpleBlocking) {
	BoardState board;

	NOTATION_TEST(board, "P0*|P63|F0|F3");

	EXPECT_PLAYER_AT(BoardPosition(0), 0, true);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 1), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 2), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 0, 3), 0, false);

	EXPECT_PLAYER_AT(BoardPosition(63), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 1), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 2), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 1, 3), 1, false);

	EXPECT_PLAYER_AT(BoardPosition(Finish, 2, 0), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 1), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 2), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 2, 3), 2, false);

	EXPECT_PLAYER_AT(BoardPosition(Finish, 3, 3), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 1), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 2), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 3), 3, false);
}

TEST(NotationParsingBoard, Full) {
	BoardState board;

	NOTATION_TEST(board, "P0*P13P63F0|P16*F0F2F3|P31P32P33P34|P48*P49*F1");

	EXPECT_PLAYER_AT(BoardPosition(0), 0, true);
	EXPECT_PLAYER_AT(BoardPosition(13), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(63), 0, false);
	EXPECT_PLAYER_AT(BoardPosition(Finish, 0, 0), 0, false);

	EXPECT_PLAYER_AT(BoardPosition(16), 1, true);
	EXPECT_PLAYER_AT(BoardPosition(Finish, 1, 0), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Finish, 1, 2), 1, false);
	EXPECT_PLAYER_AT(BoardPosition(Finish, 1, 3), 1, false);

	EXPECT_PLAYER_AT(BoardPosition(31), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(32), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(33), 2, false);
	EXPECT_PLAYER_AT(BoardPosition(34), 2, false);

	// Two blocking pieces at the same time is not possible in a valid game, but the notation supports it anyway
	EXPECT_PLAYER_AT(BoardPosition(48), 3, true);
	EXPECT_PLAYER_AT(BoardPosition(49), 3, true);
	EXPECT_PLAYER_AT(BoardPosition(Finish, 3, 1), 3, false);
	EXPECT_PLAYER_AT(BoardPosition(Kennel, 3, 3), 3, false);
}

TEST(NotationParsingBoard, Invalid) {
	NOTATION_INVALID("");
	NOTATION_INVALID("P1");
	NOTATION_INVALID("asdf");

	NOTATION_INVALID("||");
	NOTATION_INVALID("||||");

	NOTATION_INVALID("P0P1P2P3");
	NOTATION_INVALID("P0P1P2P3|");
	NOTATION_INVALID("P0P1P2P3|P4P5P6P7|P8P9P10P11");
	NOTATION_INVALID("P0P1P2P3|P4P5P6P7|P8P9P10P11|P12P13P14P15|P16P17P18P19");

	NOTATION_INVALID("|-||");
	NOTATION_INVALID("P|||");
	NOTATION_INVALID("A2|||");
	NOTATION_INVALID("|||P");
	NOTATION_INVALID("|1||");
	NOTATION_INVALID("|||1");

	NOTATION_INVALID("P123|||");
	NOTATION_INVALID("P64|||");
	NOTATION_INVALID("F4|||");

	NOTATION_INVALID("P0P1P2P3P4|||");
	NOTATION_INVALID("P0P1P2P3F0|||");
	NOTATION_INVALID("P0P0P2P3|||");
	NOTATION_INVALID("P0|P0||");
	NOTATION_INVALID("F0F0|||");

	NOTATION_INVALID("P0*P63P13F2|P16*F4F2F0|P32P33P34P31|P49*P48*F1");
}
