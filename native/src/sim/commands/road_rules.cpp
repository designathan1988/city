// What a road edit may not leave behind, judged on the network it built (Roadcraft editor/editRules.ts): a stretch too short
// for the junctions at its two ends to open out. The edit runs, the mouths are worked out, and a refusal rolls it back.
#include "sim/commands/road_edit.h"

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

Squeezes squeezes_in(const CityState &s, const RoadBox &box) {
	Squeezes out;
	s.roads.for_segments_near(box.minx, box.minz, box.maxx, box.maxz, [&](int seg) {
		const double over = squeeze_of(s.roads, seg);
		if (over <= 0) return;
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		double &worst = out[std::minmax(g.a, g.b)]; // two roads between the same two nodes: the tighter
		worst = std::max(worst, over);
	});
	return out;
}

void squeezes_moved(Squeezes &sq, int from, int to) {
	Squeezes out;
	for (const auto &road : sq) {
		const int a = road.first.first == from ? to : road.first.first, b = road.first.second == from ? to : road.first.second;
		double &worst = out[std::minmax(a, b)];
		worst = std::max(worst, road.second);
	}
	sq.swap(out);
}

bool squeezed_worse(const CityState &s, const Squeezes &before, const RoadBox &box) {
	for (const auto &now : squeezes_in(s, box)) {
		const auto was = before.find(now.first);
		if (now.second > (was == before.end() ? 0 : was->second) + SAME_SQUEEZE_M) return true;
	}
	return false;
}

EndMouth planned_mouth(const CityState &s, const Anchor &at, const std::vector<std::pair<Quad, int>> &own, int profile) {
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
	for (const auto &arm : own) arms.push_back(arm_of(arm.first, s.catalog.profile(profile), arm.second));
	const std::vector<double> lens(arms.size(), std::numeric_limits<double>::max());
	std::vector<double> out;
	std::vector<int> order;
	junction_retreats(s.catalog.tune, arms, lens, out, order, false);
	m.owns.assign(out.begin() + std::ptrdiff_t(met.size()), out.end());
	m.own = m.owns.empty() ? 0 : m.owns.front();
	for (size_t i = 0; i < met.size(); i++) m.met.emplace_back(met[i], out[i]);
	return m;
}

EndMouth end_mouth(const CityState &s, const Anchor &at, const Quad &q, int end, int profile) { return planned_mouth(s, at, { { q, end } }, profile); }

bool plan_squeezed(const CityState &s, const RoadPlan &p, const Anchor &a, const Anchor &b, int profile) {
	const RoadNetwork &net = s.roads;
	const Tuning &tn = s.catalog.tune;
	// Where the road stops along itself: its ends and each crossing, the crossed road's end when the crossing is that near it
	// (road_commands.cpp crossing_node).
	struct Stop {
		Anchor at;
		double t = 0;
	};
	std::vector<Stop> stops{ { a, 0 } };
	for (const Crossing &c : p.crossings) {
		const RoadSegment &g = net.segs[size_t(c.seg)];
		Stop st;
		st.t = c.t_new;
		if (c.t_old * g.len < tn.road_snap_end_m) st.at.node = g.a;
		else if ((1 - c.t_old) * g.len < tn.road_snap_end_m) st.at.node = g.b;
		else {
			st.at.seg = c.seg;
			st.at.t = c.t_old;
		}
		stops.push_back(st);
	}
	stops.push_back({ b, 1 });
	std::vector<double> before(stops.size(), 0.0), after(stops.size(), 0.0); // the new road's mouth on either side of each stop
	for (size_t i = 0; i < stops.size(); i++) {
		const Stop &st = stops[i];
		std::vector<std::pair<Quad, int>> own;
		if (i > 0) own.push_back({ sub_quad(p.q, stops[i - 1].t, st.t), 1 });
		if (i + 1 < stops.size()) own.push_back({ sub_quad(p.q, st.t, stops[i + 1].t), 0 });
		const EndMouth m = planned_mouth(s, st.at, own, profile);
		if (m.owns.size() == own.size() && !own.empty()) {
			if (i > 0) before[i] = m.owns.front();
			if (i + 1 < stops.size()) after[i] = m.owns.back();
		}
		// Each road met there, from this junction to its own other one: squeezed where it was not, or more than it was.
		for (size_t k = 0; k < m.met.size(); k++) {
			const int o = m.met[k].first;
			const RoadSegment &g = net.segs[size_t(o)];
			double now = 0, was = 0;
			if (st.at.node >= 0) {
				now = m.met[k].second + wanted_retreat(net, g.a == st.at.node ? g.b : g.a, o) - g.len;
				was = std::max(0.0, squeeze_of(net, o));
			} else { // the road split there: each half a new road, from the split to that road's own end
				const double cut = quad_length(sub_quad(g.q, 0, st.at.t));
				now = m.met[k].second + wanted_retreat(net, k == 0 ? g.a : g.b, o) - (k == 0 ? cut : g.len - cut);
			}
			if (now > was + SAME_SQUEEZE_M) return true;
		}
	}
	// The new road between two stops, laid in pieces no longer than road_max_seg_m (road_commands.cpp lay_pieces) whose joints
	// stand no mouth back: only a piece at a stop reaches a junction's mouth.
	for (size_t i = 0; i + 1 < stops.size(); i++) {
		const double len = quad_length(sub_quad(p.q, stops[i].t, stops[i + 1].t));
		const double pieces = std::max(1.0, std::ceil(len / tn.road_max_seg_m)), piece = len / pieces;
		const double reach = pieces > 1 ? std::max(after[i], before[i + 1]) : after[i] + before[i + 1];
		if (reach - piece > SAME_SQUEEZE_M) return true;
	}
	return false;
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

} // namespace city
