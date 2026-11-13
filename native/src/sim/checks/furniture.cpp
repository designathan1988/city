// The furniture.kept_matches_control invariant: every street furniture placement kept for the streets' revision (sim/roads/
// furniture_kept.h) was placed under the control its ends have now (a signal post stands where a junction is signalled). The
// player's edits move the revision and drop every placement; a control the traffic chooses (RoadNode::auto_signal) moves nothing,
// so traffic::apply_flow_control drops its arms' placements itself. A record: the segment's id.
// The control now is the lane graph's junction (Junction::ctl, set from traffic::control_of by build_junction for every node the
// drain rebuilds): this check runs only once the traffic has drained the change log, so the two agree, and it is a lookup where
// control_of would build the node's arms anew. Whole or scoped alike: the placements kept are those the render, the walkers and the
// probes read, so the pass is O(kept entries); nothing allocated. A segment gone keeps no placement past the revision its removal
// moved.
#include "sim/checks/records.h"

#include "sim/state/city_state.h"

namespace city::checks {

namespace {
constexpr const char *FEATURE = "furniture";
constexpr const char *KEPT_MATCHES_CONTROL = "kept_matches_control";

bool signalled(const LaneGraph &g, int n) { return n >= 0 && size_t(n) < g.junctions.size() && g.junctions[size_t(n)].ctl == Control::SIGNAL; }
} // namespace

void check_furniture(const CityState &s, const CheckScope &, Broken &broken) {
	const RoadNetwork &net = s.roads;
	const LaneGraph &g = s.traffic.g;
	s.furniture_kept.each_kept(s.street_revision, [&](int seg, FurnitureKept::Ends placed) {
		if (seg < 0 || size_t(seg) >= net.segs.size() || !net.segs[size_t(seg)].alive) return;
		const RoadSegment &r = net.segs[size_t(seg)];
		if (FurnitureKept::ends(signalled(g, r.a), signalled(g, r.b)) != placed) broken.add(FEATURE, KEPT_MATCHES_CONTROL, seg);
	});
}

} // namespace city::checks
