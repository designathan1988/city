// The streets a road is drawn along and laid in (RoadNetwork::settle_runs, pieces_of): the run of segments through joints, each
// with its start and the run's length (StreetRun), and the pieces one road the player laid was cut into.
#include "sim/roads/road_network.h"

#include <algorithm>

namespace city {

std::vector<int> RoadNetwork::pieces_of(int seg) const {
	// The piece past `node`, -1 where the road ends there.
	const auto beyond = [&](int at, int node) {
		const RoadNode &nd = nodes[size_t(node)];
		if (nd.segs.size() != 2) return -1;
		const int next = nd.segs[0] == at ? nd.segs[1] : nd.segs[0];
		const auto runs = [&](int s) { return (segs[size_t(s)].flags & (segs[size_t(s)].a == node ? SEG_RUNS_ON_A : SEG_RUNS_ON_B)) != 0; };
		return runs(at) && runs(next) ? next : -1;
	};
	const auto walk = [&](int node, std::vector<int> &out) {
		for (int at = seg, next = beyond(at, node); next >= 0 && next != seg; next = beyond(at, node)) {
			out.push_back(next);
			node = segs[size_t(next)].a == node ? segs[size_t(next)].b : segs[size_t(next)].a;
			at = next;
		}
	};
	std::vector<int> back, ahead;
	walk(segs[size_t(seg)].a, back);
	walk(segs[size_t(seg)].b, ahead);
	std::vector<int> out(back.rbegin(), back.rend());
	out.push_back(seg);
	out.insert(out.end(), ahead.begin(), ahead.end());
	return out;
}

void RoadNetwork::settle_runs() {
	runs_.resize(segs.size());
	std::vector<int> dirty;
	dirty.swap(run_dirty_);
	ChangeLog::dedup(dirty);
	// The segment before `at` on its street, -1 where the street begins (street_start of the render, the one definition).
	const auto before = [&](int at) {
		const RoadNode &n = nodes[size_t(segs[size_t(at)].a)];
		if (!n.joint || n.segs.size() != 2) return -1;
		const int back = n.segs[0] == at ? n.segs[1] : n.segs[0];
		return segs[size_t(back)].b == segs[size_t(at)].a ? back : -1; // a piece laid the other way starts its own
	};
	const auto after = [&](int at) {
		const RoadNode &n = nodes[size_t(segs[size_t(at)].b)];
		if (!n.joint || n.segs.size() != 2) return -1;
		const int next = n.segs[0] == at ? n.segs[1] : n.segs[0];
		return segs[size_t(next)].a == segs[size_t(at)].b ? next : -1;
	};
	std::vector<char> done(segs.size(), 0);
	std::vector<int> run;
	for (int s : dirty) {
		if (s < 0 || s >= int(segs.size()) || done[size_t(s)]) continue;
		if (!segs[size_t(s)].alive) {
			runs_[size_t(s)] = StreetRun();
			continue;
		}
		int head = s;
		bool ring = false;
		for (int back = before(head); back >= 0; back = before(head)) {
			if (back == s) {
				ring = true;
				break;
			}
			head = back;
		}
		run.clear();
		for (int at = head; at >= 0; at = after(at)) {
			run.push_back(at);
			done[size_t(at)] = 1;
			const int next = after(at);
			if (next == head) break;
		}
		double total = 0;
		for (int at : run) total += segs[size_t(at)].len;
		const int id = ring ? *std::min_element(run.begin(), run.end()) : head;
		double start = 0;
		for (int at : run) {
			runs_[size_t(at)] = { id, ring ? total - segs[size_t(at)].len : start, total };
			start += segs[size_t(at)].len;
		}
	}
}

} // namespace city
