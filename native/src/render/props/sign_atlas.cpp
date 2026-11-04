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

void SignAtlas::configure(int width, int height, int cell, int square_rows) {
	width_ = width;
	height_ = height;
	cell_ = cell;
	square_rows_ = square_rows;
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
	const int columns = width_ / cell_, blade = cell_ / 2;
	const int capacity = columns * (is_wide ? (height_ - square_rows_ * cell_) / blade : square_rows_);
	int &used = is_wide ? wide_used_ : square_used_;
	if (used >= capacity - 1) { // the last cell stays for the blank plate of a full sheet
		const std::string blank = key_of(type, "");
		auto b = cells_.find(blank);
		if (b != cells_.end()) return b->second;
	}
	const int x = (used % columns) * cell_, y = is_wide ? square_rows_ * cell_ + (used / columns) * blade : (used / columns) * cell_;
	used++;
	const Rect2 px(float(x), float(y), float(cell_), float(is_wide ? blade : cell_));
	pending_.push_back({ type, text, px });
	const float w = float(width_), h = float(height_);
	const Rect2 uv(px.position.x / w, px.position.y / h, px.size.x / w, px.size.y / h);
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
