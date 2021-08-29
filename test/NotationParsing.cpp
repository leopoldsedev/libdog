#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "CardPlay.hpp"
#include "Debug.hpp"


CardPlay play = CardPlay();
bool valid;

TEST(NotationParsing, StartCard) {
	valid = play.from_notation(0, "A#");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.player, 0);
	EXPECT_EQ(play.card, Ace);
	EXPECT_TRUE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "K#");
	EXPECT_EQ(play.player, 0);
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, King);
	EXPECT_TRUE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, ForwardCards) {
	valid = play.from_notation(0, "A1");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ace);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "21");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Two);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "31");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Three);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "41");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Four);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "51");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Five);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "61");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Six);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "81");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Eight);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "91");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Nine);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "T1");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ten);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "Q1");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Queen);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "K1");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "K0");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 0);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "K2");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 2);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "K3");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, AvoidFinish) {
	valid = play.from_notation(1, "T0-");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ten);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 1);
	EXPECT_EQ(play.target_pieces.at(0).rank, 0);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(1, "A0-");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ace);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 1);
	EXPECT_EQ(play.target_pieces.at(0).rank, 0);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(1, "40-");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Four);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 1);
	EXPECT_EQ(play.target_pieces.at(0).rank, 0);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(1, "717-");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Seven);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 1);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_EQ(play.counts.at(0), 7);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "K3-");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "XK3-");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_joker);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, AlternativeActions) {
	valid = play.from_notation(0, "A'3");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ace);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_TRUE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "4'2");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Four);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 2);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_TRUE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "A'3-");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ace);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_TRUE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, Seven) {
	valid = play.from_notation(0, "737");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Seven);
	EXPECT_THAT(play.target_pieces, testing::ElementsAre(PieceRef(0, 3)));
	EXPECT_THAT(play.counts, testing::ElementsAre(7));
	EXPECT_THAT(play.into_finish, testing::ElementsAre(true));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "712242'1");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Seven);
	EXPECT_THAT(play.target_pieces, testing::ElementsAre(PieceRef(0, 1), PieceRef(0, 2), PieceRef(2, 2)));
	EXPECT_THAT(play.counts, testing::ElementsAre(2, 4, 1));
	EXPECT_THAT(play.into_finish, testing::ElementsAre(true, true, true));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "701010101010101");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Seven);
	EXPECT_THAT(play.target_pieces, testing::ElementsAre(PieceRef(0, 0), PieceRef(0, 0), PieceRef(0, 0), PieceRef(0, 0), PieceRef(0, 0), PieceRef(0, 0), PieceRef(0, 0)));
	EXPECT_THAT(play.counts, testing::ElementsAre(1, 1, 1, 1, 1, 1, 1));
	EXPECT_THAT(play.into_finish, testing::ElementsAre(true, true, true, true, true, true, true));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "701-0'1-0'1-0'1-0'1-0'1-0'1-");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Seven);
	EXPECT_THAT(play.target_pieces, testing::ElementsAre(PieceRef(0, 0), PieceRef(2, 0), PieceRef(2, 0), PieceRef(2, 0), PieceRef(2, 0), PieceRef(2, 0), PieceRef(2, 0)));
	EXPECT_THAT(play.counts, testing::ElementsAre(1, 1, 1, 1, 1, 1, 1));
	EXPECT_THAT(play.into_finish, testing::ElementsAre(false, false, false, false, false, false, false));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_FALSE(play.is_joker);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, Jack) {
	valid = play.from_notation(0, "J111");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Jack);
	EXPECT_EQ(play.target_pieces.size(), 2);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_EQ(play.target_pieces.at(1).player, 1);
	EXPECT_EQ(play.target_pieces.at(1).rank, 1);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "J022");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Jack);
	EXPECT_EQ(play.target_pieces.size(), 2);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 0);
	EXPECT_EQ(play.target_pieces.at(1).player, 2);
	EXPECT_EQ(play.target_pieces.at(1).rank, 2);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "J230");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Jack);
	EXPECT_EQ(play.target_pieces.size(), 2);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 2);
	EXPECT_EQ(play.target_pieces.at(1).player, 3);
	EXPECT_EQ(play.target_pieces.at(1).rank, 0);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "J303");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Jack);
	EXPECT_EQ(play.target_pieces.size(), 2);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_EQ(play.target_pieces.at(1).player, 0);
	EXPECT_EQ(play.target_pieces.at(1).rank, 3);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, Joker) {
	valid = play.from_notation(0, "XJ111");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Jack);
	EXPECT_EQ(play.target_pieces.size(), 2);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_EQ(play.target_pieces.at(1).player, 1);
	EXPECT_EQ(play.target_pieces.at(1).rank, 1);
	EXPECT_TRUE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "XA#");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ace);
	EXPECT_TRUE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_TRUE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "XT1");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ten);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_joker);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "XA'3-");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ace);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_TRUE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_joker);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, Invalid) {
	valid = play.from_notation(0, "");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "   ");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "2#");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "A#1");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "A2sdf");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "A'1sdf");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "2A");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "11");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "A12");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "A4");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "54");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "J1");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "J#");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "J411");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "J400");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "J040");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "J004");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "J'011");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "X1");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "X'A#");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "X#");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "XXA#");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "XXXXXA#");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "73");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "7'3");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "734");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "708");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "70'1-12-23'31");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "70'1-122-33'1");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "70'1-12-2-33'1");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "70'1-12-233'101");
	EXPECT_FALSE(valid);

	// Whether or not these cases are valid is questionable
//    valid = play.from_notation(0, "70'1-0'1-0'1-0'1-0'1-0'1-0'1-");
//    EXPECT_FALSE(valid);
//
//    valid = play.from_notation(0, "73'7");
//    EXPECT_FALSE(valid);

	valid = play.from_notation(0, "701-0'1-0'1-0'1-4'1-0'1-0'1-");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "701-0'1-0'1-0'1-0'1-0'1-0'2-");
	EXPECT_FALSE(valid);
}
