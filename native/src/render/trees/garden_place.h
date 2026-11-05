// The gardens of the lots (the Roadcraft 4.0's render/gardenPlace.ts, its behaviour): what a body's layout planted, its trees,
// shrubs, runs of hedge and flower beds, stood on the ground as plants of the trees' renderer. A body's plants are read out of it
// once and kept by its key, so a chunk built again, near or far, makes no body and holds the map no longer for it.
#pragma once

#include "render/trees/tree_place.h"
#include "sim/buildings/b40/record.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

// A plant of a body, in its lot's frame: the element it comes from, its middle and its size.
struct LotPlant {
	b40::Element kind = b40::EL_TREE;
	float x = 0, y = 0, w = 0, d = 0, h = 0;
};

class GardenBook {
public:
	using Plants = std::shared_ptr<const std::vector<LotPlant>>;
	// The plants of the record's body. Any worker may ask: two asking at once make it twice and keep one.
	Plants plants(const b40::Data &data, const Building &rec);
	void clear();

private:
	std::mutex mutex_;
	std::unordered_map<uint64_t, Plants> book_;
};

// What a view of the chunk draws of a garden: near, every plant; far, one card each, and no flower, a clump under a pixel there.
enum class GardenView : uint8_t { NEAR, FAR };

// The plants of the record's garden, in the world, on the ground under each.
void place_garden(const CityState &s, const TreeLook &T, GardenBook &book, const Building &rec, GardenView view, std::vector<TreeInstance> &out);

} // namespace city
