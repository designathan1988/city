// What a pane draws: the seed of its room (one home, one lamp), the room seen through it and its frame. The room is what the
// building's own interior has behind the pane: rays from the glass to the partitions and the block's walls give its size,
// and the largest piece of furniture seen, the most of it across and up, is drawn as a box (building_pane.gdshaderinc).
#include "render/buildings/pane_rooms.h"

#include "render/buildings/facade_bay.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace city::b40 {

namespace {
constexpr float MOST_DEEP = 12.f, MOST_WIDE = 10.f, INSIDE = 0.5f, OFF_GLASS = 0.02f, LEAST_DEEP = 0.8f, LEAST_WIDE = 0.3f;
constexpr float NEAREST_PIECE = 0.2f, OFF_CENTRE_K = 0.3f, HIT_EPS = 1e-3f, PARALLEL = 1e-9f, ENDS = 1e-6f;
constexpr uint32_t FNV_BASIS = 0x811c9dc5u, FNV_PRIME = 0x01000193u, BAY_BYTE = 0xff, SLOT_MASK = 0xffff, CORNER_Q = 10;
constexpr uint64_t MIX = 0x9e3779b97f4a7c15ull;
constexpr int BAY_BYTE_SHIFT = 8, MIX_SHIFT = 29, CORNERS = 4;

// Where a ray (origin, unit direction) first crosses one of the segments, or `far`.
float first_hit(const std::vector<Partition> &walls, float ox, float oy, float dx, float dy, float far) {
	float best = far;
	for (const Partition &w : walls) {
		const float ex = w.x1 - w.x0, ey = w.y1 - w.y0, den = dx * ey - dy * ex;
		if (std::abs(den) < PARALLEL) continue;
		const float t = ((w.x0 - ox) * ey - (w.y0 - oy) * ex) / den, s = ((w.x0 - ox) * dy - (w.y0 - oy) * dx) / den;
		if (t > HIT_EPS && t < best && s >= -ENDS && s <= 1 + ENDS) best = t;
	}
	return best;
}

uint32_t fnv(uint32_t h, int32_t v) {
	uint32_t u = uint32_t(v);
	for (int i = 0; i < CORNERS; i++) {
		h = (h ^ (u & BAY_BYTE)) * FNV_PRIME;
		u >>= BAY_BYTE_SHIFT;
	}
	return h;
}

uint64_t mix(uint64_t h, float v) {
	uint32_t bits;
	v += 0.f; // -0 as 0
	std::memcpy(&bits, &v, sizeof bits);
	h = (h ^ bits) * MIX;
	return h ^ (h >> MIX_SHIFT);
}

void closed_walls(const Volume &v, std::vector<Partition> &walls) {
	std::vector<Point2> ring;
	if (v.outline.empty()) ring = { { v.x, v.y }, { v.x + v.w, v.y }, { v.x + v.w, v.y + v.d }, { v.x, v.y + v.d } };
	else
		for (const Point2 &q : v.outline) ring.push_back({ v.x + q.x * v.w, v.y + q.y * v.d });
	for (size_t i = 0; i < ring.size(); i++) {
		const Point2 &a = ring[i], &b = ring[(i + 1) % ring.size()];
		walls.push_back({ a.x, a.y, b.x, b.y });
	}
}

// The largest piece seen through a pane at (px, py), its walls `back`, `ahead` and `depth` away.
void piece_seen(const Data &data, const FloorInterior &floor, float px, float py, const Point2 &t, const Point2 &n, PaneRoom &r) {
	float score = 0;
	for (const Furniture &f : floor.furniture) {
		if (!data.interior.seen[f.kind]) continue;
		const float c = std::cos(f.angle), s = std::sin(f.angle);
		float a0 = FAR_AWAY, a1 = -FAR_AWAY, d0 = FAR_AWAY, d1 = -FAR_AWAY;
		for (int k = 0; k < CORNERS; k++) {
			const float cx = k == 1 || k == 2 ? 1.f : -1.f, cy = k >= 2 ? 1.f : -1.f;
			const float x = f.x + (cx * f.w * c - cy * f.d * s) / 2, y = f.y + (cx * f.w * s + cy * f.d * c) / 2;
			const float along = (x - px) * t.x + (y - py) * t.y, deep = (px - x) * n.x + (py - y) * n.y;
			a0 = std::min(a0, along); a1 = std::max(a1, along); d0 = std::min(d0, deep); d1 = std::max(d1, deep);
		}
		if (a1 < -r.back || a0 > r.ahead || d1 < NEAREST_PIECE || d0 > r.depth) continue;
		a0 = std::max(a0, -r.back); a1 = std::min(a1, r.ahead); d0 = std::max(d0, NEAREST_PIECE); d1 = std::min(d1, r.depth);
		const float shown = (a1 - a0) * f.h / (1 + std::abs((a0 + a1) / 2) * OFF_CENTRE_K);
		if (shown <= score) continue;
		score = shown;
		r.piece = int(f.kind); r.piece_h = f.h; r.piece_at = (a0 + a1) / 2; r.piece_half = (a1 - a0) / 2; r.piece_near = d0; r.piece_far = d1;
	}
}

PaneRoom room_at(const Placed &pl, const BayCtx &c, const FloorInterior &floor, float px, float py, float h) {
	const Point2 &t = c.plan_along, &n = c.plan_out;
	std::vector<Partition> walls = floor.partitions;
	closed_walls(pl.b->volumes[size_t(c.vol)], walls);
	PaneRoom r;
	r.depth = std::max(LEAST_DEEP, first_hit(walls, px - n.x * OFF_GLASS, py - n.y * OFF_GLASS, -n.x, -n.y, MOST_DEEP));
	const float qx = px - n.x * INSIDE, qy = py - n.y * INSIDE;
	r.back = std::max(LEAST_WIDE, first_hit(walls, qx, qy, -t.x, -t.y, MOST_WIDE));
	r.ahead = std::max(LEAST_WIDE, first_hit(walls, qx, qy, t.x, t.y, MOST_WIDE));
	r.sill = h;
	r.head = c.H - h;
	r.floor = uint8_t(floor_finish(pl.rooms->inside.purpose(), c.level, pl.id));
	piece_seen(*pl.data, floor, px, py, t, n, r);
	return r;
}

uint16_t slot_of(const Placed &pl, const BayCtx &c, const FloorInterior &floor, float qx, float qy) {
	uint32_t h = fnv(fnv(fnv(FNV_BASIS, int32_t(pl.id)), c.level), c.vol);
	for (const Space &s : floor.spaces)
		if (s.volume == c.vol && qx >= s.x && qx <= s.x + s.w && qy >= s.y && qy <= s.y + s.d)
			return uint16_t(fnv(fnv(h, int32_t(std::lround(s.x * CORNER_Q))), int32_t(std::lround(s.y * CORNER_Q))) & SLOT_MASK);
	return uint16_t(fnv(h, int32_t(c.hash)) & SLOT_MASK);
}
} // namespace

PaneLook pane_look(const Placed &pl, const BayCtx &c, float a0, float a1, float h0, float h1, bool dark, float frame) {
	PaneLook look;
	look.dark = dark;
	look.bay = uint8_t((c.hash >> BAY_BYTE_SHIFT) & BAY_BYTE);
	look.trim = c.trim;
	look.frame = frame;
	const float am = (a0 + a1) / 2, hm = (h0 + h1) / 2;
	const float px = c.plan_at.x + c.plan_along.x * am, py = c.plan_at.y + c.plan_along.y * am;
	const FloorInterior &floor = pl.rooms->inside.at(c.level);
	look.slot = slot_of(pl, c, floor, px - c.plan_out.x * INSIDE, py - c.plan_out.y * INSIDE);
	uint64_t key = uint64_t(reinterpret_cast<uintptr_t>(&floor));
	for (float v : { px, py, c.plan_out.x, c.plan_out.y, hm, c.H }) key = mix(key, v);
	auto known = pl.rooms->known.find(key);
	look.room = known != pl.rooms->known.end() ? known->second : (pl.rooms->known[key] = room_at(pl, c, floor, px, py, hm));
	return look;
}

} // namespace city::b40
