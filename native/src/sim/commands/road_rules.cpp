// What a road edit may not leave behind, judged on the network it built (Roadcraft editor/editRules.ts): a stretch too short
// for the junctions at its two ends to open out. The edit runs, the mouths are worked out, and a refusal rolls it back. The
// road tool's preview asks the same of the plan before anything is laid (plan_squeezed: the lab's verdict, a dry run of the
// commit): the junctions the plan would make, their mouths worked out as the build will, on the stretches they would leave.
#include "sim/commands/road_edit.h"

#include "sim/roads/geometry.h"
#include "sim/roads/junction.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr double SAME_SQUEEZE_M = 1e-6; // the arithmetic of laying a road again, not a distance anyone sees

// Metres the two mouths of a road would overlap.
double squeeze_of(const RoadNetwork &net, int seg) {
	const RoadSegment &g = net.segs[size_t(seg)];
	return wanted_retreat(net, g.a, seg) + wanted_retreat(net, g.b, seg) - g.len;
}
} // namespace

// How far from node `n` the junction wants the mouth of `seg`, before the road's length caps it.
double wanted_retreat(const RoadNetwork &net, int n, int seg) {
	const RoadNode &nd = net.nodes[size_t(n)];
	std::vector<Arm> arms;
	std::vector<double> out;
	std::vector<int> order;
	for (int s : nd.segs) arms.push_back(arm_of(net.segs[size_t(s)].q, net.profile_of(s), net.segs[size_t(s)].a == n ? 0 : 1));
	const std::vector<double> lens(arms.size(), std::numeric_limits<double>::max());
	junction_retreats(net.cat->tune, arms, lens, out, order, nd.bulb);
	const size_t at = size_t(std::find(nd.segs.begin(), nd.segs.end(), seg) - nd.segs.begin());
	return at < out.size() ? out[at] : 0;
}

std::map<int, double> squeezes_in(const CityState &s, const RoadBox &box) {
	std::map<int, double> out;
	s.roads.for_segments_near(box.minx, box.minz, box.maxx, box.maxz, [&](int seg) {
		const double over = squeeze_of(s.roads, seg);
		if (over > 0) out[seg] = over;
	});
	return out;
}

bool squeezed_worse(const CityState &s, const std::map<int, double> &before, const RoadBox &box) {
	for (const auto &now : squeezes_in(s, box)) {
		const auto was = before.find(now.first);
		if (now.second > (was == before.end() ? 0 : was->second) + SAME_SQUEEZE_M) return true;
	}
	return false;
}

EndMouth end_mouth(const CityState &s, const Anchor &at, const Quad &q, int end, int profile) {
	const RoadNetwork &net = s.roads;
	EndMouth m;
	m.node = at.node;
	m.seg = at.seg;
	m.t = at.t;
	std::vector<Arm> arms;
	std::vector<int> met;
	if (at.node >= 0) {
		for (int o : net.nodes[size_t(at.node)].segs) {
			arms.push_back(arm_of(net.segs[size_t(o)].q, net.profile_of(o), net.segs[size_t(o)].a == at.node ? 0 : 1));
			met.push_back(o);
		}
	} else if (at.seg >= 0) {
		const std::array<Quad, 2> halves = split_quad(net.segs[size_t(at.seg)].q, at.t);
		arms.push_back(arm_of(halves[0], net.profile_of(at.seg), 1));
		arms.push_back(arm_of(halves[1], net.profile_of(at.seg), 0));
		met.assign(2, at.seg);
	} else {
		return m;
	}
	arms.push_back(arm_of(q, s.catalog.profile(profile), end));
	const std::vector<double> lens(arms.size(), std::numeric_limits<double>::max());
	std::vector<double> out;
	std::vector<int> order;
	junction_retreats(s.catalog.tune, arms, lens, out, order, false);
	m.own = out.back();
	for (size_t i = 0; i < met.size(); i++) m.met.emplace_back(met[i], out[i]);
	return m;
}

bool EndMouth::covers(const RoadNetwork &net, int other, double at_t, double pad) const {
	const RoadSegment &g = net.segs[size_t(other)];
	for (const auto &arm : met) {
		if (arm.first != other) continue;
		const double reach = arm.second + pad;
		if (other == seg ? std::fabs(at_t - t) * g.len <= reach : (g.a == node ? at_t : 1 - at_t) * g.len <= reach) return true;
	}
	return false;
}

