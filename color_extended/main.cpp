#include <qpl/qpl.hpp>

struct color {
	qsf::rectangle rect;
	qpl::rgb original;
	qpl::rgb target;

	color() {
		this->rect.set_dimension({ 100, 100 });
	}
	void update() {
		this->rect.set_color(this->target);
	}
	void apply() {
		this->target = this->original;
		this->update();
	}

	void draw(qsf::draw_object& draw) const {
		draw.draw(this->rect);
	}
};

struct main_state : qsf::base_state {
	void init() override {

		auto n = 12;
		this->colors.resize(n);
		
		for (qpl::size i = 0u; i < this->colors.size(); ++i) {
			auto x = i % 6;
			auto y = i / 6;

			auto offset = qpl::vec(10, 10);
			this->colors[i].rect.set_position((qpl::vec(100, 100) + offset) * qpl::vec(x, y) + offset);
		}
		this->find_distant_colors();

		this->saturation_slider.set_range(-1.0, 1.0, 0.0);
		this->saturation_slider.set_dimensions({ 500, 30 }, { 30, 30 });
		this->saturation_slider.set_position({ 10, 400 });

		this->hue_slider.set_range(0.0, 1.0, 0.0);
		this->hue_slider.set_dimensions({ 500, 30 }, { 30, 30 });
		this->hue_slider.set_position({ 10, 450 });

		this->distance_slider.set_range(0.0, 2.0, 0.0);
		this->distance_slider.set_dimensions({ 500, 30 }, { 30, 30 });
		this->distance_slider.set_position({ 10, 500 });

		this->check_box.set_font("font");
		this->check_box.set_position({ 30, 550 });
	}
	void find_distant_colors() {

		qpl::size global_attempts = 0u;
		qpl::size attempts = 0u;
		std::vector<qpl::rgb> colors(this->colors.size());
		auto find_distance = this->distance_slider.get_value();

		for (qpl::size i = 0u; i < colors.size(); ) {

			bool found = false;
			while (true) {
				colors[i] = qpl::get_random_color();

				auto min_distance = qpl::f64_max;
				for (qpl::size j = 0u; j < i; ++j) {
					auto distance = colors[i].get_adjusted_difference(colors[j]);
					min_distance = qpl::min(min_distance, distance);
				}
				++global_attempts;
				++attempts;

				if (min_distance > find_distance) {
					found = true;
					break;
				}
				if (attempts > 1'000) {
					attempts = 0u;
					break;
				}
			}

			if (found) {
				++i;
			}
			else {
				if (!i) {
					qpl::println("no solution found");
					break;
				}
				constexpr auto stop = 10'000'000u;
				if (global_attempts > stop) {
					qpl::println("exceeded ", stop, " attempts");

					for (auto& i : this->colors) {
						i.original = qpl::rgb::grey_shade(100);
						i.apply();
					}
					return;
				}
				--i;
			}
		}

		qpl::println("global attempts = ", qpl::big_number_string(global_attempts));

		for (qpl::size i = 0u; i < colors.size(); ++i) {
			this->colors[i].original = colors[i];
			this->colors[i].apply();
		}
	}
	void randomize() {
		for (auto& i : this->colors) {
			i.original = qpl::get_random_color();
			i.apply();
		}
	}
	void apply() {
		auto adjusted = this->check_box.get_value();
		//qpl::println(qpl::bool_string(adjusted));

		for (auto& i : this->colors) {

			auto saturation = this->saturation_slider.get_value();
			auto hue = this->hue_slider.get_value();
			
			if (adjusted) {
				i.target = i.original.saturated_adjusted(saturation).with_added_hue(hue);
			}
			else {
				i.target = i.original.saturated(saturation).with_added_hue(hue);
			}
			//qpl::println("brightness = ", i.target.get_adjusted_brightness());
			qpl::println("hue = ", i.target.get_hue(), " - ", hue, " = ", i.target.get_hue() - hue, " - intensity = ", i.target.get_intensity());

			i.update();
		}
	}
	void updating() override {
		this->update(this->saturation_slider);
		this->update(this->hue_slider);
		this->update(this->distance_slider);
		this->update(this->check_box);


		bool update_apply =
			this->saturation_slider.value_was_modified() ||
			this->hue_slider.value_was_modified() ||
			this->check_box.value_was_modified();

		if (this->distance_slider.value_was_modified()) {
			this->find_distant_colors();
			this->apply();
		}

		if (update_apply) {
			this->apply();
		}
		if (this->event().key_holding(sf::Keyboard::C)) {
			for (auto& i : this->colors) {
				i.original = qpl::get_rainbow_color(std::fmod(this->run_time().secs_f() / 10, 1.0));

				this->apply();
			}
		}

		if (this->event().key_pressed(sf::Keyboard::Space)) {
			this->randomize();
			this->apply();
		}
		if (this->event().key_pressed(sf::Keyboard::Space)) {
			this->find_distant_colors();
			this->apply();
		}
	}
	void drawing() override {
		this->draw(this->colors);
		this->draw(this->saturation_slider);
		this->draw(this->hue_slider);
		this->draw(this->distance_slider);
		this->draw(this->check_box);
	}

	std::vector<color> colors;
	qsf::slider<qpl::f64> saturation_slider;
	qsf::slider<qpl::f64> hue_slider;
	qsf::slider<qpl::f64> distance_slider;
	qsf::check_box check_box;
};

int main() try {
	qsf::framework framework;
	framework.set_title("QPL");
	framework.add_font("font", "resources/arial.ttf");
	framework.set_dimension({ 1400u, 950u });

	framework.add_state<main_state>();
	framework.game_loop();
}
catch (std::exception& any) {
	qpl::println("caught exception:\n", any.what());
	qpl::system_pause();
}
