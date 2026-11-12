// The sheets of plate pictures (Roadcraft render/signs.ts SignSheets, which opens another sheet when one fills): each plate, by
// type and words, gets a cell the moment a chunk job asks for it, on the layer being filled, the next one when that is full.
// project/render/sign_sheet.gd paints the cells waiting (fonts and shapes are the canvas's) into the layers of the array the
// "prop" material reads. Square plates and wide name plates fill shelves of their own height.
#pragma once

#include "sim/catalog/furniture_types.h"
#include "core/generation.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/rect2.hpp>

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace city {

// Where a plate's picture is: its rectangle on its layer, uv 0..1.
struct SignCell {
	godot::Rect2 uv;
	int layer = 0;
};

class SignAtlas {
public:
	void configure(int width, int height, int cell, int blade_w, int blade_h); // a layer and its cells, in pixels
	uint64_t generation() const { return generation_; }
	SignCell cell(SignType type, const std::string &text); // any thread
	// The cells to paint since the last call, each [type, text, rectangle in pixels, layer]. Main thread.
	godot::Array take_pending();
	int cells_used() const { std::lock_guard<std::mutex> lk(mutex_); return used_; }

private:
	struct Pending {
		SignType type;
		std::string text;
		godot::Rect2 rect;
		int layer;
	};
	struct Shelf {
		int x = 0, y = 0;
		bool open = false;
	};
	godot::Rect2i place(bool wide);
	const uint64_t generation_ = next_generation();
	mutable std::mutex mutex_;
	std::unordered_map<std::string, SignCell> cells_;
	std::vector<Pending> pending_;
	Shelf shelf_[2]; // square, wide
	int layer_ = 0, top_ = 0, used_ = 0; // the layer being filled and how far down its shelves reach
	int width_ = 1, height_ = 1, cell_ = 1, blade_w_ = 1, blade_h_ = 1;
};

} // namespace city
