// Road drawer edits (Roadcraft roads/construction/segmentEdits.ts splitAt, streetEdits.ts changeProfile /
// changeProfiles / streetChain / mirrorSymmetric / alongStreet): split a segment where clicked, upgrade a segment or a
// whole street to the profile in hand (Shift), charging the difference in value or refunding a share of it.
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"

#include "sim/roads/junction.h"
#include "sim/state/city_state.h"
#include "sim/street/street_spots.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace city {

namespace {
constexpr double MIN_CHARGE = 1; // economy.ts chargeFor: |delta| < 1 moves no money
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

// Value change of putting `profile` on the segments (paid roads count fully, free roads only when they grow).
double profile_delta(const CityState &s, const std::vector<int> &segs, int profile) {
	double paid = 0, free = 0;
	for (int sg : segs) {
		const RoadSegment &g = s.roads.segs[sg];
		double d = road_value(s, profile, g.len) - road_value(s, g.profile, g.len);
		(g.flags & SEG_FREE ? free : paid) += d;
	}
	auto charge = [&](double v) { return std::fabs(v) < MIN_CHARGE ? 0.0 : v > 0 ? std::round(v) : -std::round(-v * s.catalog.tune.economy_refund_share); };
	return charge(paid) + (free > 0 ? charge(free) : 0.0);
}

// After a profile change, like a new road: what the wider road covers goes and the ground settles under it.
int settle_under(CityState &s, int seg) {
	int gone = clear_under(s, seg);
	conform_under(s, seg);
	return gone;
}
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
	std::vector<int> segs = whole_street ? street_chain(s.roads, seg) : std::vector<int>{ seg };
	double price = profile_delta(s, segs, profile);
	if (price > 0 && !s.unlimited_money && price > s.money) {
		r.error = "ui.insp.noMoney";
		r.money = price;
		return r;
	}
	std::map<int, bool> along;
	if (!mirror_symmetric(np)) along = along_street(s, segs, seg);
	double minx = BIG, minz = BIG, maxx = -BIG, maxz = -BIG;
	for (int sg : segs) {
		if (!s.roads.segs[sg].alive) continue;
		if (along.count(sg) && !along[sg]) {
			RoadSegment rec = s.roads.segs[sg];
			j_remove_segment(s, sg);
			Quad q{ rec.q.x1, rec.q.z1, rec.q.cx, rec.q.cz, rec.q.x0, rec.q.z0 };
			sg = j_add_segment(s, rec.b, rec.a, q, rec.profile, rec.zr, rec.zl, rec.flags);
		}
		const std::vector<CarriedPiece> kit = street_kit_of(s, sg);
		j_set_profile(s, sg, profile);
		carry_street_kit(s, sg, kit);
		r.buildings_removed += settle_under(s, sg);
		const RoadSegment &g = s.roads.segs[sg];
		minx = std::min(minx, double(g.minx)); minz = std::min(minz, double(g.minz));
		maxx = std::max(maxx, double(g.maxx)); maxz = std::max(maxz, double(g.maxz));
		r.segments++;
	}
	j_money(s, -price);
	finish_edit(s, minx, minz, maxx, maxz);
	r.ok = true;
	r.money = s.unlimited_money ? 0 : price;
	return r;
}

} // namespace city
