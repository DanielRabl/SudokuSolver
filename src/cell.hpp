#pragma once

#include <qpl/qpl.hpp>
#include "square.hpp"

template<qpl::size N>
struct cell {
	qpl::array<square<N>, qpl::pow(N, 2)> squares;
	bit_type<N> numbers;

	bool find(digit_type<N> number) const {
		return this->numbers[number - 1];
	}
	void set(qpl::size index, digit_type<N> number) {
		this->numbers[number - 1] = true;
		this->squares[index].number = number;
	}
	bool filled() const {
		return this->numbers.full();
	}
	bool valid() const {
		return this->numbers.data == bit_type<N>::last_bits_mask();
	}
	bit_type<N> get_digits_row(qpl::size y) const {
		bit_type<N> result;
		for (qpl::size i = 0u; i < N; ++i) {
			auto index = y * N + i;
			if (this->squares[index]) {
				result[this->squares[index].number - 1] = true;
			}
		}
		return result;
	}
	bit_type<N> get_digits_column(qpl::size x) const {
		bit_type<N> result;
		for (qpl::size i = 0u; i < N; ++i) {
			auto index = i * N + x;
			if (this->squares[index]) {
				result[this->squares[index].number - 1] = true;
			}
		}
		return result;
	}

	void reduce_cell_candidates() {
		for (auto& i : squares) {
			i.candidates.data &= (~this->numbers.data);
		}
	}
	auto get_candidates_count(digit_type<N> digit) const {
		qpl::size result = 0u;
		for (qpl::size i = 0u; i < this->squares.size(); ++i) {
			if (this->squares[i].candidates[digit]) {
				++result;
			}
		}
		return result;
	}
	void detect_naked_single(digit_type<N> digit) {
		if (this->get_candidates_count(digit) != 1) {
			return;
		}
		for (qpl::size i = 0u; i < this->squares.size(); ++i) {
			if (this->squares[i].candidates[digit]) {
				this->squares[i].candidates.data = (1 << digit);
				return;
			}
		}
	}
	void detect_naked_singles() {
		for (qpl::size i = 0u; i < this->numbers.size(); ++i) {
			if (!this->numbers[i]) {
				this->detect_naked_single(qpl::type_cast<digit_type<N>>(i));
			}
		}
	}
	bool find_singles() {
		bool found = false;
		for (auto& i : this->squares) {
			if (!i && i.candidates.number_of_set_bits() == 1u) {
				auto n = i.candidates.significant_bit();
				i.number = qpl::type_cast<digit_type<N>>(n);
				i.candidates.clear();
				this->numbers[n - 1] = true;
				found = true;
			}
		}
		return found;
	}


	std::string string() const {
		return qpl::to_string_specified(this->squares, [](auto a) {
			return qpl::u32_cast(a.number);
		});
	}

	constexpr auto begin() {
		return this->squares.begin();
	}
	constexpr auto begin() const {
		return this->squares.cbegin();
	}
	constexpr auto cbegin() {
		return this->squares.cbegin();
	}
	constexpr auto end() {
		return this->squares.end();
	}
	constexpr auto end() const {
		return this->squares.cend();
	}
	constexpr auto cend() {
		return this->squares.cend();
	}
	constexpr static qpl::size size() {
		return qpl::pow(N, 2);
	}
	constexpr auto& operator[](qpl::size index) {
		return this->squares[index];
	}
	constexpr const auto& operator[](qpl::size index) const {
		return this->squares[index];
	}
	constexpr void clear() {
		for (auto& i : this->squares) {
			i.clear();
		}
		this->numbers.clear();
	}
};