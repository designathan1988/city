// See transit_ground.h.
#include "sim/transit/transit_ground.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <limits>

namespace city {

namespace {
constexpr float HALF = 0.5f, NO_DISTANCE = std::numeric_limits<float>::max();
constexpr float WIDEST_STREET = 4.f; // of the longest run on a road: a whole street's width round a point
} // namespace

bool transit_on_road(const CityState &s, const V2 &p) {
	float y = 0;
	bool road = false;
	return transit_paved(s, p, 0, y, road) && road;
}

bool transit_paved(const CityState &s, const V2 &p, float kerb, float &y, bool &carriageway) {
	bool paved = false;
	carriageway = false;
	const float reach = s.catalog.traffic.transit_along_road_m * WIDEST_STREET;
	s.roads.for_segments_near(p.x - reach, p.y - reach, p.x + reach, p.y + reach, [&](int seg) {
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		double t, d;
		closest_t(g.q, p.x, p.y, t, d);
		const Profile &pf = s.roads.profile_of(seg);
		const bool on_road = d < std::max(pf.asph_left, pf.asph_right);
		if (carriageway || (!on_road && (paved || d >= std::max(pf.left, pf.right)))) return;
		paved = true;
		carriageway = on_road;
		y = float(s.roads.seg_y(seg, g.len * t)) + (on_road ? 0 : kerb);
	});
	return paved;
}

void transit_stop_frame(const CityState &s, const TransitStop &q, V2 &along, V2 &out) {
	along = V2(1, 0);
	for (const RailTrack &t : s.transit.tracks()) {
		if (t.id != q.track) continue;
		float dist = NO_DISTANCE;
		for (size_t i = 1; i < t.points.size(); i++) {
			const float d = ((t.points[i] + t.points[i - 1]) * HALF - q.at).len();
			if (d < dist) {
				dist = d;
				along = (t.points[i] - t.points[i - 1]).norm();
			}
		}
	}
	out = V2(along.y, -along.x); // to its left, seen from above: x east, z south
	if (q.mode != TransitMode::BUS || q.seg < 0 || size_t(q.seg) >= s.roads.segs.size()) return;
	const RoadSegment &r = s.roads.segs[size_t(q.seg)];
	double t, dist, cx, cz, dx, dz;
	closest_t(r.q, q.at.x, q.at.y, t, dist);
	r.q.at(t, cx, cz);
	r.q.deriv(t, dx, dz);
	along = V2(float(dx), float(dz)).norm();
	const V2 from = q.at - V2(float(cx), float(cz));
	out = from.len() > 0 ? from.norm() : V2(along.y, -along.x);
}

bool transit_built(const CityState &s, const V2 &p, float pad) {
	bool built = false;
	s.buildings.query(p.x - pad, p.y - pad, p.x + pad, p.y + pad, [&](int) { built = true; });
	return built;
}

} // namespace city
