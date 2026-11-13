// The sheet of plate pictures, see sign_atlas.h. Sizes in pixels, from data/render/streets.json; so is project/render/sign_sheet.gd.
#include "render/props/sign_atlas.h"

#include <godot_cpp/variant/rect2i.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {
bool wide(SignType t) { return t == SignType::STREET || t == SignType::DIRECTION; }
// A plate's picture by all it shows: its type, the player's words, a given name's keys and number (a line apart: no key holds one).
std::string key_of(SignType t, const std::string &text, const GivenName &g) {
	return std::to_string(int(t)) + '\n' + text + '\n' + g.form + '\n' + g.name + '\n' + std::to_string(g.ordinal);
}
} // namespace

void SignAtlas::configure(int width, int height, int cell, int blade_w, int blade_h) {
	width_ = width;
	height_ = height;
	cell_ = cell;
	blade_w_ = blade_w;
	blade_h_ = blade_h;
}

void SignAtlas::reset() {
	std::lock_guard<std::mutex> lk(mutex_);
	cells_.clear();
	pending_.clear();
	shelf_[0] = shelf_[1] = Shelf();
	layer_ = top_ = used_ = 0;
	generation_++;
}

// The next place of its height: on the shelf open for it, a new shelf under the others, or the first of the next layer.
Rect2i SignAtlas::place(bool is_wide) {
	const int w = is_wide ? blade_w_ : cell_, h = is_wide ? blade_h_ : cell_;
	Shelf &s = shelf_[is_wide ? 1 : 0];
	if (!s.open || s.x + w > width_) {
		if (top_ + h > height_) {
			layer_++;
			top_ = 0;
			shelf_[0] = shelf_[1] = Shelf();
		}
		s.open = true;
		s.x = 0;
		s.y = top_;
		top_ += h;
	}
	const Rect2i px(s.x, s.y, w, h);
	s.x += w;
	return px;
}

SignCell SignAtlas::cell(SignType type, const std::string &text, const GivenName &given) {
	std::lock_guard<std::mutex> lk(mutex_);
	const std::string key = key_of(type, text, given);
	auto it = cells_.find(key);
	if (it != cells_.end()) return it->second;
	const Rect2 px = Rect2(place(wide(type)));
	used_++;
	pending_.push_back({ type, text, given, px, layer_ });
	const float sheet_w = float(width_), sheet_h = float(height_);
	const SignCell out{ Rect2(px.position.x / sheet_w, px.position.y / sheet_h, px.size.x / sheet_w, px.size.y / sheet_h), layer_ };
	cells_[key] = out;
	return out;
}

Array SignAtlas::take_pending() {
	std::lock_guard<std::mutex> lk(mutex_);
	Array out;
	for (const Pending &p : pending_) {
		Array one;
		one.push_back(int(p.type));
		one.push_back(String::utf8(p.text.c_str()));
		one.push_back(p.rect);
		one.push_back(p.layer);
		one.push_back(generation_);
		one.push_back(String::utf8(p.given.form.c_str()));
		one.push_back(String::utf8(p.given.name.c_str()));
		one.push_back(int64_t(p.given.ordinal));
		out.push_back(one);
	}
	pending_.clear();
	return out;
}

} // namespace city
