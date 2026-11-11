// Where and how far a segment's kerbs drop (kerb_drop.h).
#include "render/roads/kerb_drop.h"

#include "render/roads/road_mesh.h"
#include "sim/state/city_cut.h"

#include <algorithm>

namespace city {

namespace {
// The height a share `share` of the way from `foot` up to `full`: `full` itself where nothing lowers it (a whole kerb stands exactly
// at its height, so neighbouring pieces meet with no step).
float lowered(float foot, float full, float share) { return share >= 1.f ? full : foot + (full - foot) * share; }

// Share of the full kerb at `d` metres from a crossing mouth: none over the dropped kerb, flat to its end, rising over its flare.
float crossing_share(const StreetLook &L, double d) {
	if (d < L.ramp_full) return float((L.ramp_full - d) / L.ramp_full);
	if (d <= L.ramp_flat) return 0.f;
	return std::min(1.f, float((d - L.ramp_flat) / L.ramp_flare));
}

float crossing_kerb(const StreetLook &L, const KerbDrops &k, double at) {
	float share = 1.f;
	if (k.ca) share = std::min(share, crossing_share(L, at - k.v0));
	if (k.cb) share = std::min(share, crossing_share(L, k.v1 - at));
	return lowered(L.ramp_low, L.h_curb, share);
}

// The kerb at `at` on a side with car gates `gates`: the lip across each gate, rising over drive_flare either side of it.
float drive_kerb(const StreetLook &L, const std::vector<ArcSpan> &gates, double at) {
	float share = 1.f;
	for (const ArcSpan &g : gates) {
		const double d = at < g.first ? g.first - at : at > g.second ? at - g.second : 0.0;
		share = std::min(share, float(std::min(1.0, d / L.drive_flare)));
	}
	return lowered(L.drive_low, L.h_curb, share);
}

// A surface `from_kerb` metres from a kerb standing at `kerb`, sloping up to `walk_h` across `width`.
float slope(float kerb, float walk_h, float from_kerb, float width) {
	if (kerb >= walk_h || width <= 0) return walk_h;
	return lowered(kerb, walk_h, std::min(1.f, from_kerb / width));
}
} // namespace

KerbDrops kerb_drops_of(const CityView &s, int sid) {
	const RoadSegment &g = s.roads.segs[size_t(sid)];
	const Profile &p = s.catalog.profile(g.profile);
	KerbDrops k;
	k.v0 = g.retreat_a;
	k.v1 = g.len - g.retreat_b;
	const bool kerbs_drop = !p.ring && !p.rail;
	k.ca = kerbs_drop && crosswalk_at(s, g.a, sid);
	k.cb = kerbs_drop && crosswalk_at(s, g.b, sid);
	if (kerbs_drop) kerb_gates_of(s, sid, k.drives); // the gates themselves: no margin round them (sim/state/city_cut.h)
	return k;
}

float kerb_height(const StreetLook &L, const KerbDrops &k, int side, double at) {
	return std::min(crossing_kerb(L, k, at), drive_kerb(L, k.drives[side], at));
}

float band_height(const StreetLook &L, const KerbDrops &k, int side, double at, float from_kerb, float band, float walk_h) {
	return std::min(slope(crossing_kerb(L, k, at), walk_h, from_kerb, band),
			slope(drive_kerb(L, k.drives[side], at), walk_h, from_kerb, std::min(L.drive_band, band)));
}

void kerb_marks(const StreetLook &L, const KerbDrops &k, std::vector<double> &out) {
	const double crossing[3] = { L.ramp_full, L.ramp_flat, L.ramp_flat + L.ramp_flare };
	for (double m : crossing) {
		if (k.ca) out.push_back(k.v0 + m);
		if (k.cb) out.push_back(k.v1 - m);
	}
	for (const std::vector<ArcSpan> &side : k.drives)
		for (const ArcSpan &g : side)
			for (double m : { g.first - L.drive_flare, g.first, g.second, g.second + L.drive_flare }) out.push_back(m);
}

} // namespace city
