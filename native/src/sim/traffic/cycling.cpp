// Bicycles on the road: a bicycle rides the kerb lane of a street, on the edge of the carriageway (CTB art. 58;
// DirSeg::edge_shift), the traffic behind still following it: a lane has no room to pass it at the art. 201 distance. Where
// the street has a cycle lane on that side (the profile's bike strip, DirSeg::cycle_shift) it eases into that instead, along
// the lane's middle and out at the ends, the traffic behind no longer following it once its body is in the band. They keep
// to the streets with a footway (no bicycle on motorways).
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {
// 0 at the ends of a lane of length `len`, 1 along its middle.
float inside(const TrafficParams &p, float len, float at) {
	const float d = std::min(at, len - at);
	return d <= 0 ? 0.f : smooth01(d / p.cycle_ease_m);
}
} // namespace

float cycle_shift(const CityState &s, const Vehicle &v, int ds, float front) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	const float across = d.cycle_shift != 0 ? d.cycle_shift : d.edge_shift;
	if (v.type != VehicleType::BICYCLE || across == 0 || v.lane != 0) return 0;
	const TrafficParams &p = s.catalog.traffic;
	return across * inside(p, d.len, front - p.classes[int(v.type)].length * 0.5f);
}

bool out_of_the_way(const CityState &s, const Vehicle &follower, const Vehicle &lead, int ds) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	if (follower.type == VehicleType::BICYCLE || lead.type != VehicleType::BICYCLE || d.cycle_shift == 0 || lead.lane != 0) return false;
	const TrafficParams &p = s.catalog.traffic;
	const float rear = lead.s - p.classes[int(lead.type)].length;
	return inside(p, d.len, rear) > p.cycle_clear_share && inside(p, d.len, lead.s) > p.cycle_clear_share;
}

} // namespace traffic
} // namespace city
