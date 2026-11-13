// The references to a road carried through its re-laying (road_refs.h).
#include "sim/commands/road_refs.h"

#include "sim/commands/edit_journal.h"
#include "sim/state/city_state.h"
#include "sim/transit/transit_ground.h"

#include <algorithm>

namespace city {

namespace {
bool listed(const std::vector<int> &ids, int id) { return std::find(ids.begin(), ids.end(), id) != ids.end(); }

bool alive_seg(const RoadNetwork &net, int seg) { return seg >= 0 && size_t(seg) < net.segs.size() && net.segs[size_t(seg)].alive; }

bool arm_of(const RoadNetwork &net, int node, int seg) {
	return node >= 0 && size_t(node) < net.nodes.size() && net.nodes[size_t(node)].alive && alive_seg(net, seg) &&
			listed(net.nodes[size_t(node)].segs, seg);
}

// The node a held link is written back on.
int home_of(const RoadRefs &refs, const HeldLink &h) { return h.node == refs.moved ? refs.to : h.node; }

// The piece meeting `node`, -1 when none does.
int piece_at(const RoadNetwork &net, const std::vector<int> &pieces, int node) {
	for (int p : pieces)
		if (alive_seg(net, p) && (net.segs[size_t(p)].a == node || net.segs[size_t(p)].b == node)) return p;
	return -1;
}

int stop_index(const TransitData &t, int id) {
	for (size_t i = 0; i < t.stops.size(); i++)
		if (t.stops[i].id == id) return int(i);
	return -1;
}
} // namespace

RoadRefs refs_of(const CityState &s, const std::vector<int> &segs) {
	RoadRefs refs;
	const RoadNetwork &net = s.roads;
	for (int seg : segs) {
		if (!alive_seg(net, seg)) continue;
		const RoadSegment &g = net.segs[size_t(seg)];
		for (int n : { g.a, g.b })
			for (const LaneLink &l : net.nodes[size_t(n)].links) {
				const bool from_due = listed(segs, l.from), to_due = listed(segs, l.to);
				if (!from_due && !to_due) continue;
				const bool held = std::any_of(refs.links.begin(), refs.links.end(), [&](const HeldLink &h) { return h.node == n && h.link == l; });
				if (held) continue; // a link naming two of the roads, or met again from the other road's end
				HeldLink h;
				h.node = n;
				h.link = l;
				h.from_due = from_due;
				h.to_due = to_due;
				refs.links.push_back(h);
			}
	}
	for (const TransitStop &q : s.transit.stops)
		if (q.mode == TransitMode::BUS && q.seg >= 0 && listed(segs, q.seg)) refs.stops.push_back(HeldStop{ q.id, q.seg, q.at, false });
	return refs;
}

void forget_refs_at(RoadRefs &refs, int node) {
	for (HeldLink &h : refs.links)
		if (h.node == node) h.lost = true;
}

void carry_refs(CityState &s, RoadRefs &refs, int old, const std::vector<int> &pieces) {
	const RoadNetwork &net = s.roads;
	std::vector<int> complete; // the nodes a link of this carry came back whole on
	for (HeldLink &h : refs.links) {
		const bool from = h.from_due && h.link.from == old, to = h.to_due && h.link.to == old;
		if (h.lost || (!from && !to)) continue;
		const int home = home_of(refs, h);
		const int piece = piece_at(net, pieces, home);
		if (piece < 0) {
			h.lost = true;
			continue;
		}
		if (from) {
			h.link.from = piece;
			h.from_due = false;
		}
		if (to) {
			h.link.to = piece;
			h.to_due = false;
		}
		if (!h.from_due && !h.to_due && !listed(complete, home)) complete.push_back(home);
	}
	for (int n : complete) {
		if (n < 0 || size_t(n) >= net.nodes.size() || !net.nodes[size_t(n)].alive) continue;
		std::vector<LaneLink> next = net.nodes[size_t(n)].links;
		const size_t was = next.size();
		for (HeldLink &h : refs.links) {
			if (h.lost || h.written || h.from_due || h.to_due || home_of(refs, h) != n) continue;
			h.written = true;
			// a road of it may have been cut again since it was carried (a later piece laid across it): then it stays dropped
			if (arm_of(net, n, h.link.from) && arm_of(net, n, h.link.to) && std::find(next.begin(), next.end(), h.link) == next.end())
				next.push_back(h.link);
		}
		if (next.size() != was) j_links(s, n, std::move(next));
	}
	for (HeldStop &q : refs.stops) {
		if (q.done || q.seg != old) continue;
		q.done = true;
		int best = -1;
		double nearest = 0;
		for (int p : pieces) {
			double dist = 0;
			if (transit_stop_by_road(s, q.at, p, dist) && (best < 0 || dist < nearest)) {
				best = p;
				nearest = dist;
			}
		}
		const int i = stop_index(s.transit, q.id);
		if (i >= 0) j_stop_road(s, i, best);
	}
}

} // namespace city
