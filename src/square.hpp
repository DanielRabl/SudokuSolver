#pragma once

#include <qpl/qpl.hpp>

template<qpl::size N>
using digit_type = qpl::ubit<qpl::log2(N)>;

template<qpl::size N>
using bit_type = qpl::bitset<qpl::pow(N, 2)>;

template<qpl::size N>
struct square {
	bit_type<N> candidates;
	digit_type<N> number = 0;

	square() {
		this->clear();
	}
	void clear() {
		this->number = 0u;
		this->candidates.fill(1);
	}
	operator bool() const {
		return qpl::bool_cast(this->number);
	}
};