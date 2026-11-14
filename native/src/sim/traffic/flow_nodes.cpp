// The far traffic at a node (street_flows.h, traffic.json _flow_doc): what the streets into it can send is passed to the streets
// they turn onto, each approach served in proportion to its capacity, in rounds that each end an approach or fill an exit; an
// approach with a full exit among its turns waits whole (first in, first out). The share of an approach's traffic that ends its
// trip at the street's end leaves the flows there.
#include "sim/traffic/flow_parts.h"

namespace city {
namespace traffic {

namespace {

// One approach into a node during its rounds.
struct Approach {
	int ds = -1;
	float want = 0, unserved = 0, pace = 0; // what it would pass on, what it still has to, its capacity a step
	bool waiting = false; // still asking in this round
};

// The exit an approach's turn `k` leads onto (-1: none), and the share of the approach's traffic taking it.
int exit_of(const StreetFlows &f, const Approach &a, int k) { return f.turn_to[size_t(a.ds) * FLOW_TURNS + size_t(k)]; }
float taking(const StreetFlows &f, const Approach &a, int k) { return f.turn_share[size_t(a.ds) * FLOW_TURNS + size_t(k)]; }
float room_of(const StreetFlows &f, int exit) { return f.live[size_t(exit)] ? f.room[size_t(exit)] : 0.f; }

// One round: the share of their pace every waiting approach passes on (no approach more than it has, no exit more than its room),
// passed, and each exit's room taken once by the whole of what was asked of it. The approaches still waiting after it.
int serve_round(StreetFlows &f, Approach *in, int arms) {
	float share = BIG;
	for (int i = 0; i < arms; i++) {
		if (!in[i].waiting) continue;
		share = std::min(share, in[i].unserved / in[i].pace);
		for (int k = 0; k < FLOW_TURNS; k++)
			if (const int exit = exit_of(f, in[i], k); exit >= 0) f.asked[size_t(exit)] += in[i].pace * taking(f, in[i], k);
	}
	for (int i = 0; i < arms; i++) {
		if (!in[i].waiting) continue;
		for (int k = 0; k < FLOW_TURNS; k++)
			if (const int exit = exit_of(f, in[i], k); exit >= 0 && f.asked[size_t(exit)] > FLOW_TINY) share = std::min(share, room_of(f, exit) / f.asked[size_t(exit)]);
	}
	share = share < BIG ? std::max(0.f, share) : 0.f;
	for (int i = 0; i < arms; i++) {
		if (!in[i].waiting) continue;
		for (int k = 0; k < FLOW_TURNS; k++)
			if (const int exit = exit_of(f, in[i], k); exit >= 0 && f.live[size_t(exit)]) f.came[size_t(exit)] += in[i].pace * taking(f, in[i], k) * share;
		in[i].unserved -= in[i].pace * share;
	}
	for (int i = 0; i < arms; i++) {
		if (!in[i].waiting) continue;
		for (int k = 0; k < FLOW_TURNS; k++) {
			const int exit = exit_of(f, in[i], k);
			if (exit < 0 || f.asked[size_t(exit)] <= 0) continue;
			f.room[size_t(exit)] = std::max(0.f, f.room[size_t(exit)] - f.asked[size_t(exit)] * share);
			f.asked[size_t(exit)] = 0;
		}
	}
	int asking = 0;
	for (int i = 0; i < arms; i++) {
		if (!in[i].waiting) continue;
		in[i].waiting = in[i].unserved > FLOW_TINY * in[i].pace;
		for (int k = 0; k < FLOW_TURNS && in[i].waiting; k++)
			if (const int exit = exit_of(f, in[i], k); exit >= 0 && room_of(f, exit) <= FLOW_TINY) in[i].waiting = false;
		asking += in[i].waiting ? 1 : 0;
	}
	return asking;
}

} // namespace

void flow_node(CityState &s, int n) {
	StreetFlows &f = s.traffic.flows;
	const RoadNode &node = s.roads.nodes[size_t(n)];
	if (!node.alive) return;
	Approach in[NODE_ARMS_MAX];
	int arms = 0, asking = 0;
	for (int seg : node.segs) {
		if (arms == NODE_ARMS_MAX) break;
		const int ds = in_ds(s.roads, seg, n);
		if (size_t(ds) >= f.live.size() || !f.live[size_t(ds)] || f.send[size_t(ds)] <= 0) continue;
		Approach &a = in[arms++];
		a.ds = ds;
		const bool ends = f.turn_to[size_t(ds) * FLOW_TURNS] < 0; // no street to go on by: the map's edge, a dead end
		a.want = a.unserved = f.send[size_t(ds)] * (1 - (ends ? 1.f : f.end_share[size_t(ds)]));
		a.pace = std::max(FLOW_TINY, f.cap[size_t(ds)]);
		a.waiting = a.unserved > FLOW_TINY;
		asking += a.waiting ? 1 : 0;
	}
	for (int round = 0; asking > 0 && round <= 2 * arms + 1; round++) asking = serve_round(f, in, arms);
	for (int i = 0; i < arms; i++) {
		// what left the approach: its share passed on, and its trips ending in the same proportion
		const float sent = f.send[size_t(in[i].ds)];
		const float passed = std::max(0.f, in[i].want - std::max(0.f, in[i].unserved));
		const float gone = in[i].want > FLOW_TINY ? sent * (passed / in[i].want) : sent;
		f.left[size_t(in[i].ds)] = gone;
		f.ended += double(gone - passed);
	}
}

} // namespace traffic
} // namespace city
