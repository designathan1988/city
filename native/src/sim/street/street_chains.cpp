// Preserve the original seed/forward/backward walk; share sorted eligible arm choices and exact local dependencies.
#include "sim/street/street_chains.h"
#include "sim/roads/road_network.h"
#include "core/units.h"

#include <algorithm>
#include <unordered_set>

namespace city {
namespace {
constexpr int CHAIN_GUARD = 4000;
constexpr uint64_t CHAIN_KEY = uint64_t(1) << 63;
constexpr size_t MAP_NODE_WORDS = 4, LIST_NODE_WORDS = 2, SHARED_CONTROL_WORDS = 4;
size_t limit_of(const RoadNetwork &roads) { return size_t(std::max(0.f, roads.cat->tune.sim_street_chain_cache_mb) * BYTES_PER_MIB); }
}
bool StreetChains::Value::current(const RoadNetwork &roads) const {
	if (cosine != roads.cat->tune.edit_street_cos) return false;
	for (const auto &dep : dependencies) if (roads.chain_revision(dep.first) != dep.second) return false;
	return true;
}
size_t StreetChains::footprint(const Value &v) {
	return sizeof(Value) + v.chain.capacity() * sizeof(int) + v.choices.capacity() * sizeof(Continuation) +
		v.dependencies.capacity() * sizeof(std::pair<int, uint64_t>) + sizeof(Entries::value_type) +
		sizeof(uint64_t) + (MAP_NODE_WORDS + LIST_NODE_WORDS + SHARED_CONTROL_WORDS) * sizeof(void *);
}
void StreetChains::retire(Entries::iterator it, Roots &retired) const {
	bytes_ -= it->second.value->bytes;
	recent_.erase(it->second.age);
	retired.push_back(std::move(it->second.value)); entries_.erase(it);
}
std::shared_ptr<const StreetChains::Value> StreetChains::lookup(const RoadNetwork &roads, uint64_t key) const {
	Roots retired;
	std::shared_ptr<const Value> result;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		const size_t limit = limit_of(roads);
		while (bytes_ > limit && !recent_.empty()) retire(entries_.find(recent_.back()), retired);
		const auto it = entries_.find(key);
		if (it != entries_.end()) {
			result = it->second.value;
			recent_.splice(recent_.begin(), recent_, it->second.age);
		}
	}
	// Validating dependencies and geometry both stay outside the metadata mutex. Readers never await another cold build.
	return result && result->current(roads) ? result : std::shared_ptr<const Value>();
}
void StreetChains::retain(const RoadNetwork &roads, uint64_t key, std::shared_ptr<const Value> value) const {
	const size_t limit = limit_of(roads);
	if (value->bytes > limit || !value->current(roads)) return;
	Roots retired;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		const auto was = entries_.find(key);
		if (was != entries_.end()) retire(was, retired);
		while (bytes_ + value->bytes > limit && !recent_.empty()) retire(entries_.find(recent_.back()), retired);
		recent_.push_front(key); bytes_ += value->bytes;
		entries_.emplace(key, Entry{std::move(value), recent_.begin()});
	}
}
void StreetChains::clear() {
	Entries retired;
	std::list<uint64_t> order;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		retired.swap(entries_); order.swap(recent_); bytes_ = 0;
	}
}
std::shared_ptr<const StreetChains::Value> StreetChains::choices(const RoadNetwork &roads, int segment, int end) const {
	const uint64_t key = uint64_t(segment) * 2 + uint64_t(end);
	if (auto found = lookup(roads, key)) return found;
	auto result = std::make_shared<Value>();
	result->cosine = roads.cat->tune.edit_street_cos;
	const RoadSegment &g = roads.segs[size_t(segment)];
	const int node = end == 1 ? g.b : g.a;
	result->dependencies.push_back({node, roads.chain_revision(node)});
	const Profile &profile = roads.cat->profile(g.profile);
	const Arm here = arm_of(g.q, profile, end);
	if (!profile.ring) for (int next : roads.nodes[size_t(node)].segs) {
		if (next == segment || roads.segs[size_t(next)].profile != g.profile) continue;
		const int next_end = roads.segs[size_t(next)].a == node ? 0 : 1;
		if (profile.one_way && next_end != (end == 1 ? 0 : 1)) continue;
		const Arm arm = arm_of(roads.segs[size_t(next)].q, profile, next_end);
		const double dot = -(arm.dx * here.dx + arm.dz * here.dz);
		if (dot > result->cosine) result->choices.push_back({next, next_end == 0 ? 1 : 0, dot});
	}
	std::stable_sort(result->choices.begin(), result->choices.end(), [](const Continuation &a, const Continuation &b) { return a.dot > b.dot; });
	result->bytes = footprint(*result); retain(roads, key, result);
	return result;
}
std::vector<int> StreetChains::get(const RoadNetwork &roads, int seed) const {
	const uint64_t key = CHAIN_KEY | uint64_t(seed);
	if (auto found = lookup(roads, key)) return found->chain;
	auto result = std::make_shared<Value>();
	result->cosine = roads.cat->tune.edit_street_cos;
	result->chain.push_back(seed);
	std::unordered_set<int> seen{seed};
	for (int start_end : {1, 0}) {
		int segment = seed, end = start_end;
		for (int guard = 0; guard < CHAIN_GUARD; guard++) {
			const auto options = choices(roads, segment, end);
			result->dependencies.insert(result->dependencies.end(), options->dependencies.begin(), options->dependencies.end());
			const auto next = std::find_if(options->choices.begin(), options->choices.end(), [&](const Continuation &c) { return !seen.count(c.segment); });
			if (next == options->choices.end()) break;
			segment = next->segment; end = next->end;
			result->chain.push_back(segment); seen.insert(segment);
		}
	}
	std::sort(result->dependencies.begin(), result->dependencies.end());
	result->dependencies.erase(std::unique(result->dependencies.begin(), result->dependencies.end()), result->dependencies.end());
	result->bytes = footprint(*result); retain(roads, key, result);
	return result->chain;
}
} // namespace city
