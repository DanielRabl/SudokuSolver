#pragma once

#include <qpl/qpl.hpp>
#include "square_graphic.hpp"
#include "grid.hpp"

struct grid_graphic {
	std::vector<square_graphic> squares;

	void reset() {
		for (auto& i : this->squares) {
			i.is_number = i.is_number_before = true;
		}
	}

	template<qpl::size N>
	void update(const grid<N>& field) {
		this->squares.resize(field.size());

		for (qpl::size c = 0u; c < field.cells.size(); ++c) {
			for (qpl::size i = 0u; i < field.cells[c].size(); ++i) {
				auto index = c * cell<N>::size() + i;
				auto& square = this->squares[index];
				auto initialized = square.initialized;
				if (!initialized) {
					auto cx = c % N;
					auto cy = c / N;

					auto x = (i % N) + (cx * N);
					auto y = (i / N) + (cy * N);

					auto cell_off = qpl::vec(cx, cy) * 5;
					auto normal_off = qpl::vec(2, 2);

					auto offset = qpl::vec(50, 50);
					auto position = offset + qpl::vec(x, y) * (square_graphic::dimension() + normal_off) + cell_off;
					square.init<N>(position);
				}
				square.update_info(field.cells[c][i]);
			}
		}
	}

	void draw(qsf::draw_object& draw) const {
		draw.draw(this->squares);
	}
};