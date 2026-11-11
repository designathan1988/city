// Every lot against the paving round it (CityProbes::lot_paving): nothing of a lot's rectangle, 1 cm in from its lines, lies on the
// paving as it is drawn (sim/street/paving_outline.h: the segments' bands, the junctions' plates out to their corners' footway backs,
// the turning bulbs), a corner's round back included; the land a lot stands on is the land the paving leaves.
#include "api/city_probes.h"

#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"
#include "sim/street/paving_outline.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <algorithm>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float PAVED_IN_M = 0.01f; // a lot's rectangle this far in from its lines: what of it lies on the paving stands on it
constexpr double PAVED_SEEN_M2 = 1e-4; // less than this on the paving is the outline's own rounding (its chords, its millimetres)
constexpr float HALF = 0.5f;

// How much of a lot's rectangle, PAVED_IN_M in from its lines, lies on the paving as it is drawn round it (m2).
double on_paving(const CityState &s, const Building &rec, const b40::LotFrame &f) {
	const float half = rec.w * HALF - PAVED_IN_M, near = PAVED_IN_M, far = rec.d - PAVED_IN_M;
	const V2 corners[4] = { f.at(-half, near), f.at(half, near), f.at(half, far), f.at(-half, far) };
	Clipper2Lib::Path64 path;
	float x0 = corners[0].x, z0 = corners[0].y, x1 = x0, z1 = z0;
	for (const V2 &c : corners) {
		path.push_back(paving_point(c.x, c.y));
		x0 = std::min(x0, c.x);
		z0 = std::min(z0, c.y);
		x1 = std::max(x1, c.x);
		z1 = std::max(z1, c.y);
	}
	const Clipper2Lib::Paths64 paving = paving_near(s.roads, x0, z0, x1, z1);
	if (paving.empty()) return 0;
	Clipper2Lib::Paths64 lot;
	add_positive(lot, std::move(path));
	double area = 0;
	for (const Clipper2Lib::Path64 &p : Clipper2Lib::Intersect(lot, paving, Clipper2Lib::FillRule::NonZero)) area += Clipper2Lib::Area(p);
	return area / (PAVING_UNITS_PER_M * PAVING_UNITS_PER_M);
}
} // namespace

Dictionary CityProbes::lot_paving() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	int lots = 0, paved = 0;
	double worst = 0;
	Array first;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive) continue;
		const b40::LotFrame f = b40::lot_frame(rec);
		const double on = on_paving(s, rec, f);
		lots++;
		if (on <= PAVED_SEEN_M2) continue;
		paved++;
		if (on > worst) {
			worst = on;
			const V2 middle = f.at(0, rec.d * HALF);
			first = Array::make(id, middle.x, middle.y, on);
		}
	}
	Dictionary d;
	d["lots"] = lots; // lots held against the paving round them
	d["paved"] = paved; // of them, those whose rectangle stands on the paving as it is drawn
	d["worst"] = first; // building, x, z (its middle), m2 of it on the paving
	return d;
}

} // namespace city
