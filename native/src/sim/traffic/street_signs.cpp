// The signs the rules put up (Roadcraft world/roads/derivedSigns.ts signsAtNode): at a junction by priority the rule's
// plate on each arm that gives way, a metre past its stop line, for the traffic arriving; on a stretch long enough, the speed
// plate just past the mouth, for the traffic leaving. Both at the right of that traffic. Distances: street_furniture.json signs.
#include "sim/traffic/traffic_system.h"

#include "sim/state/city_state.h"
#include "sim/street/street_spots.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace city {
namespace traffic {

namespace {
constexpr int STRAIGHT_ON_ARMS = 2; // a node where one street simply goes on carries no speed plate

void put(const CityState &s, int seg, double v, int side, SignType type, const std::string &text, float dir, std::vector<SignStand> &out) {
	const RoadNetwork &net = s.roads;
	const Profile &p = net.profile_of(seg);
	if (!sidewalk_on(p, side)) return;
	double x, z, y, fx, fz, lx, lz;
	footway_point(net, seg, v, sidewalk_u(p, side, plate_kerb(net, seg)), x, z, y);
	segment_frame(net, seg, v, fx, fz, lx, lz);
	SignStand st;
	st.x = float(x);
	st.y = float(y);
	st.z = float(z);
	st.type = type;
	st.text = text;
	st.face_x = float(fx) * dir;
	st.face_z = float(fz) * dir;
	out.push_back(st);
}
} // namespace

void junction_signs(const CityState &s, int n, std::vector<SignStand> &out) {
	const RoadNetwork &net = s.roads;
	const RoadNode &nd = net.nodes[size_t(n)];
	if (!nd.alive) return;
	const SignDef &d = s.catalog.furniture.signs;
	const CrossingDef &c = s.catalog.furniture.crossing;
	std::vector<int> arms = nd.segs;
	std::sort(arms.begin(), arms.end());
	for (int seg : arms) {
		const RoadSegment &g = net.segs[size_t(seg)];
		const Profile &p = net.profile_of(seg);
		if (!g.alive || g.a == g.b || p.edges.empty() || p.rail) continue;
		const bool at_a = g.a == n;
		const double mouth = at_a ? g.retreat_a : g.retreat_b;
		const double usable = g.len - g.retreat_a - g.retreat_b;
		const auto arc = [&](double from_mouth) { return at_a ? g.retreat_a + from_mouth : g.len - g.retreat_b - from_mouth; };
		const int rule = approach_rule(s, n, seg);
		const bool arrives = at_a ? !p.back.empty() : !p.fwd.empty();
		if (rule != RULE_NONE && arrives) {
			const double at = std::min(double(c.stop_to_m + d.stop_past_line_m), usable * d.far_share);
			// arriving at a the traffic runs b to a: its right is the left sidewalk, and the plate looks back at it
			put(s, seg, arc(at), at_a ? 1 : 0, rule == RULE_STOP ? SignType::STOP : SignType::YIELD, "", at_a ? 1.f : -1.f, out);
		}
		const bool leaves = at_a ? !p.fwd.empty() : !p.back.empty();
		if (int(nd.segs.size()) <= STRAIGHT_ON_ARMS || !leaves || usable + mouth < d.speed_min_length_m) continue;
		const int limit = int(std::lround(p.speed / d.speed_round_kmh) * d.speed_round_kmh);
		put(s, seg, arc(d.speed_past_mouth_m), at_a ? 0 : 1, SignType::SPEED, std::to_string(limit), at_a ? -1.f : 1.f, out);
	}
}

// The rule of every leg of a junction under one: a Stop on all of them, or a mini-roundabout's and a yield's give way. RULE_NONE: each leg its own.
int leg_rule(const RoadNode &node) {
	if (node.ctl == CTL_STOP || (node.ctl == CTL_AUTO && node.auto_signal == AUTO_ALL_STOP)) return RULE_STOP;
	return node.ctl == CTL_YIELD || node.ctl == CTL_MINI ? RULE_YIELD : RULE_NONE;
}

int plate_rule(const RoadNetwork &net, int seg, int n) {
	const RoadSegment &g = net.segs[size_t(seg)];
	const int end = g.a == n ? 0 : 1;
	return (g.flags & seg_rule_mask(end)) >> seg_rule_shift(end);
}

} // namespace traffic
} // namespace city
