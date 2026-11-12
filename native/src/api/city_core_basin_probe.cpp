// CityProbes::basin_rims: the basins of water of lots made on the spot, with no city (an open lot's fountain, a house's pool), each
// body placed on level ground and each basin drawn alone and read from its mesh (render/buildings/basin_census.h): the water to the
// basin's outline and the rim right past it, all round. And the paving a basin stands on (a park's round-point): the hard cover
// laid over the lot's ground that holds the whole basin, round as the basin is.
#include "api/city_probes.h"

#include "render/buildings/basin_census.h"
#include "sim/buildings/b40/element_shape.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"

using namespace godot;

namespace city {

namespace {
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
		b40::Placed pl;
		pl.data = &data;
		pl.look = &look;
		pl.b = pl.whole = &b;
		pl.lot = b40::Basis{ Vector3(), Vector3(1, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0) };
		pl.ground = [](const Vector3 &) { return 0.f; };
		census.add(pl, seed + k);
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
