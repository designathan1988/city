// The sheets of plate pictures (Roadcraft render/signs.ts SignSheets, which opens another sheet when one fills): each plate, by
// type and words (the player's, or a given name's translation keys), gets a cell the moment a chunk job asks for it, on the layer
// being filled, the next one when that is full. project/render/sign_sheet.gd paints the cells waiting (fonts and shapes are the
// canvas's, the words in the player's language) into the layers of the array the "prop" material reads. Square plates and wide
// name plates fill shelves of their own height.
#pragma once

#include "sim/catalog/furniture_types.h"
#include "sim/street/street_works.h"

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
	void reset(); // a new city: the sheets start empty, under a new generation
	SignCell cell(SignType type, const std::string &text, const GivenName &given); // any thread
	// The cells to paint since the last call, each [type, text, rectangle in pixels, layer, generation, form, name, ordinal] (the
	// last three a given name's, GivenName). Main thread. A cell of a newer generation than the sheets painted so far: they hold
	// another city's plates, cleared before it is painted.
	godot::Array take_pending();
	int cells_used() const { return used_; }
	int64_t generation() const {
		std::lock_guard<std::mutex> lk(mutex_);
		return generation_;
	}

private:
	struct Pending {
		SignType type;
		std::string text;
		GivenName given;
		godot::Rect2 rect;
		int layer;
	};
	struct Shelf {
		int x = 0, y = 0;
		bool open = false;
	};
	godot::Rect2i place(bool wide);
	mutable std::mutex mutex_;
	std::unordered_map<std::string, SignCell> cells_;
	std::vector<Pending> pending_;
	Shelf shelf_[2]; // square, wide
	int layer_ = 0, top_ = 0, used_ = 0; // the layer being filled and how far down its shelves reach
	int64_t generation_ = 0; // moved by every reset
	int width_ = 1, height_ = 1, cell_ = 1, blade_w_ = 1, blade_h_ = 1;
};

} // namespace city
