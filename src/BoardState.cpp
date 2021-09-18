#include <libdog/BoardState.hpp>

#include "Util.hpp"
#include "Debug.hpp"


namespace libdog {

BoardState::BoardState() : pieces{{
		{{ Piece(0, 0, BoardPosition(Kennel, 0, 0), false), Piece(0, 1, BoardPosition(Kennel, 0, 1), false), Piece(0, 2, BoardPosition(Kennel, 0, 2), false), Piece(0, 3, BoardPosition(Kennel, 0, 3), false) }},
		{{ Piece(1, 0, BoardPosition(Kennel, 1, 0), false), Piece(1, 1, BoardPosition(Kennel, 1, 1), false), Piece(1, 2, BoardPosition(Kennel, 1, 2), false), Piece(1, 3, BoardPosition(Kennel, 1, 3), false) }},
		{{ Piece(2, 0, BoardPosition(Kennel, 2, 0), false), Piece(2, 1, BoardPosition(Kennel, 2, 1), false), Piece(2, 2, BoardPosition(Kennel, 2, 2), false), Piece(2, 3, BoardPosition(Kennel, 2, 3), false) }},
		{{ Piece(3, 0, BoardPosition(Kennel, 3, 0), false), Piece(3, 1, BoardPosition(Kennel, 3, 1), false), Piece(3, 2, BoardPosition(Kennel, 3, 2), false), Piece(3, 3, BoardPosition(Kennel, 3, 3), false) }}
	}} {
	reset();
}

BoardState::BoardState(const BoardState& other) : pieces(other.pieces) {
	assert(other.check_state());

	path.fill(nullptr);

	for (int player = 0; player < PLAYER_COUNT; player++) {
		kennels[player].fill(nullptr);
		finishes[player].fill(nullptr);

		for (int i = 0; i < PIECE_COUNT; i++) {
			PiecePtr piece_ptr = &pieces[player][i];

			get_piece(piece_ptr->position) = piece_ptr;
		}
	}

	assert(check_state());
}

BoardState& BoardState::operator=(const BoardState& other) {
	assert(other.check_state());

	if (this != &other) {
		pieces = other.pieces;
		path.fill(nullptr);

		for (int player = 0; player < PLAYER_COUNT; player++) {
			kennels[player].fill(nullptr);
			finishes[player].fill(nullptr);

			for (int i = 0; i < PIECE_COUNT; i++) {
				PiecePtr piece_ptr = &pieces[player][i];

				get_piece(piece_ptr->position) = piece_ptr;
			}
		}
	}

	return *this;
}

bool operator==(const BoardState& a, const BoardState& b) {
	assert(a.check_state());
	assert(b.check_state());

	for (int player = 0; player < PLAYER_COUNT; player++) {
		for (int i = 0; i < PIECE_COUNT; i++) {
			const Piece* piece_ptr = &a.pieces[player][i];

			const Piece* piece_ptr_b = b.get_piece(piece_ptr->position);

			if (piece_ptr_b == nullptr) {
				return false;
			}

			assert(piece_ptr != nullptr);
			assert(piece_ptr_b != nullptr);
			if (*piece_ptr != *piece_ptr_b) {
				return false;
			}
		}
	}

	return true;
}

void BoardState::reset() {
	for (std::size_t player = 0; player != kennels.size(); player++) {
		for (std::size_t j = 0; j != kennels.size(); j++) {
			Piece* piece = &pieces[player][j];

			kennels[player][j] = piece;

			piece->position = BoardPosition(Kennel, player, j);
			piece->blocking = false;
		}
	}

	for (std::size_t player = 0; player != finishes.size(); player++) {
		finishes[player].fill(nullptr);
	}

	path.fill(nullptr);
}

PiecePtr BoardState::ref_to_piece(const PieceRef& piece_ref) const {
	int player = piece_ref.player;
	int rank = piece_ref.rank;

	std::vector<PiecePtr> pieces_ordered;
	pieces_ordered.reserve(PIECE_COUNT);

	for (std::size_t i = 0; i < PLAYER_COUNT; i++) {
		PiecePtr to_insert = const_cast<PiecePtr>(&pieces[player][i]);

		std::size_t insert_idx = 0;

		for (; insert_idx < pieces_ordered.size(); insert_idx++) {
			PiecePtr piece_ptr = pieces_ordered[insert_idx];
			assert(piece_ptr != nullptr);
			if (piece_ptr->is_behind(*to_insert)) {
				break;
			}
		}

		pieces_ordered.insert(pieces_ordered.begin() + insert_idx, to_insert);
	}

	PiecePtr result = pieces_ordered[rank];
	return result;
}

PiecePtr& BoardState::ref_to_piece_ptr_ref(const PieceRef& piece_ref) {
	PiecePtr piece_ptr = ref_to_piece(piece_ref);
	return get_piece(piece_ptr->position);
}

BoardPosition BoardState::ref_to_pos(const PieceRef& piece_ref) const {
	PiecePtr piece_ptr = ref_to_piece(piece_ref);
	return piece_ptr->position;
}

bool BoardState::get_kennel_piece(int player, PiecePtr** result) {
	auto& kennel = kennels.at(player);

	for (std::size_t i = 0; i < kennel.size(); i++) {
		PiecePtr* piece = &(kennel.at(i));

		if (*piece != nullptr) {
			*result = piece;
			return true;
		}
	}

	return false;
}

void BoardState::start_piece(PiecePtr& piece) {
	assert(piece->position.area == Kennel);

	int start_path_idx = get_start_path_idx(piece->player);
	BoardPosition start_position = BoardPosition(start_path_idx);
	move_piece(piece, start_position, true);
}

PiecePtr& BoardState::get_piece(int path_idx) {
	return path.at(path_idx);
}

const PiecePtr& BoardState::get_piece(BoardPosition position) const {
	switch(position.area) {
		case Path:
			return path.at(position.idx);
		case Kennel:
			return kennels.at(position.player).at(position.idx);
		case Finish:
			return finishes.at(position.player).at(position.idx);
		default:
			assert(false);
	}
}

PiecePtr& BoardState::get_piece(BoardPosition position) {
	return const_cast<PiecePtr&>(static_cast<const BoardState&>(*this).get_piece(position));
}

PiecePtr& BoardState::get_start(int player) {
	int start_path_idx = get_start_path_idx(player);
	PiecePtr& piece = get_piece(BoardPosition(start_path_idx));
	return piece;
}

void BoardState::send_to_kennel(int from_path_idx, int count) {
	bool backwards = (count < 0);
	int step = backwards ? -1 : 1;

	for (int i = 0; i != count; i += step) {
		int path_idx = positive_mod(from_path_idx + step + i, PATH_LENGTH);

		if (path_idx == from_path_idx) {
			continue;
		}

		PiecePtr& piece = get_piece(BoardPosition(path_idx));

		if (piece != nullptr) {
			assert(!piece->blocking);
			place_at_kennel(piece);
		}
	}
}

void BoardState::place_at_kennel(PiecePtr& piece) {
	assert(piece != nullptr);
	assert(!piece->blocking);

	auto& kennel = kennels.at(piece->player);

	for (int i = kennel.size() - 1; i >= 0; i--) {
		if (kennel.at(i) == nullptr) {
			move_piece(piece, BoardPosition(Kennel, piece->player, i), false);

			return;
		}
	}
}

void BoardState::move_piece(PiecePtr& piece, BoardPosition position, bool blocking) {
	assert(piece != nullptr);

	PiecePtr& target = get_piece(position);

	piece->position = position;
	piece->blocking = blocking;

	target = piece;
	piece = nullptr;
}

void BoardState::swap_pieces(PiecePtr& piece1, PiecePtr& piece2) {
	assert(piece1 != nullptr);
	assert(piece2 != nullptr);

	std::swap(piece1->position, piece2->position);
	std::swap(piece1, piece2);
}

bool BoardState::check_finish_full(int player) {
	std::array<PiecePtr, FINISH_LENGTH>& finish = finishes.at(player);

	for (std::size_t i = 0; i < finish.size(); i++) {
		if (finish.at(i) == nullptr) {
			return false;
		}
	}

	return true;
}

std::vector<PieceRef> BoardState::get_pieces_in_area(int player, Area area) {
	std::vector<PieceRef> result;

	for (std::size_t i = 0; i < PIECE_COUNT; i++) {
		PieceRef piece_ref(player, i);
		PiecePtr piece = ref_to_piece(piece_ref);

		if (piece->position.area == area) {
			result.push_back(piece_ref);
		}
	}

	return result;
}

bool BoardState::start_piece(int player, bool modify_state) {
	PiecePtr& piece_on_start = get_start(player);

	if (piece_on_start != nullptr && piece_on_start->blocking) {
		// Start must not be blocked by a blocking piece
		return false;
	}

	PiecePtr* piece;
	bool success = get_kennel_piece(player, &piece);

	if (!success) {
		// No piece left in kennel
		return false;
	}

	if (success && modify_state) {
		PiecePtr piece_for_kennel = piece_on_start;

		start_piece(*piece);

		// Only place it in kennel afterwards, otherwise the piece that is starting could be in the way
		if (piece_for_kennel != nullptr) {
			place_at_kennel(piece_for_kennel);
		}
	}

	return true;
}

bool BoardState::move_piece(PiecePtr& piece, int count, bool avoid_finish, bool modify_state, bool remove_all_on_way) {
	if (piece == nullptr) {
		return false;
	}

	// TODO This is not a good initialization
	BoardPosition position_result = BoardPosition(0);
	int count_on_path = 0;

	if (piece->position.area == Kennel) {
		// Moving in kennel is not possible
		return false;
	}

	if (piece->position.area == Path) {
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
		// TODO This is not a good initialization
		BoardPosition path_position_result = BoardPosition(0);
		BoardPosition finish_position_result = BoardPosition(0);
		int path_count_on_path = count;
		int finish_count_on_path;
		bool path_free = check_move_on_path(piece->position.idx, count, path_position_result);
		bool finish_free = try_enter_finish(piece->player, piece->position.idx, count, piece->blocking, finish_position_result, finish_count_on_path);

		// Table above can be summarized in this if-chain
		if (!avoid_finish && finish_free) {
			position_result = finish_position_result;
			count_on_path = finish_count_on_path;
		} else if (path_free) {
			position_result = path_position_result;
			count_on_path = path_count_on_path;
		} else {
			return false;
		}
	} else {
		assert(piece->position.area == Finish);

		bool legal = check_move_in_finish(piece->player, piece->position.idx, count, position_result);

		if (!legal) {
			return false;
		}
	}

	if (modify_state) {
		// Change board state
		if (remove_all_on_way) {
			assert(count_on_path >= 0);
			if (piece->position.area == Path) {
				send_to_kennel(piece->position.idx, count_on_path);
			}
		} else {
			PiecePtr& piece_to_send_back = get_piece(position_result);
			if (piece_to_send_back != nullptr) {
				place_at_kennel(piece_to_send_back);
			}
		}

		move_piece(piece, position_result, false);
	}

	return true;
}

int BoardState::calc_steps_into_finish(int player, int from_path_idx, int count, bool piece_blocking, int& count_on_path_result) {
	count_on_path_result = calc_steps_on_path(player, from_path_idx, piece_blocking, count, true);
	int steps_into_finish = count - count_on_path_result;
	return steps_into_finish;
}

bool BoardState::try_enter_finish(int player, int from_path_idx, int count, bool piece_blocking, BoardPosition& position_result, int& count_on_path_result) {
	int steps_into_finish = calc_steps_into_finish(player, from_path_idx, count, piece_blocking, count_on_path_result);
	int& steps_on_path = count_on_path_result;

	if (steps_on_path != 0) {
		bool legal = check_move_on_path(from_path_idx, steps_on_path, position_result);
		if (!legal) return false;
	}

	if (steps_into_finish > 0) {
		// Piece transitions from path area to finish area
		bool backwards = count < 0;
		assert(!backwards);

		// The calc_steps_into_finish() call already makes sure that this case is impossible here
		assert(!(steps_on_path == 0 && piece_blocking)); // Piece cannot go into finish while it is blocking

		// In this check position index "-1" in the finish represents the position right before the finish
		bool legal = check_move_in_finish(player, -1, steps_into_finish, position_result);
		if (!legal) return false;
	}

	return true;
}

int BoardState::possible_forward_steps_in_finish(int player, int from_finish_idx) {
	std::array<PiecePtr, FINISH_LENGTH>& finish = finishes.at(player);

	int result = 0;

	if (from_finish_idx < -1) {
		result = -from_finish_idx - 1;
		from_finish_idx = -1;
	}

	assert(from_finish_idx >= -1);

	for (int i = from_finish_idx + 1; i < FINISH_LENGTH; i++) {
		if (finish.at(i) == nullptr) {
			result++;
		} else {
			break;
		}
	}

	return result;
}

bool BoardState::possible_one_step_on_path(int from_path_idx, bool backwards) {
	int step = backwards ? -1 : 1;
	int path_idx = positive_mod(from_path_idx + step, PATH_LENGTH);
	PiecePtr& piece = get_piece(path_idx);

	return (piece == nullptr || !piece->blocking);
}

int BoardState::possible_forward_steps_on_path(int from_path_idx, bool backwards) {
	int step = backwards ? -1 : 1;

	int result = 0;
	int limit = backwards ? -PATH_LENGTH : PATH_LENGTH;

	// TODO Loop could be made faster by going in increments of PATH_SECTION_LENGTH
	for (int i = 0; i != limit; i += step) {
		int path_idx = positive_mod(from_path_idx + step + i, PATH_LENGTH);

		if (path_idx == from_path_idx) {
			continue;
		}

		PiecePtr& piece = get_piece(path_idx);

		if (piece != nullptr && piece->blocking) {
			break;
		}

		result++;
	}

	return result;
}

int BoardState::possible_steps_of_piece(int player, BoardPosition position, bool piece_blocking, bool backwards) {
	if (position.area == Path) {
		int steps_on_path = possible_forward_steps_on_path(position.idx, backwards);
		int steps_possible = steps_on_path;

		if (!backwards) {
			int steps_on_path_if_finish = calc_steps_on_path(player, position.idx, piece_blocking, PATH_LENGTH, true);
			int steps_into_finish = possible_forward_steps_in_finish(player, -1);
			steps_possible = steps_on_path_if_finish + steps_into_finish;
		}

		return steps_possible;
	} else if (position.area == Finish) {
		if (backwards) {
			// Cannot move backwards in Kennel
			return 0;
		}

		return possible_forward_steps_in_finish(player, position.idx);
	} else if (position.area == Kennel) {
		// Cannot move in Kennel
		return 0;
	} else {
		assert(false);
	}
}

bool BoardState::check_block(int from_path_idx, int count) {
	bool backwards = (count < 0);

	if (count == 1 || count == -1) {
		return !possible_one_step_on_path(from_path_idx, backwards);
	}

	int possible_steps = possible_forward_steps_on_path(from_path_idx, backwards);

	if (backwards) {
		count *= -1;
	}

	bool blocked = (count > possible_steps);

	return blocked;
}

bool BoardState::check_move_on_path(int from_path_idx, int count, BoardPosition& position_result) {
	int target_path_idx = positive_mod(from_path_idx + count, PATH_LENGTH);

	bool blocked = check_block(from_path_idx, count);
	if (blocked) {
		// Piece cannot leapfrog another piece that is blocking
		return false;
	}

	position_result = BoardPosition(target_path_idx);

	return true;
}

bool BoardState::check_move_in_finish(int player, int from_finish_idx, int count, BoardPosition& position_result) {
	bool backwards = count < 0;

	if (backwards) {
		// Piece cannot go backwards in finish
		return false;
	}

	int steps_possible = possible_forward_steps_in_finish(player, from_finish_idx);

	if (steps_possible < count) {
		// Piece cannot go further than length of finish
		// Piece cannot leapfrog another piece in finish
		return false;
	}

	int finish_idx_target = from_finish_idx + count;

	position_result = BoardPosition(Finish, player, finish_idx_target);

	return true;
}

bool BoardState::move_multiple_pieces(std::vector<MoveSpecifier> move_actions, bool modify_state) {
	// Try out the moves on a copy of the board state
	BoardState copy = BoardState(*this);
	bool legal = copy.move_multiple_pieces_naive(move_actions);

	if (legal && modify_state) {
		legal = move_multiple_pieces_naive(move_actions);
		assert(legal);
	}

	return legal;
}

bool BoardState::swap_pieces(PiecePtr& piece_1, PiecePtr& piece_2, bool modify_state) {
	if (piece_1 == nullptr || piece_2 == nullptr) {
		return false;
	}

	if (piece_1->position.area != Path || piece_2->position.area != Path) {
		// Only pieces on path can be swapped
		return false;
	}

	if (piece_1->blocking || piece_2->blocking) {
		// Blocking pieces cannot be swapped
		return false;
	}

	if (modify_state) {
		swap_pieces(piece_1, piece_2);
	}

	return true;
}

// TODO Add more checks now that unique_ptr isn't used anymore
// TODO the following things must be unique: piece position, piece player/idx pair
// TODO A piece must be pointed to only once from the board
bool BoardState::check_state() const {
	for (int player = 0; player != PLAYER_COUNT; player++) {
		for (int idx = 0; idx != PIECE_COUNT; idx++) {
			const Piece& piece = pieces.at(player).at(idx);
			if (piece.player != player || piece.idx != idx) {
				goto invalid_state;
			}
		}
	}

	std::array<int, PLAYER_COUNT> piece_cnt;
	piece_cnt.fill(0);

	for (int player = 0; player != PLAYER_COUNT; player++) {
		bool expect_pieces_only = false;

		for (std::size_t j = 0; j != kennels.size(); j++) {
			const PiecePtr& piece = kennels.at(player).at(j);

			if (piece != nullptr) {
				if (piece->player != player) {
					goto invalid_state;
				}

				if (piece->position != BoardPosition(Kennel, player, j)) {
					goto invalid_state;
				}

				piece_cnt.at(piece->player)++;

				expect_pieces_only = true;
			} else {
				if (expect_pieces_only) {
					goto invalid_state;
				}
			}
		}
	}

	for (std::size_t i = 0; i != path.size(); i++) {
		const PiecePtr& piece = path.at(i);

		if (piece != nullptr) {
			if (piece->position != BoardPosition(i)) {
				goto invalid_state;
			}

			piece_cnt.at(piece->player)++;
		}
	}

	for (int player = 0; player != PLAYER_COUNT; player++) {
		for (std::size_t j = 0; j != finishes.size(); j++) {
			const PiecePtr& piece = finishes.at(player).at(j);

			if (piece != nullptr) {
				if (piece->player != player) {
					goto invalid_state;
				}

				if (piece->position != BoardPosition(Finish, player, j)) {
					goto invalid_state;
				}

				piece_cnt.at(piece->player)++;
			}
		}
	}

	for (int count : piece_cnt) {
		if (count != PIECE_COUNT) {
			goto invalid_state;
		}
	}

	return true;

invalid_state:
	std::cout << *this << std::endl;

	return false;
}

bool BoardState::move_multiple_pieces_naive(std::vector<MoveSpecifier> move_actions) {
	bool legal = true;

	// All piece references are resolved in the starting position
	// Note that it is not sufficient to resolve them to PiecePtr&
	// because a PiecePtr essentially is a reference to a slot. If a
	// move specifier causes another piece to be sent back to kernel,
	// the PiecePtr of that piece would change to be nullptr.
	// Consequently, pointers to the actual pieces are required. These
	// pointers stay valid even if the pieces they point to are moved.
	std::vector<Piece*> piece_ptrs;
	for (MoveSpecifier move_specifier : move_actions) {
		PiecePtr piece = ref_to_piece(move_specifier.piece_ref);
		piece_ptrs.push_back(piece);
	}

	for (std::size_t i = 0; i < move_actions.size(); i++) {
		MoveSpecifier move_action = move_actions.at(i);

		Piece* piece_ptr = piece_ptrs.at(i);
		PiecePtr& piece = get_piece(piece_ptr->position);
		int count = move_action.count;
		bool avoid_finish = move_action.avoid_finish;

		legal = move_piece(piece, count, avoid_finish, true, true);

		if (!legal) {
			break;
		}
	}

	return legal;
}

// 0 = nothing, 1 = path, 2 = finish, 3 = kennel, 4 = char, 5 = block indicator
std::array<std::array<int, 41>, 21> vis_map_spec = {
	{
		{ 4, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 4, 5, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4 },
		{ 0, 0, 0, 0, 0, 4, 0, 4, 0, 4, 0, 4, 0, 0, 4, 0, 1, 0, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 4, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 },
		{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1 },
		{ 1, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 1 },
		{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
		{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 5, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 4 },
	}
};
std::array<std::array<int, 41>, 21> vis_map_val = {
	{
		{ '0', 0, 0, 0, 0, 3, 0, 2, 0, 1, 0, 0, 0, 0, '0', 0, 0, 0, 63, 0, 62, 0, 61, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '3' },
		{ 0, 0, 0, 0, 0, '3', 0, '2', 0, '1', 0, '0', 0, 0, '1', 0, 1, 0, '\\', 0, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '2', 0, 2, 0, 0, 0, 0, '0', 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '3', 0, 3, 0, 0, 1, 0, '1', 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 2, 0, '2', 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 3, 0, '3', 0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
		{ 12, 0, 11, 0, 10, 0, 9, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 51, 0, 50, 0, 49, 0, 48 },
		{ 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '/', 0, 47 },
		{ 14, 0, 0, 10, 0, 11, 0, 12, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33, 0, 32, 0, 31, 0, 30, 0, 0, 46 },
		{ 15, 0, '/', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45 },
		{ 16, 0, 17, 0, 18, 0, 19, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 0, 41, 0, 42, 0, 43, 0, 44 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 22, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 21, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 20, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, '\\', 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ '1', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 29, 0, 30, 0, 31, 0, 32, 2, 0, 0, 0, 20, 0, 21, 0, 22, 0, 23, 0, 0, 0, 0, '2' },
	}
};

std::string BoardState::to_str() const {
	std::stringstream ss;

	const char field_char = '.';
	const char filler_char = ' ';
	const char blocked_char = '*';

	for (std::size_t row = 0; row != vis_map_spec.size(); row++) {
		for (std::size_t col = 0; col != vis_map_spec.at(0).size(); col++) {
			int spec = vis_map_spec.at(row).at(col);
			int val = vis_map_val.at(row).at(col);

			switch (spec) {
				case 1: {
					const PiecePtr& piece = path.at(val);

					if (piece == nullptr) {
						ss << field_char;
					} else {
						ss << piece->player;
					}

					break;
				}
				case 2: {
					int player = val / 10;
					int finish_idx = val % 10;

					const PiecePtr& piece = finishes.at(player).at(finish_idx);

					if (piece == nullptr) {
						ss << field_char;
					} else {
						ss << piece->player;
					}

					break;
				}
				case 3: {
					int player = val / 10;
					int kennel_idx = val % 10;

					const PiecePtr& piece = kennels.at(player).at(kennel_idx);

					if (piece == nullptr) {
						ss << field_char;
					} else {
						ss << piece->player;
					}

					break;
				}
				case 4: {
					ss << ((char) (val));
					break;
				}
				case 5: {
					int player = val;
					int path_idx = get_start_path_idx(player);
					const PiecePtr& piece = path.at(path_idx);

					if (piece != nullptr && piece->blocking) {
						ss << blocked_char;
					} else {
						ss << filler_char;
					}

					break;
				}
				default: {
					ss << filler_char;
					break;
				}
			}
		}
		ss << std::endl;
	}

	return ss.str();
}

}
