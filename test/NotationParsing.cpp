#include <gtest/gtest.h>

#include "CardPlay.hpp"
#include "Debug.hpp"


CardPlay play = CardPlay();
bool valid;

TEST(NotationParsing, StartCard) {
	valid = play.from_notation(0, "A#");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, Ace);
	EXPECT_TRUE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_TRUE(play.is_valid());

	valid = play.from_notation(0, "K#");
	EXPECT_TRUE(valid);
	EXPECT_EQ(play.card, King);
	EXPECT_TRUE(play.start_card);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, ForwardCards) {
	GTEST_SKIP();
	play.from_notation(0, "A1");
	EXPECT_EQ(play.card, Ace);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "21");
	EXPECT_EQ(play.card, Two);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "31");
	EXPECT_EQ(play.card, Three);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "41");
	EXPECT_EQ(play.card, Four);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "51");
	EXPECT_EQ(play.card, Five);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "61");
	EXPECT_EQ(play.card, Six);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "81");
	EXPECT_EQ(play.card, Eight);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "91");
	EXPECT_EQ(play.card, Nine);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "T1");
	EXPECT_EQ(play.card, Ten);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "Q1");
	EXPECT_EQ(play.card, Queen);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "K1");
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 1);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "K0");
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 0);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "K2");
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 2);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "K3");
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "K3-");
	EXPECT_EQ(play.card, King);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, AlternativeActions) {
	GTEST_SKIP();
	play.from_notation(0, "A'3");
	EXPECT_EQ(play.card, Ace);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_TRUE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "4'2");
	EXPECT_EQ(play.card, Four);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 2);
	EXPECT_TRUE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_FALSE(play.ace_one);
	EXPECT_TRUE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());

	play.from_notation(0, "A'3-");
	EXPECT_EQ(play.card, Ace);
	EXPECT_EQ(play.target_pieces.size(), 1);
	EXPECT_EQ(play.target_pieces.at(0).player, 0);
	EXPECT_EQ(play.target_pieces.at(0).rank, 3);
	EXPECT_FALSE(play.into_finish.at(0));
	EXPECT_FALSE(play.start_card);
	EXPECT_TRUE(play.ace_one);
	EXPECT_FALSE(play.four_backwards);
	EXPECT_TRUE(play.is_valid());
}

TEST(NotationParsing, Seven) {
	GTEST_SKIP();
	// TODO
}

TEST(NotationParsing, Jack) {
	GTEST_SKIP();
	// TODO
}

TEST(NotationParsing, Joker) {
	GTEST_SKIP();
	// TODO
}

TEST(NotationParsing, Invalid) {
	GTEST_SKIP();
	valid = play.from_notation(0, "2#");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "11");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "A12");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "A4");
	EXPECT_FALSE(valid);

	valid = play.from_notation(0, "54");
	EXPECT_FALSE(valid);
}
