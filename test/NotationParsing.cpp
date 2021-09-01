#include <variant>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Notation.hpp"
#include "Action.hpp"
#include "Debug.hpp"


#define IS_ACTION(x, t) (std::holds_alternative<t>(x))

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b

#define NOTATION_INVALID(player, notation) do { \
	optional<ActionVar> var1 = try_parse_notation(player, notation); \
	EXPECT_FALSE(var1.has_value()); \
} while(0)

#define NOTATION_TEST(var1, player, notation, card, action_type, is_joker) do { \
	var1 = try_parse_notation(player, notation); \
	EXPECT_TRUE(var1.has_value()); \
	EXPECT_EQ(action_get_card_raw(var1.value()), card); \
	EXPECT_TRUE(IS_ACTION(var1.value(), action_type)); \
	if (is_joker) { \
		EXPECT_TRUE(action_is_joker(var1.value())); \
		EXPECT_EQ(action_get_card(var1.value()), Joker); \
	} else { \
		EXPECT_FALSE(action_is_joker(var1.value())); \
		EXPECT_EQ(action_get_card(var1.value()), card); \
	} \
	EXPECT_TRUE(action_is_valid(var1.value())); \
	EXPECT_EQ(notation, to_notation(player, var1.value())); \
} while(0)

#define GIVE_TEST(player, notation, card, is_joker) do { \
	optional<ActionVar> var1; \
	NOTATION_TEST(var1, player, notation, card, Give, is_joker); \
} while(0)

#define DISCARD_TEST(player, notation, card, is_joker) do { \
	optional<ActionVar> var1; \
	NOTATION_TEST(var1, player, notation, card, Discard, is_joker); \
} while(0)

#define _START_TEST(var1, player, notation, card, is_joker) do { \
	optional<ActionVar> var1; \
	NOTATION_TEST(var1, player, notation, card, Start, is_joker); \
} while(0)

#define START_TEST(player, notation, card, is_joker) \
	_START_TEST(CONCAT(a_opt, __COUNTER__), player, notation, card, is_joker)

#define _MOVE_TEST(var1, var2, player, notation, card, count, piece_rank, avoid_finish, is_joker) do { \
	optional<ActionVar> var1; \
	NOTATION_TEST(var1, player, notation, card, Move, is_joker); \
\
	Move var2 = std::get<Move>(var1.value()); \
	EXPECT_EQ(var2.get_count(), count); \
	EXPECT_EQ(var2.get_piece_ref(), PieceRef(player, piece_rank)); \
	EXPECT_EQ(var2.get_avoid_finish(), avoid_finish); \
} while(0)

#define MOVE_TEST(player, notation, card, count, piece_rank, avoid_finish, is_joker) \
	_MOVE_TEST(CONCAT(a_opt, __COUNTER__), CONCAT(move, __COUNTER__), player, notation, card, count, piece_rank, avoid_finish, is_joker)\

#define _SWAP_TEST(var1, var2, player, notation, rank, player_other, rank_other, is_joker) do { \
	optional<ActionVar> var1; \
	NOTATION_TEST(var1, player, notation, Jack, Swap, is_joker); \
\
	Swap var2 = std::get<Swap>(var1.value()); \
	EXPECT_EQ(var2.get_piece_1(), PieceRef(player, rank)); \
	EXPECT_EQ(var2.get_piece_2(), PieceRef(player_other, rank_other)); \
} while(0)

#define SWAP_TEST(player, notation, rank, player_other, rank_other, is_joker) \
	_SWAP_TEST(CONCAT(a_opt, __COUNTER__), CONCAT(swap, __COUNTER__), player, notation, rank, player_other, rank_other, is_joker)\


TEST(NotationParsing, StartCard) {
	START_TEST(0, "A#", Ace, false);
	START_TEST(0, "K#", King, false);
}

TEST(NotationParsing, ForwardCards) {
	MOVE_TEST(0, "A1", Ace, 11, 1, false, false);
	MOVE_TEST(0, "21", Two, 2, 1, false, false);
	MOVE_TEST(0, "31", Three, 3, 1, false, false);
	MOVE_TEST(0, "41", Four, 4, 1, false, false);
	MOVE_TEST(0, "51", Five, 5, 1, false, false);
	MOVE_TEST(0, "61", Six, 6, 1, false, false);
	MOVE_TEST(0, "81", Eight, 8, 1, false, false);
	MOVE_TEST(0, "91", Nine, 9, 1, false, false);
	MOVE_TEST(0, "T1", Ten, 10, 1, false, false);
	MOVE_TEST(0, "Q1", Queen, 12, 1, false, false);
	MOVE_TEST(0, "K1", King, 13, 1, false, false);

	MOVE_TEST(0, "K0", King, 13, 0, false, false);
	MOVE_TEST(0, "K2", King, 13, 2, false, false);
	MOVE_TEST(0, "K3", King, 13, 3, false, false);
}

