#pragma once

#include <qpl/qpl.hpp>
#include "cell.hpp"

template<qpl::size N>
struct grid {
	qpl::array<cell<N>, qpl::pow(N, 2)> cells;
	qpl::size solve_step_ctr = 0u;
	qpl::size last_solve_step = 0u;

	constexpr static qpl::size size() {
		return qpl::pow(N, 4);
	}
	constexpr static qpl::size row_size() {
		return qpl::pow(N, 2);
	}
	constexpr static qpl::size column_size() {
		return qpl::pow(N, 2);
	}
	auto& get_cell(qpl::size x, qpl::size y) {
		auto cx = x / N;
		auto cy = y / N;
		auto index = cy * N + cx;
		return this->cells[index];
	}
	const auto& get_cell(qpl::size x, qpl::size y) const {
		auto cx = x / N;
		auto cy = y / N;
		auto index = cy * N + cx;
		return this->cells[index];
	}
	auto& get_square(qpl::size x, qpl::size y) {
		auto sx = x % N;
		auto sy = y % N;
		auto index = sy * N + sx;

		return this->get_cell(x, y)[index];
	}
	void set_square(qpl::size x, qpl::size y, digit_type<N> digit) {
		auto sx = x % N;
		auto sy = y % N;
		auto index = sy * N + sx;

		this->get_cell(x, y).set(index, digit);
	}
	const auto& get_square(qpl::size x, qpl::size y) const {
		auto sx = x % N;
		auto sy = y % N;
		auto index = sy * N + sx;

		return this->get_cell(x, y)[index];
	}
	void reset() {
		this->solve_step_ctr = 0u;
		this->last_solve_step = 0u;
		this->clear();
	}
	void clear() {
		for (auto& cell : this->cells) {
			cell.clear();
		}
	}
	bool stuck() const {
		return (this->solve_step_ctr - this->last_solve_step) > 20;
	}
	bool filled() const {
		for (auto& i : this->cells) {
			if (!i.filled()) {
				return false;
			}
		}
		return true;
	}
	bool valid_rows() const {
		for (qpl::size y = 0u; y < column_size(); ++y) {
			auto bits = this->get_digits_row(y);
			if (!bits.full()) {
				return false;
			}
		}
		return true;
	}
	bool valid_columns() const {
		for (qpl::size x = 0u; x < row_size(); ++x) {
			auto bits = this->get_digits_column(x);
			if (!bits.full()) {
				return false;
			}
		}
		return true;
	}
	bool valid_rows_columns() const {
		return this->valid_columns() && this->valid_rows();
	}

	bool valid_cells() const {
		for (auto& i : this->cells) {
			if (!i.valid()) {
				return false;
			}
		}
		return true;
	}
	bool valid() const {
		return this->valid_cells() && this->valid_rows_columns();
	}
	bool duplicate_search_row(qpl::size x, qpl::size y, digit_type<N> number) const {
		for (qpl::size i = 0u; i < row_size(); ++i) {
			if ((x / N) == (i / N)) {
				continue;
			}
			if (this->get_square(i, y).number == number) {
				return true;
			}
		}
		return false;
	}
	bool duplicate_search_column(qpl::size x, qpl::size y, digit_type<N> number) const {
		for (qpl::size i = 0u; i < row_size(); ++i) {
			if ((y / N) == (i / N)) {
				continue;
			}
			if (this->get_square(x, i).number == number) {
				return true;
			}
		}
		return false;
	}
	bool duplicate_search(qpl::size x, qpl::size y, digit_type<N> number) const {
		return this->duplicate_search_row(x, y, number) || this->duplicate_search_column(x, y, number);
	}
	void fill(qpl::size numbers) {
		qpl::size ctr = 0u;

		while (ctr < numbers) {
			auto x = qpl::random<qpl::size>(0u, qpl::pow(N, 2) - 1);
			auto y = qpl::random<qpl::size>(0u, qpl::pow(N, 2) - 1);

			if (this->get_square(x, y)) {
				continue;
			}
			auto random_digit = qpl::type_cast<digit_type<N>>(qpl::random<qpl::size>(1u, cell<N>::size()));

			if (this->get_cell(x, y).find(random_digit)) {
				continue;
			}
			if (this->duplicate_search(x, y, random_digit)) {
				continue;
			}
			this->set_square(x, y, random_digit);
			++ctr;
		}
	}
	bit_type<N> get_digits_row(qpl::size y) const {
		bit_type<N> result;
		for (qpl::size x = 0u; x < N; ++x) {
			auto index = (y / N) * N + x;
			result |= this->cells[index].get_digits_row(y % N);
		}
		return result;
	}
	bit_type<N> get_digits_column(qpl::size x) const {
		bit_type<N> result;
		for (qpl::size y = 0u; y < N; ++y) {
			auto index = (y * N) + (x / N);
			result |= this->cells[index].get_digits_column(x % N);
		}
		return result;
	}

	void reduce_row_candidates() {
		for (qpl::size y = 0u; y < column_size(); ++y) {
			auto bits = this->get_digits_row(y);
			for (qpl::size i = 0u; i < qpl::pow(N, 2); ++i) {
				this->get_square(i, y).candidates.data &= (~bits.data);
			}
		}
	}
	void reduce_column_candidates() {
		for (qpl::size x = 0u; x < row_size(); ++x) {
			auto bits = this->get_digits_column(x);
			for (qpl::size i = 0u; i < qpl::pow(N, 2); ++i) {
				this->get_square(x, i).candidates.data &= (~bits.data);
			}
		}
	}
	void reduce_candidates() {
		this->reduce_cell_candidates();
		this->reduce_xy_candidates();
	}
	void reduce_cell_candidates() {
		//qpl::println("reduce_cell_candidates()");
		for (auto& cell : this->cells) {
			cell.reduce_cell_candidates();
		}
	}
	void reduce_rows_columns_candidates() {
		//qpl::println("reduce_rows_columns_candidates()");
		this->reduce_row_candidates();
		this->reduce_column_candidates();
	}
	void detect_naked_singles() {
		//qpl::println("detect_naked_singles()");
		for (auto& i : this->cells) {
			i.detect_naked_singles();
		}
	}
	bool find_singles() {
		//qpl::println("find_singles()");
		bool found = false;
		for (auto& i : this->cells) {
			if (i.find_singles()) {
				found = true;
			}
		}
		return found;
	}
	void solve_step() {
		if (this->solve_step_ctr % 2 == 0) {
			switch ((this->solve_step_ctr / 2) % 3) {
			case 0u: this->reduce_cell_candidates(); break;
			case 1u: this->reduce_rows_columns_candidates(); break;
			case 2u: this->detect_naked_singles(); break;
			}
		}
		else {
			if (this->find_singles()) {
				this->last_solve_step = this->solve_step_ctr;
			}
		}
		++this->solve_step_ctr;
	}

};