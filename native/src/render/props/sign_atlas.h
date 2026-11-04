// The sheet of plate pictures (Roadcraft render/signs.ts SignSheets): each plate, by type and words, gets a cell the moment a
// chunk job asks for it; project/render/sign_sheet.gd paints the cells waiting (fonts and shapes are the canvas's) into the
// texture the "prop" material reads. Square plates and wide name plates fill their own rows; a sheet full gives a plate of
// new words the blank picture of its type.
#pragma once

#include "sim/catalog/furniture_types.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/rect2.hpp>

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace city {

class SignAtlas {
public:
	void configure(int width, int height, int cell, int square_rows, int blade_w, int blade_h); // the sheet, in pixels
	void reset();
	// The plate's rectangle on the sheet, uv 0..1. Any thread.
	godot::Rect2 cell(SignType type, const std::string &text);
	// The cells to paint since the last call, each [type, text, x, y, width, height] in pixels. Main thread.
	godot::Array take_pending();
	int cells_used() const { return square_used_ + wide_used_; }
	int square_used() const { return square_used_; }

private:
	struct Pending {
		SignType type;
		std::string text;
		godot::Rect2 rect;
	};
	std::mutex mutex_;
	std::unordered_map<std::string, godot::Rect2> cells_;
	std::vector<Pending> pending_;
	int square_used_ = 0, wide_used_ = 0;
	int width_ = 1, height_ = 1, cell_ = 1, square_rows_ = 0, blade_w_ = 1, blade_h_ = 1;
};

} // namespace city
