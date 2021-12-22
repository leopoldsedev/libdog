#pragma once


#define INCLUDE_AVOID_FINISH_IN_POSSIBLE_MOVES (false)

// Note that changing the constants after this comment will likely break the implementation

#define PLAYER_COUNT (4)
#define PIECE_COUNT (4)
#define PATH_LENGTH (64)
#define PATH_SECTION_LENGTH (PATH_LENGTH / PLAYER_COUNT)
#define FINISH_LENGTH (PIECE_COUNT)
#define KENNEL_SIZE (PIECE_COUNT)

#define DECK_COUNT (2)
#define JOKER_COUNT (3 * DECK_COUNT)
#define STARTING_HANDOUT_SIZE (6)
#define MIN_HANDOUT_SIZE (2)

#define GET_TEAM_PLAYER_IDX(player_idx) (((player_idx) + 2) % PLAYER_COUNT)

#define IS_VALID_PLAYER(x) (0 <= x && x < PLAYER_COUNT)
#define IS_VALID_PIECE_RANK(x) (0 <= x && x < PIECE_COUNT)


#define RULE_ALLOW_SEVEN_MOVE_TEAMMATE_IF_BLOCKED (true)
