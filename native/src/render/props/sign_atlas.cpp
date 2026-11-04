// The sheet of plate pictures, see sign_atlas.h. Sizes in pixels, from data/render/streets.json; so is project/render/sign_sheet.gd.
#include "render/props/sign_atlas.h"

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {
bool wide(SignType t) { return t == SignType::STREET || t == SignType::DIRECTION; }
std::string key_of(SignType t, const std::string &text) { return std::to_string(int(t)) + "|" + text; }
} // namespace

void SignAtlas::configure(int width, int height, int cell, int square_rows, int blade_w, int blade_h) {
	width_ = width;
	height_ = height;
	cell_ = cell;
	square_rows_ = square_rows;
	blade_w_ = blade_w;
	blade_h_ = blade_h;
}

void SignAtlas::reset() {
	std::lock_guard<std::mutex> lk(mutex_);
	cells_.clear();
	pending_.clear();
	square_used_ = wide_used_ = 0;
}

Rect2 SignAtlas::cell(SignType type, const std::string &text) {
	std::lock_guard<std::mutex> lk(mutex_);
	const std::string key = key_of(type, text);
	auto it = cells_.find(key);
	if (it != cells_.end()) return it->second;
	const bool is_wide = wide(type);
	const int w = is_wide ? blade_w_ : cell_, h = is_wide ? blade_h_ : cell_, columns = width_ / w;
	const int capacity = columns * (is_wide ? (height_ - square_rows_ * cell_) / h : square_rows_);
	int &used = is_wide ? wide_used_ : square_used_;
	if (used >= capacity - 1) { // the last cell stays for the blank plate of a full sheet
		const std::string blank = key_of(type, "");
		auto b = cells_.find(blank);
		if (b != cells_.end()) return b->second;
	}
	const int x = (used % columns) * w, y = (is_wide ? square_rows_ * cell_ : 0) + (used / columns) * h;
	used++;
	const Rect2 px = Rect2(float(x), float(y), float(w), float(h));
	pending_.push_back({ type, text, px });
	const float sheet_w = float(width_), sheet_h = float(height_);
	const Rect2 uv(px.position.x / sheet_w, px.position.y / sheet_h, px.size.x / sheet_w, px.size.y / sheet_h);
	cells_[key] = uv;
	return uv;
}

Array SignAtlas::take_pending() {
	std::lock_guard<std::mutex> lk(mutex_);
	Array out;
	for (const Pending &p : pending_) {
		Array one;
		one.push_back(int(p.type));
		one.push_back(String::utf8(p.text.c_str()));
		one.push_back(p.rect);
		out.push_back(one);
	}
	pending_.clear();
	return out;
}

} // namespace city
