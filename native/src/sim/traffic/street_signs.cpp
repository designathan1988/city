// The signs the rules put up (Roadcraft world/roads/derivedSigns.ts signsAtNode): at a junction by priority the rule's
// plate on each arm that gives way, a metre past its stop line, for the traffic arriving; on a stretch long enough, the speed
// plate just past the mouth, for the traffic leaving. Both at the right of that traffic. Distances: street_furniture.json signs.
// Which plates an arm carries and where its rule wants each is arm_plates'; where each stands is its segment's placement
// (sim/roads/street_furniture.h: a plate's post is a piece of the street, off the poles and the posts, the later pieces off it),
// and a plate is drawn, and audited, where that post stands.
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
} // namespace

void arm_plates(const CityState &s, int n, int seg, std::vector<RulePlate> &out) {
	const RoadNetwork &net = s.roads;
	const RoadNode &nd = net.nodes[size_t(n)];
	const RoadSegment &g = net.segs[size_t(seg)];
	const Profile &p = net.profile_of(seg);
	if (!nd.alive || !g.alive || g.a == g.b || p.edges.empty() || p.rail) return;
	const SignDef &d = s.catalog.furniture.signs;
	const bool at_a = g.a == n;
	const double mouth = at_a ? g.retreat_a : g.retreat_b;
	const double usable = g.len - g.retreat_a - g.retreat_b;
	const auto arc = [&](double from_mouth) { return at_a ? g.retreat_a + from_mouth : g.len - g.retreat_b - from_mouth; };
	const auto want = [&](double v, int side, SignType type, const std::string &text, float dir) {
		if (sidewalk_on(p, side)) out.push_back(RulePlate{ v, side, n, type, text, dir });
	};
	const int rule = approach_rule(s, n, seg);
	const bool arrives = at_a ? !p.back.empty() : !p.fwd.empty();
	if (rule != RULE_NONE && arrives) {
		const double at = std::min(net.stop_line_back(n, seg) + d.stop_past_line_m, usable * d.far_share);
		// arriving at a the traffic runs b to a: its right is the left sidewalk, and the plate looks back at it
		want(arc(at), at_a ? 1 : 0, rule == RULE_STOP ? SignType::STOP : SignType::YIELD, "", at_a ? 1.f : -1.f);
	}
	const bool leaves = at_a ? !p.fwd.empty() : !p.back.empty();
	if (int(nd.segs.size()) <= STRAIGHT_ON_ARMS || !leaves || usable + mouth < d.speed_min_length_m) return;
	const int limit = int(std::lround(p.speed / d.speed_round_kmh) * d.speed_round_kmh);
	want(arc(d.speed_past_mouth_m), at_a ? 0 : 1, SignType::SPEED, std::to_string(limit), at_a ? -1.f : 1.f);
}

void segment_plates(const CityState &s, int seg, std::vector<RulePlate> &out) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	if (!g.alive || g.a == g.b) return;
	arm_plates(s, g.a, seg, out);
	arm_plates(s, g.b, seg, out);
}

void junction_signs(const CityState &s, int n, std::vector<SignStand> &out) {
	const RoadNetwork &net = s.roads;
	const RoadNode &nd = net.nodes[size_t(n)];
	if (!nd.alive) return;
	std::vector<int> arms = nd.segs;
	std::sort(arms.begin(), arms.end());
	std::vector<RulePlate> wanted;
	std::vector<FurnitureSpot> posts;
	for (int seg : arms) {
		wanted.clear();
		segment_plates(s, seg, wanted);
		if (wanted.empty()) continue;
		posts.clear();
		street_furniture_of(s, seg, furniture_bit(FurnitureKind::SIGN), posts);
		for (const FurnitureSpot &f : posts) {
			// this node's plates on the arm: a post of the placement (the player's own plates are items), by its place in the list
			if (f.item >= 0 || f.node != n || f.station < 0 || size_t(f.station) >= wanted.size()) continue;
			const RulePlate &w = wanted[size_t(f.station)];
			double x, z, y, fx, fz, lx, lz;
			footway_point(net, seg, f.v, f.u, x, z, y);
			segment_frame(net, seg, f.v, fx, fz, lx, lz);
			SignStand st;
			st.x = float(x);
			st.y = float(y);
			st.z = float(z);
			st.type = w.type;
			st.text = w.text;
			st.face_x = float(fx) * w.dir;
			st.face_z = float(fz) * w.dir;
			out.push_back(st);
		}
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
