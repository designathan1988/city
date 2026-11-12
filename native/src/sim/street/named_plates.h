// Whole named-street geometry, prepared once per physical-road/name publication and indexed by ownership chunk.
// Callers hold the source map shared; immutable leases keep an in-flight reader independent of cache replacement.
#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace city {
class RoadNetwork;
class StreetWorks;
struct SignStand;

class NamedPlates {
public:
	void append(const RoadNetwork &roads, const StreetWorks &works, int64_t chunk, std::vector<SignStand> &out) const;
	void coverage(const RoadNetwork &roads, const StreetWorks &works, std::unordered_set<std::string> &used, std::unordered_set<int> &named) const;
private:
	struct Base;
	struct Geometry;
	std::shared_ptr<const Base> base(const RoadNetwork &roads, const StreetWorks &works) const;
	mutable std::mutex mutex_;
	mutable std::shared_ptr<const Base> topology_;
	mutable std::shared_ptr<const Geometry> ready_;
};
} // namespace city
