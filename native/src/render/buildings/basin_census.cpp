// A basin's water and rim read from the mesh its lot writes of it (basin_census.h).
#include "render/buildings/basin_census.h"

#include "core/math.h"
#include "sim/buildings/b40/element_shape.h"

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int LOOKS = 180; // the ways looked out from a basin's middle
constexpr int HALVINGS = 24; // of the look's length, to where it leaves the outline
constexpr float PROBE_M = 0.05f; // a look reads the mesh this far inside the outline and this far past it
constexpr float UP_LEAST = 0.5f; // a face whose normal has this much of the vertical faces up
constexpr float LEVEL_LEAST = 1e-9f; // under this much of the vertical a triangle's plane has no height over a point

// The highest face of the shell that faces up over (x, z) of the world: its height there; false where none lies.
bool top_at(const b40::Shell &s, float x, float z, float &y) {
	bool any = false;
	for (size_t i = 0; i + 2 < s.idx.size(); i += 3) {
		const Vector3 &a = s.pos[size_t(s.idx[i])], &b = s.pos[size_t(s.idx[i + 1])], &c = s.pos[size_t(s.idx[i + 2])];
		if (s.nrm[size_t(s.idx[i])].y < UP_LEAST) continue;
		const auto side = [&](const Vector3 &p, const Vector3 &q) { return (q.x - p.x) * (z - p.z) - (q.z - p.z) * (x - p.x); };
		const float ab = side(a, b), bc = side(b, c), ca = side(c, a);
		if ((ab < 0 || bc < 0 || ca < 0) && (ab > 0 || bc > 0 || ca > 0)) continue; // outside the triangle
		const Vector3 n = (b - a).cross(c - a);
		if (std::fabs(n.y) < LEVEL_LEAST) continue;
		const float h = a.y - (n.x * (x - a.x) + n.z * (z - a.z)) / n.y;
		if (!any || h > y) y = h;
		any = true;
	}
	return any;
}
} // namespace

void BasinCensus::add(const b40::Placed &pl, int id) {
	const b40::Body &b = *pl.b;
	b40::Body one = b;
	b40::Placed only = pl;
	only.b = &one;
	for (const b40::LotElement &e : b.elements) {
		if (e.kind != b40::EL_POOL) continue;
		basins++;
		round += e.round ? 1 : 0;
		one.elements.assign(1, e);
		b40::Shell shell, parts;
		b40::emit_lot(shell, parts, only);
		const b40::Outline o = b40::outline_of(e);
		for (int k = 0; k < LOOKS; k++) {
			const float turn = TAU_F * float(k) / float(LOOKS), dx = std::cos(turn), dy = std::sin(turn);
			float in = 0, out = e.w + e.d; // the look leaves the outline between the two
			for (int h = 0; h < HALVINGS; h++) {
				const float mid = (in + out) / 2;
				(o.holds(e.x + dx * mid, e.y + dy * mid) ? in : out) = mid;
			}
			const auto top = [&](float along, float &y) {
				const Vector3 p = pl.lot.at(e.x + dx * along, e.y + dy * along, 0);
				return top_at(shell, p.x, p.z, y);
			};
			looks++;
			float water = 0, rim = 0;
			if (!top(in - PROBE_M, water)) {
				dry++;
				if (first_dry.is_empty()) first_dry = Array::make(id, e.x, e.y, turn, in);
				continue;
			}
			if (top(out + PROBE_M, rim) && rim > water) continue;
			open++;
			if (first_open.is_empty()) first_open = Array::make(id, e.x, e.y, turn, out);
		}
	}
}

void BasinCensus::put(Dictionary &d) const {
	d["basins"] = basins; // the lots' basins of water: a pool, a fountain's basin
	d["basins_round"] = round; // of them, the round ones
	d["looks"] = looks; // the ways looked out from the basins' middles
	d["dry"] = dry; // looks with no water drawn right inside the basin's outline
	d["first_dry"] = first_dry; // the body, the basin's middle in the lot's frame, the look's turn (radians), where it leaves the outline
	d["rim_open"] = open; // looks with no rim standing over the water right past the outline
	d["first_open"] = first_open; // the same of the first
}

} // namespace city
