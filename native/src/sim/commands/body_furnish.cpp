// What the Interior tab edits (the 4.0's placeCore and editFurniture): the cores through the building, and each floor's furniture as the player arranged it. The first
// change to a floor keeps what was there: the pieces made for it become the player's.
#include "sim/buildings/b40/blocks.h"
#include "sim/buildings/interior/interior.h"
#include "sim/commands/body_ops.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace city {

using namespace b40;

namespace {
constexpr const char *NO_PIECE = "tool.failed";
constexpr float REACH_PAST_M = 0.25f, REACH_UNSIZED_M = 0.6f, QUARTER = TAU_F / 4.f;

int top_of(const Body &b) {
	int top = 0;
	for (const Volume &v : b.volumes) top = std::max(top, v.top());
	return top;
}

float on_grid(float x, float step) { return std::round(x / step) * step; }

std::vector<PlacedPiece> &pieces_of(Body &b, const FloorEdit &e) {
	auto known = b.furnishing.find(e.level);
	if (known != b.furnishing.end()) return known->second;
	Body solid = b;
	resolve_blocks(solid);
	Interior inside(*e.data, solid, e.purpose);
	std::vector<PlacedPiece> list;
	for (const Furniture &f : inside.at(e.level).furniture) list.push_back({ uint8_t(f.kind), f.x, f.y, f.angle });
	return b.furnishing[e.level] = std::move(list);
}

bool on_a_floor(const Body &b, int level) { return level >= 0 && level < top_of(b); }
} // namespace

std::string op_add_core(Body &b, const Tuning &t, float x, float y, int kind) {
	if (kind < CORE_LIFT || kind > CORE_STAIR_LIFT || b.volumes.empty()) return NO_PIECE;
	Core c{ 0, 0, top_of(b) - 1, CoreKind(kind) };
	c.x = on_grid(x - core_width(c) / 2, float(t.build_grid_m));
	c.y = on_grid(y - CORE_SIDE_M / 2, float(t.build_grid_m));
	b.cores.push_back(c);
	return "";
}

std::string op_remove_core(Body &b, float x, float y) {
	const size_t before = b.cores.size();
	b.cores.erase(std::remove_if(b.cores.begin(), b.cores.end(), [&](const Core &c) { return x >= c.x && x <= c.x + core_width(c) && y >= c.y && y <= c.y + CORE_SIDE_M; }), b.cores.end());
	return b.cores.size() != before ? "" : "building.problem.noCore";
}

int piece_under(const Data &data, const std::vector<PlacedPiece> &list, float x, float y) {
	int best = -1;
	float nearest = std::numeric_limits<float>::max();
	for (int i = 0; i < int(list.size()); i++) {
		const PlacedPiece &p = list[size_t(i)];
		const float *size = data.interior.size[p.kind];
		const float widest = std::max(size[0], size[1]), reach = widest > 0 ? widest / 2 + REACH_PAST_M : REACH_UNSIZED_M;
		const float d = std::hypot(p.x - x, p.y - y);
		if (d < reach && d < nearest) {
			best = i;
			nearest = d;
		}
	}
	return best;
}

int piece_at(Body &b, const FloorEdit &e, float x, float y) { return on_a_floor(b, e.level) ? piece_under(*e.data, pieces_of(b, e), x, y) : -1; }

std::string op_put_piece(Body &b, const Tuning &t, const FloorEdit &e, int kind, float x, float y, float angle) {
	if (!on_a_floor(b, e.level) || kind < 0 || kind >= FK_COUNT) return NO_PIECE;
	const float step = float(t.build_grid_m) / 2;
	pieces_of(b, e).push_back({ uint8_t(kind), on_grid(x, step), on_grid(y, step), angle });
	return "";
}

std::string op_move_piece(Body &b, const Tuning &t, const FloorEdit &e, int index, float x, float y) {
	if (!on_a_floor(b, e.level)) return NO_PIECE;
	std::vector<PlacedPiece> &list = pieces_of(b, e);
	if (index < 0 || index >= int(list.size())) return NO_PIECE;
	const float step = float(t.build_grid_m) / 2;
	list[size_t(index)].x = on_grid(x, step);
	list[size_t(index)].y = on_grid(y, step);
	return "";
}

std::string op_turn_piece(Body &b, const FloorEdit &e, int index) {
	if (!on_a_floor(b, e.level)) return NO_PIECE;
	std::vector<PlacedPiece> &list = pieces_of(b, e);
	if (index < 0 || index >= int(list.size())) return NO_PIECE;
	PlacedPiece &p = list[size_t(index)];
	p.angle = std::fmod(p.angle + QUARTER, TAU_F);
	return "";
}

std::string op_remove_piece(Body &b, const FloorEdit &e, float x, float y) {
	if (!on_a_floor(b, e.level)) return NO_PIECE;
	std::vector<PlacedPiece> &list = pieces_of(b, e);
	const int i = piece_under(*e.data, list, x, y);
	if (i < 0) return "building.problem.noPiece";
	list.erase(list.begin() + i);
	return "";
}

} // namespace city
