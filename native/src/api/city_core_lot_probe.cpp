// Every lot's edges against what is round it (CityProbes::lot_fronts). Its level: the lot's covers as they are drawn at its front
// (the ground 0.3 m inside its front at a quarter, the middle and three quarters of its frontage, plus the lot's plate: render/buildings
// lot_emit.cpp) held against the footway's top 0.3 m outside (sim/street/footway.h footway_at: the road's level there, plus the
// sidewalk's height where it is raised): a lot meets the footway it faces at the footway's top, no step down and no bank between. Its
// seams: where a footway runs along a lot's front or side, the footway's outer edge lies on the lot's line, no land between and nothing
// of the lot over the footway. Its neighbours: a lot beside it in a row shares its side, no strip of bare land between the two.
#include "api/city_probes.h"

#include "sim/buildings/b40/record.h"
#include "sim/state/city_state.h"
#include "sim/street/footway.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <cmath>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float FRONT_STEP_M = 0.3f; // the heights and the footways are read this far inside and outside the lot's lines
constexpr float FRONT_SHARES[] = { -0.25f, 0.f, 0.25f }; // along the frontage from its middle, in frontages
constexpr double FOOTWAY_FIND_M = 0.25; // how far off a sidewalk's band the outside point may lie
constexpr float MEET_M = 0.01f; // two levels or two lines this close meet
constexpr float NEIGHBOUR_REACH_M = 2.f; // bare land narrower than this between two lots is a seam between them, not a way
constexpr float HALF = 0.5f;

// The worst of a set of readings: how many, how many off, and the first worst one as [building, x, z, value].
struct Tally {
	int seen = 0, off = 0;
	float worst = 0;
	Array first;
	void add(int id, const V2 &at, float value, bool bad) {
		seen++;
		if (!bad) return;
		off++;
		if (std::fabs(value) > std::fabs(worst) || first.is_empty()) {
			worst = value;
			first = Array::make(id, at.x, at.y, value);
		}
	}
};

// The sidewalk outside a lot's line at `out` and how far its outer edge stands outside the line at `on` along `away` (the line's
// outward unit): positive, land between the two; negative, the lot reaches over the footway. False where no sidewalk runs there.
bool seam_at(const CityState &s, const V2 &out, const V2 &on, const V2 &away, FootwayHit &hit, float &seam) {
	hit = footway_at(s.roads, out.x, out.y, FOOTWAY_FIND_M);
	if (!hit.ok || hit.side < 0) return false; // no sidewalk there (a corner's mouth, a median)
	const Profile &p = s.roads.profile_of(hit.seg);
	const float edge_u = hit.side == 0 ? p.edges.front().u0 : p.edges.back().u1;
	double ex, ez, ey;
	footway_point(s.roads, hit.seg, hit.v, edge_u, ex, ez, ey);
	seam = (V2(float(ex), float(ez)) - on).dot(away);
	return true;
}
} // namespace

Dictionary CityProbes::lot_fronts() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const Tuning &tn = s.catalog.tune;
	Tally level, seams, sides;
	for (int id = 0; id < int(s.buildings.items.size()); id++) {
		const Building &rec = s.buildings.items[size_t(id)];
		if (!rec.alive) continue;
		const b40::LotFrame f = b40::lot_frame(rec);
		for (float share : FRONT_SHARES) {
			const float x = share * rec.w;
			const V2 out = f.at(x, -FRONT_STEP_M), on = f.at(x, 0), in = f.at(x, FRONT_STEP_M);
			FootwayHit hit;
			float seam;
			if (!seam_at(s, out, on, f.e1 * -1.f, hit, seam)) continue;
			seams.add(id, on, seam, std::fabs(seam) > MEET_M);
			const Profile &p = s.roads.profile_of(hit.seg);
			const Strip &edge = hit.side == 0 ? p.edges.front().strip : p.edges.back().strip;
			double wx, wz, y;
			footway_point(s.roads, hit.seg, hit.v, hit.u, wx, wz, y);
			const float top = float(y) + (edge.flush ? 0.f : tn.street_sidewalk_m);
			const float gap = s.terrain.height_at(in.x, in.y) + tn.lot_plate_m - top;
			level.add(id, in, gap, std::fabs(gap) > MEET_M);
		}
		for (float side : { -1.f, 1.f }) {
			const float edge = rec.w * HALF, mid = rec.d * HALF;
			const V2 on = f.at(side * edge, mid), away = f.e0 * side;
			FootwayHit hit;
			float seam;
			if (seam_at(s, f.at(side * (edge + FRONT_STEP_M), mid), on, away, hit, seam)) {
				seams.add(id, on, seam, std::fabs(seam) > MEET_M);
				continue;
			}
			const V2 next = f.at(side * (edge + MEET_M), mid), beyond = f.at(side * (edge + NEIGHBOUR_REACH_M), mid);
			if (on_road(s.roads, next.x, next.y)) continue; // a street beside it
			float h;
			if (s.terrain.lots.level_at(next.x, next.y, h)) sides.add(id, on, 0.f, false);
			else if (s.terrain.lots.level_at(beyond.x, beyond.y, h)) sides.add(id, on, NEIGHBOUR_REACH_M, true);
		}
	}
	Dictionary d;
	d["fronts"] = level.seen; // points of lot fronts with a sidewalk outside them
	d["off"] = level.off; // of them, those whose lot does not meet the footway's top
	d["worst"] = level.first; // building, x, z, the lot's surface less the footway's top (m)
	d["seams"] = seams.seen; // points of lots' fronts and sides with a sidewalk outside them
	d["seams_off"] = seams.off; // of them, those whose line is not the footway's outer edge
	d["seam_worst"] = seams.first; // building, x, z, how far the footway's edge stands outside the lot's line (m)
	d["sides"] = sides.seen; // lots' sides with another lot beside them, within NEIGHBOUR_REACH_M
	d["sides_apart"] = sides.off; // of them, those with bare land between the two
	d["side_worst"] = sides.first; // building, x, z, the reach the neighbour was found within (m)
	return d;
}

} // namespace city
