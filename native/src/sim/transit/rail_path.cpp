// Rail paths on an immutable graph: original Dijkstra tie order, goal-cost termination, reusable scratch leases,
// and bounded endpoint answers. Readers never wait on another reader's geometry/search work.
#include "sim/transit/transit_data.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <mutex>
#include <utility>

namespace city {

namespace {
constexpr float NO_DISTANCE = std::numeric_limits<float>::max();
using Item = std::pair<float, int>;
using PathKey = std::array<float, 4>;
struct OnStretch { int edge = -1; float share = 0; V2 at; };

OnStretch on_graph(const RailGraph &g, const V2 &p) {
	OnStretch best;
	float dist = NO_DISTANCE;
	for (size_t i = 0; i < g.edges.size(); i++) {
		float share = 0;
		const V2 q = rail_segment_point(p, g.points[size_t(g.edges[i].a)], g.points[size_t(g.edges[i].b)], share);
		const float d = (p - q).len();
		if (d < dist) { dist = d; best = { int(i), share, q }; }
	}
	return best;
}

struct Workspace {
	std::vector<float> dist;
	std::vector<int> prev;
	std::vector<uint64_t> stamp;
	std::vector<Item> open;
	uint64_t search = 0;
	void begin(size_t n) {
		dist.resize(n); prev.resize(n); stamp.resize(n, 0); open.clear();
		if (++search == 0) { std::fill(stamp.begin(), stamp.end(), 0); search = 1; }
	}
	float &distance(int node) {
		const size_t k = size_t(node);
		if (stamp[k] != search) { stamp[k] = search; dist[k] = NO_DISTANCE; prev[k] = -1; }
		return dist[k];
	}
	void push(float d, int k) { open.emplace_back(d, k); std::push_heap(open.begin(), open.end(), std::greater<Item>()); }
	Item pop() {
		std::pop_heap(open.begin(), open.end(), std::greater<Item>());
		const Item value = open.back(); open.pop_back(); return value;
	}
	size_t bytes() const {
		return sizeof(Workspace) + dist.capacity() * sizeof(float) + prev.capacity() * sizeof(int)
			+ stamp.capacity() * sizeof(uint64_t) + open.capacity() * sizeof(Item);
	}
};
} // namespace

struct RailQueries {
	struct Answer {
		std::shared_ptr<const std::vector<V2>> points;
		std::list<PathKey>::iterator recent;
		size_t bytes;
	};
	explicit RailQueries(size_t bytes) : limit(bytes) {}
	std::mutex mutex;
	const size_t limit;
	size_t retained = 0;
	std::map<PathKey, Answer> answers;
	std::list<PathKey> recent;
	std::unique_ptr<Workspace> idle;

