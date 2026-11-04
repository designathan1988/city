// What a road edit may not leave behind, judged on the network it built (Roadcraft editor/editRules.ts): a stretch too short
// for the junctions at its two ends to open out. The edit runs, the mouths are worked out, and a refusal rolls it back.
#include "sim/commands/road_edit.h"

#include "sim/roads/junction.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <limits>

namespace city {

namespace {
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

// Metres the two mouths of a road would overlap.
double squeeze_of(const RoadNetwork &net, int seg) {
	const RoadSegment &g = net.segs[size_t(seg)];
	return wanted_retreat(net, g.a, seg) + wanted_retreat(net, g.b, seg) - g.len;
}
} // namespace

std::map<int, double> squeezes_in(const CityState &s, const RoadBox &box) {
	std::map<int, double> out;
	s.roads.for_segments_near(box.minx, box.minz, box.maxx, box.maxz, [&](int seg) {
		const double over = squeeze_of(s.roads, seg);
		if (over > 0) out[seg] = over;
	});
	return out;
}

bool squeezed_worse(const CityState &s, const std::map<int, double> &before, const RoadBox &box) {
	const double slack = s.catalog.tune.road_squeeze_slack_m;
	for (const auto &now : squeezes_in(s, box)) {
		const auto was = before.find(now.first);
		if (now.second > (was == before.end() ? 0 : was->second) + slack) return true;
	}
	return false;
}

} // namespace city
