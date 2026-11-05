// Road drawer edits (Roadcraft roads/construction/segmentEdits.ts splitAt, streetEdits.ts changeProfile /
// changeProfiles / streetChain / mirrorSymmetric / alongStreet): split a segment where clicked, upgrade a segment or a
// whole street to the profile in hand (Shift), charging the difference in value or refunding a share of it.
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"
#include "sim/economy/road_economy.h"

#include "sim/roads/junction.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace city {

namespace {
constexpr size_t JUNCTION_ARMS = 3; // fewer arms need no control
constexpr float SAME_WIDTH_M = 0.01f;
constexpr int CHAIN_GUARD = 4000;

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

// After a profile change, like a new road: what the wider road covers goes and the ground settles under it.
int settle_under(CityState &s, int seg) {
	int gone = clear_under(s, seg);
	conform_under(s, seg);
	return gone;
}
} // namespace

std::vector<int> street_chain(const CityState &s, int s0) {
	const RoadNetwork &net = s.roads;
	int prof = net.segs[s0].profile;
	const Profile &p = s.catalog.profile(prof);
	std::vector<int> out{ s0 };
	for (int start_end : { 1, 0 }) {
		int sg = s0, end = start_end;
		for (int guard = 0; guard < CHAIN_GUARD; guard++) {
			int n = end == 1 ? net.segs[sg].b : net.segs[sg].a;
			Arm here = arm_of(net.segs[sg].q, p, end);
			double best_dot = s.catalog.tune.edit_street_cos;
			int best = -1, best_end = 0;
			for (int t : net.nodes[n].segs) {
				if (std::count(out.begin(), out.end(), t) || net.segs[t].profile != prof || s.catalog.profile(prof).ring) continue;
				int t_end = net.segs[t].a == n ? 0 : 1;
				if (p.one_way && t_end != (end == 1 ? 0 : 1)) continue;
				Arm arm = arm_of(net.segs[t].q, p, t_end);
				double d = -(arm.dx * here.dx + arm.dz * here.dz);
				if (d > best_dot) { best_dot = d; best = t; best_end = t_end; }
			}
			if (best < 0) break;
			out.push_back(best);
			sg = best;
			end = best_end == 0 ? 1 : 0;
		}
	}
	return out;
}

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
	std::vector<int> segs = whole_street ? street_chain(s, seg) : std::vector<int>{ seg };
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
			sg = j_add_segment(s, rec.b, rec.a, q, rec.profile, rec.zr, rec.zl, rec.flags);
		}
		j_set_profile(s, sg, profile);
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

} // namespace city
