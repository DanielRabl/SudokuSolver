#include <qpl/qpl.hpp>

template<qpl::size N>
struct square {
	qpl::ubit<qpl::log2(N)> number = 0;
	qpl::bitset<N * N> candidates;

	square() {
		this->candidates.fill(1);
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

template<qpl::size N>
struct field {
	std::array<square<N>, qpl::pow(N, 4)> squares;
};

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
	std::vector< square_graphic> squares;

	template<qpl::size N>
	void create(const field<N>& field) {
		this->squares.resize(field.squares.size());

		for (qpl::size i = 0u; i < field.squares.size(); ++i) {
			auto& square = this->squares[i];
			if (!square.initialized) {
				square.init();
			}
			square.update_info(field.squares[i]);

			auto x = (i % (N * N));
			auto y = (i / (N * N));

			auto cell_x = x / N;
			auto cell_y = y / N;

			auto cell_off = qpl::vec(cell_x, cell_y) * 5;
			auto normal_off = qpl::vec(2, 2);

			auto offset = qpl::vec(50, 50);
			square.set_position(offset + qpl::vec(x, y) * (square_graphic::dimension() + normal_off) + cell_off);
		}
	}

	void draw(qsf::draw_object& draw) const {
		draw.draw(this->squares);
	}
};
constexpr auto N_SIZE = 3;

struct main_state : qsf::base_state {

	void init() override {
		for (auto& i : this->field.squares) {
			i.number = qpl::random(0, N_SIZE * N_SIZE);
		}
		this->field_graphic.create(this->field);
	}
	void updating() override {

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
