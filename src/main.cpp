#include <qpl/qpl.hpp>



template<qpl::size N>
struct square {
	using utype = qpl::ubit<qpl::log2(N)>;

	utype number = 0;
	qpl::bitset<N * N> candidates;

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
template<qpl::size N>
struct cell {
	qpl::array<square<N>, qpl::pow(N, 2)> squares;

	bool find(square<N>::utype number) const {
		for (auto& i : this->squares) {
			if (i == number) {
				return true;
			}
		}
		return false;
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
	}
};
template<qpl::size N>
struct field {
	qpl::array<cell<N>, qpl::pow(N, 2)> cells;

	constexpr qpl::size size() const {
		return qpl::pow(N, 4);
	}
	auto& get_cell(qpl::size x, qpl::size y) {
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
	void clear() {
		for (auto& cell : this->cells) {
			cell.clear();
		}
	}
	void fill(qpl::size numbers) {
		qpl::size ctr = 0u;

		while (ctr < numbers) {
			auto x = qpl::random<qpl::size>(0u, (N * N) - 1);
			auto y = qpl::random<qpl::size>(0u, (N * N) - 1);


			if (this->get_square(x, y)) {
				continue;
			}
			auto random_digit = qpl::random<qpl::size>(0u, cell<N>::size() - 1);

			if (!this->get_cell(x, y).find(random_digit)) {
				this->get_square(x, y).number = random_digit;
				++ctr;
			}
		}
	}
};


template<qpl::size N>
constexpr auto candidates_string() {
	constexpr auto size = N * (N * 2) - 1;
	std::array<char, size + 1> result;
	for (qpl::size i = 0u; i < size; ++i) {
		result[i] = ' ';
		if (i % (N * 2) == (N * 2) - 1) {
			result[i] = '\n';
		}
	}
	result[size] = '\0';
	return result;
}

struct square_graphic {
	qsf::rectangle rect;
	qsf::text number;
	qsf::text candidates;
	bool is_number = true;
	bool initialized = false;

	constexpr static qpl::vector2f dimension() {
		return { 64, 64 };
	}

	void init() {
		this->rect.set_dimension(this->dimension());
		this->number.set_font(qsf::get_font("font"));
		this->candidates.set_font(qsf::get_font("font"));
		this->number.set_character_size(40);
		this->candidates.set_character_size(18);

		this->number.set_color(qpl::rgb::black());
		this->candidates.set_color(qpl::rgb::black());
		this->initialized = true;
	}
	template<qpl::size N>
	void update_info(const square<N>& square) {
		this->is_number = square.number != 0;

		if (this->is_number) {
			this->number.set_string(qpl::u32_cast(square.number));
		}
		else {
			constexpr auto string = candidates_string<N>();
			std::string text = string.data();
			for (qpl::size i = 0u; i < square.candidates.size(); ++i) {
				auto c = square.candidates[i];
				if (c) {
					text[i * 2] = i + 1 + '0';
				}
			}
			this->candidates.set_string(text);
		}
	}
	void set_position(qpl::vector2f position) {
		auto center = position + this->dimension() / 2;
		this->rect.set_position(position);
		this->candidates.set_center(center);
		this->number.set_center(center);
	}

	void draw(qsf::draw_object& draw) const {
		draw.draw(this->rect);
		if (this->is_number) {
			draw.draw(this->number);
		}
		else {
			draw.draw(this->candidates);
		}
	}
};
struct field_graphic {
	std::vector<square_graphic> squares;

	template<qpl::size N>
	void create(const field<N>& field) {
		this->squares.resize(field.size());

		for (qpl::size c = 0u; c < field.cells.size(); ++c) {
			for (qpl::size i = 0u; i < field.cells[c].size(); ++i) {
				auto index = c * cell<N>::size() + i;
				auto& square = this->squares[index];
				if (!square.initialized) {
					square.init();
				}
				square.update_info(field.cells[c][i]);

				auto cx = c % N;
				auto cy = c / N;

				auto x = (i % N) + (cx * N);
				auto y = (i / N) + (cy * N);

				auto cell_off = qpl::vec(cx, cy) * 5;
				auto normal_off = qpl::vec(2, 2);

				auto offset = qpl::vec(50, 50);
				square.set_position(offset + qpl::vec(x, y) * (square_graphic::dimension() + normal_off) + cell_off);
			}
		}
	}

	void draw(qsf::draw_object& draw) const {
		draw.draw(this->squares);
	}
};
constexpr auto N_SIZE = 3;

struct main_state : qsf::base_state {

	void randomize() {
		this->field.clear();
		this->field.fill(20);
		this->field_graphic.create(this->field);
	}
	void benchmark() {
		qpl::small_clock clock;
		qpl::size ctr = 0u;
		while (true) {
			this->field.clear();
			this->field.fill(20);
			++ctr;

			if (qpl::get_time_signal(0.5)) {
				qpl::println(qpl::big_number_string(ctr / clock.elapsed_f()), " / sec");
			}
		}
	}

	void init() override {
		this->randomize();

	}
	void updating() override {
		if (this->event().key_pressed(sf::Keyboard::X)) {
			this->randomize();
		}
	}
	void drawing() override {
		this->draw(this->field_graphic);
	}
	field<N_SIZE> field;
	field_graphic field_graphic;
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
