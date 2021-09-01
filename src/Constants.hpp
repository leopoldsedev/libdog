#pragma once


#define PLAYER_COUNT (4)
#define PIECE_COUNT (4)
#define PATH_LENGTH (64)
#define PATH_SECTION_LENGTH (PATH_LENGTH / PLAYER_COUNT)
#define FINISH_LENGTH (PIECE_COUNT)
#define KENNEL_SIZE (PIECE_COUNT)

#define GET_TEAM_PLAYER_IDX(player_idx) (((player_idx) + 2) % PLAYER_COUNT)
