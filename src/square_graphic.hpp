#pragma once
#include <qpl/qpl.hpp>
#include "square.hpp"

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
constexpr auto full_candidates_string() {
	constexpr auto size = N * (N * 2) - 1;
	std::array<char, size + 1> result;
	for (qpl::size i = 0u; i < size; ++i) {
		if (i % 2 == 0) {
			result[i] = qpl::to_string(i)[0];
		}
		else {
			result[i] = ' ';
		}
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
	qsf::text erased_candidates;
	qpl::vector2f position;
	bool is_number = true;
	bool is_number_before = true;
	bool initialized = false;

	constexpr static qpl::vector2f dimension() {
		return { 64, 64 };
	}

	template<qpl::size N>
	void init(qpl::vector2f position) {
		this->rect.set_dimension(this->dimension());
		this->number.set_font(qsf::get_font("font"));
		this->number.set_character_size(40);
		this->number.set_color(qpl::rgb::black());
		this->number.set_string("0");

		this->candidates.set_font(qsf::get_font("font"));
		this->candidates.set_character_size(18);
		this->candidates.set_color(qpl::rgb::black());
		this->candidates.set_string(full_candidates_string<N>().data());

		this->erased_candidates.set_font(qsf::get_font("font"));
		this->erased_candidates.set_character_size(18);
		this->erased_candidates.set_color(qpl::rgb::red());
		this->erased_candidates.set_string(full_candidates_string<N>().data());

		this->set_position(position);

		this->initialized = true;
	}
	template<qpl::size N>
	void update_info(const square<N>& square) {
		this->is_number_before = this->is_number;
		this->is_number = square.number != 0;

		if (this->is_number && this->is_number_before) {
			this->number.set_string(qpl::u32_cast(square.number));
			this->number.set_center(this->center());
		}
		else {
			constexpr auto string = candidates_string<N>();

			std::string erased_text = string.data();
			std::string text = string.data();
			for (qpl::size i = 0u; i < square.candidates.size(); ++i) {
				auto c = square.candidates[i];
				auto before = this->candidates.get_string()[i * 2] != ' ';
				if (c) {
					text[i * 2] = i + 1 + '0';
				}
				else if (before) {
					erased_text[i * 2] = i + 1 + '0';
				}
			}
			this->erased_candidates.set_string(erased_text);
			this->candidates.set_string(text);
		}
	}

	qpl::vector2f center() const {
		return this->position + this->dimension() / 2;
	}

	void set_position(qpl::vector2f position) {
		this->position = position;
		this->rect.set_position(this->position);
		this->candidates.set_center(this->center());
		this->erased_candidates.set_center(this->center());
		this->number.set_center(this->center());
	}

	void draw(qsf::draw_object& draw) const {
		draw.draw(this->rect);
		if (this->is_number && this->is_number_before) {
			draw.draw(this->number);
		}
		else {
			draw.draw(this->candidates);
			draw.draw(this->erased_candidates);
		}
	}
};