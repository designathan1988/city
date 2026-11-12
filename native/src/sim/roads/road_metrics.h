// Shared station offsets and run lengths derived from physical road connectivity. The two continuation rules stay distinct.
#pragma once

#include <memory>
#include <mutex>

namespace city {
class RoadNetwork;
struct RoadMetric { double start = 0, length = 0; };
class RoadMetrics {
public:
	RoadMetric get(const RoadNetwork &roads, int segment) const;
private:
	struct Base;
	mutable std::mutex mutex_;
	mutable std::shared_ptr<const Base> ready_;
};
} // namespace city
