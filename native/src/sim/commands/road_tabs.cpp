// Road drawer edits (Roadcraft roads/construction/segmentEdits.ts splitAt, streetEdits.ts changeProfile /
// changeProfiles / streetChain / mirrorSymmetric / alongStreet): split a segment where clicked, upgrade a segment or a
// whole street to the profile in hand (Shift), charging the difference in value or refunding a share of it.
#include "sim/commands/commands.h"
#include "sim/commands/street_commands.h"
#include "sim/commands/road_edit.h"
#include "sim/economy/road_economy.h"

#include "sim/roads/junction.h"
#include "sim/state/city_state.h"
#include "sim/street/street_spots.h"

#include <algorithm>
#include <tuple>
#include <cmath>
#include <map>

namespace city {

namespace {
constexpr size_t JUNCTION_ARMS = 3; // fewer arms need no control
constexpr float SAME_WIDTH_M = 0.01f;

// The profile reads the same from both ends (strips mirrored, lane directions turned).
bool mirror_symmetric(const Profile &p) {
	const auto &st = p.strips;
	size_t n = st.size();
	for (size_t i = 0; i < n; i++) {
		const Strip &a = st[i], &b = st[n - 1 - i];
		if (a.kind != b.kind || std::fabs(a.width - b.width) > SAME_WIDTH_M || a.bus != b.bus || a.rail != b.rail || a.pave != b.pave) return false;
		if (a.kind == StripKind::LANE && a.dir != -b.dir) return false;
	}
	return true;
}

// Whether each segment of a connected street runs the way `ref` does, walking through the nodes they share.
std::map<int, bool> along_street(const CityState &s, const std::vector<int> &segs, int ref) {
	const RoadNetwork &net = s.roads;
	std::map<int, bool> out{ { ref, true } };
	std::vector<int> queue{ ref };
	while (!queue.empty()) {
		int sg = queue.back();
		queue.pop_back();
		bool fwd = out[sg];
		int ahead = fwd ? net.segs[sg].b : net.segs[sg].a, behind = fwd ? net.segs[sg].a : net.segs[sg].b;
		for (int t : net.nodes[ahead].segs)
			if (std::count(segs.begin(), segs.end(), t) && !out.count(t)) { out[t] = net.segs[t].a == ahead; queue.push_back(t); }
		for (int t : net.nodes[behind].segs)
			if (std::count(segs.begin(), segs.end(), t) && !out.count(t)) { out[t] = net.segs[t].b == behind; queue.push_back(t); }
	}
	return out;
}

// After a profile change, like a new road: what the wider road covers goes (the ground under it follows its new casing
// when the edit settles: sim/commands/road_grade_sync.cpp).
int settle_under(CityState &s, int seg) { return clear_under(s, seg); }
} // namespace

EditResult split_road_at(CityState &s, double x, double z) {
	EditResult r;
	double t, d;
	int seg = s.roads.find_segment_near(x, z, s.catalog.tune.road_pick_m, t, d);
	if (seg < 0) {
		r.error = "hint.split";
		return r;
	}
	const RoadSegment &g = s.roads.segs[seg];
	double split_min = s.catalog.tune.edit_split_min_m;
	if (t * g.len < split_min || (1 - t) * g.len < split_min) {
		r.error = "tools.toast.splitNearEnd";
		return r;
	}
	double minx = g.minx, minz = g.minz, maxx = g.maxx, maxz = g.maxz;
	split_segment(s, seg, t);
	finish_edit(s, minx, minz, maxx, maxz);
	r.ok = true;
	r.segments = 2;
	return r;
}

EditResult change_profile_at(CityState &s, double x, double z, int profile, bool whole_street) {
	EditResult r;
	double t, d;
	int seg = s.roads.find_segment_near(x, z, s.catalog.tune.road_pick_m, t, d);
	const Profile &np = s.catalog.profile(profile);
	if (seg < 0 || s.catalog.profile(s.roads.segs[seg].profile).ring || np.ring) {
		r.error = seg < 0 ? "tools.hint.upgradePick" : "ui.insp.ring";
		return r;
	}
	std::vector<int> segs = whole_street ? street_chain(s.roads, seg) : s.roads.pieces_of(seg);
	std::map<int, bool> along;
	if (!mirror_symmetric(np)) along = along_street(s, segs, seg);
	RoadBox box;
	for (int sg : segs) box.add(s.roads.segs[sg]);
	const std::map<int, double> before = squeezes_in(s, box);
	std::vector<int> changed;
	for (int sg : segs) {
		if (!s.roads.segs[sg].alive) continue;
		if (along.count(sg) && !along[sg]) {
			RoadSegment rec = s.roads.segs[sg];
			j_remove_segment(s, sg);
			Quad q{ rec.q.x1, rec.q.z1, rec.q.cx, rec.q.cz, rec.q.x0, rec.q.z0 };
			sg = j_add_segment(s, rec.b, rec.a, q, rec.profile, rec.zr, rec.zl, flags_reversed(rec.flags));
		}
		const std::vector<CarriedPiece> kit = street_kit_of(s, sg);
		j_set_profile(s, sg, profile);
		carry_street_kit(s, sg, kit); // what stands on its sidewalks moves to the new ones
		changed.push_back(sg);
	}
	if (!settle_road_edit(s, r)) return r; // before the ground moves: the caller rolls the journal back
	for (int sg : changed) {
		r.buildings_removed += settle_under(s, sg);
		box.add(s.roads.segs[sg]);
		r.segments++;
	}
	finish_edit(s, box.minx, box.minz, box.maxx, box.maxz);
	if (squeezed_worse(s, before, box)) {
		r.error = "hint.rule.squeezed";
		return r;
	}
	r.ok = true;
	return r;
}

EditResult set_node_control(CityState &s, int node, int ctl) {
	EditResult r;
	RoadNetwork &net = s.roads;
	if (node < 0 || node >= int(net.nodes.size()) || !net.nodes[node].alive || ctl < 0 || ctl >= CTL_COUNT) {
		r.error = "hint.control.miss";
		return r;
	}
	if (net.nodes[node].segs.size() < JUNCTION_ARMS) {
		r.error = "hint.control.notJunction";
		return r;
	}
	j_set_control(s, node, uint8_t(ctl));
	double x = net.nodes[node].x, z = net.nodes[node].z;
	finish_edit(s, x, z, x, z);
	r.ok = true;
	return r;
}

EditResult set_road_end(CityState &s, int node, bool bulb) {
	EditResult r;
	RoadNetwork &net = s.roads;
	if (node < 0 || node >= int(net.nodes.size()) || !net.nodes[node].alive || net.nodes[node].segs.size() != 1) {
		r.error = "tool.failed";
		return r;
	}
	j_set_plain(s, node, !bulb);
	net.refresh_node(node);
	if (bulb && !net.nodes[node].bulb) { // no room: back to the cut, shape included
		j_set_plain(s, node, true);
		net.refresh_node(node);
		r.error = "inspector.end.noRoom";
		return r;
	}
	double x = net.nodes[node].x, z = net.nodes[node].z;
	finish_edit(s, x, z, x, z);
	r.ok = true;
	return r;
}

EditResult toggle_lane_link(CityState &s, int node, const LaneLink &link, const std::vector<LaneLink> &built) {
	EditResult r;
	const auto same_lane = [&](const LaneLink &l) { return l.from == link.from && l.from_lane == link.from_lane; };
	const std::vector<LaneLink> &held = s.roads.nodes[size_t(node)].links;
	std::vector<LaneLink> own, rest;
	for (const LaneLink &l : held) (same_lane(l) ? own : rest).push_back(l);
	if (own.empty())
		for (const LaneLink &l : built)
			if (same_lane(l)) own.push_back(l);
	const auto at = std::find(own.begin(), own.end(), link);
	if (at != own.end()) own.erase(at);
	else own.push_back(link);
	if (own.empty()) {
		r.error = "hint.lane.last";
		return r;
	}
	rest.insert(rest.end(), own.begin(), own.end());
	std::sort(rest.begin(), rest.end(), [](const LaneLink &a, const LaneLink &b) {
		return std::tie(a.from, a.from_lane, a.to, a.to_lane) < std::tie(b.from, b.from_lane, b.to, b.to_lane);
	});
	j_links(s, node, std::move(rest));
	const RoadNode &n = s.roads.nodes[size_t(node)];
	finish_edit(s, n.x, n.z, n.x, n.z);
	r.ok = true;
	return r;
}

} // namespace city
