// The barriers as they are drawn (Roadcraft render/barriers.ts): each run cut into straight pieces that step with the ground,
// every piece a box of one MultiMesh, its colour the instance's. A wall with its coping, a fence's rails and posts, a hedge,
// a guardrail's beam on posts, a railing's bars. Measures: data/render/streets.json "barriers". A piece belongs to the
// chunk of its centre.
#include "render/props/barrier_boxes.h"

#include "core/color.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
const Vector3 UP(0, 1, 0);

struct Piece {
	float x = 0, z = 0, dx = 1, dz = 0, length = 0;
};

// A box along (dx, dz), its foot at height y.
void box(PackedFloat32Array &buf, int64_t key, const Piece &p, float y, float length, float width, float height, const Color &col) {
	if (chunk_of(p.x, p.z) != key) return;
	const Vector3 along(p.dx, 0, p.dz), across(-p.dz, 0, p.dx);
	mm_push(buf, along * length, UP * height, across * width, Vector3(p.x, y + height * 0.5f, p.z), col, 0, 0, 0, 0);
}

struct Run {
	const CityState &s;
	const StreetLook &L;
	const BarrierLook &B;
	int64_t key;
	PackedFloat32Array &buf;
	float ground(float x, float z) const { return street_ground(s, L, x, z); }
};

void solid_piece(const Run &r, BarrierKind kind, const Piece &p, float g0, float g1) {
	const BarrierDef &d = r.s.catalog.furniture.barriers;
	const float height = d.height_m[int(kind)], thick = d.thickness_m[int(kind)];
	const float low = std::min(g0, g1) - r.B.sink, top = std::max(g0, g1), rise = top - std::min(g0, g1);
	if (kind == BarrierKind::WALL) {
		box(r.buf, r.key, p, low, p.length + thick, thick, height + rise + r.B.sink, r.B.wall);
		box(r.buf, r.key, p, top + height, p.length + thick * r.B.coping_over, thick * r.B.coping_over, r.B.coping_m, r.B.coping);
	} else if (kind == BarrierKind::HEDGE) {
		box(r.buf, r.key, p, low, p.length + thick * r.B.hedge_lap, thick, height + rise + r.B.sink, r.B.hedge);
	} else {
		for (float level : r.B.rail_levels) box(r.buf, r.key, p, top + height * level - r.B.rail_m * 0.5f, p.length, r.B.rail_thick_m, r.B.rail_m, r.B.fence);
	}
}

void kerb_piece(const Run &r, bool guard, const Piece &p, float g) {
	const BarrierLook &B = r.B;
	if (guard) {
		box(r.buf, r.key, p, g + B.beam_foot_m, p.length + B.lap_m, B.beam_thick_m, B.guard_beam_m, B.beam);
		return;
	}
	box(r.buf, r.key, p, g + B.bar_top_m, p.length + B.lap_m, B.bar_m, B.bar_m, B.railing);
	box(r.buf, r.key, p, g + B.bar_foot_m, p.length + B.lap_m, B.bar_low_m, B.bar_low_m, B.railing);
}

// A run's posts along a stretch, and a railing's bars between them.
void posts(const Run &r, BarrierKind kind, float ax, float az, float bx, float bz, bool first) {
	const BarrierLook &B = r.B;
	const float len = std::hypot(bx - ax, bz - az), dx = (bx - ax) / len, dz = (bz - az) / len;
	const float height = r.s.catalog.furniture.barriers.height_m[int(kind)];
	const bool guard = kind == BarrierKind::GUARDRAIL, fence = kind == BarrierKind::FENCE;
	const float step = guard ? B.guard_step_m : fence ? B.fence_step_m : B.rail_step_m;
	const float size = guard ? B.guard_post_m : fence ? B.fence_post_m : B.rail_post_m;
	const Color &col = guard ? B.guard_post : fence ? B.fence : B.railing;
	const int n = std::max(1, int(std::lround(len / step)));
	for (int k = first ? 0 : 1; k <= n; k++) {
		const float t = float(k) / n;
		const Piece p{ ax + (bx - ax) * t, az + (bz - az) * t, dx, dz, 0 };
		box(r.buf, r.key, p, r.ground(p.x, p.z) - B.sink, size, size, height + B.sink, col);
	}
	if (kind != BarrierKind::RAILING) return;
	const int bars = std::max(1, int(std::lround(len / B.bar_step_m)));
	for (int k = 1; k < bars; k++) {
		const float t = float(k) / bars;
		const Piece p{ ax + (bx - ax) * t, az + (bz - az) * t, dx, dz, 0 };
		box(r.buf, r.key, p, r.ground(p.x, p.z) + B.bar_foot_m, B.bar_thin_m, B.bar_thin_m, B.bar_top_m - B.bar_foot_m, B.railing);
	}
}