	std::shared_ptr<const std::vector<V2>> find(const PathKey &key) {
		std::lock_guard<std::mutex> lk(mutex);
		const auto it = answers.find(key);
		if (it == answers.end()) return {};
		recent.splice(recent.end(), recent, it->second.recent);
		return it->second.points; // only the root crosses the lock, not the path copy
	}
	std::unique_ptr<Workspace> acquire() {
		std::unique_ptr<Workspace> work;
		{
			std::lock_guard<std::mutex> lk(mutex);
			if (idle) { retained -= idle->bytes(); work = std::move(idle); }
		}
		return work ? std::move(work) : std::make_unique<Workspace>();
	}
	void room(size_t bytes, std::vector<std::shared_ptr<const std::vector<V2>>> &retired) { // metadata lock held
		while (!recent.empty() && retained > limit - bytes) {
			const auto it = answers.find(recent.front());
			retained -= it->second.bytes; retired.push_back(std::move(it->second.points));
			answers.erase(it); recent.pop_front();
		}
	}
	void remember(const PathKey &key, const std::vector<V2> &path) {
		// Copy outside the metadata lock. A simultaneous cold miss computes independently instead of waiting.
		const size_t estimate = path.size() * sizeof(V2) + sizeof(Answer) + sizeof(std::vector<V2>)
			+ sizeof(PathKey) * 2 + sizeof(void *) * 9;
		if (estimate > limit) return;
		auto points = std::make_shared<const std::vector<V2>>(path);
		const size_t bytes = estimate + (points->capacity() - path.size()) * sizeof(V2);
		if (bytes > limit) return;
		std::vector<std::shared_ptr<const std::vector<V2>>> retired;
		std::lock_guard<std::mutex> lk(mutex);
		if (answers.count(key)) return;
		if (idle && bytes > limit - idle->bytes()) return;
		room(bytes, retired);
		if (retained > limit - bytes) return; // an idle workspace may own the remaining capacity
		recent.push_back(key);
		answers.emplace(key, Answer{ std::move(points), std::prev(recent.end()), bytes }); retained += bytes;
	}
	void release(std::unique_ptr<Workspace> work) {
		const size_t bytes = work->bytes();
		if (bytes > limit) return;
		std::vector<std::shared_ptr<const std::vector<V2>>> retired;
		std::lock_guard<std::mutex> lk(mutex);
		if (idle) return; // retain at most one idle scratch; concurrent work has no exclusive geometry gate
		room(bytes, retired);
		retained += bytes; idle = std::move(work);
	}
};

RailGraph::RailGraph() : RailGraph(0) {}
RailGraph::RailGraph(size_t retained_bytes) : queries(std::make_shared<RailQueries>(retained_bytes)) {}

std::vector<V2> rail_path(const RailGraph &g, const V2 &from, const V2 &to) {
	if (!std::isfinite(from.x) || !std::isfinite(from.y) || !std::isfinite(to.x) || !std::isfinite(to.y)) return {};
	const PathKey key{ from.x, from.y, to.x, to.y };
	const auto owner = g.queries;
	if (const auto hit = owner->find(key)) return *hit;
	const OnStretch s = on_graph(g, from), e = on_graph(g, to);
	if (s.edge < 0 || e.edge < 0) { owner->remember(key, {}); return {}; }
	if (s.edge == e.edge) {
		std::vector<V2> direct{ s.at, e.at }; owner->remember(key, direct); return direct;
	}
	auto work = owner->acquire();
	work->begin(g.points.size());
	const RailGraph::Edge &se = g.edges[size_t(s.edge)], &te = g.edges[size_t(e.edge)];
	work->distance(se.a) = se.length * s.share;
	work->distance(se.b) = se.length * (1 - s.share);
	work->push(work->distance(se.a), se.a);
	work->push(work->distance(se.b), se.b);
	float goal = NO_DISTANCE;
	while (!work->open.empty()) {
		const auto [d, k] = work->pop();
		if (d > work->distance(k)) continue;
		// Nonnegative edge lengths: no unsettled node beyond this cost can improve either destination connector.
		// Include equal costs to retain original strict relaxation and endpoint-a tie preference.
		if (d > goal) break;
		if (k == te.a) goal = std::min(goal, d + te.length * e.share);
		if (k == te.b) goal = std::min(goal, d + te.length * (1 - e.share));
		for (int ei : g.at[size_t(k)]) {
			const auto &edge = g.edges[size_t(ei)];
			const int other = edge.a == k ? edge.b : edge.a;
			if (d + edge.length < work->distance(other)) {
				work->distance(other) = d + edge.length; work->prev[size_t(other)] = k;
				work->push(work->distance(other), other);
			}
		}
	}
	const float da = work->distance(te.a), db = work->distance(te.b);
	const float via_a = da == NO_DISTANCE ? NO_DISTANCE : da + te.length * e.share;
	const float via_b = db == NO_DISTANCE ? NO_DISTANCE : db + te.length * (1 - e.share);
	std::vector<V2> chain;
	if (via_a != NO_DISTANCE || via_b != NO_DISTANCE) {
		for (int k = via_a <= via_b ? te.a : te.b; k >= 0; k = work->prev[size_t(k)]) chain.push_back(g.points[size_t(k)]);
		std::reverse(chain.begin(), chain.end()); chain.insert(chain.begin(), s.at); chain.push_back(e.at);
	}
	owner->remember(key, chain);
	owner->release(std::move(work));
	return chain;
}

} // namespace city
