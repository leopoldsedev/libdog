#pragma once

#include <array>
#include <vector>
#include <memory>
#include <utility>
#include <cassert>

#include "Piece.hpp"
#include "PieceRef.hpp"
#include "BoardPosition.hpp"
#include "BoardUtil.hpp"
#include "Notation.hpp"
#include "Constants.hpp"
#include "Action.hpp"


namespace libdog {

using BoardStateRepr = std::array<int, PIECE_COUNT * PLAYER_COUNT>;

class OneStep {
	public:
		PiecePtr piece;
		BoardPosition previous_position;
		bool was_blocking;
		PiecePtr previous_occupant;

		OneStep(PiecePtr piece, BoardPosition previous_position, bool was_blocking,  PiecePtr previous_occupant) : piece(piece), previous_position(previous_position), was_blocking(was_blocking), previous_occupant(previous_occupant) {;
		}
};

class BoardState {
	public:
		// TODO Think about a way to properly abstract away the fact that after the last path index the first path index begins again

		std::array<std::array<Piece, PIECE_COUNT>, PLAYER_COUNT> pieces;

		std::array<PiecePtr, PATH_LENGTH> path;
		std::array<std::array<PiecePtr, FINISH_LENGTH>, PLAYER_COUNT> finishes;
		std::array<std::array<PiecePtr, KENNEL_SIZE>, PLAYER_COUNT> kennels;

		std::vector<OneStep> one_step_undo_stack;
		bool undo_stack_activated = false;

		BoardState();

		BoardState(const BoardState& other);

		BoardState& operator=(const BoardState& other);

		friend bool operator==(const BoardState& a, const BoardState& b);

		void reset();

		PiecePtr ref_to_piece(const PieceRef& piece_ref) const;

		PiecePtr& ref_to_piece_ptr_ref(const PieceRef& piece_ref);

		BoardPosition ref_to_pos(const PieceRef& piece_ref) const;

		bool get_kennel_piece(int player, PiecePtr** result);

		void start_piece(PiecePtr& piece);

		PiecePtr& get_piece(int path_idx);

		const PiecePtr& get_piece(BoardPosition position) const;

		PiecePtr& get_piece(BoardPosition position);

		PiecePtr& get_start(int player);

		void send_to_kennel(int from_path_idx, int count);

		void place_at_kennel(PiecePtr& piece);

		void move_piece(PiecePtr& piece, BoardPosition position, bool blocking = false);

		bool move_piece(PiecePtr& piece, int count, bool avoid_finish, bool modify_state, bool remove_all_on_way);

		void undo_one_step();

		void swap_pieces(PiecePtr& piece1, PiecePtr& piece2);

		bool check_finish_full(int player);

		std::vector<PieceRef> get_pieces_in_area(int player, Area area);

		bool start_piece(int player, bool modify_state = true);

		bool move_multiple_pieces(std::vector<MoveSpecifier> move_actions, bool modify_state);

		bool swap_pieces(PiecePtr& piece_1, PiecePtr& piece_2, bool modify_state);

		bool try_enter_finish(int player, int from_path_idx, int count, bool piece_blocking, BoardPosition& position_result, int& count_on_path_result);

		BoardStateRepr get_repr() const;

		bool check_state() const;

		friend std::ostream& operator<<(std::ostream& os, BoardState const& obj) {
			  return os << obj.to_str();
		}

	private:
		bool check_move_on_path(int from_path_idx, int count, BoardPosition& position_result);

		bool check_move_in_finish(int player, int from_finish_idx, int count, BoardPosition& position_result);

		int calc_steps_into_finish(int player, int from_path_idx, int count, bool piece_blocking, int& count_on_path_result);

		int possible_forward_steps_in_finish(int player, int from_finish_idx);

		bool possible_one_step_on_path(int from_path_idx, bool backwards);

		int possible_forward_steps_on_path(int from_path_idx, bool backwards);

		int possible_steps_of_piece(int player, BoardPosition position, bool piece_blocking, bool backwards);

		bool check_block(int from_path_idx, int count);

		bool move_multiple_pieces_naive(std::vector<MoveSpecifier> move_actions);

		std::string to_str() const;
};

}