TEST(NotationParsing, AvoidFinish) {
	MOVE_TEST(1, "T0-", Ten, 10, 0, true, false);
	MOVE_TEST(1, "A0-", Ace, 11, 0, true, false);
	MOVE_TEST(1, "40-", Four, 4, 0, true, false);
	MOVE_TEST(0, "K3-", King, 13, 3, true, false);

	MOVE_TEST(0, "XK3-", King, 13, 3, true, true);

	optional<ActionVar> a_opt;

	NOTATION_TEST(a_opt, 1, "717-", Seven, MoveMultiple, false);
	MoveMultiple move_mult = std::get<MoveMultiple>(a_opt.value());
	EXPECT_THAT(
		move_mult.get_move_specifiers(),
		testing::ElementsAre(
			MoveSpecifier(1, 1, 7, true)
		)
	);
}

TEST(NotationParsing, AlternativeActions) {
	MOVE_TEST(0, "A'3", Ace, 1, 3, false, false);
	MOVE_TEST(0, "A'3-", Ace, 1, 3, true, false);
	MOVE_TEST(0, "4'2", Four, -4, 2, false, false);
}

TEST(NotationParsing, Seven) {
	optional<ActionVar> a_opt;

	NOTATION_TEST(a_opt, 0, "737", Seven, MoveMultiple, false);
	MoveMultiple move_mult = std::get<MoveMultiple>(a_opt.value());
	EXPECT_THAT(
		move_mult.get_move_specifiers(),
		testing::ElementsAre(
			MoveSpecifier(0, 3, 7, false)
		)
	);

	NOTATION_TEST(a_opt, 0, "712242'1", Seven, MoveMultiple, false);
	move_mult = std::get<MoveMultiple>(a_opt.value());
	EXPECT_THAT(
		move_mult.get_move_specifiers(),
		testing::ElementsAre(
			MoveSpecifier(0, 1, 2, false),
			MoveSpecifier(0, 2, 4, false),
			MoveSpecifier(2, 2, 1, false)
		)
	);

	NOTATION_TEST(a_opt, 0, "701010101010101", Seven, MoveMultiple, false);
	move_mult = std::get<MoveMultiple>(a_opt.value());
	EXPECT_THAT(
		move_mult.get_move_specifiers(),
		testing::ElementsAre(
			MoveSpecifier(0, 0, 1, false),
			MoveSpecifier(0, 0, 1, false),
			MoveSpecifier(0, 0, 1, false),
			MoveSpecifier(0, 0, 1, false),
			MoveSpecifier(0, 0, 1, false),
			MoveSpecifier(0, 0, 1, false),
			MoveSpecifier(0, 0, 1, false)
		)
	);

	NOTATION_TEST(a_opt, 0, "701-0'1-0'1-0'1-0'1-0'1-0'1-", Seven, MoveMultiple, false);
	move_mult = std::get<MoveMultiple>(a_opt.value());
	EXPECT_THAT(
		move_mult.get_move_specifiers(),
		testing::ElementsAre(
			MoveSpecifier(0, 0, 1, true),
			MoveSpecifier(2, 0, 1, true),
			MoveSpecifier(2, 0, 1, true),
			MoveSpecifier(2, 0, 1, true),
			MoveSpecifier(2, 0, 1, true),
			MoveSpecifier(2, 0, 1, true),
			MoveSpecifier(2, 0, 1, true)
		)
	);
}

TEST(NotationParsing, Jack) {
	SWAP_TEST(0, "J111", 1, 1, 1, false);
	SWAP_TEST(0, "J022", 0, 2, 2, false);
	SWAP_TEST(0, "J230", 2, 3, 0, false);
	SWAP_TEST(0, "J313", 3, 1, 3, false);
}

TEST(NotationParsing, Joker) {
	SWAP_TEST(0, "XJ111", 1, 1, 1, true);

	START_TEST(0, "XA#", Ace, true);
	MOVE_TEST(0, "XT1", Ten, 10, 1, false, true);
	MOVE_TEST(0, "XA'3-", Ace, 1, 3, true, true);
}

