// The lots' car gates against the road surface (CityProbes::drive_kerbs): for every street, each car gate of the lots facing it
// (sim/buildings/lot_access.h street_gates, the gate itself), the top of the kerb at the gate's middle on its side as the road
// surface draws it (render/roads/kerb_drop.h), held against the lip a vehicle access keeps (streets.json dropped_kerb drive_low_m);
// and where a few of them stand, spread over the city, for the photos to look at.
#include "api/city_probes.h"

#include "render/render_world.h"
#include "render/roads/kerb_drop.h"
#include "render/roads/road_mesh.h"
#include "render/roads/road_section.h"
#include "sim/buildings/lot_access.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr int GATE_POINTS = 8; // the gates whose place the probe tells, spread evenly over those it checked

// Whether the profile has a kerb between a raised footway and the carriageway on `side`: the kerb a car gate's drive crosses.
bool kerb_on(const StreetLook &L, const Profile &p, int side) {
	for (const Piece &pc : pieces_of(L, p, true))
		if (pc.ramp == Ramp::KERB && street_side(pc.u0 + pc.u1) == side) return true;
	return false;
}
} // namespace

Dictionary CityProbes::drive_kerbs() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	const CityView view(s);
	const StreetLook &L = render_->street_look();
	int gates = 0, full = 0;
	Array first;
	std::vector<Vector3> places; // each gate checked: the kerb's top at its middle, in the world
	for (int sid = 0; sid < int(s.roads.segs.size()); sid++) {
		const RoadSegment &g = s.roads.segs[size_t(sid)];
		if (!g.alive) continue;
		const Profile &p = s.catalog.profile(g.profile);
		std::vector<ArcSpan> spans[2];
		street_gates(s.roads, s.buildings, sid, 0.0, spans); // the gates themselves
		const KerbDrops k = kerb_drops_of(view, sid);
		for (int side : { STREET_RIGHT, STREET_LEFT }) {
			if (!kerb_on(L, p, side)) continue;
			for (const ArcSpan &gate : spans[side]) {
				const double mid = (gate.first + gate.second) * 0.5;
				if (mid < k.v0 || mid > k.v1) continue; // its middle faces a junction's mouth: no kerb of this segment there
				gates++;
				const float top = kerb_height(L, k, side, mid);
				const SegFrame fr = seg_frame(view, sid, mid);
				places.push_back(fr.c + fr.left * (side == STREET_LEFT ? p.left : -p.right) + Vector3(0, top, 0));
				if (top <= L.drive_low) continue; // at the lip, or lower where a crossing's dropped kerb reaches the gate
				full++;
				if (first.is_empty()) first = Array::make(sid, side, mid, top);
			}
		}
	}
	Array points;
	for (int i = 0; i < GATE_POINTS && !places.empty(); i++) points.push_back(places[places.size() * size_t(i) / GATE_POINTS]);
	Dictionary d;
	d["points"] = points; // where a few of the gates stand: the lot's edge of the street at the gate's middle
	d["gates"] = gates; // car gates facing a kerb
	d["not_lowered"] = full; // of them, those whose kerb stands above the lip
	d["first"] = first; // segment, side, arc, kerb top
	d["lip_m"] = L.drive_low;
	return d;
}

} // namespace city
