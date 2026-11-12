// Linear preparation of oriented joint-prefix stations and flag-linked run lengths. Queries never walk the same prefix again.
#include "sim/roads/road_metrics.h"
#include "sim/roads/road_network.h"

#include <array>
#include <algorithm>
#include <cstdint>
#include <vector>

namespace city {
struct RoadMetrics::Base {
	uint64_t geometry = 0;
	int topology = 0;
	std::vector<RoadMetric> values;
};

RoadMetric RoadMetrics::get(const RoadNetwork &roads, int segment) const {
	const auto current = [&](const Base &b) { return b.geometry == roads.geometry_revision && b.topology == roads.version; };
	std::shared_ptr<const Base> base;
	{
		std::lock_guard<std::mutex> lk(mutex_);
		if (ready_ && current(*ready_)) base = ready_;
	}
	if (!base) {
		auto next = std::make_shared<Base>();
		next->geometry = roads.geometry_revision; next->topology = roads.version;
		const size_t n = roads.segs.size(); next->values.resize(n);
		std::vector<int> previous(n, -1), following(n, -1);
		for (size_t id = 0; id < n; id++) {
			const auto &g = roads.segs[id];
			if (!g.alive) continue;
			const auto &node = roads.nodes[size_t(g.a)];
			if (!node.joint || node.segs.size() != 2) continue;
			const int back = node.segs[0] == int(id) ? node.segs[1] : node.segs[0];
			if (back == int(id) || roads.segs[size_t(back)].b != g.a) continue;
			previous[id] = back; following[size_t(back)] = int(id);
		}
		std::vector<uint8_t> seen(n, 0);
		for (size_t root = 0; root < n; root++) {
			if (!roads.segs[root].alive || previous[root] >= 0) continue;
			double start = 0;
			for (int id = int(root); id >= 0 && !seen[size_t(id)]; id = following[size_t(id)]) {
				seen[size_t(id)] = 1; next->values[size_t(id)].start = start; start += roads.segs[size_t(id)].len;
			}
		}
		// Every remaining degree-two oriented component is a ring. The old query starts at its own piece and
		// sums all the other pieces, so its mathematical station is ring length minus that piece, not a new arbitrary seam.
		for (size_t seed = 0; seed < n; seed++) {
			if (!roads.segs[seed].alive || seen[seed]) continue;
			std::vector<int> ring; double total = 0;
			for (int id = int(seed); id >= 0 && !seen[size_t(id)]; id = previous[size_t(id)]) {
				seen[size_t(id)] = 1; ring.push_back(id); total += roads.segs[size_t(id)].len;
			}
			for (int id : ring) next->values[size_t(id)].start = total - roads.segs[size_t(id)].len;
		}
		// pieces_of uses mutual RUNS_ON flags, not the oriented-joint rule and not street_chain's greedy profile walk.
		const auto beyond = [&](int id, int node_id) {
			const auto &node = roads.nodes[size_t(node_id)];
			if (node.segs.size() != 2) return -1;
			const int other = node.segs[0] == id ? node.segs[1] : node.segs[0];
			const auto runs = [&](int s) {
				const auto &g = roads.segs[size_t(s)];
				return (g.flags & (g.a == node_id ? SEG_RUNS_ON_A : SEG_RUNS_ON_B)) != 0;
			};
			return runs(id) && runs(other) ? other : -1;
		};
		std::vector<std::array<int, 2>> joins(n, {-1, -1});
		for (size_t id = 0; id < n; id++) if (roads.segs[id].alive)
			joins[id] = {beyond(int(id), roads.segs[id].a), beyond(int(id), roads.segs[id].b)};
		std::fill(seen.begin(), seen.end(), 0);
		for (size_t seed = 0; seed < n; seed++) {
			if (!roads.segs[seed].alive || seen[seed]) continue;
			std::vector<int> run{int(seed)}; seen[seed] = 1; double length = 0; bool ring = true;
			for (size_t at = 0; at < run.size(); at++) {
				const int id = run[at]; length += roads.segs[size_t(id)].len;
				for (int other : joins[size_t(id)]) {
					if (other < 0) { ring = false; continue; }
					if (!seen[size_t(other)]) { seen[size_t(other)] = 1; run.push_back(other); }
				}
			}
			// Original ring pieces_of walks both ends, visiting every other piece twice. Preserve that algebra here.
			for (int id : run) next->values[size_t(id)].length = ring ? length * 2 - roads.segs[size_t(id)].len : length;
		}
		std::shared_ptr<const Base> retired;
		{
			std::lock_guard<std::mutex> lk(mutex_);
			if (!ready_ || !current(*ready_)) { retired = std::move(ready_); ready_ = next; }
			base = ready_;
		}
	}
	return segment >= 0 && size_t(segment) < base->values.size() ? base->values[size_t(segment)] : RoadMetric{};
}
} // namespace city