namespace {
// Where a planned road's junction cuts a road it meets: at `t` of it, the mouth wanted on the stretch before and after.
struct Cut {
	double t = 0, before = 0, after = 0;
};

// A stop of the planned road along it: a junction or an end, the mouths it wants on the new road behind and ahead of it.
struct PlanStop {
	double t = 0, behind = 0, ahead = 0;
};

// What a planned road's junctions do to the mouths around them: its stops, the cuts of the roads it crosses or ends on, and the
// mouths the existing junctions it joins now want on their roads, by (road, node).
struct PlannedMouths {
	std::vector<PlanStop> stops;
	std::map<int, std::vector<Cut>> cuts;
	std::map<std::pair<int, int>, double> ends;
};

std::vector<double> mouths_of(const CityState &s, const std::vector<Arm> &arms) {
	std::vector<double> out;
	std::vector<int> order;
	junction_retreats(s.catalog.tune, arms, std::vector<double>(arms.size(), std::numeric_limits<double>::max()), out, order, false);
	return out;
}

// An end of the planned road, by end_mouth: a junction with a node, a cut of the road it lands on, or a dead end.
void plan_end(PlannedMouths &pm, const EndMouth &m, double t) {
	pm.stops.push_back({ t, t > 0 ? m.own : 0, t < 1 ? m.own : 0 });
	if (m.node >= 0)
		for (const auto &arm : m.met) pm.ends[{ arm.first, m.node }] = arm.second;
	else if (m.seg >= 0 && m.met.size() == 2) pm.cuts[m.seg].push_back({ m.t, m.met[0].second, m.met[1].second });
}

// A crossing of the planned road at `t`, as crossing_node lays it: the crossed road's end when close, else a cut of it.
void plan_crossing(const CityState &s, PlannedMouths &pm, const Quad &q, const Profile &own, const Crossing &c) {
	const RoadNetwork &net = s.roads;
	const RoadSegment &g = net.segs[size_t(c.seg)];
	const std::array<Quad, 2> mine = split_quad(q, c.t_new);
	std::vector<Arm> arms;
	const double snap = s.catalog.tune.road_snap_end_m;
	const int node = c.t_old * g.len < snap ? g.a : (1 - c.t_old) * g.len < snap ? g.b : -1;
	if (node >= 0) {
		const std::vector<int> &segs = net.nodes[size_t(node)].segs;
		for (int o : segs) arms.push_back(arm_of(net.segs[size_t(o)].q, net.profile_of(o), net.segs[size_t(o)].a == node ? 0 : 1));
		arms.push_back(arm_of(mine[0], own, 1));
		arms.push_back(arm_of(mine[1], own, 0));
		const std::vector<double> out = mouths_of(s, arms);
		for (size_t i = 0; i < segs.size(); i++) pm.ends[{ segs[i], node }] = out[i];
		pm.stops.push_back({ c.t_new, out[segs.size()], out.back() });
		return;
	}
	const std::array<Quad, 2> theirs = split_quad(g.q, c.t_old);
	arms = { arm_of(theirs[0], net.profile_of(c.seg), 1), arm_of(theirs[1], net.profile_of(c.seg), 0), arm_of(mine[0], own, 1), arm_of(mine[1], own, 0) };
	const std::vector<double> out = mouths_of(s, arms);
	pm.cuts[c.seg].push_back({ c.t_old, out[0], out[1] });
	pm.stops.push_back({ c.t_new, out[2], out[3] });
}
} // namespace

bool plan_squeezed(const CityState &s, const RoadPlan &plan, const Anchor &a, const Anchor &b, int profile) {
	const RoadNetwork &net = s.roads;
	PlannedMouths pm;
	plan_end(pm, end_mouth(s, a, plan.q, 0, profile), 0);
	for (const Crossing &c : plan.crossings) plan_crossing(s, pm, plan.q, s.catalog.profile(profile), c);
	plan_end(pm, end_mouth(s, b, plan.q, 1, profile), 1);
	// the new road between its stops (crossings come sorted along it, find_crossings): no stretch of it may be squeezed
	const std::vector<double> table = arc_table(plan.q, ROAD_ARC_STEPS);
	for (size_t i = 0; i + 1 < pm.stops.size(); i++) {
		const PlanStop &x = pm.stops[i], &y = pm.stops[i + 1];
		if (x.ahead + y.behind - (arc_at(table, y.t) - arc_at(table, x.t)) > SAME_SQUEEZE_M) return true;
	}
	// each road met: no stretch its new junctions leave may be more squeezed than the road was (squeezed_worse)
	std::map<int, std::vector<Cut>> met = pm.cuts;
	for (const auto &e : pm.ends) met[e.first.first];
	for (auto &m : met) {
		const int o = m.first;
		const RoadSegment &g = net.segs[size_t(o)];
		const double was = std::max(0.0, squeeze_of(net, o));
		const auto mouth_at = [&](int n) {
			const auto f = pm.ends.find({ o, n });
			return f != pm.ends.end() ? f->second : wanted_retreat(net, n, o);
		};
		std::sort(m.second.begin(), m.second.end(), [](const Cut &x, const Cut &y) { return x.t < y.t; });
		double from_t = 0, from_mouth = mouth_at(g.a);
		for (const Cut &c : m.second) {
			if (from_mouth + c.before - (arc_at(g.table, c.t) - arc_at(g.table, from_t)) > was + SAME_SQUEEZE_M) return true;
			from_t = c.t;
			from_mouth = c.after;
		}
		if (from_mouth + mouth_at(g.b) - (g.len - arc_at(g.table, from_t)) > was + SAME_SQUEEZE_M) return true;
	}
	return false;
}

} // namespace city
