// The rules of a scenic city's streets (sim/catalog/scenic.h) for the graph builder (road_gen.h GraphRules), and its zoning:
// a road's rank is its profile's priority (local 0 up to highway), only a local street gives way to one it hugs, the land
// beside a highway or a railway takes no zone, and a street keeps the section it was laid with. A place's zone comes from its
// parks and works, then the district it lies in (the nearest seed) and the heat round it.
#pragma once

#include "sim/catalog/catalog.h"
#include "sim/citygen/road_gen.h"

#include <vector>

namespace city {

// The line a named road runs on: its own, or its river's course moved to the side.
Polyline scenic_road_line(const ScenicCity &city, const ScenicRoad &road);

class ScenicRules : public road_gen::GraphRules {
public:
	ScenicRules(const ScenicCity &city, const Catalog &catalog, TerrainGen &land);
	int rank(int profile) const override;
	bool yields(int profile) const override { return rank(profile) == 0; }
	int zone_beside(int profile, double x, double z) const override;
	int section(int profile, int, double, double, double) const override { return profile; }

	// The zone at p; `on_main`: beside a road that outranks a local street.
	int zone_at(V2 p, bool on_main) const;
	int district_at(V2 p) const; // the index of the nearest district seed
	bool in_park(V2 p) const;
	bool near_highway(V2 p, float reach) const; // within `reach` of a named highway's line
	bool highway(int profile) const;
	const std::vector<Polyline> &road_lines() const { return lines_; } // by named road

private:
	const ScenicCity &city_;
	const Catalog &catalog_;
	TerrainGen &land_;
	std::vector<Polyline> lines_;
	std::vector<size_t> highways_; // the named roads that are highways
};

} // namespace city
