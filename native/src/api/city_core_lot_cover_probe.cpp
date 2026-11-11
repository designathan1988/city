// Every lot's covers against one another (CityProbes::lot_covers): the covers render/buildings/lot_covers.h lays for the render, each
// placed as the building chunks place it (its floor on the ground under its blocks), and wherever a cover lies over one before it in
// their order, it stands over it by at least half the cover step: two covers never cross, never share one depth.
#include "api/city_probes.h"

#include "render/buildings/lot_covers.h"
#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <algorithm>
#include <cmath>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {
constexpr float INSET = 0.05f; // of the way from a corner to the middle: a sample stays off the edges two pieces share
constexpr float HALF = 0.5f;
constexpr float LEAST_UP = 1e-9f; // a piece whose normal has no more of the vertical than this stands on edge: no height over a point

// A piece and its box on the ground (x, z).
struct Laid {
	b40::CoverPiece c;
	float x0, z0, x1, z1;
};

Laid laid(const b40::CoverPiece &c) {
	Laid l{ c, c.p[0].x, c.p[0].z, c.p[0].x, c.p[0].z };
	for (int i = 1; i < c.count; i++) {
		l.x0 = std::min(l.x0, c.p[i].x);
		l.x1 = std::max(l.x1, c.p[i].x);
		l.z0 = std::min(l.z0, c.p[i].z);
		l.z1 = std::max(l.z1, c.p[i].z);
	}
	return l;
}

// The piece's height over (x, z), on the plane of its first three corners: false where (x, z) is not inside it.
bool height_on(const b40::CoverPiece &c, float x, float z, float &y) {
	float sign = 0;
	for (int i = 0; i < c.count; i++) {
		const Vector3 &a = c.p[i], &b = c.p[(i + 1) % c.count];
		const float cross = (b.x - a.x) * (z - a.z) - (b.z - a.z) * (x - a.x);
		if (cross == 0) continue;
		if (sign == 0) sign = cross;
		else if ((cross > 0) != (sign > 0)) return false;
	}
	const Vector3 n = (c.p[1] - c.p[0]).cross(c.p[2] - c.p[0]);
	if (std::fabs(n.y) < LEAST_UP) return false;
	y = c.p[0].y - (n.x * (x - c.p[0].x) + n.z * (z - c.p[0].z)) / n.y;
	return true;
}
} // namespace

Dictionary CityProbes::lot_covers() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const Tuning &tn = s.catalog.tune;
	b40::BuildingLook look;
	look.lot_plate = tn.lot_plate_m;
	look.cover_step = tn.lot_cover_step_m;
	const float least = tn.lot_cover_step_m * HALF;
	int lots = 0, samples = 0, under = 0;
	float worst = 0;
	Array first;
	std::vector<Laid> pieces;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive) continue;
		const std::shared_ptr<const b40::Body> body = b40::record_mass(s.buildings.designs, rec);
		const b40::LotFrame f = b40::lot_frame(rec);
		const float floor = b40::lot_floor(*body, rec, f, [&s](float x, float z) { return s.terrain.height_at(x, z); });
		b40::Placed pl;
		pl.data = &s.buildings.designs;
		pl.look = &look;
		pl.b = pl.whole = body.get();
		pl.lot = b40::Basis{ Vector3(f.o.x, floor, f.o.y), Vector3(f.e0.x, 0, f.e0.y), Vector3(f.e1.x, 0, f.e1.y), Vector3(0, 1, 0) };
		pl.ground = [&s](const Vector3 &p) { return s.terrain.height_at(p.x, p.z); };
		pieces.clear();
		b40::lot_covers(pl, [&pieces](const b40::CoverPiece &c) { pieces.push_back(laid(c)); });
		if (pieces.empty()) continue;
		lots++;
		for (const Laid &later : pieces) {
			Vector3 mid;
			for (int i = 0; i < later.c.count; i++) mid += later.c.p[i];
			mid /= float(later.c.count);
			for (int i = 0; i <= later.c.count; i++) {
				const Vector3 q = i < later.c.count ? later.c.p[i] + (mid - later.c.p[i]) * INSET : mid;
				for (const Laid &earlier : pieces) {
					if (earlier.c.rank >= later.c.rank || q.x < earlier.x0 || q.x > earlier.x1 || q.z < earlier.z0 || q.z > earlier.z1) continue;
					float y;
					if (!height_on(earlier.c, q.x, q.z, y)) continue;
					samples++;
					const float gap = q.y - y;
					if (gap >= least) continue;
					under++;
					if (first.is_empty() || gap < worst) {
						worst = gap;
						first = Array::make(id, q.x, q.z, gap, later.c.element, earlier.c.element);
					}
				}
			}
		}
	}
	Dictionary d;
	d["lots"] = lots; // lots with covers
	d["samples"] = samples; // points of a cover over an earlier cover of its lot
	d["under"] = under; // of them, where the later does not stand over the earlier by half a step (crossed, or at one depth)
	d["worst"] = first; // building, x, z, the later less the earlier (m), the two elements
	return d;
}

} // namespace city
