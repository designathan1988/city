// Shared immutable regional ecology and plant exclusion geometry. Source revisions outlive render-log drains;
// near/far trees and cover borrow the same bases, with independent cold misses and explicit byte retention.
#pragma once
#include "core/bounds_index.h"
#include "sim/nature/ecology.h"
#include "sim/roads/geometry.h"
#include <array>
#include <list>
#include <map>
#include <memory>
#include <mutex>

namespace city {
struct CityState;
struct PlantOccupancy {
	struct Road { Quad curve; float half; };
	struct Lot { float x, z, c, s, w, d; };
	std::vector<Road> roads;
	std::vector<Lot> rectangles;
	std::vector<int> lots; // original selection/order, also used by the park/garden pass
	BoundsIndex road_index, lot_index;
	bool near_road(float x, float z, float margin) const;
	bool in_lot(float x, float z, float margin) const;
	size_t retained_bytes() const;
};
struct PlantGround {
	std::shared_ptr<const PlantOccupancy> occupancy;
	std::shared_ptr<const EcologyField> eco;
};
class PlantGroundCache {
public:
	std::shared_ptr<const PlantGround> get(const CityState &state, int64_t key) const;
private:
	using Revisions = std::array<uint64_t, 9>;
	struct Entry {
		Revisions revisions;
		std::shared_ptr<const PlantGround> value;
		size_t bytes;
		std::list<int64_t>::iterator recent;
	};
	mutable std::mutex mutex_;
	mutable std::map<int64_t, Entry> entries_;
	mutable std::list<int64_t> recent_;
	mutable size_t retained_ = 0;
};
} // namespace city
