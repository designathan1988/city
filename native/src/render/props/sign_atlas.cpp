// The sheet of plate pictures, see sign_atlas.h. Sizes in pixels; project/render/sign_sheet.gd makes the sheet this size.
#include "render/props/sign_atlas.h"

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int SHEET_PX = 2048, CELL_PX = 256, COLUMNS = SHEET_PX / CELL_PX;
constexpr int BLADE_PX = CELL_PX / 2; // a name or direction plate: half a cell tall
constexpr int SQUARE_ROWS = 4, WIDE_ROWS = (SHEET_PX - SQUARE_ROWS * CELL_PX) / BLADE_PX;
constexpr int SQUARE_CELLS = SQUARE_ROWS * COLUMNS, WIDE_CELLS = WIDE_ROWS * COLUMNS;

bool wide(SignType t) { return t == SignType::STREET || t == SignType::DIRECTION; }
std::string key_of(SignType t, const std::string &text) { return std::to_string(int(t)) + "|" + text; }
} // namespace

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
	int &used = is_wide ? wide_used_ : square_used_;
	if (used >= (is_wide ? WIDE_CELLS : SQUARE_CELLS) - 1) { // the last cell stays for the blank plate of a full sheet
		const std::string blank = key_of(type, "");
		auto b = cells_.find(blank);
		if (b != cells_.end()) return b->second;
	}
	const int x = (used % COLUMNS) * CELL_PX, y = is_wide ? SQUARE_ROWS * CELL_PX + (used / COLUMNS) * BLADE_PX : (used / COLUMNS) * CELL_PX;
	used++;
	const Rect2 px(float(x), float(y), float(CELL_PX), float(is_wide ? BLADE_PX : CELL_PX));
	pending_.push_back({ type, text, px });
	const Rect2 uv(px.position / float(SHEET_PX), px.size / float(SHEET_PX));
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
