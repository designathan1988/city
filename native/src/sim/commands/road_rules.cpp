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
	junction_retreats(arms, lens, out, order, nd.bulb);
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
	junction_retreats(arms, lens, out, order, false);
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

} // namespace city