void place_run(const Run &r, const Barrier &b) {
	const bool kerb = b.kind == BarrierKind::GUARDRAIL || b.kind == BarrierKind::RAILING;
	for (size_t i = 0; i + 3 < b.pts.size(); i += 2) {
		const float ax = b.pts[i], az = b.pts[i + 1], bx = b.pts[i + 2], bz = b.pts[i + 3], len = std::hypot(bx - ax, bz - az);
		if (len < r.B.piece_min_m) continue;
		const int pieces = std::max(1, int(std::ceil(len / r.B.piece_m)));
		for (int k = 0; k < pieces; k++) {
			const float t0 = float(k) / pieces, t1 = float(k + 1) / pieces;
			const float x0 = ax + (bx - ax) * t0, z0 = az + (bz - az) * t0, x1 = ax + (bx - ax) * t1, z1 = az + (bz - az) * t1;
			const Piece p{ (x0 + x1) * 0.5f, (z0 + z1) * 0.5f, (bx - ax) / len, (bz - az) / len, len / pieces };
			const float g0 = r.ground(x0, z0), g1 = r.ground(x1, z1);
			if (kerb) kerb_piece(r, b.kind == BarrierKind::GUARDRAIL, p, std::max(g0, g1));
			else solid_piece(r, b.kind, p, g0, g1);
		}
		if (kerb || b.kind == BarrierKind::FENCE) posts(r, b.kind, ax, az, bx, bz, i == 0);
	}
}

Color colour(const Dictionary &d, const char *key) { return rgba8_to_color(parse_hex_color(String(d[key]).utf8().get_data())); }
} // namespace

bool BarrierLook::load(const Dictionary &t, PackedStringArray &errors) {
	if (!t.has("barriers")) {
		errors.push_back("streets.json: falta barriers");
		return false;
	}
	const Dictionary b = t["barriers"];
	piece_m = b["piece_m"]; sink = b["sink_m"]; piece_min_m = b["piece_min_m"]; coping_m = b["coping_m"]; coping_over = b["coping_over"]; hedge_lap = b["hedge_lap"];
	rail_m = b["rail_m"]; rail_thick_m = b["rail_thick_m"]; fence_step_m = b["fence_step_m"]; fence_post_m = b["fence_post_m"];
	guard_step_m = b["guard_step_m"]; guard_post_m = b["guard_post_m"]; guard_beam_m = b["guard_beam_m"]; beam_thick_m = b["beam_thick_m"]; beam_foot_m = b["beam_foot_m"]; lap_m = b["lap_m"];
	rail_step_m = b["railing_step_m"]; rail_post_m = b["railing_post_m"]; bar_m = b["bar_m"]; bar_low_m = b["bar_low_m"]; bar_thin_m = b["bar_thin_m"];
	bar_step_m = b["bar_step_m"]; bar_top_m = b["bar_top_m"]; bar_foot_m = b["bar_foot_m"];
	const Array levels = b["rail_levels"];
	for (int k = 0; k < levels.size(); k++) rail_levels.push_back(levels[k]);
	wall = colour(b, "wall_color"); coping = colour(b, "coping_color"); hedge = colour(b, "hedge_color"); fence = colour(b, "fence_color");
	beam = colour(b, "beam_color"); guard_post = colour(b, "guard_post_color"); railing = colour(b, "railing_color");
	return true;
}

void place_barriers(const CityState &s, const StreetLook &L, const BarrierLook &B, int64_t key, PackedFloat32Array &buf) {
	const float x0 = key_x(key) * CHUNK, z0 = key_y(key) * CHUNK;
	const Run run{ s, L, B, key, buf };
	s.street.barriers_in(x0, z0, x0 + CHUNK, z0 + CHUNK, [&](int id) { place_run(run, s.street.barriers.items[size_t(id)]); });
}

} // namespace city