TEST(NotationParsing, Give) {
	GIVE_TEST(0, "GA", Ace, false);
	GIVE_TEST(0, "G2", Two, false);
	GIVE_TEST(0, "G3", Three, false);
	GIVE_TEST(0, "G4", Four, false);
	GIVE_TEST(0, "G5", Five, false);
	GIVE_TEST(0, "G6", Six, false);
	GIVE_TEST(0, "G7", Seven, false);
	GIVE_TEST(0, "G8", Eight, false);
	GIVE_TEST(0, "G9", Nine, false);
	GIVE_TEST(0, "GT", Ten, false);
	GIVE_TEST(0, "GJ", Jack, false);
	GIVE_TEST(0, "GQ", Queen, false);
	GIVE_TEST(0, "GK", King, false);
	GIVE_TEST(0, "GX", Joker, false);
}

TEST(NotationParsing, Discard) {
	DISCARD_TEST(0, "DA", Ace, false);
	DISCARD_TEST(0, "D2", Two, false);
	DISCARD_TEST(0, "D3", Three, false);
	DISCARD_TEST(0, "D4", Four, false);
	DISCARD_TEST(0, "D5", Five, false);
	DISCARD_TEST(0, "D6", Six, false);
	DISCARD_TEST(0, "D7", Seven, false);
	DISCARD_TEST(0, "D8", Eight, false);
	DISCARD_TEST(0, "D9", Nine, false);
	DISCARD_TEST(0, "DT", Ten, false);
	DISCARD_TEST(0, "DJ", Jack, false);
	DISCARD_TEST(0, "DQ", Queen, false);
	DISCARD_TEST(0, "DK", King, false);
	DISCARD_TEST(0, "DX", Joker, false);
}

TEST(NotationParsing, Invalid) {
	NOTATION_INVALID(0, "");
	NOTATION_INVALID(0, "   ");

	NOTATION_INVALID(0, "f2");
	NOTATION_INVALID(0, "a#");
	NOTATION_INVALID(0, "2#");
	NOTATION_INVALID(0, "2A");

	NOTATION_INVALID(0, "A#1");
	NOTATION_INVALID(0, "A2sdf");
	NOTATION_INVALID(0, "A'1sdf");
	NOTATION_INVALID(0, "A011");

	NOTATION_INVALID(0, "11");
	NOTATION_INVALID(0, "4'0-");
	NOTATION_INVALID(0, "A12");
	NOTATION_INVALID(0, "A4");
	NOTATION_INVALID(0, "54");

	NOTATION_INVALID(0, "J1");
	NOTATION_INVALID(0, "J#");
	NOTATION_INVALID(0, "J411");
	NOTATION_INVALID(0, "J400");
	NOTATION_INVALID(0, "J040");
	NOTATION_INVALID(0, "J004");
	NOTATION_INVALID(0, "J303");
	NOTATION_INVALID(0, "J301");
	NOTATION_INVALID(0, "J'011");

	NOTATION_INVALID(0, "X1");
	NOTATION_INVALID(0, "X'A#");
	NOTATION_INVALID(0, "X#");
	NOTATION_INVALID(0, "XXA#");
	NOTATION_INVALID(0, "XXXXXA#");

	NOTATION_INVALID(0, "73");
	NOTATION_INVALID(0, "7'3");
	NOTATION_INVALID(0, "734");
	NOTATION_INVALID(0, "708");
	NOTATION_INVALID(0, "70'1-12-23'31");
	NOTATION_INVALID(0, "70'1-122-33'1");
	NOTATION_INVALID(0, "70'1-12-2-33'1");
	NOTATION_INVALID(0, "70'1-12-233'101");
	NOTATION_INVALID(0, "701-0'1-0'1-0'1-4'1-0'1-0'1-");
	NOTATION_INVALID(0, "701-0'1-0'1-0'1-0'1-0'1-0'2-");

	// Whether or not these cases are valid is questionable
//    NOTATION_INVALID(0, "70'1-0'1-0'1-0'1-0'1-0'1-0'1-");
//    NOTATION_INVALID(0, "73'7");

	NOTATION_INVALID(0, "G#");
	NOTATION_INVALID(0, "G");
	NOTATION_INVALID(0, "G1");
	NOTATION_INVALID(0, "GTT");
	NOTATION_INVALID(0, "G'T");
	NOTATION_INVALID(0, "XG3");

	NOTATION_INVALID(0, "D#");
	NOTATION_INVALID(0, "D");
	NOTATION_INVALID(0, "D1");
	NOTATION_INVALID(0, "DTT");
	NOTATION_INVALID(0, "D'T");
	NOTATION_INVALID(0, "XD3");
}
