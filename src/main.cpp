#include <qpl/qpl.hpp>
#include "grid.hpp"
#include "grid_graphic.hpp"


constexpr auto N = 3;
constexpr auto FILL = 30;

struct main_state : qsf::base_state {

	void reset() {
		this->current_grid.reset();
		this->current_grid = this->created_grid;
		this->current_grid.solve_step_ctr = 0u;

		this->grid_graphic.reset();
		this->grid_graphic.update(this->current_grid);
		this->step_ctr = 0u;
	}
	void randomize() {
		this->created_grid.reset();
		this->created_grid.fill(FILL);
		this->reset();
	}
	void set_valid() {
		this->created_grid.reset();
		this->created_grid = this->get_valid_grid();
		this->reset();
	}
	void benchmark_fill() {
		qpl::small_clock clock;
		qpl::size ctr = 0u;
		while (true) {
			this->current_grid.reset();
			this->current_grid.fill(FILL);
			++ctr;

			if (qpl::get_time_signal(0.5)) {
				qpl::println(qpl::big_number_string(ctr / clock.elapsed_f()), " / sec");
			}
		}
	}

	grid<N> get_valid_grid() const {
		grid<N> created;
		grid<N> grid;

		while (true) {
			created.reset();
			created.fill(FILL);
			grid = created;

			while (true) {
				grid.solve_step();

				if (grid.stuck()) {
					break;
				}
				if (grid.filled() && grid.valid()) {
					return created;
				}
			}
		}
		return grid;
	}

	void benchmark_solve() {
		qpl::small_clock clock;
		qpl::size ctr = 0u;

		qpl::size filled_ctr = 0u;
		qpl::size stuck_ctr = 0u;

		grid<N> grid;
		while (true) {
			grid.reset();
			grid.fill(FILL);

			while (true) {
				grid.solve_step();

				if (grid.filled()) {
					if (grid.valid()) {
						++filled_ctr;
						break;
					}
				}
				if (grid.stuck()) {
					++stuck_ctr;
					break;
				}
			}

			++ctr;

			if (qpl::get_time_signal(0.5)) {
				qpl::println(
					qpl::big_number_string(ctr / clock.elapsed_f()), " / sec ",
					qpl::big_number_string(filled_ctr), " filled - ",
					qpl::big_number_string(stuck_ctr), " stuck - "
				);

			}
		}
	}

	void init() override {
		this->randomize();
		//this->set_valid();
		//this->benchmark_solve();
	}
	void next_step() {
		if (this->step_ctr % 2 == 0) {
			this->current_grid.solve_step();
		}
		this->grid_graphic.update(this->current_grid);
		++this->step_ctr;
	}
	void updating() override {
		if (this->event().key_pressed(sf::Keyboard::C)) {
			this->randomize();
		}
		if (this->event().key_pressed(sf::Keyboard::V)) {
			this->set_valid();
		}
		if (this->event().key_pressed(sf::Keyboard::R)) {
			this->reset();
		}
		if (this->event().key_pressed(sf::Keyboard::Space)) {
			this->next_step();
		}
	}
	void drawing() override {
		this->draw(this->grid_graphic);
	}
	qpl::size step_ctr = 0u;
	grid<N> created_grid;
	grid<N> current_grid;
	grid_graphic grid_graphic;
};

int main() try {
	qsf::framework framework;
	framework.set_title("QPL");
	framework.add_font("font", "resources/consola.ttf");
	framework.set_dimension({ 1400u, 950u });

	framework.add_state<main_state>();
	framework.game_loop();
}
catch (std::exception& any) {
	qpl::println("caught exception:\n", any.what());
	qpl::system_pause();
}
