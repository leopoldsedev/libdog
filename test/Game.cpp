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
	check_state(game);
	EXPECT_TRUE(game.play_notation(0, "40"));
	check_state(game);

	EXPECT_NE(game.board_state.path.at(4), nullptr);
}

TEST(CardTest, SendToKennel) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "A#"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(0, "Q0"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "A'0"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(0, "50"));
	check_state(game);

	EXPECT_PLAYER_AT(17, 0);
	EXPECT_NE(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);

	EXPECT_TRUE(game.play_notation(1, "K#"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(0, "30"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "4'0"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(0, "20"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "T0"));
	check_state(game);

	EXPECT_PLAYER_AT(22, 1);
	EXPECT_NE(game.board_state.kennels.at(0).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(3), nullptr);

	EXPECT_TRUE(game.play_notation(0, "XA#"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "K#"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(0, "XA0"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "4'1"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(0, "X50"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "T0-"));
	check_state(game);

	EXPECT_PLAYER_AT(16, 0);
	EXPECT_PLAYER_AT(22, 1);
	EXPECT_EQ(game.board_state.kennels.at(1).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(1).at(3), nullptr);

	EXPECT_TRUE(game.play_notation(0, "K#"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "K#"));
	check_state(game);

	EXPECT_PLAYER_AT(16, 1);
	EXPECT_EQ(game.board_state.kennels.at(0).at(0), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(1), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(2), nullptr);
	EXPECT_NE(game.board_state.kennels.at(0).at(3), nullptr);
}

TEST(CardTest, Swap) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "A#"));
	check_state(game);
	EXPECT_EQ(game.board_state.path.at(0)->blocking, true);
	EXPECT_EQ(game.board_state.path.at(16)->blocking, true);

	EXPECT_FALSE(game.play_notation(0, "J010"));

	EXPECT_TRUE(game.play_notation(0, "A'0"));
	check_state(game);
	EXPECT_EQ(game.board_state.path.at(1)->blocking, false);
	EXPECT_EQ(game.board_state.path.at(16)->blocking, true);

	EXPECT_FALSE(game.play_notation(0, "J010"));

	EXPECT_TRUE(game.play_notation(1, "A'0"));
	check_state(game);
	EXPECT_EQ(game.board_state.path.at(1)->blocking, false);
	EXPECT_EQ(game.board_state.path.at(17)->blocking, false);

	EXPECT_PLAYER_AT(1, 0);
	EXPECT_PLAYER_AT(17, 1);

	EXPECT_TRUE(game.play_notation(0, "J010"));
	check_state(game);

	EXPECT_PLAYER_AT(1, 1);
	EXPECT_PLAYER_AT(17, 0);
}

TEST(CardTest, Seven) {
	DogGame game(false, false);

	EXPECT_TRUE(game.play_notation(0, "A#"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "A#"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(0, "Q0"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(1, "A'0"));
	check_state(game);
	EXPECT_TRUE(game.play_notation(0, "707"));
	check_state(game);

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

// TODO Remove
//bool action_lists_contains(std::vector<ActionVar> list, ActionVar e) {
//    for (ActionVar action : list) {
//        if (action == e) {
//            return true;
//        }
//    }
//
//    return false;
//}
//
//bool action_lists_eq(std::vector<ActionVar> list1, std::vector<ActionVar> list2) {
//    if (list1.size() != list2.size()) {
//        return false;
//    }
//
//    for (ActionVar action : list1) {
//        bool contains = action_lists_contains(list2, action);
//        if (!contains) {
//            return false;
//        }
//    }
//
//    return true;
//}

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
