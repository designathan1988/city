// CityProbes::basin_rims: the basins of water of lots made on the spot, with no city (an open lot's fountain, a house's pool), each
// body placed on level ground and each basin drawn alone and read from its mesh (render/buildings/basin_census.h): the water to the
// basin's outline and the rim right past it, all round. And the paving a basin stands on (a park's round-point): the hard cover
// laid over the lot's ground that holds the whole basin, round as the basin is. CityProbes::covers_on_slope: covers laid by hand on
// sloping ground, each drawn whole.
#include "api/city_probes.h"

#include "render/buildings/basin_census.h"
#include "render/buildings/lot_census.h"
#include "sim/buildings/b40/cover_layers.h"
#include "sim/buildings/b40/element_shape.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"

using namespace godot;

namespace city {

namespace {
constexpr int COVER_FIELDS = 5; // a cover laid by hand: x, y, width, depth, round
constexpr float SLOPE_ALONG = 0.02f, SLOPE_IN = 0.03f; // the sloping ground of covers_on_slope: rise per metre along the front and in

// The pavings the basins of a body stand on, and those of them that are round.
struct Pavings {
	int count = 0, round = 0;
	Array first_square;
	void take(const b40::Body &b, int seed) {
		for (const b40::LotElement &basin : b.elements) {
			if (basin.kind != b40::EL_POOL) continue;
			const b40::Footprint f = b40::outline_of(basin).bounds();
			bool ground = true; // the first cover is the lot's own ground
			for (const b40::LotElement &c : b.elements) {
				if (c.kind != b40::EL_SURFACE) continue;
				if (ground) {
					ground = false;
					continue;
				}
				if (c.paint < 0 || b.paints[size_t(c.paint)].finish == b40::FIN_NONE) continue; // a lawn
				const b40::Outline o = b40::outline_of(c);
				if (!o.holds(f.x0, f.y0) || !o.holds(f.x1, f.y0) || !o.holds(f.x1, f.y1) || !o.holds(f.x0, f.y1)) continue;
				count++;
				round += c.round ? 1 : 0;
				if (!c.round && first_square.is_empty()) first_square = Array::make(seed, c.x, c.y, c.w, c.d);
			}
		}
	}
};
} // namespace

// Covers laid by hand on a lot `w` by `d` whose ground slopes both ways, so each is cut out of the ground's triangles: rows of
// (x, y, width, depth, round) in the lot's frame. How many are drawn short of their outline (render/buildings/lot_census.h).
Dictionary CityProbes::covers_on_slope(const Array &rows, float w, float d) const {
	const b40::Data &data = state_->buildings.designs;
	b40::Body b;
	b.plan.W = w;
	b.plan.D = d;
	const int paint = b.add_paint(data.color("concrete"));
	for (const Variant &row : rows) {
		const Array a = row;
		if (a.size() < COVER_FIELDS) continue;
		b40::LotElement e;
		e.kind = b40::EL_SURFACE;
		e.x = float(a[0]);
		e.y = float(a[1]);
		e.w = float(a[2]);
		e.d = float(a[3]);
		e.round = bool(a[4]);
		e.h = b40::cover_h(b40::COVER_WAY);
		e.paint = paint;
		b.elements.push_back(e);
	}
	const b40::BuildingLook look;
	const b40::Placed pl = b40::placed_alone(data, look, b, [](const Vector3 &p) { return p.x * SLOPE_ALONG + p.z * SLOPE_IN; });
	Array first;
	Dictionary r;
	r["covers"] = int(b.elements.size());
	r["covers_short"] = covers_short(pl, first); // covers whose mesh holds less ground than their outline
	r["first_short"] = first; // the first: its row, middle, size, the m2 drawn and the outline's
	return r;
}

// `fn`: an open lot's function (BuildingFn: a park, a square), or 0 for a lot of `zone` round a building.
Dictionary CityProbes::basin_rims(int fn, int zone, float w, float d, int seed, int count) const {
	const b40::Data &data = state_->buildings.designs;
	const b40::BuildingLook look;
	BasinCensus census;
	Pavings pavings;
	for (int k = 0; k < count; k++) {
		b40::Body b;
		if (fn != FN_NONE) {
			Building rec;
			rec.fn = uint8_t(fn);
			rec.w = w;
			rec.d = d;
			rec.seed = uint32_t(seed + k);
			b40::make_seed_body(data, rec, b);
		} else {
			b40::make_body(data, b, zone, 0, w, d, uint32_t(seed + k), b40::LotEdges());
		}
		census.add(b40::placed_alone(data, look, b, [](const Vector3 &) { return 0.f; }), seed + k);
		pavings.take(b, seed + k);
	}
	Dictionary r;
	r["bodies"] = count;
	census.put(r);
	r["pavings"] = pavings.count; // hard covers laid over the lot's ground that hold a whole basin: the paving it stands on
	r["pavings_round"] = pavings.round; // of them, the round ones
	r["first_square"] = pavings.first_square; // the first that is not: seed, the cover's middle, width and depth
	return r;
}

} // namespace city
